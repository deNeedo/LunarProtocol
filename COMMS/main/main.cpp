#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "esp_err.h"
#include "hal/gpio_types.h"

#include <RadioLib.h>

constexpr gpio_num_t LORA_LED = static_cast<gpio_num_t>(48);

static void blink_task(void *arg)
{
    (void)arg;

    const gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << static_cast<uint64_t>(LORA_LED)),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    ESP_ERROR_CHECK(gpio_config(&io_conf));

    while (true) {
        gpio_set_level(LORA_LED, 1);
        vTaskDelay(pdMS_TO_TICKS(10));
        gpio_set_level(LORA_LED, 0);
        vTaskDelay(pdMS_TO_TICKS(990));
    }
}

extern "C" void app_main(void)
{
    xTaskCreate(blink_task, "blink", 2048, nullptr, 5, nullptr);
}

