#include <stdio.h>
#include <string.h>
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_err.h"
#include "ble_ota.h"
#include "ble_ota_partition_test.h"
#include "driver/gpio.h"
#include "lv_conf.h"
#include "lvgl_helpers.h"

static const char *TAG = "esp32dev";
/*
esp_netif_t *sta_netif;
esp_netif_t *ap_netif;

static void event_handler(void* arg,esp_event_base_t event_base,int32_t event_id,void* event_data){
    if(event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP){
        ESP_LOGI(TAG, "STA got IP, enabling NAT for AP...");
        ESP_ERROR_CHECK(esp_netif_napt_enable(ap_netif));
    }
}
*/
void tick_task(void* arg){
    while(1){
        vTaskDelay(pdMS_TO_TICKS(5));
        lv_tick_inc(5);
    }
}

void lv_timer_handler_task(void* arg){
    while(1){
        uint32_t time_till_next = lv_timer_handler();
        if(time_till_next == LV_NO_TIMER_READY) time_till_next = 5;
        vTaskDelay(pdMS_TO_TICKS(time_till_next));
    }
}

void app_main(void)
{
    nvs_flash_init();
    /*
    esp_event_loop_create_default();
    esp_event_handler_instance_register(IP_EVENT,IP_EVENT_STA_GOT_IP,&event_handler,NULL,NULL);
    esp_netif_init();
    sta_netif = esp_netif_create_default_wifi_sta();
    ap_netif = esp_netif_create_default_wifi_ap();
    wifi_init_config_t wifi_init_cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_init_cfg);
    wifi_config_t stacfg = {
        .sta = {
            .ssid = "",
            .password = "",
            .threshold.authmode = WIFI_AUTH_WPA_WPA2_PSK
        }
    };
    wifi_config_t apcfg = {
        .ap = {
            .ssid = "esp32-s3",
            .ssid_len = strlen("esp32-s3"),
            .password = "123456789",
            .channel = 6,
            .max_connection = 4,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        }
    };
    esp_wifi_set_mode(WIFI_MODE_APSTA);
    esp_wifi_set_config(WIFI_IF_STA,&stacfg);
    esp_wifi_set_config(WIFI_IF_AP,&apcfg);
    esp_wifi_start();
    ESP_ERROR_CHECK(esp_wifi_connect());
    ESP_LOGI(TAG,"wifi apsta 启动成功");*/
    partition_test();
    gpio_change_partition();
    lv_init();
    lvgl_driver_init();
    xTaskCreate(tick_task,"tick_task",16,NULL,3,NULL);
    xTaskCreate(lv_timer_handler_task,"lv_timer_handler_task",64,NULL,3,NULL);
    static lv_disp_draw_buf_t draw_buf;
    static lv_color_t buf1[2048];
    lv_disp_draw_buf_init(&draw_buf, buf1, NULL,2048);
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.flush_cb = st7735s_flush;
    disp_drv.draw_buf = &draw_buf;
    disp_drv.hor_res = 128;
    disp_drv.ver_res = 160;
    lv_disp_drv_register(&disp_drv);
    
    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "Hello World!");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
    
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << GPIO_NUM_35),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);
    while(1){
        ESP_LOGI(TAG,"set led on");
        gpio_set_level(GPIO_NUM_35,1);
        vTaskDelay(pdMS_TO_TICKS(1000));
        ESP_LOGI(TAG,"set led off");
        gpio_set_level(GPIO_NUM_35,0);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
