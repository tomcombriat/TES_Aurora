#ifndef _AURORA_SCREEN_
#define _AURORA_SCREEN_
#include <RotaryEncoder.h>
#include <Button.h>  // from TES_eSax-lib
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "AuroraParameters.h"

class AuroraScreen {
public:
  AuroraScreen(){};
  AuroraScreen(Adafruit_SSD1306* _screen, RotaryEncoder* _enc, Button* _button, AuroraParameters* _param, unsigned long _response_time = 100)
    : screen(_screen), enc(_enc), button(_button), param(_param), response_time(_response_time){};

  void update() {
    if (next_update < millis()) {
      next_update = millis() + response_time;
      has_changed = false;

      button->update();
      if (button->has_been_released()) {
        currentItem += 1;
        if (currentItem == N_item) currentItem = 0;
        has_changed = true;
      }
      int delta = enc->getPosition();
      if (delta != 0) {
        enc->setPosition(0);
        has_changed = true;
      }
      if (has_changed) {
        switch (currentItem) {
          case 0:  // period
            {
              long new_period = param->period + delta;
              if (new_period < 0) param->period = 0;
              else param->period = new_period;
              drawValues("Period", param->period);
              break;
            }

          case 1:  // speeder
            {
              int16_t new_speeder = param->speeder + delta;
              if (new_speeder < 1) param->speeder = 1;
              else param->speeder = new_speeder;
              drawValues("Speeder", param->speeder);
              break;
            }
          case 2:  // MIDI_channel
            {
              int8_t new_midi_channel = param->midi_channel + delta;
              if (new_midi_channel < 0) param->midi_channel = 0;
              else if (new_midi_channel > 15) param->midi_channel = 15;
              else param->midi_channel = new_midi_channel;
              drawValues("MIDI channel", param->midi_channel);
              break;
            }

          case 3:  // MIDI MSB
            {
              int16_t new_midi_control_MSB = param->midi_control_MSB + delta;
              if (new_midi_control_MSB < 0) param->midi_control_MSB = 0;
              else if (new_midi_control_MSB > 127) param->midi_control_MSB = 127;
              else param->midi_control_MSB = new_midi_control_MSB;
              drawValues("MIDI MSB", param->midi_control_MSB);
              break;
            }

          case 4:  // MIDI LSB
            {
              int16_t new_midi_control_LSB = param->midi_control_LSB + delta;
              if (new_midi_control_LSB < 0) param->midi_control_LSB = 0;
              else if (new_midi_control_LSB > 127) param->midi_control_LSB = 127;
              else param->midi_control_LSB = new_midi_control_LSB;
              drawValues("MIDI LSB", param->midi_control_LSB);
              break;
            }

          case 5:  // MIDI pitchbend cc
            {
              int16_t new_midi_pitchbend_amplitude_control = param->midi_pitchbend_amplitude_control + delta;
              if (new_midi_pitchbend_amplitude_control < 0) param->midi_pitchbend_amplitude_control = 0;
              else if (new_midi_pitchbend_amplitude_control > 127) param->midi_pitchbend_amplitude_control = 127;
              else param->midi_pitchbend_amplitude_control = new_midi_pitchbend_amplitude_control;
              drawValues("MIDI PB CC", param->midi_pitchbend_amplitude_control);
              break;
            }
          case 6:  // save
            {
              drawValues("Save preset", 0);
              if (delta) {
                EEPROM.put(0, *param);
                EEPROM.commit();
              }
              break;
            }
            /*case 7:  // recall
            {
              drawValues("Recall preset", 0);
              if (delta) {
                EEPROM.get(0, *param);
                //EEPROM.commit();
              }
              break;
            }*/
          case 7:  // Lowest hue
            {
              param->lowest_hue += delta<<4;
              //strip.setPixelColor(0,strip.gamma32(strip.ColorHSV(param->lowest_hue))); // passed in global to allow differtent type of strips…
              drawValues("Hue min", param->lowest_hue,1);
              break;
            }
          case 8:  // Highest hue
            {
              param->highest_hue += delta<<4;
              //strip.setPixelColor(0,strip.gamma32(strip.ColorHSV(param->highest_hue))); // passed in global to allow differtent type of strips…
              drawValues("Hue max", param->highest_hue,1);
              break;
            }
        }
        screen->display();
      }
    }
  }


private:
  Button* const button = nullptr;
  RotaryEncoder* const enc = nullptr;
  Adafruit_SSD1306* const screen = nullptr;
  AuroraParameters* param = nullptr;
  unsigned long response_time, next_update = 0;
  bool has_changed = false;
  uint8_t currentItem;
  const uint8_t N_item = 9;

  void drawValues(String title, int value, uint8_t value_size=4) {
    //Refresh the top bar
    screen->fillRect(0, 0, screen->width(), 20, SSD1306_WHITE);
    screen->setCursor(10, 13);
    screen->setTextColor(SSD1306_BLACK);
    screen->setTextSize(1);
    screen->print(title);

    // Refresh the value
    screen->fillRect(0, 20, screen->width(), screen->height() - 20, SSD1306_BLACK);
    screen->setCursor(30, 60);
    screen->setTextSize(value_size);
    screen->setTextColor(SSD1306_WHITE);
    screen->print(value);
  }
};






#endif