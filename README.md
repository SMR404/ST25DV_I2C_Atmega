# ST25DV_I2C_Atmega
Basic Arduino library for I2C between an atmega MCU and ST25DV dynamic NFC tag.


In this library: We edit registers in user, system, dynamic, and FTM mailbox memory areas of the ST25DV over I2C. 
   
   This code was written by referencing the ST25DVxxK datasheet and was made to be as basic and low-level as possible.
   
   The sole motivation for writing this library was to be able to get a byte of data into the Fast Transfer Mode (FTM) mailbox buffer as quickly as possible from tag power-up (can be done in <2 mS).
   
   By default, ST25DV tags have certain static system config registers set so that FTM cannot be enabled without first changing them. To resolve this,
   included is ST25DV_firstTimeSetup() which, when TWI has been establsihed, will set the specific registers so that FTM can be enabled from then on, even after POR. This function only needs to be run once.
   
   This library has been tested with the Atmega328P (on an Arduino uno).
