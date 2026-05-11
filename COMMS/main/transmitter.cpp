#include <cstdint>
#include <string>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"

#include "driver/gpio.h"
#include "driver/spi_master.h"

#include <RadioLib.h>

namespace {

constexpr const char* TAG = "lora_tx";

// From LoRa/LoRa_TRANSMITTER/LoRa_TRANSMITTER.ino
constexpr gpio_num_t LORA_CS = static_cast<gpio_num_t>(41);
constexpr gpio_num_t LORA_DIO1 = static_cast<gpio_num_t>(39);
constexpr gpio_num_t LORA_RST = static_cast<gpio_num_t>(42);
constexpr gpio_num_t LORA_BUSY = static_cast<gpio_num_t>(40);
constexpr gpio_num_t LORA_LED = static_cast<gpio_num_t>(48);

// TODO: Set to your actual SPI wiring.
constexpr gpio_num_t LORA_SPI_SCK = static_cast<gpio_num_t>(12);
constexpr gpio_num_t LORA_SPI_MOSI = static_cast<gpio_num_t>(11);
constexpr gpio_num_t LORA_SPI_MISO = static_cast<gpio_num_t>(13);

constexpr spi_host_device_t LORA_SPI_HOST = SPI2_HOST;

class EspIdfHal final : public RadioLibHal {
public:
  EspIdfHal()
      : RadioLibHal(GPIO_MODE_INPUT, GPIO_MODE_OUTPUT, 0, 1, GPIO_INTR_POSEDGE, GPIO_INTR_NEGEDGE) {}

  void init() override {
    (void)gpio_install_isr_service(0);

    spi_bus_config_t bus_cfg{};
    bus_cfg.miso_io_num = LORA_SPI_MISO;
    bus_cfg.mosi_io_num = LORA_SPI_MOSI;
    bus_cfg.sclk_io_num = LORA_SPI_SCK;
    bus_cfg.quadwp_io_num = GPIO_NUM_NC;
    bus_cfg.quadhd_io_num = GPIO_NUM_NC;
    ESP_ERROR_CHECK(spi_bus_initialize(LORA_SPI_HOST, &bus_cfg, SPI_DMA_CH_AUTO));

    spi_device_interface_config_t dev_cfg{};
    dev_cfg.mode = 0;
    dev_cfg.clock_speed_hz = 8 * 1000 * 1000;
    dev_cfg.spics_io_num = GPIO_NUM_NC;  // CS handled by RadioLib (manual)
    dev_cfg.queue_size = 1;
    ESP_ERROR_CHECK(spi_bus_add_device(LORA_SPI_HOST, &dev_cfg, &dev_));
  }

  void term() override {
    if (dev_) {
      (void)spi_bus_remove_device(dev_);
      dev_ = nullptr;
    }
    (void)spi_bus_free(LORA_SPI_HOST);
  }

  void pinMode(uint32_t pin, uint32_t mode) override {
    if (pin == RADIOLIB_NC) {
      return;
    }
    gpio_config_t cfg{};
    cfg.pin_bit_mask = 1ULL << pin;
    cfg.mode = static_cast<gpio_mode_t>(mode);
    cfg.pull_up_en = GPIO_PULLUP_DISABLE;
    cfg.pull_down_en = GPIO_PULLDOWN_DISABLE;
    cfg.intr_type = GPIO_INTR_DISABLE;
    ESP_ERROR_CHECK(gpio_config(&cfg));
  }

  void digitalWrite(uint32_t pin, uint32_t value) override {
    if (pin == RADIOLIB_NC) {
      return;
    }
    gpio_set_level(static_cast<gpio_num_t>(pin), value ? 1 : 0);
  }

  uint32_t digitalRead(uint32_t pin) override {
    if (pin == RADIOLIB_NC) {
      return 0;
    }
    return static_cast<uint32_t>(gpio_get_level(static_cast<gpio_num_t>(pin)));
  }

  void attachInterrupt(uint32_t interruptNum, void (*interruptCb)(void), uint32_t mode) override {
    if (interruptNum == RADIOLIB_NC) {
      return;
    }
    gpio_set_intr_type(static_cast<gpio_num_t>(interruptNum), static_cast<gpio_int_type_t>(mode));
    ESP_ERROR_CHECK(gpio_isr_handler_add(static_cast<gpio_num_t>(interruptNum),
                                        reinterpret_cast<gpio_isr_t>(interruptCb), nullptr));
  }

