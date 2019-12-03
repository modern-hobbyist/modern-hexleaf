/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#include <Arduino.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include "Nanohex.h"
#include "credentials.h"
#include "fauxmoESP.h"

fauxmoESP fauxmo;

#define SERIAL_BAUDRATE     115200

#define HEXLEAF  "HexLeaf"

bool party;
bool breathing;
bool power = false;
bool alexaToggle = false;
bool alexaBrightness = false;
int brightness = 128;

Nanohex *hexController;

void wifiSetup() {

    // Set WIFI module to STA mode
    WiFi.mode(WIFI_STA);

    // Connect
    Serial.printf("[WIFI] Connecting to %s ", ssid);
    WiFi.begin(ssid, pass);

    // Wait
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(100);
    }
    Serial.println();

    // Connected!
    Serial.printf("[WIFI] STATION Mode, SSID: %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());

}

void setup()
{
  // Debug console
  Serial.begin(SERIAL_BAUDRATE);

  wifiSetup();

  hexController = new Nanohex();

  Blynk.begin(auth, ssid, pass);
  
  blynkPowerOff();
  
  fauxmo.createServer(true); // not needed, this is the default value
  fauxmo.setPort(80); // This is required for gen3 devices

  fauxmo.enable(true);

  fauxmo.addDevice(HEXLEAF);

  fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state, unsigned char value) {
      //Get out of this function as quickly as possible
      Serial.printf("[MAIN] Device #%d (%s) state: %s value: %d\n", device_id, device_name, state ? "ON" : "OFF", value);

      if (strcmp(device_name, HEXLEAF)==0) {
          if(!power && state || power && !state){
            alexaToggle = true;
          }

          if(power && value != brightness){
            alexaBrightness = true;
            brightness = value;
          }
      }
  });
}

void loop()
{
  Blynk.run();
  fauxmo.handle();
  
  if(power){
    if(party){
//      Serial.println("PARTY MODE!");
    }else if(breathing){
//      Serial.println("Breathing mode.");
    }else{
//      Serial.println("Steady Mode.");
    }
    
  }else{

  }

  if(alexaToggle){
    if(power){
      blynkPowerDown();
      power = false;
    }else{
      blynkPowerOn();
      power = true;
    }
    alexaToggle = false;
  }

  if(alexaBrightness){
    Blynk.virtualWrite(V4, brightness);
    alexaBrightness = false;
  }

  hexController->update();
}

blynkPowerOff(){
  Blynk.virtualWrite(V0, 0);
  Blynk.virtualWrite(V1, 0);
  Blynk.virtualWrite(V2, 0);
}

blynkPowerOn(){
  Blynk.virtualWrite(V0, 0);
  Blynk.virtualWrite(V1, 0);
  Blynk.virtualWrite(V2, 1);
}

BLYNK_WRITE(V0)
{
  party = param.asInt();
 
  if(power && party){
    breathing = false;
    Blynk.virtualWrite(V1, 0);
    Serial.println("Party");
  }
}

BLYNK_WRITE(V1)
{
  breathing = param.asInt();
  
  if(power && breathing){
    party = false;
    Blynk.virtualWrite(V0, 0);
    Serial.println("Breathing");
  }
}

BLYNK_WRITE(V2)
{
  power = param.asInt();
  Serial.println(power);
  if(!power){
    party = false;
    breathing = false;
    Blynk.virtualWrite(V0, 0);
    Blynk.virtualWrite(V1, 0);
    Serial.println("Power Off");
  }else{
     Serial.println("Power On");
  }
  fauxmo.setState(HEXLEAF, power, brightness);
}

BLYNK_WRITE(V3)  
{
    //Don't need to update alexa since she can't set the color
    int r = param[0].asInt();
    int g = param[1].asInt();
    int b = param[2].asInt();
    hexController->set_primary(CRGB(r,g,b));
    Serial.printf("Primary: (%d, %d, %d) \n", r, g, b);
}

BLYNK_WRITE(V4)  
{
    brightness = param.asInt();
    Serial.printf("Brightness: %d \n", brightness);
    fauxmo.setState(HEXLEAF, power, brightness);
}
