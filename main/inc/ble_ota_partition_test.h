#ifndef _ble_ota_partition_test_h_
#define _ble_ota_partition_test_h_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_ota_ops.h"
#include "esp_log.h"

#define BLE_TAG "partition_test"
#define GPIO_CHANGE_PIN GPIO_NUM_0

static bool test(void){
    ESP_LOGI(BLE_TAG,"test...");
    return true;
}

static void gpio_change_partition_task(void* arg){
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << GPIO_CHANGE_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);
    while(1){
        ESP_LOGI(BLE_TAG,"gpio change partition task is runing");
        if(gpio_get_level(GPIO_CHANGE_PIN) == 0){
            ESP_LOGI(BLE_TAG,"gpio%d is low",GPIO_CHANGE_PIN);
            const esp_partition_t *factory_partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_FACTORY, NULL);
            if (factory_partition == NULL) {
                ESP_LOGE(BLE_TAG, "Factory partition not found!");
                goto erro;
            }
            if (esp_ota_set_boot_partition(factory_partition) != ESP_OK) {
                ESP_LOGE(BLE_TAG, "set_boot_partition to factory failed!");
                goto erro;
            }
            esp_restart();
        }
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
    erro:
    ESP_LOGE(BLE_TAG, "run to factory failed");
    vTaskDelete(NULL);
}

static void gpio_change_partition(){
    xTaskCreate(gpio_change_partition_task,"gpio_change_partition_task",4096,NULL,1,NULL);
}

static void partition_test(){
    esp_ota_img_states_t ota_state;
    const esp_partition_t *running = esp_ota_get_running_partition();
    if (esp_ota_get_state_partition(running, &ota_state) == ESP_OK) {
        if (ota_state == ESP_OTA_IMG_PENDING_VERIFY) {
            bool test_is_ok = test();
            if (test_is_ok) {
                ESP_LOGI(BLE_TAG, "Diagnostics completed successfully! Continuing execution ...");
                esp_ota_mark_app_valid_cancel_rollback();
            } else {
                ESP_LOGE(BLE_TAG, "Diagnostics failed! Start rollback to the previous version ...");
                esp_ota_mark_app_invalid_rollback_and_reboot();
            }
        }
    }
}

#endif