  void detachInterrupt(uint32_t interruptNum) override {
    if (interruptNum == RADIOLIB_NC) {
      return;
    }
    (void)gpio_isr_handler_remove(static_cast<gpio_num_t>(interruptNum));
  }

  void delay(RadioLibTime_t ms) override { vTaskDelay(pdMS_TO_TICKS(ms)); }

  void delayMicroseconds(RadioLibTime_t us) override {
    const int64_t start = esp_timer_get_time();
    while ((esp_timer_get_time() - start) < static_cast<int64_t>(us)) {
      taskYIELD();
    }
  }

  RadioLibTime_t millis() override { return static_cast<RadioLibTime_t>(esp_timer_get_time() / 1000); }
  RadioLibTime_t micros() override { return static_cast<RadioLibTime_t>(esp_timer_get_time()); }

  long pulseIn(uint32_t pin, uint32_t state, RadioLibTime_t timeout) override {
    const int64_t start = esp_timer_get_time();
    while (digitalRead(pin) == state) {
      if (static_cast<RadioLibTime_t>(esp_timer_get_time() - start) >= timeout) {
        return 0;
      }
      taskYIELD();
    }
    while (digitalRead(pin) != state) {
      if (static_cast<RadioLibTime_t>(esp_timer_get_time() - start) >= timeout) {
        return 0;
      }
      taskYIELD();
    }
    const int64_t pulse_start = esp_timer_get_time();
    while (digitalRead(pin) == state) {
      if (static_cast<RadioLibTime_t>(esp_timer_get_time() - start) >= timeout) {
        return 0;
      }
      taskYIELD();
    }
    return static_cast<long>(esp_timer_get_time() - pulse_start);
  }

  void spiBegin() override {}
  void spiBeginTransaction() override {}

  void spiTransfer(uint8_t* out, size_t len, uint8_t* in) override {
    spi_transaction_t t{};
    t.length = len * 8;
    t.tx_buffer = out;
    t.rx_buffer = in;
    ESP_ERROR_CHECK(spi_device_transmit(dev_, &t));
  }

  void spiEndTransaction() override {}
  void spiEnd() override {}
  void yield() override { taskYIELD(); }

private:
  spi_device_handle_t dev_ = nullptr;
};

void lora_tx_task(void*) {
  gpio_config_t led_cfg{};
  led_cfg.pin_bit_mask = 1ULL << static_cast<uint64_t>(LORA_LED);
  led_cfg.mode = GPIO_MODE_OUTPUT;
  led_cfg.pull_up_en = GPIO_PULLUP_DISABLE;
  led_cfg.pull_down_en = GPIO_PULLDOWN_DISABLE;
  led_cfg.intr_type = GPIO_INTR_DISABLE;
  ESP_ERROR_CHECK(gpio_config(&led_cfg));
  gpio_set_level(LORA_LED, 0);

  static EspIdfHal hal;
  static Module mod(&hal, static_cast<uint32_t>(LORA_CS), static_cast<uint32_t>(LORA_DIO1),
                    static_cast<uint32_t>(LORA_RST), static_cast<uint32_t>(LORA_BUSY));
  static SX1262 radio(&mod);

  ESP_LOGI(TAG, "Starting LoRa TX...");
  const int16_t init_state = radio.begin(868.0);
  if (init_state != RADIOLIB_ERR_NONE) {
    ESP_LOGE(TAG, "Init failed, code: %d", static_cast<int>(init_state));
    vTaskDelete(nullptr);
  }
  ESP_LOGI(TAG, "LoRa init OK!");

  int counter = 0;
  while (true) {
    const std::string msg = std::to_string(counter);
    ESP_LOGI(TAG, "Sending: %s", msg.c_str());

    const int16_t tx_state = radio.transmit(msg.c_str());
    if (tx_state == RADIOLIB_ERR_NONE) {
      ESP_LOGI(TAG, "Sent successfully!");
      gpio_set_level(LORA_LED, 1);
    } else {
      ESP_LOGE(TAG, "Send failed, code: %d", static_cast<int>(tx_state));
    }

    vTaskDelay(pdMS_TO_TICKS(10));
    gpio_set_level(LORA_LED, 0);
    vTaskDelay(pdMS_TO_TICKS(90));
    counter++;
  }
}

}  // namespace

extern "C" void lora_transmitter_start() {
  xTaskCreate(lora_tx_task, "lora_tx", 8192, nullptr, 5, nullptr);
}

