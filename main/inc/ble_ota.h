#ifndef _ble_ota_h_
#define _ble_ota_h_

#include "freertos/FreeRTOS.h"

bool ble_ota_ringbuf_init(uint32_t ringbuf_size);
size_t write_to_ringbuf(const uint8_t *data, size_t size);
void ota_task(void *arg);
void ota_recv_fw_cb(uint8_t *buf, uint32_t length);
void ble_ota_init(void);
void ble_ota_gatt_create(void);

#endif