#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "driver/ledc.h"
#include "driver/i2c.h"
#include "driver/uart.h"
#include <sys/socket.h>

#include "esp_log.h"
#include "kalam32_config.h"
#include "kalam32_support.h"
#include "kalam32_tcp.h"

#include "ads1292r.h"

esp_err_t event_handler(void *ctx, system_event_t *event)
{
    return ESP_OK;
}


void app_main(void)
{
    nvs_flash_init();
    tcpip_adapter_init();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );

    //kalam32_uart_init();
    kalam_wifi_init();
    vTaskDelay(1000/ portTICK_PERIOD_MS);

    ads1292_Init();
    vTaskDelay(1000/ portTICK_PERIOD_MS);

    kalam32_tcp_server_start();

    //Initing MAX30003 and SPI
    //max30003_initchip(PIN_SPI_MISO,PIN_SPI_MOSI,PIN_SPI_SCK,PIN_SPI_CS);
    //max30205_initchip(PIN_I2C_SDA,PIN_I2C_SCL);
    //ads1292r_start();

    gpio_set_direction(GPIO_NUM_5, GPIO_MODE_OUTPUT);
    int level = 0;

    while (true)
    {
        gpio_set_level(GPIO_NUM_5, level);
        level = !level;

        //max_temp=max30205_readTemperature();
        //printf("Temp: %f\n", max_temp);

        vTaskDelay(1000/ portTICK_PERIOD_MS);
        //MAX30003_ReadID();
        //max30003_read_send_data();
        //uart_write_bytes(uart_num, (const char *) uart_data, 25);
        //uart_tx_chars(uart_num, (const char *) uart_data, 5);
        //putc('a',stdout);

    }
}
