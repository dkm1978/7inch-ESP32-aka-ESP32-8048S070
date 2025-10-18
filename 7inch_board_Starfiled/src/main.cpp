#include <7inch_board.h>
#include "camelot.h"

#define NSTARS 2000
float sx[NSTARS] = {};
float sy[NSTARS] = {};
float sz[NSTARS] = {};
uint8_t scale=255;
float dx=0;
float ddx=-0.01;
float text_col=255;
uint8_t line=0;
const char* line1 = "One Year Camelot ?";
const char* line2 = "no";
const char* line3 = "...?";
const char* line4 = "7inch china board demo :P";
const char* line5 = "https://github.com/dkm1978";
const char* line6 = "Fun with ESP32 + 16bit LCD";
const char* line7 = "You will find it on Aliexpress";


int16_t szer,wys;
uint16_t szer1,wys1;

void setup() {
BoardInit(true,55);
delay(1000);
Screen->setBackLight(255);
showPicFX();
delay(2000);
cleanVerticalLineFX();


}

void loop()
{

  uint8_t spawnDepthVariation = 255;

  for(int i = 0; i < NSTARS; ++i)
  {
    if (sz[i] <= 1)
    {
      sx[i] = random(800);
      sy[i] = random(480);
      sz[i] = spawnDepthVariation--;
    }
    else
    {
      int old_screen_x = ((int)sx[i] - 400) * scale / sz[i] + 400;
      int old_screen_y = ((int)sy[i] - 240) * scale / sz[i] + 240;

      // This is a faster pixel drawing function for occassions where many single pixels must be drawn
      //tft.drawPixel(old_screen_x, old_screen_y,TFT_BLACK);
      Screen->drawPixel(old_screen_x, old_screen_y,BLACK);

      sz[i] -= 1;
      sy[i] -= dx;
      sx[i] +=dx;
      if (sz[i] > 1)
      {
        int screen_x = ((int)sx[i] - 400) * scale / sz[i] + 400;
        int screen_y = ((int)sy[i] - 240) * scale / sz[i] + 240;
  
        if (screen_x >= 0 && screen_y >= 0 && screen_x < 800 && screen_y < 480)
        {
          uint8_t r, g, b;
          r = g = b = 255 - sz[i];
          Screen->drawPixel(screen_x, screen_y,Screen->color565(r,g,b));
        }
        else
          sz[i] = 0; // Out of screen, die.
      }
    }
  }
  dx=dx+ddx;
  if (dx>3) ddx=ddx*-1;
  if (dx<-3) ddx=ddx*-1;

  Screen->setCursor(320,230);
  Screen->setTextSize(4);
  Screen->setTextColor(Screen->color565(0,text_col,0));
  
  switch (line)
  {
    case 0:
    
    Screen->getTextBounds(line1,0,0,&szer,&wys,&szer1,&wys1);
    Screen->setCursor(400-(szer1/2),240-(wys1/2));
    Screen->print(line1);
    break;

    case 1:
    Screen->getTextBounds(line2,0,0,&szer,&wys,&szer1,&wys1);
    Screen->setCursor(400-(szer1/2),240-(wys1/2));
    Screen->print(line2);
    break;

    case 2:
    Screen->getTextBounds(line3,0,0,&szer,&wys,&szer1,&wys1);
    Screen->setCursor(400-(szer1/2),240-(wys1/2));
    Screen->print(line3);
    break;

    case 3:
    Screen->getTextBounds(line4,0,0,&szer,&wys,&szer1,&wys1);
    Screen->setCursor(400-(szer1/2),240-(wys1/2));
    Screen->print(line4);
    break;
    case 4:
    Screen->getTextBounds(line5,0,0,&szer,&wys,&szer1,&wys1);
    Screen->setCursor(400-(szer1/2),240-(wys1/2));
    Screen->print(line5);
    break;
    case 5:
    Screen->getTextBounds(line6,0,0,&szer,&wys,&szer1,&wys1);
    Screen->setCursor(400-(szer1/2),240-(wys1/2));
    Screen->print(line6);
    break;
    case 6:
    Screen->getTextBounds(line7,0,0,&szer,&wys,&szer1,&wys1);
    Screen->setCursor(400-(szer1/2),240-(wys1/2));
    Screen->print(line7);
    break;

    default:
      break;
  }




  text_col -=1;
  if (text_col==0) {text_col=255;line++;}
  if (line==7) line =0;

  
}


void DrawPixel(int x,int y,uint16_t colour){
   Screen->drawPixel(x,y,colour);
}

void DrawFastHline(int x,int y,uint32_t colour){

  Screen->drawFastHLine(0,y,800,0);
   delay(5);
}
void DrawFastVline(int x,int y,uint32_t colour){

     Screen->drawFastHLine(x,0,480,0);
     delay(5);
}

