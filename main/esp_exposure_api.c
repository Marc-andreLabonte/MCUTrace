/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/



/****************************************************************************
*
* This file is for Exposure notification API
* 
* https://www.blog.google/documents/62/Exposure_Notification_-_Bluetooth_Specification_v1.1.pdf
*
****************************************************************************/

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#include "esp_gap_ble_api.h"
#include "esp_exposure_api.h"



const uint8_t uuid_zeros[ESP_UUID_LEN_128] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/* For Notification exposure format refer to google contact tracing specification: 
https://blog.google/documents/58/Contact_Tracing_-_Bluetooth_Specification_v1.1_RYGZbKW.pdf
http://www.davidgyoungtech.com/2020/04/24/hacking-with-contact-tracing-beacons
https://developers.google.com/android/exposure-notifications/verification-system
https://www.blog.google/documents/60/Exposure_Notification_-_Cryptography_Specification_v1.1.pdf
*/
/* Exposure notification service header */
esp_ble_exposure_head_t exposure_common_head = {
    .flags = {0x02, 0x01, 0x1A},
    .length = 0x03,
    .type = 0x03,
    .serviceUUID = EXPOSURE_NOTIFICATION_UUID
};

/* Exposure notification service data*/
esp_ble_exposure_data_t exposure_config = {
    .length = 0x17,
    .type = 0x16,
    .serviceUUID = EXPOSURE_NOTIFICATION_UUID,
    .rolling_identifier_uuid = TEST_UUID,
    .metadata = TEST_METADATA
};

/* 
Called by scanner to check if adv received is exposure notification type
*/

bool esp_ble_is_notification_exposure (uint8_t *adv_data, uint8_t adv_data_len){
    bool result = false;

    if ((adv_data != NULL) && (adv_data_len == 0x1F)){
        if (!memcmp(adv_data, (uint8_t*)&exposure_common_head, sizeof(exposure_common_head))){
            result = true;
        }
    }

    return result;
}

esp_err_t esp_ble_config_notification_data (esp_ble_exposure_data_t *exposure_config, esp_ble_notification_t *notification_adv_data){
    if ((exposure_config == NULL) || (notification_adv_data == NULL) || (!memcmp(exposure_config->rolling_identifier_uuid, uuid_zeros, sizeof(uuid_zeros)))){
        return ESP_ERR_INVALID_ARG;
    }

    memcpy(&notification_adv_data->exposure_head, &exposure_common_head, sizeof(esp_ble_exposure_head_t));
    memcpy(&notification_adv_data->exposure_data, exposure_config, sizeof(esp_ble_exposure_data_t));

    return ESP_OK;
}
