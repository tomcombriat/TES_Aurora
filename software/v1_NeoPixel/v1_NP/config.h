#ifndef _AURORA_CONFIG_
#define _AURORA_CONFIG_

/******************
Hardware parameters, should probably not be changed
*******************/
// internal
#define NONE 1
#define NEOPIXEL 100
#define DOTSTAR 200

//// V1
// Rotary
#define ROTARY_PIN1 19
#define ROTARY_PIN2 18
#define PUSH_BUTTON_PIN 22

// Screen
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
#define SCREEN_SDA 20
#define SCREEN_SCL 21

// Strips
#define STRIP1_SCK 2
#define STRIP1_TX 3
#define STRIP2_SCK 10
#define STRIP2_TX 11
/********************************/
/*** END OF HARDWARE PARAMETERS */
/********************************/

/** STRIP 1*/
#define STRIP1_TYPE DOTSTAR
#define STRIP1_NLED 300

#if (STRIP1_TYPE == DOTSTAR)
#include <Adafruit_DotStar.h>
#define STRIP1_COLORTYPE DOTSTAR_RGB
#elif (STRIP1_TYPE == NEOPIXEL)
#include <Adafruit_NeoPixel.h>
#define STRIP1_COLORTYPE NEO_GRB + NEO_KHZ800
#endif


#define STRIP2_TYPE NEOPIXEL
#define STRIP2_NLED 150


#if (STRIP2_TYPE==DOTSTAR)
#include <Adafruit_DotStar.h>
#define STRIP2_COLORTYPE DOTSTAR_BRG
#elif (STRIP2_TYPE == NEOPIXEL)
#include <Adafruit_NeoPixel.h>
#define STRIP2_COLORTYPE NEO_GRB + NEO_KHZ800
#endif










#endif
