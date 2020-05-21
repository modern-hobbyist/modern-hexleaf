#include <stdint.h>
#include <FastLED.h>

/* Number of LEDs in each box/leaf */
#define LEDS_IN_BOX 7
/*The number of boxes */
#define NUM_BOXES 8
/*The pin the LED is connected to */
#define LED_PIN 27
/*Don't change unless you know what you're doing */
#define TOTAL_LEDS LEDS_IN_BOX *NUM_BOXES

CRGB leds[TOTAL_LEDS];
int reversedLEDS[NUM_BOXES];

class Hexnode
{
  private:
    uint16_t ledStart;
    uint16_t ledEnd;
    CRGB color;

  public:
    Hexnode(uint8_t index) : color(CRGB(0, 0, 0))

    {
        ledStart = index * LEDS_IN_BOX;
        ledEnd = ledStart + LEDS_IN_BOX - 1;
    }

    void set_color(CRGB c)
    {
        color = c;
        Serial.print("Red: ");
        Serial.print(c.r);
        Serial.print(", Green: ");
        Serial.print(c.g);
        Serial.print(", Blue: ");
        Serial.println(c.b);
    }

    int draw()
    {
        for (uint16_t ledPos = ledStart; ledPos <= ledEnd; ledPos++)
        {
            leds[ledPos] = color;
        }
        return 0;
    }
};

class Nanohex
{
  private:
    Hexnode *nodes[NUM_BOXES];
    uint16_t drawEveryNthMs;
    unsigned long lastDrew;
    CRGB primary;
    CRGB secondary;
    uint16_t mode;
    uint8_t brightness;

  public:
    Nanohex() : lastDrew(0),
                drawEveryNthMs(60),
                primary(CRGB(0, 60, 120)),
                secondary(CRGB(0, 0, 0))
    {
        FastLED.addLeds<WS2811, LED_PIN, GRB>(leds, TOTAL_LEDS);
        for (uint8_t i = 0; i < NUM_BOXES; i++)
            nodes[i] = new Hexnode(i);
    }

    void set_brightness(uint8_t val)
    {
        brightness = val;
    }

    void set_primary(CRGB c)
    {
        primary = c;
    }

    void set_secondary(CRGB c)
    {
        secondary = c;
    }

    void set_color_of(uint8_t idx, CRGB color)
    {
        if(reversedLEDS[idx] == 1){
            nodes[idx]->set_color(CRGB(color.r, color.b, color.g));
        }else{
            nodes[idx]->set_color(color);
        }
    }

    void update()
    {

        if (millis() - lastDrew > drawEveryNthMs)
        {
            for (uint8_t i = 0; i < NUM_BOXES; i++)
                int ret = nodes[i]->draw();
            FastLED.setBrightness(brightness);
            FastLED.show();
            lastDrew = millis();
        }
    }

    void color_all(CRGB color)
    {
        for (uint8_t i = 0; i < NUM_BOXES; i++)
            set_color_of(i, color);
    }
};
