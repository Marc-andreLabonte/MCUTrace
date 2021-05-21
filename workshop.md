

# Part 1: Quick protocol overview

Quick intro to Apple-Google exposure notification protocol

## Lexicon

- Public Health Authority (PHA).  Manage contact tracing to reduce disease damage and spread.  Users sends last 14 TEK to public health upon positive test.  Daily, public health prepares package with TEK associated with positive tests for the day to all users for key matching
- Temporary exposure keys (TEK).  Generated once a day on device.  Kept secret unless a used test positive and agrees to share with public health.  Once a TEK is known, one can compute RPI generated from it. 
- Rolling Proximity Identifiers (RPI).  Derived from TEK with some sort of hashing, sent over bluetooth.  Once 
- Key matching.  Happens locally on device.  User download TEK package from PHA, compute RPI and compare from RPI collected from bluetooth scanning.  If a match is found, exposure notification is generated.

## Bluetooth part

- Wireshark: Use 'btcommon.eir_ad.entry.uuid_16 == 0xfd6f' display filter 
- http://www.davidgyoungtech.com/2020/04/24/hacking-with-contact-tracing-beacons

### Google documents

- Old document: https://blog.google/documents/58/Contact_Tracing_-_Bluetooth_Specification_v1.1_RYGZbKW.pdf
- Current document: https://blog.google/documents/70/Exposure_Notification_-_Bluetooth_Specification_v1.2.2.pdf
- Cryptography specification: https://blog.google/documents/69/Exposure_Notification_-_Cryptography_Specification_v1.2.1.pdf
    - See page 5, Rolling Proximity Identifier generation from Temporary exposure keys

## Sending temporary exposure keys to PHA

https://developers.google.com/android/exposure-notifications/verification-system#flow-diagram

- Once sent to PHA upon positive diagnosis, temporary exposure keys will be refered as diagnosis keys (Bluetooth spec, page 3)

## Key matching

- Get zip file from public health
https://developers.google.com/android/exposure-notifications/exposure-key-file-format

- Compute RPI from TEK contained in zip file, match against RPI collected by bluetooth scanner

* Repository: https://github.com/google/exposure-notifications-internals

* File where matching magic happens: https://github.com/google/exposure-notifications-internals/blob/main/exposurenotification/src/main/cpp/matching_helper.cc

### Hardware requirement.

- Cell phones do have plenty of local resources (cpu, memory, storage) to handle key matching
- Much less capacity on MCU like ESP Vroom 32
- Still to be determined if sufficient.
- Possible to attach PSRAM chip on the SPI bus with the Vroom 32

* Need to uncompress possibly large zip files, needs ram
* Need to compute 144 RPI from each TEK, check against scanned RPI in storage

#### Quebec 

On our worst day, we had a bit over 3000 positive cases.  Assuming 100% enrollement (not realistic) 
we have 14*3000 TEK = 42000 TEK

#### Canada 

Worst day, jan 3rd 2021, 11383 cases
11383 * 14 = 159362 TEK

If TEK from all of Canada's positive cases are sent to all users, a single zip file will do.  Probably need to allow for inter-provincial contact tracing.

# Part 2: Setting up the bluetooth sniffer

## Install Wireshark if not already done

- On Linux, use your favourite package manager
- On Windows, get it from https://www.wireshark.org/

### On Ubuntu, you need a PPA to get a current version of Wireshark

Wireshark 3.4 is required to parse Exposure Notification received in bluetooth

```
sudo add-apt-repository ppa:wireshark-dev/stable
sudo apt update
sudo apt install wireshark
```

## Update group membership

Make sure you are in the wireshark and dialout groups

```
sudo usermod $user -a -G dialout,wireshark
```

As $user, give yourself access to these groups without logging off, then check with the `id` command

```
su user
```

## Install dependencies

```
sudo apt install python3-serial screen 
```

Download nRF sniffer Wireshark plugin from Nordic Semiconductor website

https://www.nordicsemi.com/Software-and-Tools/Development-Tools/nRF-Sniffer-for-Bluetooth-LE/Download

Follow instructions here to install the plugin.  On Linux, plugin folder should be `~/.config/wireshark/extcap/` 

https://infocenter.nordicsemi.com/index.jsp?topic=%2Fug_sniffer_ble%2FUG%2Fsniffer_ble%2Finstalling_sniffer_plugin.html

Test by launching Wireshark, an interface named "nRF sniffer for bluetooth LE" should appear.  You should see bluetooth traffic when capturing from it. 

To filter exposure notification traffic, use the following display filter

```
btcommon.eir_ad.entry.uuid_16 == 0xfd6f
```
    
# Part 3: Walkthrough the code

- Structures in esp_exposure_api.h and exposure_api.c
    - exposure_common_head is the bluetooth header, static
    - exposure_config contains RPI and AEM, needs to be updated every 10 minutes
    
- Heart of the program in exposure_notification.c
    - app_main is the entry point
    - timer_rpi_interval called by timer, handle rolling of timer_rpi_interval
    - esp_gap_cb receive events from bluetooth scanner and advertiser
    
- Timing in exposure_timer.c
    - borrowed code
    - only one timer is being used
    - set to 10 seconds instead of 10 minutes for demonstration purposes
    - timer would call timer_rpi_interval to trigger rolling of Proximity Identifiers

- Cryptography in exposure_crypto.c
    - relies on mbedtls, call functions as prescribed by specification
    - Google's design: https://github.com/google/exposure-notifications-internals/blob/main/CRYPTO.md


# Part 4: Building and running it

- Install esp32 toolchain
	- https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started

- Under esp-idf directory run `. ./export.sh`

- configure project

idf.py set-target esp32
idf.py menuconfig

**IMPORTANT** enable mbedtls_hkdf in menuconfig (commited to sdkconfig file)
    -    https://github.com/espressif/esp-idf/issues/5255
    

- `idf.py build`

- idf.py -p /dev/ttyUSB2 flash monitor

# Part 5: Conclusion 

## Many parts missing

Far from being complete.  Code mostly written as an experiment but could probably reuse a lot of existing Google code with compatible license.

## Privacy issues

Why i would create my own dongle instead of using what is there

### Telemetry

Personally not confortable with telemetry being collected by involved parties.

- Telemetry collected: https://developers.google.com/android/exposure-notifications/telemetry-design
- More data collected by official apps: https://www.canada.ca/en/public-health/services/diseases/coronavirus-disease-covid-19/covid-alert/privacy-policy/assessment.html#a8


- potentially hostile key server, can be rigged against recommendations to weaken anonymity of users.  Threat actor would be hostile public health autority or hostile governement.                             
Ref: https://developer.apple.com/documentation/exposurenotification/setting_up_a_key_server

Old specification would derive TEK from a device key.
- FIXED by updated spec? same version number v1.1 so i am confused - device key from which daily tracing keys (which becomes TEK when diagnosed) are derived.  Thread actor would be technology companies (Google, Apple), Application developpers.  Can a contact tracing app uncover device key.
- See old document: https://blog.google/documents/58/Contact_Tracing_-_Bluetooth_Specification_v1.1_RYGZbKW.pdf
Mitigate by generating daily keys randomly, delete daily keys older than 14 days.

Cellphones gather lots of data for many other purposes.  
