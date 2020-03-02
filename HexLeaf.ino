/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#include <Arduino.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include "Nanohex.h"
#include "fauxmoESP.h"
#include "credentials.h"

fauxmoESP fauxmo;

#define SERIAL_BAUDRATE     115200

#define ID_LIGHT           "NanoLeaf"

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
  hexController->set_mode(1);
  hexController->set_primary(CRGB::White);

  
  Blynk.begin(auth, ssid, pass);
  Blynk.virtualWrite(V0, 0);
  Blynk.virtualWrite(V1, 0);
  Blynk.virtualWrite(V2, 0);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);

  // By default, fauxmoESP creates it's own webserver on the defined port
    // The TCP port must be 80 for gen3 devices (default is 1901)
    // This has to be done before the call to enable()
    fauxmo.createServer(true); // not needed, this is the default value
    fauxmo.setPort(80); // This is required for gen3 devices

    // You have to call enable(true) once you have a WiFi connection
    // You can enable or disable the library at any moment
    // Disabling it will prevent the devices from being discovered and switched
    fauxmo.enable(true);

    // You can use different ways to invoke alexa to modify the devices state:
    // "Alexa, turn yellow lamp on"
    // "Alexa, turn on yellow lamp
    // "Alexa, set yellow lamp to fifty" (50 means 50% of brightness, note, this example does not use this functionality)

    // Add virtual devices
    fauxmo.addDevice(ID_LIGHT);

    fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state, unsigned char value) {
        
        // Callback when a command from Alexa is received. 
        // You can use device_id or device_name to choose the element to perform an action onto (relay, LED,...)
        // State is a boolean (ON/OFF) and value a number from 0 to 255 (if you say "set kitchen light to 50%" you will receive a 128 here).
        // Just remember not to delay too much here, this is a callback, exit as soon as possible.
        // If you have to do something more involved here set a flag and process it in your main loop.
        
        Serial.printf("[MAIN] Device #%d (%s) state: %s value: %d\n", device_id, device_name, state ? "ON" : "OFF", value);

        // Checking for device_id is simpler if you are certain about the order they are loaded and it does not change.
        // Otherwise comparing the device_name is safer.

        //TODO Check if the ID is one of the ID's provided above
        if (strcmp(device_name, ID_LIGHT)==0) {
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
      Blynk.virtualWrite(V0, 0);
      Blynk.virtualWrite(V1, 0);
      Blynk.virtualWrite(V2, 0);
      power = false;
    }else{
      Blynk.virtualWrite(V0, 0);
      Blynk.virtualWrite(V1, 0);
      Blynk.virtualWrite(V2, 1);
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

BLYNK_WRITE(V0)
{
  party = param.asInt();
  Serial.println("Party");
  if(party){
    breathing = false;
    Blynk.virtualWrite(V1, 0);
  }
}

BLYNK_WRITE(V1)
{
  breathing = param.asInt();
  Serial.println("Breathing");
  if(breathing){
    party = false;
    Blynk.virtualWrite(V0, 0);
  }
}

BLYNK_WRITE(V2)
{
  power = param.asInt();
  if(!power){
    party = false;
    breathing = false;
    Blynk.virtualWrite(V0, 0);
    Blynk.virtualWrite(V1, 0);
    hexController->set_primary(CRGB::Black);
  }else{
    hexController->set_primary(CRGB::White);
  }
  fauxmo.setState(ID_LIGHT, power, brightness);
}

BLYNK_WRITE(V3)  
{
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
    fauxmo.setState(ID_LIGHT, power, brightness);
}
