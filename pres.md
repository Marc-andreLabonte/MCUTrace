

# Quick protocol overview

## Lexicon

- Public Health Authority (PHA).  Manage contact tracing to reduce disease damage and spread.  Users sends last 14 TEK to public health upon positive test.  Daily, public health prepares package with TEK associated with positive tests for the day to all users for key matching
- Temporary exposure keys (TEK).  Generated once a day on device.  Kept secret unless a used test positive and agrees to share with public health.  Once a TEK is known, one can compute RPI generated from it. 
- Rolling Proximity Identifiers (RPI).  Derived from TEK with some sort of hashing, sent over bluetooth.  Once 
- Key matching.  Happens locally on device.  User download TEK package from PHA, compute RPI and compare from RPI collected from bluetooth scanning.  If a match is found, exposure notification is generated.

## Bluetooth part

- Wireshark
- http://www.davidgyoungtech.com/2020/04/24/hacking-with-contact-tracing-beacons

## Sending temporary exposure keys to PHA

https://developers.google.com/android/exposure-notifications/verification-system#flow-diagram

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


## Now lets look at the code
