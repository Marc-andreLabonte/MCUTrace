/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/



/****************************************************************************
*
* This file is for bluetooth notification exposure advertisement definitions
*
****************************************************************************/

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"

/* Major and Minor part are stored in big endian mode in iBeacon packet,
 * need to use this macro to transfer while creating or processing
 * iBeacon data */
#define ENDIAN_CHANGE_U16(x) ((((x)&0xFF00)>>8) + (((x)&0xFF)<<8))

/* static values to test if bluetooth adv are transmitted and can be received */
#define TEST_UUID    {0xFD, 0xA5, 0x06, 0x93, 0xA4, 0xE2, 0x4F, 0xB1, 0xAF, 0xCF, 0xC6, 0xEB, 0x07, 0x64, 0x78, 0x25}
#define TEST_METADATA    {0xCA, 0xFE, 0xBA, 0xBE}
#define EXPOSURE_NOTIFICATION_UUID 0xFD6F

/* About __attribute__((packed))
https://stackoverflow.com/questions/11770451/what-is-the-meaning-of-attribute-packed-aligned4
packed means no padding?
*/

typedef struct {
    uint8_t flags[3];
    uint8_t length;
    uint8_t type;
    uint16_t serviceUUID;
}__attribute__((packed)) esp_ble_exposure_head_t;

typedef struct {
    uint8_t length;
    uint8_t type;
    uint16_t serviceUUID;
    uint8_t rolling_identifier_uuid[16];
    uint8_t metadata[4];
}__attribute__((packed)) esp_ble_exposure_data_t;


typedef struct {
    esp_ble_exposure_head_t exposure_head;
    esp_ble_exposure_data_t exposure_data;
}__attribute__((packed)) esp_ble_notification_t;


extern esp_ble_exposure_head_t exposure_common_head;

bool esp_ble_is_notification_exposure (uint8_t *adv_data, uint8_t adv_data_len);

esp_err_t esp_ble_config_notification_data (esp_ble_exposure_data_t *exposure_config, esp_ble_notification_t *notification_adv_data);
