# ST25DV_I2C_Atmega
Basic Arduino library for I2C between an Atmega MCU and ST25DV dynamic NFC tag.
---------------------------------------



This library: Allows editing of registers in user, system, dynamic, and FTM mailbox memory areas of the ST25DV over I2C. 
   
This code was written by referencing the ST25DVxxK datasheet and was made to be as basic and low-level as possible.
     
   By default, ST25DV tags have certain static system config registers set so that FTM cannot be enabled without first changing them. To resolve this,
   included is ST25DV_firstTimeSetup() which, when I2C has been establsihed, will set the specific registers so that FTM can be enabled from then on, even after POR. This function only needs to be run once.
   
   This library has been tested with the Atmega328P (on an Arduino uno).
   
   
The sole motivation for writing this library was to be able to get a byte of data into the Fast Transfer Mode (FTM) mailbox buffer as quickly as possible from tag power-up (can be done in <2 mS).

For reference, 3.3V must be applied to the ST245DV's VCC pin for around 700 uSeconds before I2C with it can successfully be established. The process for actually establishing I2C, enabling FTM, and writing a byte to FTM mailbox (and also verifying it) takes an additional ~764 uSeconds. This particular scenario can be tested in the write_FTM_byte.ino example. This makes for a total time of ~1.5 mS from power up to when the tag has data in the FTM buffer.
