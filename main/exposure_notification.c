/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/



/****************************************************************************
*
*
****************************************************************************/

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "nvs_flash.h"

#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"
#include "esp_gatt_defs.h"
#include "esp_bt_main.h"
#include "esp_bt_defs.h"
#include "esp_exposure_api.h"
#include "exposure_timer.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"

#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"

static const char* DEMO_TAG = "Exposure Notification";
extern esp_ble_exposure_data_t exposure_config;

///Declare static functions
static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);

/*

From Apple-Google Bluetooth specification v1.1:
The scanning interval and window shall have sufficient coverage to discover nearby Exposure Notification Service advertisements within 5 minutes.

FIXME: Might need adjustment to manage power
*/

static esp_ble_scan_params_t ble_scan_params = {
    .scan_type              = BLE_SCAN_TYPE_ACTIVE,
    .own_addr_type          = BLE_ADDR_TYPE_RANDOM,
    .scan_filter_policy     = BLE_SCAN_FILTER_ALLOW_ALL,
    .scan_interval          = 0x50,
    .scan_window            = 0x30,
    .scan_duplicate         = BLE_SCAN_DUPLICATE_DISABLE
};

/*
Advertising interval Time = N * 0x625 ms. E.g: 0x800 * 0.625ms = 1.28 sec
From Apple-Google Bluetooth specification v1.1:
The broadcasting interval is subject to change, but is currently recommended to be 200-270 milliseconds

*/

static esp_ble_adv_params_t ble_adv_params = {
    .adv_int_min        = 0x140, // 200ms 
    .adv_int_max        = 0x1b0, // 270ms
    .adv_type           = ADV_TYPE_NONCONN_IND,
    .own_addr_type      = BLE_ADDR_TYPE_RANDOM,
    .channel_map        = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

/* 
Registered as part of app_main()
Handle scan start/stop, start advertising

Callback events documented in ./esp-idf/components/bt/host/bluedroid/api/include/api/esp_gap_ble_api.h

FIXME: use scan started event to set random addr
        use ESP_GAP_BLE_SET_STATIC_RAND_ADDR_EVT (random addr set) to start advertising

AFAIK: bluetooth and/or wifi has to be running for the random generator to work
see esp_random(void) warning about hardware RNG
*/

static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    esp_err_t err;

    switch (event) {
    case ESP_GATTC_REG_EVT:{
        break;
    }
    case ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT:{
        
        ESP_LOGI(DEMO_TAG, "Start advertising");
        esp_ble_gap_start_advertising(&ble_adv_params);
        break;
    }
    case ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT: {
        //the unit of the duration is second, 0 means scan permanently
        uint32_t duration = 0;
        esp_ble_gap_start_scanning(duration);
        break;
    }
    case ESP_GAP_BLE_SCAN_START_COMPLETE_EVT:
        ESP_LOGI(DEMO_TAG, "Scan started");
        if ((err = param->scan_start_cmpl.status) != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(DEMO_TAG, "Scan start failed: %s", esp_err_to_name(err));
        }
        /* configure advertising when scanner has started */
        esp_ble_notification_t notification_adv_data;
        esp_err_t status = esp_ble_config_notification_data (&exposure_config, &notification_adv_data);
        if (status == ESP_OK){
            esp_ble_gap_config_adv_data_raw((uint8_t*)&notification_adv_data, sizeof(notification_adv_data));
        }
        else {
            ESP_LOGE(DEMO_TAG, "Fail to configure ble advertising: %s\n", esp_err_to_name(status));
        }
        break;
    case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
        //adv start complete event to indicate adv start successfully or failed
        if ((err = param->adv_start_cmpl.status) != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(DEMO_TAG, "Adv start failed: %s", esp_err_to_name(err));
        }
        break;
    case ESP_GAP_BLE_SCAN_RESULT_EVT: {
        esp_ble_gap_cb_param_t *scan_result = (esp_ble_gap_cb_param_t *)param;
        switch (scan_result->scan_rst.search_evt) {
        case ESP_GAP_SEARCH_INQ_RES_EVT:
            /* Search for BLE Notification Exposure Packet */
            if (esp_ble_is_notification_exposure(scan_result->scan_rst.ble_adv, scan_result->scan_rst.adv_data_len)){
                esp_ble_exposure_data_t *exposure_config = (esp_ble_exposure_data_t*)(scan_result->scan_rst.ble_adv);
                ESP_LOGI(DEMO_TAG, "-----Rolling Proximity Identifier Found-----");
                ESP_LOGI("RPI: Device address: ", MACSTR, MAC2STR(scan_result->scan_rst.bda));
                esp_log_buffer_hex("RPI: Proximity UUID:", exposure_config->rolling_identifier_uuid, ESP_UUID_LEN_128);

                //uint16_t major = ENDIAN_CHANGE_U16(ibeacon_data->ibeacon_vendor.major);
                //uint16_t minor = ENDIAN_CHANGE_U16(ibeacon_data->ibeacon_vendor.minor);
                //ESP_LOGI(DEMO_TAG, "Major: 0x%04x (%d)", major, major);
                //ESP_LOGI(DEMO_TAG, "Minor: 0x%04x (%d)", minor, minor);
                //ESP_LOGI(DEMO_TAG, "Measured power (RSSI at a 1m distance):%d dbm", ibeacon_data->ibeacon_vendor.measured_power);
                //ESP_LOGI(DEMO_TAG, "RSSI of packet:%d dbm", scan_result->scan_rst.rssi);
            }
            break;
        default:
            break;
        }
        break;
    }

    case ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT:
        if ((err = param->scan_stop_cmpl.status) != ESP_BT_STATUS_SUCCESS){
            ESP_LOGE(DEMO_TAG, "Scan stop failed: %s", esp_err_to_name(err));
        }
        else {
            ESP_LOGI(DEMO_TAG, "Stop scan successfully");
        }
        break;

    case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
        if ((err = param->adv_stop_cmpl.status) != ESP_BT_STATUS_SUCCESS){
            ESP_LOGE(DEMO_TAG, "Adv stop failed: %s", esp_err_to_name(err));
        }
        else {
            ESP_LOGI(DEMO_TAG, "Stop adv successfully");
        }
        break;

    default:
        break;
    }
}


