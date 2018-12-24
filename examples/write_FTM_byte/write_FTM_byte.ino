/*
   write_FTM_byte.ino - This sketch is meant to be ran after the ST25DV has had it's static system register configured so that FTM can be enabled.
   This sketch enables FTM and writes a single byte into the FTM mailbox and then reads it to check that it was written correctly.

   Make sure the ST25DV has a steady power source at 3.3V or 5V before running sketch.

   Created by Simon R. (SMR404) - 12/23/2018
   Released into the public domain
*/

#include <ST25DV_I2C_Atmega.h>

ST25DV_I2C_Atmega st25DV;

void setup() {
  Serial.begin(19200);

  int time1 = micros();   //timing is included to report the speed of the FTM enable and write process
  int time2;
  
  Wire.begin();
  Wire.setClock(400000); //400khz I2C clock frequency, which is  the maximum speed for the atmega328

  if (st25DV.ST25DV_enableFTM())    //enableFTM returns 1 if successful
  {
    uint8_t dataBytes[1];
    uint16_t memAddress = 0x2008;   //0x2008 is the start of the FTM I2C memory addresses, it must be the first to be written to every time
    uint8_t numberOfBytes = 1;

    dataBytes[0] = 0b00011010; //arbitrary test value

    st25DV.ST25DV_register_write(dataBytes, memAddress, numberOfBytes, 0); //write the value into the FTM register
    dataBytes[0] = 0;
    st25DV.ST25DV_register_read(dataBytes, memAddress, numberOfBytes, dynamicMem_ftmMem_reg);  //read the value from the FTM register
    if (dataBytes[0] == 0b00011010) //the values should be same
    {
      time2 = micros();
      Serial.println("Successfully wrote to FTM");
      Serial.print("Elapsed Time (uS): ");
      Serial.print(time2 - time1);
    }
    else  //if the values aren't the same, something went wrong
    {
      Serial.println("Failed to verify write to FTM");
    }
  }
  else
  {
    Serial.println("Failed to verify that FTM is enabled");
  }
}

void loop()
{

}
