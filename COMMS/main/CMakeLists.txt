idf_component_register(
    SRCS "transmitter.cpp"
    INCLUDE_DIRS "."
    REQUIRES RadioLib
    PRIV_REQUIRES esp_driver_gpio esp_driver_spi freertos esp_common esp_timer
)

# RadioLib headers trigger overloaded-virtual warnings on newer GCC; ESP-IDF
# builds with -Werror, so disable that warning for this component.
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-error=overloaded-virtual -Wno-overloaded-virtual)
