# Automated contact tracing experiment on ESP Vroom32

## Abstract

This workshop aim to teach practical knowledge of automated contact tracing protocols by implementing the Apple-Google one for Covid19 on a ESP Vroom 32 MCU.   A Bluefruit LE sniffer will also be used to observe advertisements sent by devices using the Apple-Google exposure notification protocol.

## Description

Workshop should go as follows:

Part 1: Quick review on the Apple-Google exposure notification protocol, split into 3 main parts 

1- Broadcast of rolling proximity identifiers over Bluetooth LE and scanning for such identifiers transmitted by nearby devices.

2- Transmission of temporary exposure keys, from which rolling proximity indentifiers are generated, to public health authorities upon diagnosis.

3- Key matching protocol occurring on device to determine if the owner was in close proximity to another user who then tested positive, triggering the notification.

Part 2: Setting up and test the Bluefruit LE sniffer

Part 3: Walk-through  of the Bluetooth portion of the protocol code that is to be compiled and flashed on the ESP Vroom 32.  That covers the scanning code, the advertising code and critical data structures involved.

Part 4: Build, compile and flash the ESP Vroom32.  Run the Bluefruit sniffer to see rolling proximity identifiers being transmitted.  Play with timeouts to see identifiers being rotated. 

Part 5: Conclusion

## Notes

Require Bluefruit bluetooth sniffer:
https://www.mouser.ca/ProductDetail/Adafruit/2269?qs=%2Fha2pyFaduh2NF1zdLdGfiVxSmZPWTDS2cYtyuIjclY%3D
Require ESP Vroom 32 development board: https://www.amazon.ca/KeeYees-Development-Bluetooth-Microcontroller-ESP-WROOM-32/dp/B07PP1R8YK


## How to prepare for the workshop

Review of the Google and Apple Documents, procure ESP Vroom 32 and Bluefruit LE sniffer, setup the ESP IDF tool chain.

Setting up the ESP IDF tool chain: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/

Google and Apple Documents: 

- [https://blog.google/documents/58/Contact_Tracing_-_Bluetooth_Specification_v1.1_RYGZbKW.pdf](https://blog.google/documents/58/Contact_Tracing_-_Bluetooth_Specification_v1.1_RYGZbKW.pdf)
- [https://www.blog.google/documents/62/Exposure_Notification_-_Bluetooth_Specification_v1.1.pdf](https://www.blog.google/documents/62/Exposure_Notification_-_Bluetooth_Specification_v1.1.pdf)
- [https://developers.google.com/android/exposure-notifications/verification-system](https://developers.google.com/android/exposure-notifications/verification-system)
- [https://www.blog.google/documents/60/Exposure_Notification_-_Cryptography_Specification_v1.1.pdf](https://www.blog.google/documents/60/Exposure_Notification_-_Cryptography_Specification_v1.1.pdf)
- [https://github.com/google/exposure-notifications-internals](https://github.com/google/exposure-notifications-internals)
- [https://developer.apple.com/documentation/exposurenotification/setting_up_a_key_server](https://developer.apple.com/documentation/exposurenotification/setting_up_a_key_server)

## Physical equipment required

- Bluefruit bluetooth sniffer, e.g. [this one from Mouser](https://www.mouser.ca/ProductDetail/Adafruit/2269?qs=%2Fha2pyFaduh2NF1zdLdGfiVxSmZPWTDS2cYtyuIjclY%3D)
- ESP Vroom 32 development board, e.g. [this one from Amazon](https://www.amazon.ca/KeeYees-Development-Bluetooth-Microcontroller-ESP-WROOM-32/dp/B07PP1R8YK)
