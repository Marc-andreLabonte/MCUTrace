

# Part 1: Quick protocol overview

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
- Cryptography specification: https://blog.google/documents/70/Exposure_Notification_-_Cryptography_Specification_v1.2.2.pdf
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

* Need to uncompress possibly large zip files
* Need to compute 144 RPI from each TEK, check against scanned RPI in storage

#### Quebec 

On our worst day, we had a bit over 3000 positive cases.  Assuming 100% enrollement (not realistic) 
we have 14*3000 TEK = 42000 TEK

#### Canada 

Worst day, jan 3rd 2021, 11383 cases
11383 * 14 = 159362 TEK

If TEK from all of Canada's positive cases are sent to all users, a single zip file will do.  Probably need to allow for inter-provincial contact tracing.

# Part 2: Setting up the bluetooth sniffer

Install Wireshark if not already done

- On Linux, use your favourite package manager
- On Windows, get it from https://www.wireshark.org/

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

- Structures in esp_exposure_api.h
- Heart of the program in exposure_notification.c
- Timing in exposure_timer.c
- Cryptography in exposure_crypto.c
    - Google's design: https://github.com/google/exposure-notifications-internals/blob/main/CRYPTO.md


# Part 4: Building and running it

- Under esp-idf directory run `. ./export.sh`

- `idf.py build`

- idf.py -p /dev/ttyUSB2 flash monitor

# Part 5: Conclusion 


## Privacy issues

- Telemetry collected: https://developers.google.com/android/exposure-notifications/telemetry-design
- potentially hostile key server, can be rigged against recommendations to weaken anonymity of users.  Threat actor would be hostile public health autority or hostile governement.                             
Ref: https://developer.apple.com/documentation/exposurenotification/setting_up_a_key_server
- FIXED by updated spec? same version number v1.1 so i am confused - device key from which daily tracing keys (which becomes TEK when diagnosed) are derived.  Thread actor would be technology companies (Google, Apple), Application developpers.  Can a contact tracing app uncover device key.  Mitigate by generating daily keys randomly, delete daily keys older than 14 days.
- More data collected by official apps: https://www.canada.ca/en/public-health/services/diseases/coronavirus-disease-covid-19/covid-alert/privacy-policy/assessment.html#a8

