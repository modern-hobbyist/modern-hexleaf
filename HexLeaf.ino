/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#include <Arduino.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include "Nanohex.h"
#include <Espalexa.h>
#include "credentials.h"

Espalexa espalexa;

#define SERIAL_BAUDRATE     115200

#define STATE_PRIMARY 1
#define STATE_SOLID 2
#define STATE_BREATHING 3
#define BREATH_TIME 10000 //10 seconds
#define ID_LIGHT            "NanoLeaf"
CRGB primary_color = CRGB(0, 153, 204);
CRGB secondary_color = CRGB(254, 201, 1);

int state = STATE_PRIMARY;
bool power = true;
bool alexaToggle = false;
bool alexaBrightness = false;
int brightness = 255;
int updated = true;
int rgb[] = {0, 153, 204};
Nanohex *hexController;
bool primary_one = false;
int last_time = 0;

void hexLeafChanged(EspalexaDevice* dev);

EspalexaDevice* hexLeafAlexa;

BLYNK_WRITE(V0)
{
  state = param.asInt();
  updated = true;
}

BLYNK_WRITE(V1)  
{
    brightness = param.asInt();
    Serial.printf("Brightness: %d \n", brightness);
    hexLeafAlexa->setValue(0);
    updated = true;
}

BLYNK_WRITE(V2)
{
  power = param.asInt();
  updated = true;
  hexLeafAlexa->setState(power);
}

BLYNK_WRITE(V3)  
{   
    rgb[0] = param[0].asInt();
    rgb[1] = param[1].asInt();
    rgb[2] = param[2].asInt();
    state = STATE_SOLID;
    Blynk.virtualWrite(V0, STATE_SOLID); //Need to tell the input to default to solid
    updated = true;
}

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
  hexController->set_primary(CRGB::White);

  for(int i = 0; i < NUM_BOXES; i++){
    reversedLEDS[i] = 0;
  }

  //Set any reversed BRG/BRG panels
  reversedLEDS[2] = 1;
  reversedLEDS[5] = 1;
  
  Blynk.begin(auth, ssid, pass);
  Blynk.virtualWrite(V0, 1);
  Blynk.virtualWrite(V1, 255);
  Blynk.virtualWrite(V2, 1);
  Blynk.virtualWrite(V3, 255, 255, 255);

  //Alexa configuration
  hexLeafAlexa = new EspalexaDevice("Hex Leaf", hexLeafChanged, EspalexaDeviceType::dimmable);
  espalexa.addDevice(hexLeafAlexa);
  espalexa.begin();

  hexLeafAlexa->setState(true);
}

void loop()
{
  Blynk.run();
  espalexa.loop();
  if(updated){
    update_hexes();
  }

  if(alexaBrightness){
    Blynk.virtualWrite(V4, brightness);
    alexaBrightness = false;
  }

  hexController->update();
}

void hexLeafChanged(EspalexaDevice* d) {
  if (d == nullptr) return; //this is good practice, but not required

  Serial.print("Power changed to ");
  int inputBrightness = d->getValue();

  Serial.println(d->getState());

  power = d->getState();
  Blynk.virtualWrite(V2, power);

  if(inputBrightness != brightness){
    alexaBrightness = true;
    brightness = inputBrightness;
  }
  updated = true;
}

void update_hexes(){
  if(power){
    hexController->set_brightness(brightness);
    if(state == STATE_PRIMARY){ // Primary
      updated = false;
      //TODO set alternating hexes to Modern Hobbyist logo colors
      hexController->set_primary(primary_color);
      hexController->set_secondary(secondary_color);
      for(int i = 0; i<NUM_BOXES; i++){
        if(i % 2 == 0){
          Serial.println("Setting primary to this hex");
          hexController->set_color_of(i, primary_color);
        }else{
          Serial.println("setting secondary to this hex");
          hexController->set_color_of(i, secondary_color);
        }
      }
    }else if(state == STATE_SOLID){ // Solid
      updated = false;
      hexController->color_all(CRGB(rgb[0], rgb[1], rgb[2]));
    }else if(state == STATE_BREATHING){ // Breathing
      //Currently gets called every 10 seconds
      if(millis() - last_time >= BREATH_TIME){
        last_time = millis();
        if(primary_one){
          primary_one = false;
          Serial.println("setting to secondary color");
          hexController->color_all(secondary_color);
        }else{
          Serial.println("setting to primary color");
          primary_one = true;
          hexController->color_all(primary_color);
        }
      }
    }
  }else{
    hexController->set_brightness(0);
    hexLeafAlexa->setValue(0);
    updated = false;
  }
}
