#ifndef _ARDUINO_GFX_LIBRARIES_H_
#define _ARDUINO_GFX_LIBRARIES_H_
#include "Arduino.h"
#include "Arduino_DataBus.h"
#include "databus/Arduino_ESP32RGBPanel.h"
#include "display/Arduino_RPi_DPI_RGBPanel.h"
#include "Arduino_GFX.h"
//#include "font/u8g2.h"


#if !defined(LITTLE_FOOT_PRINT)
#include "canvas/Arduino_Canvas.h"
#include "canvas/Arduino_Canvas_Indexed.h"
#include "canvas/Arduino_Canvas_3bit.h"
#include "canvas/Arduino_Canvas_Mono.h"
#endif // !defined(LITTLE_FOOT_PRINT)

#define TFT_BL 2

/* 
 * Touch support ( added by Proteus )
 */
#include "TAMC_GT911.h"

#define TOUCH_SDA  19
#define TOUCH_SCL 20
#define TOUCH_INT -1
#define TOUCH_RST 38
#define TOUCH_WIDTH  800
#define TOUCH_HEIGHT 480

TAMC_GT911 Touch = TAMC_GT911(TOUCH_SDA, TOUCH_SCL, TOUCH_INT, TOUCH_RST, TOUCH_WIDTH, TOUCH_HEIGHT);

/*End Touch support */

Arduino_ESP32RGBPanel *bus = new Arduino_ESP32RGBPanel(
    GFX_NOT_DEFINED /* CS */, GFX_NOT_DEFINED /* SCK */, GFX_NOT_DEFINED /* SDA */,
    41 /* DE */, 40 /* VSYNC */, 39 /* HSYNC */, 42 /* PCLK */,
    14 /* R0 */, 21 /* R1 */, 47 /* R2 */, 48 /* R3 */, 45 /* R4 */,
    9 /* G0 */, 46 /* G1 */, 3 /* G2 */, 8 /* G3 */, 16 /* G4 */, 1 /* G5 */,
    15 /* B0 */, 7 /* B1 */, 6 /* B2 */, 5 /* B3 */, 4 /* B4 */
);
Arduino_RPi_DPI_RGBPanel *Screen = new Arduino_RPi_DPI_RGBPanel(
  bus,
  800 /* width */, 0 /* hsync_polarity */, 210 /* hsync_front_porch */, 30 /* hsync_pulse_width */, 16 /* hsync_back_porch */,
  480 /* height */, 0 /* vsync_polarity */, 22 /* vsync_front_porch */, 13 /* vsync_pulse_width */, 10 /* vsync_back_porch */,
  1 /* pclk_active_neg */, 16000000 /* prefer_speed */, true /* auto_flush */);


/**
 * Inicjalizacja ekranu oraz dotyku.
 *
 * @param screenON Czy ekran ma być włączony przy starcie.
 * @param backlight jasność podświetlenia 0-255.
 * 
 * @note inicjalizuje obiekty [Screen] oraz [Touch]
 */
void BoardInit(bool screenON,uint8_t bakcligth){

  Screen->begin();
  if (screenON) {
  Screen->displayOn();
  Screen->setBackLight(bakcligth);
  }
  else 
  Screen->displayOff();

  Touch.begin();
  Touch.setRotation(1);

}

#endif // _ARDUINO_GFX_LIBRARIES_H_
