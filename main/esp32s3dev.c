#include <stdio.h>
#include <string.h>
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_err.h"

static const char *TAG = "esp32dev";
esp_netif_t *sta_netif;
esp_netif_t *ap_netif;

static void event_handler(void* arg,esp_event_base_t event_base,int32_t event_id,void* event_data){
    if(event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP){
        ESP_LOGI(TAG, "STA got IP, enabling NAT for AP...");
        ESP_ERROR_CHECK(esp_netif_napt_enable(ap_netif));
    }
}

void app_main(void)
{
    nvs_flash_init();
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
    ESP_LOGI(TAG,"wifi apsta 启动成功");
}
