#include <Adafruit_NeoPixel.h>
#include <RotaryEncoder.h>
#include <Button.h>  // from TES_eSax-lib
#include <MIDI.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "AuroraParameters.h"

params = AuroraParameters();
uint8_t pitchbendAmplitude = 0;
int16_t pitchbend = 0;

//     int32_t color = strip.gamma32(strip.ColorHSV(((manager.get_note()[0])<<10) + ((PB.get_value()*pitchbend_amp_CC.get_value())>>3)));

/*
TODO:
 - simple menu that has constant pointers to the screen, the rotary and the button and to the parameter struct
 - make the parameter structure

*/


/** SCREEN */
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);



/** STRIP*/
#define PIN 11
#define N_LED 120

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_LED, PIN, NEO_GRB + NEO_KHZ800);
uint32_t color[N_LED];
uint8_t brightness;
uint32_t color_base;

void handleCC(byte channel, byte control1, byte control2) {
  Serial.print(control1);
  Serial.print(" ");
  Serial.println(control2);
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

void handleNoteOn(byte channel, byte note, byte velocity) {
  if (channel == params.midi_channel) {
    color_base = strip.gamma32(strip.ColorHSV(((note) << 10) + ((pitchbend * pitchbendAmplitude) >> 3)));
    brightness = velocity << 1;
  }
}

void handleNoteOff(byte channel, byte note, byte velocity) {
  if (channel == params.midi_channel) {
    brightness = 0;
  }
}


MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  Serial1.setRX(1);

  MIDI.setHandleControlChange(handleCC);
  MIDI.turnThruOff();  // done in hw
  MIDI.begin(MIDI_CHANNEL_OMNI);


  strip.begin();
  strip.setBrightness(255);
  strip.show();  // Initialize all pixels to 'off'


  Wire.setSDA(20);
  Wire.setSCL(21);
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  delay(500);
  display.display();
}
unsigned long big_del = 1000, next_update = 0, next_big_update = 0;








void loop() {


  if (millis() > next_big_update) {
    color[0] = random(2147483640);
    strip.setPixelColor(0, color[0]);
    next_big_update += big_del;

  } else if (millis() > next_big_update - 500) {
    color[0] = 0;
    strip.setPixelColor(0, color[0]);
  }
  strip.show();



  if (millis() > next_update) {
    next_update += params.period;

    uint16_t r, g, b;
    b = ((color_base & 255) * brightness) >> 8;
    g = (((color_base >> 8) & 255) * brightness) >> 8;
    r = (((color_base >> 16)) * brightness) >> 8;
    color[0] = b + (g << 8) + (r << 16);
    strip.setPixelColor(0, color[0]);

    for (uint8_t s = 0; s < params.speeder; s++) {
      for (int i = N_LED; i > 0; i--) {
        color[i] = color[i - 1];
        strip.setPixelColor(i, color[i]);
      }
    }
    strip.show();
  }
}

void loop1() {

  while (MIDI.read())
    ;

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Coucou");
  display.display();
  delay(20);
}
