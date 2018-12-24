/*
 * ST25DV_I2C_Atmega.h - Library for interfacing Atmega MCU with ST25DV dynamic nfc tag.
 * Created by Simon R. (SMR404) - 12/23/2018
 * Released into the public domain
*/

#ifndef ST25DV_I2C_Atmega_h
#define ST25DV_I2C_Atmega_h

#include "Arduino.h"
#include "Wire.h"

class ST25DV_I2C_Atmega
{
    public:
        //these are constant values for ST25DV_register_read and write function arguments that specify what type of register is being operated on
        #define dynamicMem_ftmMem_reg 0
        #define userMem_reg 1
        #define sysMem_reg 2
        
        //functions
        ST25DV_I2C_Atmega();
        int ST25DV_register_write(uint8_t * dataBytes, uint16_t memAddress, uint8_t numberOfBytes, uint8_t registerType);
        int ST25DV_register_read(uint8_t * dataBytes, uint16_t memAddress, uint8_t numberOfBytes, uint8_t registerType);
        int ST25DV_enableFTM();
        int ST25DV_presentPassword(uint8_t * password_64bits);
        int ST25DV_firstTimeSetup();
    private:
};

#endif
    
