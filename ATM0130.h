#ifndef ATM0130_H_
#define ATM0130_H_

#include <Arduino.h>
#include <SPI.h>

//#include <ESP8266WiFi.h>
//#include <WiFiClient.h> 
//#include <ESP8266WebServer.h>
#include <time.h>

#include <FS.h>
#include <string.h>

#include "device.h"
#include "image.h"
#include "font.h"
#include "etcdata.h"
#include "GameObject.h"

#define SERVERPORT 80

//SSの代わり
#define CS 2

//ピンの割り当て (D/C , /RES)
#define DC 6
#define RES 7

#define WIDTH 120
#define HEIGHT 120

#define PI 3.141592653589793

class ATM0130 {
  public:
    ATM0130(uint8_t pin_dat_cmd, uint8_t pin_reset);
    void begin(void);

    void clearScreen(uint16_t color);
    void updateScreen();

    void drawBlock_4px(int16_t x, int16_t y, const uint16_t (&block)[4][4]);
    void drawBlock_8px(int16_t x, int16_t y, const uint16_t (&block)[8][8]);
    void drawBlock_16px(int16_t x, int16_t y, const uint16_t (&block)[16][16]);
    void drawBlock_16px(int16_t x, int16_t y, const uint16_t (&block)[16][16], uint8_t cut_left, uint8_t cut_right, uint8_t cut_top, uint8_t cut_bottom);
    void drawBlock_32px(int16_t x, int16_t y, const uint16_t (&block)[32][32]);
    void drawBlock(int16_t x, int16_t y, uint8_t imagex, uint8_t imagey, uint8_t sizex, uint8_t sizey, const uint16_t (&block)[120][120]);
    void drawFlipBlock_4px(int16_t x, int16_t y, const uint16_t (&block)[4][4]);
    void drawFlipBlock_8px(int16_t x, int16_t y, const uint16_t (&block)[8][8]);
    void drawFlipBlock_16px(int16_t x, int16_t y, const uint16_t (&block)[16][16]);
    void drawFlipBlock_32px(int16_t x, int16_t y, const uint16_t (&block)[32][32]);
    void drawFlipBlock(int16_t x, int16_t y, uint8_t imagex, uint8_t imagey, uint8_t sizex, uint8_t sizey, const uint16_t (&block)[120][120]);
    
    void drawBlock_4px(int16_t x, int16_t y, const uint16_t (&block)[4][4],bool flip);
    void drawBlock_8px(int16_t x, int16_t y, const uint16_t (&block)[8][8],bool flip);
    void drawBlock_16px(int16_t x, int16_t y, const uint16_t (&block)[16][16],bool flip);
    void drawBlock_32px(int16_t x, int16_t y, const uint16_t (&block)[32][32],bool flip);
    
    void drawRotateBlock_4px(int16_t x, int16_t y, const uint16_t (&block)[4][4]);
    void drawRotateFlipBlock_4px(int16_t x, int16_t y, const uint16_t (&block)[4][4]);
    
    void drawRotateBlock_16px(int16_t x, int16_t y, const uint16_t (&block)[16][16]);
    void drawRotateFlipBlock_16px(int16_t x, int16_t y, const uint16_t (&block)[16][16]);

    void setColor(uint16_t c);
    void drawDot(int16_t x, int16_t y);
    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1);

    void drawRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
    void drawFillRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);

    void drawCircle(uint8_t x0, uint8_t y0, uint8_t r);
    void drawFillCircle(uint8_t x0, uint8_t y0, uint8_t r);

    void putStr(int16_t Xp, int16_t Yp, const char * s);
    void putStr(int16_t Xp, int16_t Yp, String str);
    void putCharacter(int16_t x, int16_t y, char c);

    uint64_t frame;

  private:
    SPISettings spi_settings;
    uint8_t width = 240;
    uint8_t height = 240;
    void writeReg(uint8_t data);
    void writeData(uint8_t data);
    void resetLCD(void);
    void setWindow(uint8_t x, uint8_t y, uint8_t width, uint8_t height);

    void putPixel(uint16_t color) ;
    uint16_t convRGB(uint8_t red, uint8_t green, uint8_t blue) ;

    uint16_t color;
    uint8_t char_x, char_y;
    uint8_t dat_cmd, reset;
    bool is_reset_inv;

    volatile uint16_t backScreen[120][120];
    //240を指定すると恐らくでかすぎてエラーが起こる。
};

//ピンの割り当て (D/C , /RES)
ATM0130 myATM0130 = ATM0130(DC, RES);
//ESP8266WebServer server(SERVERPORT);
Device device = Device();

#endif