void ble_exposure_appRegister(void)
{
    esp_err_t status;

    ESP_LOGI(DEMO_TAG, "register callback");

    //register the scan callback function to the gap module
    if ((status = esp_ble_gap_register_callback(esp_gap_cb)) != ESP_OK) {
        ESP_LOGE(DEMO_TAG, "gap register error: %s", esp_err_to_name(status));
        return;
    }

}

void ble_exposure_init(void)
{
    esp_bluedroid_init();
    esp_bluedroid_enable();
    ble_exposure_appRegister();
}

void timer_rpi_interval()
{
    ESP_LOGI(DEMO_TAG, "Generate new RPI");
}

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    esp_bt_controller_init(&bt_cfg);
    // issue, need to start, stop, restart ble
    // https://github.com/espressif/esp-idf/issues/6418
    esp_bt_controller_enable(ESP_BT_MODE_BLE);

    ble_exposure_init();

    esp_bd_addr_t myaddr;
    esp_fill_random(myaddr, 6);
    myaddr[0] &= 0x3F; // first 2 msb need to be 0
    esp_ble_gap_set_rand_addr(myaddr);
    ESP_LOGI("Generating random address: ", MACSTR, MAC2STR(myaddr));
    esp_ble_gap_config_local_privacy(true);

    /* configure scanner */
    esp_err_t scan_ret = esp_ble_gap_set_scan_params(&ble_scan_params);
    if (scan_ret){
        ESP_LOGE(DEMO_TAG, "set scan params error, error code = %x", scan_ret);
    }
    start_timer();

}
