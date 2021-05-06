# Notification exposure on ESP Vroom32

**Disclaimer**

The code is NOT functional yet and currently only for educational purposes.  If you wish to join automated contact tracing effort, use the official app [https://www.canada.ca/en/public-health/services/diseases/coronavirus-disease-covid-19/covid-alert.html](https://www.canada.ca/en/public-health/services/diseases/coronavirus-disease-covid-19/covid-alert.html)

## Project goals and motivation

Implement Apple-Google exposure notification protocol on a small bluetooth enabled dongle instead of a smart phone.

Although the exposure notification protocol takes good care in protecting user privacy, smartphones do gather lots of data for other purposes which make some people not confortable with carrying them at all.

A remotly exploitable flaw in the bluetooth stack can compromise an entire smartphone with all the enclosed data: [https://threatpost.com/critical-android-bluetooth-bug-enables-rce-no-user-interaction-needed/152699/](https://threatpost.com/critical-android-bluetooth-bug-enables-rce-no-user-interaction-needed/152699/).  Using a separate dongle exposes only the temporary exposure keys generated and the rolling proximity identifiers collected during the last 14 days.

At last, building it yourself is a good way to really understand something.

## Status

- BLE scanning can receive rolling proximity identifiers from nearby smartphones which are running the official app.

- Advertising works but the code does **not** generate rolling proximity identifiers according to specification yet.

- Upload of temporary exposure keys to public health autority **not yet implemented**

- Key matching to determine exposure **not yet implemented**

## Prerequisites 


### ESP32 toolchain

Follow instructions on Expressif website.

https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html#installation-step-by-step

It is suggested to test with the hello world example, see the "Start a project" step

https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html#step-5-start-a-project

### Bluefruit BLE sniffer

Install Wireshark if not already done

- On Linux, use your favourite package manager
- On Windows, get it from https://www.wireshark.org/

Download nRF sniffer Wireshark plugin from Nordic Semiconductor website

https://www.nordicsemi.com/Software-and-Tools/Development-Tools/nRF-Sniffer-for-Bluetooth-LE/Download

Follow instructions here to install the plugin

https://infocenter.nordicsemi.com/index.jsp?topic=%2Fug_sniffer_ble%2FUG%2Fsniffer_ble%2Finstalling_sniffer_plugin.html

Test by launching Wireshark, an interface named "nRF sniffer for bluetooth LE" should appear.  You should see bluetooth traffic when capturing from it. 

To filter exposure notification traffic, use the following display filter

```
btcommon.eir_ad.entry.uuid_16 == 0xfd6f
```

## NSEC 2021 Workshop

See workshop.md

 
