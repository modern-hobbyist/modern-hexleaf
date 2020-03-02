#include <Wire.h>

int devices[] = {4, 5};
#define NUMDEVS  2

void setup()
{
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start Serial for output
}

void loop()
{
  for(int i = 0; i<NUMDEVS; i++){
    Wire.requestFrom(devices[i], 6);    

    while(Wire.available())    
    { 
      char c = Wire.read(); 
      Serial.print(c);        
    }
  
    delay(500);
  }
  delay(500);
}
