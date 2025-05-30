#include <Adafruit_NeoPixel.h>
#include <RotaryEncoder.h>
#include <Button.h>  // from TES_eSax-lib
#include <MIDI.h>
#include <EEPROM.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "folder.h"  // from Mozzi
#include "AuroraScreen.h"
#include "AuroraParameters.h"
#include "loaded6pt7b.h"
#include "loaded8pt7b.h"


AuroraParameters params;
//params = AuroraParameters();
uint8_t pitchbendAmplitude = 0;
uint16_t brightness = 0;  // on 14bits
uint8_t note;
int16_t pitchbend = 0;


/** ROTARY */
#define ROTARY_PIN1 19
#define ROTARY_PIN2 18
RotaryEncoder encoder(ROTARY_PIN1, ROTARY_PIN2, RotaryEncoder::LatchMode::FOUR3);
void checkPosition() {
  encoder.tick();  // just call tick() to check the state.
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

/** PUSH BUTTON */
Button pushButton(22);


/** SCREEN */
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


AuroraScreen auroraScreen(&display, &encoder, &pushButton, &params, 100);
//AuroraScreen auroraScreen();



/** COLOR FOLDER*/
WaveFolder<uint32_t> folder;

/** STRIP*/
#define PIN 11
#define N_LED 120

Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_LED, PIN, NEO_GRB + NEO_KHZ800);
uint32_t color[N_LED];
unsigned long next_update = 0;

void handleCC(byte _channel, byte control1, byte control2) {
  if (_channel == params.midi_channel && control1 == params.midi_control_MSB) {
    brightness &= 0b00000001111111;
    brightness += control2 << 7;
  }
  if (_channel == params.midi_channel && control1 == params.midi_control_LSB) {
    brightness &= 0b11111110000000;
    brightness += control2;
    //Serial.println(control2);
  } else if (_channel == params.midi_channel && control1 == params.midi_pitchbend_amplitude_control) pitchbendAmplitude = control2;
}

void handleNoteOn(byte _channel, byte _note, byte _velocity) {
  if (_channel == params.midi_channel) {
    note = _note;
    brightness = _velocity << 7;
  }
}

void handleNoteOff(byte channel, byte note, byte velocity) {
  if (channel == params.midi_channel) {
    brightness = 0;
  }
}

void handlePitchBend(byte _channel, int _pitchbend) {
  if (_channel == params.midi_channel) {
    pitchbend = _pitchbend;
  }
}


MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

void setup() {
  //Serial.begin(115200);
  EEPROM.begin(256);
  if (pushButton.is_pressed()) EEPROM.put(0, params);  // factory reset
  else EEPROM.get(0, params);


  pinMode(LED_BUILTIN, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(ROTARY_PIN1), checkPosition, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ROTARY_PIN2), checkPosition, CHANGE);

  Serial.begin(115200);
  Serial1.setRX(1);

  MIDI.setHandleControlChange(handleCC);
  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.setHandleNoteOff(handleNoteOff);
  MIDI.setHandlePitchBend(handlePitchBend);
  MIDI.turnThruOff();  // done in hw
  MIDI.begin(MIDI_CHANNEL_OMNI);


  strip.begin();
  strip.setBrightness(255);
  strip.show();  // Initialize all pixels to 'off'

  folder.setLimits(params.lowest_hue, params.highest_hue);

  Wire.setSDA(20);
  Wire.setSCL(21);
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);
  display.setCursor(30, 20);
  display.setFont(&loaded8pt7b);
  display.print("TES");
  display.setFont(&loaded6pt7b);
  display.setTextSize(1);
  display.setCursor(50, 40);
  display.print("Aurora v1");
  display.setCursor(0, 63);
  display.print("Pwd by Adafruit");
  display.display();
  delay(500);
}









void loop() {


  if (millis() > next_update) {
    next_update += params.period;
    /*Serial.print((uint16_t((note) << 10) + ((pitchbend * pitchbendAmplitude) >> 3)));
Serial.print(" ");
Serial.println(folder.next(uint16_t(((note) << 10) + ((pitchbend * pitchbendAmplitude) >> 3))));*/
    color[0] = strip.gamma32(strip.ColorHSV(folder.next(((note) << 10) + ((pitchbend * pitchbendAmplitude) >> 3))));  //, 255, brightness >> 6)); // with gamma on the value
    uint8_t r = (uint8_t)(color[0] >> 16), g = (uint8_t)(color[0] >> 8), b = (uint8_t)color[0];
    uint8_t br = brightness >> 6;
    r = (r * br) >> 8;
    g = (g * br) >> 8;
    b = (b * br) >> 8;
    color[0] = b + (g << 8) + (r << 16);
    strip.setPixelColor(0, color[0]);

    for (uint8_t s = 0; s < params.speeder; s++) {
      for (int i = N_LED; i > 0; i--) {
        color[i] = color[i - 1];  // propagation
        if (s == params.speeder - 1) strip.setPixelColor(i, color[i]);
      }
    }
    strip.show();
  }
}

void loop1() {

  while (MIDI.read())
    ;

  auroraScreen.update();

  /* display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Coucou");
  display.display();
  delay(20);*/
  /* Serial.print(note);
  Serial.print(" ");
  Serial.print(brightness);
  Serial.print(" ");
  Serial.println(color[0]);*/
  /*pushButton.update();
  if (pushButton.is_pressed()) digitalWrite(LED_BUILTIN,HIGH);
  else digitalWrite(LED_BUILTIN,LOW);*/
}
