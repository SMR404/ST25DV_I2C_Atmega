/*
 * firstTimeSetup.ino - This sketch is used to program the ST25DV's static system registers to allow for Fast Transfer Mode (FTM) to be enabled immediately afterward. 
 * This change will persist across power cycles, as long as the specific MB_MODE static system register is not reset with an explicit write in the future.
 * This sketch assumes a new ST25DV chip is being used, and that its I2C security password is factory default.
 * This sketch only needs to be run once. It should not be run at every device power-on because it is slow, and it would be unnecessary.
 * 
 * Make sure the ST25DV has a steady power source at 3.3V or 5V before running sketch.
 * 
 * Created by Simon R. (SMR404) - 12/23/2018
 * Released into the public domain
*/

#include <ST25DV_I2C_Atmega.h>
ST25DV_I2C_Atmega st25DV;

void setup() {
  Serial.begin(19200);

  Wire.begin();
  Wire.setClock(400000); //400khz I2C clock frequency, which is  the maximum speed for the atmega328

  if (st25DV.ST25DV_firstTimeSetup())    //attempt setup and report success or failure
  {
    Serial.println("Successfully wrote to static sys registers, setup complete");
  }
  else
  {
    Serial.println("Failure");
  }
}

void loop() 
{
  
}
