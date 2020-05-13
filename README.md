# HexLeaf
This project is my attempt to create my own NanoLeafs based on NanoLeafs and by adding alexa to it. This will allow me to not only control the colors and brightness of the lights with the Blynk app, 
but to also turn them on/off with Alexa. These lights will be in my home office, so I can say "Alexa, turn on the office" and she will turn all my office lights on, including the HexLeaf setup. ***However the fauxmo library I'm currently using for Alexa integration does not seem to work with the most recent Echo update, I will update this when the library is updated.***

# Hardware List
* [ESP32 Dev Module](https://www.amazon.com/gp/product/B07Q576VWZ/ref=ppx_yo_dt_b_asin_title_o00_s00?ie=UTF8&psc=1)
* [WS2811 Led Strip](https://www.amazon.com/BTF-LIGHTING-300LEDs-Addressable-Flexible-Non-waterproof/dp/B01CNL6K52/ref=sr_1_1_sspa?dchild=1&keywords=ws2811&qid=1589404282&sr=8-1-spons&psc=1&spLa=ZW5jcnlwdGVkUXVhbGlmaWVyPUExRFhaSUtURTkzNkJOJmVuY3J5cHRlZElkPUEwMTEzOTI3MzlQQkpDSzNDQkomZW5jcnlwdGVkQWRJZD1BMDYwMTg0MUpGSzRBNTNJWUNUViZ3aWRnZXROYW1lPXNwX2F0ZiZhY3Rpb249Y2xpY2tSZWRpcmVjdCZkb05vdExvZ0NsaWNrPXRydWU=)
* [AnyCubic i3 Mega 3d Printer](https://www.anycubic.com/collections/anycubic-mega-3d-printers/products/anycubic-i3-mega-s?variant=30151431192636)

# Software 
This software uses the FastLED library and some custom classes to construct an array of HexNodes which can each be set to an individual color. There are a few things to configure in the code before you can get started, so make sure to download this project and open it up in the Arduino IDE. 

1. Open Project in Arduino IDE
1. Copy `credentials_example.h` to `credentials.h`
1. Set Credentials for WiFi in `credentials.h`
    ```
    char ssid[] = "My WiFi Connection";
    char pass[] = "examplePassword";
    ```
1. Download the Blynk app and create a new project
    1. This will create an Auth key for you
1. Add Blynk Auth key to `credentials.h`
    ```
    char auth[] = "BLYNK AUTH CODE";
    ```
1. Configure settings in `Nanohex.h`
    ```
    /* Number of LEDs in each box/leaf */
    #define LEDS_IN_BOX 7
    /*The number of boxes */
    #define NUM_BOXES 8
    /*The pin the LED is connected to */
    #define LED_PIN 27
    ```
1. Configure settings in `HexLeaf.ino`
    ```
    #define ID_LIGHT            "NanoLeaf"
    CRGB primary_color = CRGB(0, 153, 204);
    CRGB secondary_color = CRGB(254, 201, 1);
    ```
1. Setup the Blynk App
    ![BlynkImage](https://github.com/csteamengine/HexLeaf/blob/master/images/IMG_5288.png)
    1. Configure a Button on V2 for power
    1. Configure the ZERGBA on V3 with the `MERGE` setting
    1. Configure a brightness slider on V1
    1. Configure a 'Segmented Switch' on V0 for:
        1. Primary Mode
        1. Solid Mode
        1. Breathing Mode
1. Plug it in, press Play and control your hexes!
1. If Alexa integration is working *(currently its not)*, then have Alexa scan for new devices and include your lights in any routines you want!


