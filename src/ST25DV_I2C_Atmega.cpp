/*
 * ST25DV_I2C_Atmega.cpp - Library for interfacing Atmega MCU with ST25DV dynamic nfc tag.
 * Created by Simon R. (SMR404) - 12/23/2018
 * Released into the public domain
*/

#include "ST25DV_I2C_Atmega.h"

ST25DV_I2C_Atmega::ST25DV_I2C_Atmega()
{
} 



/*
    @description  Write the contents of 1 or more registers (registers may be in memory areas of dynamic, FTM mailbox, user, or system)
    @parameter    dataBytes : pointer of the array from which the contents to be written is copied from (make sure it can accomodate numberOfBytes)
    @parameter    memAddress : 16 bit address of 1st register to write to (subsequent register addresses are auto-incremented)
    @parameter    numberOfBytes : how many bytes (registers) to be written
    @parameter    registerType : specifies the type of register being operated on (range of values: 0-2). Value of 0: dynamic or FTM mailbox registers.
                                 Value of 1: User memory registers. Value of 2: System memory registers. Values outside 0-2 do nothing.
    @retval       wireStatus : Describes the outcome of TWI transmission as described by Wire.h documentation
*/
int ST25DV_I2C_Atmega::ST25DV_register_write(uint8_t * dataBytes, uint16_t memAddress, uint8_t numberOfBytes, uint8_t registerType)
{
    if (registerType > 2) //if an invalid register type is entered
  {
    return 1;  //do nothing and complain about it
  }

  uint8_t deviceSelectCode;   //a leading command code that is submitted to ST25DV to specify the operation that is to follow

  if (registerType < 2) //if the register type is not system memory
  {
    deviceSelectCode = (0b10100110 >> 1);  //this value represents the device select code for a register write in the user memory, dynamic register, or mailbox memory
  }
  else
  {
    deviceSelectCode = (0b10101110 >> 1);  //this value represents the device select code for a system register or password present command
  }

  Wire.beginTransmission(deviceSelectCode); //send the device select code to specify the coming operation
  Wire.write(memAddress >> 8);   //send the first half of the 16 bit address
  Wire.write(memAddress & 0xFF);   //send the second half of the 16 bit address
  Wire.write(dataBytes, numberOfBytes);     //we want to now send the byte of data to be stored
  int wireStatus = Wire.endTransmission(1);  //actually send the data and end the tranmssion. Also, store the output from endTransmission to see if everything worked well

  //Note: a register write in EEPROM (in user or system memory) of 1, 2, 3, or 4 bytes will take 5mS to complete. 5, 6, 7, or 8 bytes will take 10 mS, etc.
  if (registerType > 0) //if operating on a system or user memory register, time is needed for it to complete EEPROM write because the ST25DV is unresponsive during
  {
    int wait = 5 * (numberOfBytes / 4);   //5 comes from 5 mS, 4 comes from 4 bytes in an EEPROM block
    if ((numberOfBytes % 4) != 0)         //if writing to a number of bytes not a multiple of 4, add another 5 mS to correct for above integer division truncation
    {
      wait = wait + 5;
    }

    delay(wait); //wait for all the EEPROM writing to complete before moving on
  }
  return wireStatus; //any TWI error handling is to be done outside this function
}





/*
    @description  Read the contents of 1 or more registers (registers may be in memory areas of dynamic, FTM mailbox, user, or system)
    @parameter    dataBytes : pointer of the array from which the contents to be read is copied from (make sure it can accomodate numberOfBytes)
    @parameter    memAddress : 16 bit address of 1st register to read from (subsequent register addresses are auto-incremented)
    @parameter    numberOfBytes : how many bytes (registers) to be read
    @parameter    registerType : specifies the type of register being operated on (range of values: 0-2). Value of 0: dynamic or FTM mailbox registers.
                                 Value of 1: User memory registers. Value of 2: System memory registers. Values outside 0-2 do nothing.
    @retval       wireStatus : Describes the outcome of TWI transmission as described by Wire.h documentation
*/
int ST25DV_I2C_Atmega::ST25DV_register_read(uint8_t * dataBytes, uint16_t memAddress, uint8_t numberOfBytes, uint8_t registerType)
{
  if (registerType > 2) //if an invalid register type is entered
  {
    return 1;  //do nothing and complain about it
  }

  uint8_t deviceSelectCode;   //a leading command code that is submitted to ST25DV to specify the operation that is to follow

  if (registerType < 2) //if the register type is not system memory
  {
    deviceSelectCode = (0b10100110 >> 1);  //this value represents the device select code for a register write in the user memory, dynamic register, or mailbox memory
  }
  else
  {
    deviceSelectCode = (0b10101110 >> 1);  //this value represents the device select code for a system register or password present command
  }

  Wire.beginTransmission(deviceSelectCode); //send the device select code to specify the coming operation
  Wire.write(memAddress >> 8);   //send the first half of the 16 bit address
  Wire.write(memAddress & 0xFF);   //send the second half of the 16 bit address
  int wireStatus = Wire.endTransmission(1); //as per the data sheet's instructions, we stop a transmission early and start over later
  if (wireStatus > 0) //if endTransmission returned something other than 0, something went wrong
  {
    return wireStatus; //any TWI error handling is to be done outside this function
  }
  //begin the request again
  Wire.requestFrom(deviceSelectCode, numberOfBytes);

  int i = 0;
  while (Wire.available() && (i < numberOfBytes))  //if data is available on the wire
  {
    dataBytes[i] = Wire.read(); //get the data
    i++;
  }
  wireStatus = Wire.endTransmission(1);   //stop the transmission

  return wireStatus; //any TWI error handling is to be done outside this function
}





