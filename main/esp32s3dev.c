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
void lv_tick_task(void* arg){
    while(1){
        vTaskDelay(pdMS_TO_TICKS(10));
        lv_tick_inc(10);
    }
}

void lv_timer_handler_task(void* arg){
    while(1){
        uint32_t time_till_next = lv_timer_handler();
        if(time_till_next == LV_NO_TIMER_READY) time_till_next = 100;
        vTaskDelay(pdMS_TO_TICKS(time_till_next));
    }
}

static void anim_x_cb(void * var, int32_t v)
{
    lv_obj_set_x(var, v);
}
static void anim_size_cb(void * var, int32_t v)
{
    lv_obj_set_size(var, v, v);
}

static void sw_event_cb(lv_event_t * e)
{
    lv_obj_t * sw = lv_event_get_target(e);
    lv_obj_t * label = lv_event_get_user_data(e);

    if(lv_obj_has_state(sw, LV_STATE_CHECKED)) {
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, label);
        lv_anim_set_values(&a, lv_obj_get_x(label), 64-lv_obj_get_height(label));
        lv_anim_set_time(&a, 500);
        lv_anim_set_exec_cb(&a, anim_x_cb);
        lv_anim_set_path_cb(&a, lv_anim_path_overshoot);
        lv_anim_start(&a);
    }
    else {
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, label);
        lv_anim_set_values(&a, lv_obj_get_x(label), -lv_obj_get_width(label));
        lv_anim_set_time(&a, 500);
        lv_anim_set_exec_cb(&a, anim_x_cb);
        lv_anim_set_path_cb(&a, lv_anim_path_ease_in);
        lv_anim_start(&a);
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
    xTaskCreate(lv_tick_task,"lv_tick_task",1024,NULL,1,NULL);
    xTaskCreate(lv_timer_handler_task,"lv_timer_handler_task",4096,NULL,1,NULL);

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

    lv_obj_t *screen = lv_scr_act();
    static lv_style_t style_screen;
    lv_style_init(&style_screen);
    lv_style_set_bg_color(&style_screen, lv_color_hex(0x0000FF)); // 蓝色背景
    //lv_style_set_bg_opa(&style_screen, LV_OPA_COVER);                       // 不透明度100%
    lv_style_set_text_color(&style_screen,lv_color_hex(0xFFFFFF));
    lv_style_set_text_font(&style_screen,&lv_font_montserrat_12);
    lv_obj_add_style(screen, &style_screen, LV_PART_MAIN);

    /*lv_obj_t *container = lv_obj_create(lv_scr_act());
    lv_obj_set_size(container, 124, 60); // 设置容器大小
    lv_obj_center(container);*/

    /*lv_obj_t *label = lv_label_create(screen);
    lv_obj_center(label);
    lv_obj_set_size(label,100,60);
    static lv_style_t style_label;
    lv_style_init(&style_label);
    lv_style_set_bg_color(&style_label, lv_color_hex(0xFF0000));
    lv_style_set_bg_opa(&style_label, LV_OPA_COVER);
    lv_style_set_radius(&style_label,5);
    static lv_grad_dsc_t grad;
    grad.dir = LV_GRAD_DIR_VER;
    grad.stops_count = 2;
    grad.stops[0].color = lv_palette_lighten(LV_PALETTE_GREY, 1);
    grad.stops[1].color = lv_palette_main(LV_PALETTE_BROWN);
    grad.stops[0].frac = 128;
    grad.stops[1].frac = 148;
    lv_style_set_bg_grad(&style_label, &grad);
    lv_obj_add_style(label, &style_label, LV_PART_MAIN);
    lv_label_set_text(label, "Hello World!");*/

    lv_obj_t * sw_label = lv_label_create(screen);
    lv_label_set_text(sw_label, "Hello");
    lv_obj_set_pos(sw_label, 40, 10);

    lv_obj_t * sw = lv_switch_create(screen);
    lv_obj_center(sw);
    lv_obj_add_event_cb(sw, sw_event_cb, LV_EVENT_VALUE_CHANGED, sw_label);

    lv_obj_t * obj = lv_obj_create(screen);
    lv_obj_set_style_bg_color(obj, lv_palette_main(LV_PALETTE_RED), 0);
    lv_obj_set_style_radius(obj, LV_RADIUS_CIRCLE, 0);
    lv_obj_align(obj, LV_ALIGN_LEFT_MID, 10, 30);
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a, 10, 50);
    lv_anim_set_time(&a, 1000);
    lv_anim_set_playback_delay(&a, 100);
    lv_anim_set_playback_time(&a, 300);
    lv_anim_set_repeat_delay(&a, 500);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);
    lv_anim_set_exec_cb(&a, anim_size_cb);
    lv_anim_start(&a);
    lv_anim_set_exec_cb(&a, anim_x_cb);
    lv_anim_set_values(&a, 10, 240);
    lv_anim_start(&a);

    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << GPIO_NUM_35),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);
    while(1){
        lv_obj_add_state(sw, LV_STATE_CHECKED);
        lv_event_send(sw, LV_EVENT_VALUE_CHANGED, NULL);
        ESP_LOGI(TAG,"set led on");
        gpio_set_level(GPIO_NUM_35,1);
        vTaskDelay(pdMS_TO_TICKS(3000));
        lv_obj_clear_state(sw, LV_STATE_CHECKED);
        lv_event_send(sw, LV_EVENT_VALUE_CHANGED, NULL);
        ESP_LOGI(TAG,"set led off");
        gpio_set_level(GPIO_NUM_35,0);
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}