/*
    @description  enables Fast Transfer Mode once TWI is up
    @retval       returns an int value of 1 for success and 0 for failure
*/
int ST25DV_I2C_Atmega::ST25DV_enableFTM()
{
  uint8_t dataBytes[1];
  uint16_t MB_CTRL_Dyn = 0x2006; //address of MB_CTRL_Dyn register that enables/disables FTM mode
  uint8_t numberOfBytes = 1;  //only 1 byte is written
  
  dataBytes[0] = 0b00000001;  //flip bit 0 to 1
  ST25DV_register_write(dataBytes, MB_CTRL_Dyn, numberOfBytes, dynamicMem_ftmMem_reg); //enable FTM mode by writing the new value to the register
  
  dataBytes[0] = 0;
  
  //check to make sure the register has the correct value
  ST25DV_register_read(dataBytes, MB_CTRL_Dyn, numberOfBytes, dynamicMem_ftmMem_reg);
  
  //format the read byte to isolate bit 0
  dataBytes[0] = dataBytes[0] & 0b00000001;

  if (dataBytes[0] == 0b00000001)
  {
    return 1; //success code
  }
  else
  {
    return 0; //failure code
  }
}





/*
    @description  Present the I2C password to attempt to open an I2C security session. Does not indicate if security session was successfully opened.
    @parameter    password_64bits : the 64 bit I2C password

*/
int ST25DV_I2C_Atmega::ST25DV_presentPassword(uint8_t * password_64bits)
{
  const uint8_t deviceSelectCode = (0b10101110 >> 1);  //this value represents the device select code for a system register or password present command
  Wire.beginTransmission(deviceSelectCode); //send the device select code to specify the coming operation
  Wire.write(0x09); //send the first half of the 16 bit I2C password adress
  Wire.write(0x00); //send the second half of the 16 bit I2C password adress
  Wire.write(password_64bits, 8); //send the 8 byte, 64 bit password
  Wire.write(0x09); //send the validation byte
  Wire.write(password_64bits, 8); //resend the 8 byte, 64 bit password
  int wireStatus = Wire.endTransmission(1);    //end the transmission

  return wireStatus; //any TWI error handling is to be done outside this function
}






/*
    @description  Very first time setup of new ST25DV tag. Default factoy password is presented so that system config registers can be changed to allow FTM to be enabled
    @retval       return value of 1 indicates a successful setup. Return value of 0 indicates a generic failure (could be due to TWI issues, among others)
*/
int ST25DV_I2C_Atmega::ST25DV_firstTimeSetup()
{
  uint8_t password_64bits[8]; //this is first time setup, so the 64bit password is 0000000000000000h.
  int i;
  for (i = 0; i < 8; i++)
  {
    password_64bits[i] = 0;
  }

  ST25DV_presentPassword(password_64bits);    //present the default I2C password

  uint8_t dataBytes[1];
  
  //check if the password worked by reading the I2C_SSO_Dyn register to see if the I2C security session is open
  ST25DV_register_read(dataBytes, 0x2004, 1, dynamicMem_ftmMem_reg);  //this is reading a dynamic register  
  //format the read byte to isolate bit 0
  dataBytes[0] = dataBytes[0] & 0b00000001;
  if (dataBytes[0] != 0b00000001)  //if the contents of 0x2004 was not 1, something went wrong so return failure code 0
  {
    return 0;
  }

  //change the system config bits so that FTM can be turned off/on
  dataBytes[0] = 0b00000001;
  ST25DV_register_write(dataBytes, 0x000D, 1, sysMem_reg); //write byte to the static system register (MB_MODE)
  dataBytes[0] = 0;
  ST25DV_register_read(dataBytes, 0x000D, 1, sysMem_reg);  //read byte from system register

  //format the read byte to isolate bit 0
  dataBytes[0] = dataBytes[0] & 0b00000001;
  
  if (dataBytes[0] != 0b00000001)
  {
    return 0; //if the contents of 0x000D was not 255, something went wrong so return failure code 0
  }

  return 1; //if everything worked, return success code of 1
}
