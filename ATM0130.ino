//AE-ATM0310B3 with Arduino UNO R3
//2020/01/12

#include "ATM0130.h"

#define swap(x,y) do{uint8_t temp=x;x=y;y=temp;}while(0)


ATM0130::ATM0130(uint8_t pin_dat_cmd, uint8_t pin_reset) {
  dat_cmd = pin_dat_cmd;
  frame = 0;
  reset = pin_reset;
  pinMode(dat_cmd, OUTPUT);
  pinMode(reset, OUTPUT);
  pinMode(CS, OUTPUT);
  pinMode(SCK, OUTPUT);
  pinMode(MOSI, OUTPUT);

  spi_settings = SPISettings(16000000, MSBFIRST, SPI_MODE0);
  SPI.begin();
  //↑これがなかったから表示されていなかった。

  setColor(0x0000);
}

/* 秋月電子のものを引用 */
void ATM0130::begin(void) {
  digitalWrite(CS, HIGH);
  resetLCD();
  SPI.beginTransaction(spi_settings);
  digitalWrite(CS, LOW);

  writeReg(0x11);
  delay(100);

  writeReg(0x36);  //MADCTL
  writeData(0x00);
  //MY=0
  //MX=0
  //MV=0
  //ML=0
  //RGB=0
  //MH=0
  writeReg(0x3A);
  writeData(0x55); //65K color , 16bit / pixel

  ////--------------------------------ST7789V Frame rate

  writeReg(0xb2);
  writeData(0x0c);
  writeData(0x0c);
  writeData(0x00);
  writeData(0x33);
  writeData(0x33);

  delay(2);

  writeReg(0xb7);
  writeData(0x75);

  delay(2);
  ////---------------------------------ST7789V Power
  writeReg(0xc2);
  writeData(0x01);

  delay(2);

  writeReg(0xc3);
  writeData(0x10);

  delay(2);

  writeReg(0xc4);
  writeData(0x20);

  delay(2);

  writeReg(0xc6);
  writeData(0x0f);

  writeReg(0xb0);
  writeData(0x00);
  writeData(0xf0); //RRRR RGGGG GGGB BBBB

  delay(2);

  writeReg(0xD0);
  writeData(0xA4);
  writeData(0xA1);
  delay(2);

  ////--------------------------------ST7789V gamma
  writeReg(0x21);

  delay(2);

  writeReg(0xbb);
  writeData(0x3b);

  delay(2);

  writeReg(0xE0);    //Set Gamma
  writeData(0xF0);
  writeData(0x0b);
  writeData(0x11);
  writeData(0x0e);
  writeData(0x0d);
  writeData(0x19);
  writeData(0x36);
  writeData(0x33);
  writeData(0x4b);
  writeData(0x07);
  writeData(0x14);
  writeData(0x14);
  writeData(0x2c);
  writeData(0x2e);

  delay(2);

  writeReg(0xE1);    //Set Gamma
  writeData(0xF0);
  writeData(0x0d);
  writeData(0x12);
  writeData(0x0b);
  writeData(0x09);
  writeData(0x03);
  writeData(0x32);
  writeData(0x44);
  writeData(0x48);
  writeData(0x39);
  writeData(0x16);
  writeData(0x16);
  writeData(0x2d);
  writeData(0x30);

  writeReg(0x2A);
  writeData(0x00);
  writeData(0x00);
  writeData(0x00);
  writeData(0xEF);

  writeReg(0x2B);
  writeData(0x00);
  writeData(0x00);
  writeData(0x00);
  writeData(0xEF);

  writeReg(0x29);    //Display on

  delay(2);
  writeReg(0x2c);
  digitalWrite(CS, HIGH);
  SPI.endTransaction();
}


void ATM0130::setColor(uint16_t c) {
  color = c;
}

void ATM0130::writeReg(uint8_t data) {
  digitalWrite(dat_cmd, LOW);
  SPI.transfer(data);
}

void ATM0130::writeData(uint8_t data) {
  digitalWrite(dat_cmd, HIGH);
  SPI.transfer(data);
}

void ATM0130::resetLCD(void) {
  digitalWrite(reset, HIGH);
  delay(20);
  digitalWrite(reset, LOW);
  delay(20);
  digitalWrite(reset, HIGH);
  delay(20);
}

void ATM0130::setWindow(uint8_t x, uint8_t y, uint8_t width, uint8_t height) {
  writeReg(0x2A);
  writeData(0x00);
  writeData(x);
  writeData(0x00);
  writeData(x + width - 1);

  writeReg(0x2B);
  writeData(0x00);
  writeData(y);
  writeData(0x00);
  writeData(y + height - 1);

  writeReg(0x2c);
}

void ATM0130::putPixel(uint16_t color) {
  digitalWrite(dat_cmd, HIGH);
  SPI.transfer(color >> 8);
  SPI.transfer(color & 0xFF);
}

uint16_t ATM0130::convRGB(uint8_t red, uint8_t green, uint8_t blue) {
  uint16_t color = 0;
  color = blue >> 3;
  color |= ((green & 0xFC) << 3);
  color |= ((red & 0xF8) << 8);
  return color;
}

void ATM0130::drawBlock_4px(int16_t x, int16_t y, const uint16_t (&block)[4][4]) {
  uint8_t firsti = 0, endi = 4;
  uint8_t firstj = 0, endj = 4; //firsti,firstjを経由しないと下の二重ループがちゃんと機能しない。
  if (x < 0) firsti = abs(x);
  else if (x + endi > WIDTH) endi = WIDTH;
  if (y < 0) firstj = abs(y);
  else if (y + endj > HEIGHT) endj = HEIGHT;

  for (uint8_t j = firstj; j < endj; j++) {
    for (uint8_t i = firsti; i < endi; i++) {
      if (pgm_read_word_near(&(block[j][i])) == SKELETON) continue;
      else backScreen[y + j][x + i] = pgm_read_word_near(&(block[j][i]));
    }
  }
  return;
}

void ATM0130::drawBlock_8px(int16_t x, int16_t y, const uint16_t (&block)[8][8]) {
  uint8_t firsti = 0, endi = 8;
  uint8_t firstj = 0, endj = 8; //firsti,firstjを経由しないと下の二重ループがちゃんと機能しない。
  if (x < 0) firsti = abs(x);
  else if (x + endi > WIDTH) endi = WIDTH;
  if (y < 0) firstj = abs(y);
  else if (y + endj > HEIGHT) endj = HEIGHT;

  for (uint8_t j = firstj; j < endj; j++) {
    for (uint8_t i = firsti; i < endi; i++) {
      if (pgm_read_word_near(&(block[j][i])) == SKELETON) continue;
      else backScreen[y + j][x + i] = pgm_read_word_near(&(block[j][i]));
    }
  }
  return;
}

void ATM0130::drawBlock_16px(int16_t x, int16_t y, const uint16_t (&block)[16][16]) {
  uint8_t firsti = 0, endi = 16;
  uint8_t firstj = 0, endj = 16; //firsti,firstjを経由しないと下の二重ループがちゃんと機能しない。
  if (x < 0) firsti = abs(x);
  else if (x + endi > WIDTH) endi = WIDTH;
  if (y < 0) firstj = abs(y);
  else if (y + endj > HEIGHT) endj = HEIGHT;

  for (uint8_t j = firstj; j < endj; j++) {
    for (uint8_t i = firsti; i < endi; i++) {
      if (pgm_read_word_near(&(block[j][i])) == SKELETON) continue;
      else backScreen[y + j][x + i] = pgm_read_word_near(&(block[j][i]));
    }
  }
  return;
}

void ATM0130::drawBlock_16px(int16_t x, int16_t y, const uint16_t (&block)[16][16], uint8_t cut_top, uint8_t cut_bottom, uint8_t cut_left, uint8_t cut_right) {
  volatile uint8_t firsti = cut_left, endi = 16 - cut_right;
  volatile uint8_t firstj = cut_top , endj = 16 - cut_bottom;  //firsti,firstjを経由しないと下の二重ループがちゃんと機能しない。

  if (x < 0) firsti = abs(x);
  else if (x + endi > WIDTH) endi = WIDTH;
  if (y < 0) firstj = abs(y);
  else if (y + endj > HEIGHT) endj = HEIGHT;

  for (uint8_t j = firstj; j < endj; j++) {
    for (uint8_t i = firsti; i < endi; i++) {
      if (pgm_read_word_near(&(block[j][i])) == SKELETON) continue;
      backScreen[y + j][x + i] = pgm_read_word_near(&(block[j][i]));
    }
  }
  return;
}

void ATM0130::drawBlock_32px(int16_t x, int16_t y, const uint16_t (&block)[32][32]) {
  uint8_t firsti = 0, endi = 32;
  uint8_t firstj = 0, endj = 32; //firsti,firstjを経由しないと下の二重ループがちゃんと機能しない。
  if (x < 0) firsti = abs(x);
  else if (x + endi > WIDTH) endi = WIDTH;
  if (y < 0) firstj = abs(y);
  else if (y + endj > HEIGHT) endj = HEIGHT;

  for (uint8_t j = firstj; j < endj; j++) {
    for (uint8_t i = firsti; i < endi; i++) {
      if (pgm_read_word_near(&(block[j][i])) == SKELETON) continue;
      backScreen[y + j][x + i] = pgm_read_word_near(&(block[j][i]));
    }
  }
  return;
}


void ATM0130::drawBlock(int16_t x, int16_t y, uint8_t imagex, uint8_t imagey, uint8_t sizex, uint8_t sizey, const uint16_t (&block)[120][120]) {
  uint8_t firsti = 0, endi = sizex;
  uint8_t firstj = 0, endj = sizey;
  
  if (x < 0) firsti -= x;
  else if (x + endi >= WIDTH) endi = WIDTH - 1;
  if (y < 0) firstj -= y;
  else if (y + endj >= HEIGHT) endj = HEIGHT - 1;

  for (uint16_t j = firstj; j < endj; j++) {
    for (uint16_t i = firsti; i < endi; i++) {
      if (pgm_read_word_near(&(block[imagey + j][imagex + i])) == SKELETON || x+i < 0 || x+i >= WIDTH) continue;
      backScreen[y + j][x + i] = pgm_read_word_near(&(block[imagey + j][imagex + i]));
    }
  }
  return;
}

void ATM0130::drawFlipBlock_4px(int16_t x, int16_t y, const uint16_t (&block)[4][4]) {
  uint8_t firsti = 0, endi = 4;
  uint8_t firstj = 0, endj = 4; //firsti,firstjを経由しないと下の二重ループがちゃんと機能しない。
  if (x < 0) firsti = abs(x);
  else if (x + endi > WIDTH) endi = WIDTH;
  if (y < 0) firstj = abs(y);
  else if (y + endj > HEIGHT) endj = HEIGHT;

  for (uint8_t j = firstj; j < endj; j++) {
    for (uint8_t i = firsti; i < endi; i++) {
      if (pgm_read_word_near(&(block[j][i])) == SKELETON) continue;
      backScreen[y + j][x + endi - 1 - i] = pgm_read_word_near(&(block[j][i]));
    }
  }
  return;
}

void ATM0130::drawFlipBlock_8px(int16_t x, int16_t y, const uint16_t (&block)[8][8]) {
  uint8_t firsti = 0, endi = 8;
  uint8_t firstj = 0, endj = 8; //firsti,firstjを経由しないと下の二重ループがちゃんと機能しない。
  if (x < 0) firsti = abs(x);
  else if (x + endi > WIDTH) endi = WIDTH;
  if (y < 0) firstj = abs(y);
  else if (y + endj > HEIGHT) endj = HEIGHT;

  for (uint8_t j = firstj; j < endj; j++) {
    for (uint8_t i = firsti; i < endi; i++) {
      if (pgm_read_word_near(&(block[j][i])) == SKELETON) continue;
      backScreen[y + j][x + endi - 1 - i] = pgm_read_word_near(&(block[j][i]));
    }
  }
  return;
}

void ATM0130::drawFlipBlock_16px(int16_t x, int16_t y, const uint16_t (&block)[16][16]) {
  uint8_t firsti = 0, endi = 16;
  uint8_t firstj = 0, endj = 16; //firsti,firstjを経由しないと下の二重ループがちゃんと機能しない。
  if (x < 0) firsti = abs(x);
  else if (x + endi > WIDTH) endi = WIDTH;
  if (y < 0) firstj = abs(y);
  else if (y + endj > HEIGHT) endj = HEIGHT;

  for (uint8_t j = firstj; j < endj; j++) {
    for (uint8_t i = firsti; i < endi; i++) {
      if (pgm_read_word_near(&(block[j][i])) == SKELETON) continue;
      backScreen[y + j][x + endi - 1 - i] = pgm_read_word_near(&(block[j][i]));
    }
  }
  return;
}

void ATM0130::drawFlipBlock_32px(int16_t x, int16_t y, const uint16_t (&block)[32][32]) {
  uint8_t firsti = 0, endi = 32;
  uint8_t firstj = 0, endj = 32; //firsti,firstjを経由しないと下の二重ループがちゃんと機能しない。
  if (x < 0) firsti = abs(x);
  else if (x + endi > WIDTH) endi = WIDTH;
  if (y < 0) firstj = abs(y);
  else if (y + endj > HEIGHT) endj = HEIGHT;

  for (uint8_t j = firstj; j < endj; j++) {
    for (uint8_t i = firsti; i < endi; i++) {
      if (pgm_read_word_near(&(block[j][i])) == SKELETON) continue;
      backScreen[y + j][x + endi - 1 - i] = pgm_read_word_near(&(block[j][i]));
    }
  }
  return;
}

void ATM0130::drawFlipBlock(int16_t x, int16_t y, uint8_t imagex, uint8_t imagey, uint8_t sizex, uint8_t sizey, const uint16_t (&block)[120][120]) {
  uint8_t firsti = 0, endi = sizex;
  uint8_t firstj = 0, endj = sizey;

  
  if (x < 0) firsti = abs(x);
  else if (x + endi > WIDTH) endi = WIDTH;
  if (y < 0) firstj = abs(y);
  else if (y + endj > HEIGHT) endj = HEIGHT;

  for (uint16_t j = firstj; j < endj; j++) {
    for (uint16_t i = firsti; i < endi; i++) {
      if (pgm_read_word_near(&(block[imagey + j][imagex + i])) == SKELETON) continue;
      backScreen[y + j][x + endi - 1 - i] = pgm_read_word_near(&(block[imagey + j][imagex + i]));
    }
  }
  return;
}

void ATM0130::drawRotateBlock_4px(int16_t x, int16_t y, const uint16_t (&block)[4][4]){
  uint8_t firsti = 0, endi = 4;
  uint8_t firstj = 0, endj = 4; //firsti,firstjを経由しないと下の二重ループがちゃんと機能しない。
  if (x < 0) firsti = abs(x);
  else if (x + endi > WIDTH) endi = WIDTH;
  if (y < 0) firstj = abs(y);
  else if (y + endj > HEIGHT) endj = HEIGHT;

  for (uint8_t j = firstj; j < endj; j++) {
    for (uint8_t i = firsti; i < endi; i++) {
      if (pgm_read_word_near(&(block[j][i])) == SKELETON) continue;
      backScreen[y + i][x + j] = pgm_read_word_near(&(block[j][i]));
    }
  }
  return;
}

void ATM0130::drawRotateFlipBlock_4px(int16_t x, int16_t y, const uint16_t (&block)[4][4]){
  uint8_t firsti = 0, endi = 4;
  uint8_t firstj = 0, endj = 4; //firsti,firstjを経由しないと下の二重ループがちゃんと機能しない。
  if (x < 0) firsti = abs(x);
  else if (x + endi > WIDTH) endi = WIDTH;
  if (y < 0) firstj = abs(y);
  else if (y + endj > HEIGHT) endj = HEIGHT;

  for (uint8_t j = firstj; j < endj; j++) {
    for (uint8_t i = firsti; i < endi; i++) {
      if (pgm_read_word_near(&(block[j][i])) == SKELETON) continue;
      backScreen[y + i][x + endj - 1 - j] = pgm_read_word_near(&(block[j][i]));
    }
  }
  return;
}

void ATM0130::drawRotateBlock_16px(int16_t x, int16_t y, const uint16_t (&block)[16][16]){
  uint8_t firsti = 0, endi = 16;
  uint8_t firstj = 0, endj = 16; //firsti,firstjを経由しないと下の二重ループがちゃんと機能しない。
  if (x < 0) firsti = abs(x);
  else if (x + endi > WIDTH) endi = WIDTH;
  if (y < 0) firstj = abs(y);
  else if (y + endj > HEIGHT) endj = HEIGHT;

  for (uint8_t j = firstj; j < endj; j++) {
    for (uint8_t i = firsti; i < endi; i++) {
      if (pgm_read_word_near(&(block[j][i])) == SKELETON) continue;
      backScreen[y + i][x + j] = pgm_read_word_near(&(block[j][i]));
    }
  }
  return;
}

void ATM0130::drawRotateFlipBlock_16px(int16_t x, int16_t y, const uint16_t (&block)[16][16]){
  uint8_t firsti = 0, endi = 16;
  uint8_t firstj = 0, endj = 16; //firsti,firstjを経由しないと下の二重ループがちゃんと機能しない。
  if (x < 0) firsti = abs(x);
  else if (x + endi > WIDTH) endi = WIDTH;
  if (y < 0) firstj = abs(y);
  else if (y + endj > HEIGHT) endj = HEIGHT;

  for (uint8_t j = firstj; j < endj; j++) {
    for (uint8_t i = firsti; i < endi; i++) {
      if (pgm_read_word_near(&(block[j][i])) == SKELETON) continue;
      backScreen[y + i][x + endj - 1 - j] = pgm_read_word_near(&(block[j][i]));
    }
  }
  return;
}

void ATM0130::drawBlock_4px(int16_t x, int16_t y, const uint16_t (&block)[4][4],bool flip) {
  if(flip) drawFlipBlock_4px(x,y,block);
  else     drawBlock_4px(x,y,block);    
}

void ATM0130::drawBlock_8px(int16_t x, int16_t y, const uint16_t (&block)[8][8],bool flip) {
  if(flip) drawFlipBlock_8px(x,y,block);
  else     drawBlock_8px(x,y,block);    
}

void ATM0130::drawBlock_16px(int16_t x, int16_t y, const uint16_t (&block)[16][16],bool flip) {
  if(flip) drawFlipBlock_16px(x,y,block);
  else     drawBlock_16px(x,y,block);    
}

void ATM0130::drawBlock_32px(int16_t x, int16_t y, const uint16_t (&block)[32][32],bool flip) {
  if(flip) drawFlipBlock_32px(x,y,block);
  else     drawBlock_32px(x,y,block);    
}

void ATM0130::updateScreen() {
  uint8_t colorH;
  uint8_t colorL;
  SPI.beginTransaction(spi_settings);
  digitalWrite(CS, LOW);
  setWindow(0, 0, WIDTH * 2, HEIGHT * 2);

  digitalWrite(dat_cmd, HIGH);
  for (uint8_t i = 0; i < HEIGHT; i++) {
    uint8_t j = 0;
    for (j = 0; j < WIDTH; j++) {
      colorH = backScreen[i][j] >> 8;
      colorL = backScreen[i][j] & 0x00FF;
      SPI.transfer(colorH);
      SPI.transfer(colorL);
      SPI.transfer(colorH);
      SPI.transfer(colorL);
    }
    for (j = 0; j < WIDTH; j++) {
      colorH = backScreen[i][j] >> 8;
      colorL = backScreen[i][j] & 0x00FF;
      SPI.transfer(colorH);
      SPI.transfer(colorL);
      SPI.transfer(colorH);
      SPI.transfer(colorL);
    }
  }
  digitalWrite(CS, HIGH);
  SPI.endTransaction();

  this->frame++;
}

void ATM0130::clearScreen(uint16_t color) {
  for (int i = 0; i < WIDTH; i++) {
    for (int j = 0; j < HEIGHT; j++) {
      backScreen[j][i] = color;
    }
  }
}

void ATM0130::drawDot(int16_t x, int16_t y) {
  if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) return;
  backScreen[y][x] = color;
}

void ATM0130::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1) {
  uint8_t x, y, tmp;
  double slope = 0;

  if (x0 <       0) x0 =          0;
  if (y0 <       0) y0 =          0;
  if (x0 >=  WIDTH) x0 = WIDTH  - 1;
  if (y0 >= HEIGHT) y0 = HEIGHT - 1;
  if (x1 <       0) x1 =          0;
  if (y1 <       0) y1 =          0;
  if (x1 >=  WIDTH) x1 = WIDTH  - 1;
  if (y1 >= HEIGHT) y1 = HEIGHT - 1;
  if (x0 == x1 && y0 == y1) {
    drawDot(x0, y0);
    return;
  }

  if (abs(x1 - x0) > abs(y1 - y0)) {
    if (x1 < x0) {                         // 座標値の入れ替え
      tmp = x0;
      x0 = x1;
      x1 = tmp;
      tmp = y0;
      y0 = y1;
      y1 = tmp;
    }
    slope = (double)(y1 - y0) / (x1 - x0); // 傾斜率の算出
    for (int i = 0; i <= (x1 - x0); i++) {
      x = x0 + i;
      y = y0 + slope * i;
      drawDot(x, y);
    }
  } else {
    if (y1 < y0) {                         // 座標値の入れ替え
      tmp = x0;
      x0 = x1;
      x1 = tmp;
      tmp = y0;
      y0 = y1;
      y1 = tmp;
    }
    slope = (double)(x1 - x0) / (y1 - y0); // 傾斜率の算出
    for (int i = 0; i <= (y1 - y0); i++) {
      x = x0 + slope * i;
      y = y0 + i;
      drawDot(x, y);
    }
  }
}

/*****四角形を描画する*****/
void ATM0130::drawRectangle(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
  drawLine(x0, y0, x1, y0);
  drawLine(x1, y0, x1, y1);
  drawLine(x0, y0, x0, y1);
  drawLine(x0, y1, x1, y1);
}

void ATM0130::drawFillRectangle(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
  for (uint8_t i = x0; i <= x1; i++) {
    for (uint8_t j = y0; j <= y1; j++) {
      backScreen[j][i] = color;
    }
  }
}

void ATM0130::drawCircle(uint8_t x0, uint8_t y0, uint8_t r) {
  int16_t x;
  int16_t y;
  int16_t err;
  int16_t old_err;

  x = 0;
  y = -r;
  err = 2 - 2 * r;
  do {
    drawDot(x0 - x, y0 + y);
    drawDot(x0 - y, y0 - x);
    drawDot(x0 + x, y0 - y);
    drawDot(x0 + y, y0 + x);
    if ((old_err = err) <= x)   err += ++x * 2 + 1;
    if (old_err > y || err > x) err += ++y * 2 + 1;
  } while (y < 0);

  return ;
}

void ATM0130::drawFillCircle(uint8_t x0, uint8_t y0, uint8_t r) {
  int16_t x;
  int16_t y;
  int16_t dpy;
  int16_t dmy;
  int16_t err;
  int16_t old_err;
  bool ChangeX;

  x = 0;
  y = -r;
  err = 2 - 2 * r;
  ChangeX = true;
  do {
    if (ChangeX) {
      if (y0 - y < 0) {
        dmy = 0;
      }
      else dmy = y0 - y;
      if (y0 + y < 0) {
        dpy = 0;
      }
      else dpy = y0 + y;

      drawLine(x0 - x, dmy, x0 - x, dpy);
      drawLine(x0 + x, dmy, x0 + x, dpy);
    } // if
    ChangeX = (old_err = err) <= x;
    if (ChangeX)            err += ++x * 2 + 1;
    if (old_err > y || err > x) err += ++y * 2 + 1;
  } while (y <= 0);

  return;
}

void ATM0130::putStr(int16_t Xp, int16_t Yp, const char * s) {
  int16_t x, y;

  x = Xp ;
  y = Yp ;
  while (*s) {
    if (*s == '\n') {
      x = Xp;
      y = y + 8;
      *s++;
    }
    else if (*s == '\0') {
      break;
    }
    else if (x + 4 > WIDTH) {
      x = Xp;
      y = y + 8;
    }
    putCharacter(x, y, *s++);
    x = x + 6 ;  // 文字と文字の間は１ドットあける
  }
}

void ATM0130::putStr(int16_t Xp, int16_t Yp, String str){
  int16_t x, y;
  char s[50];
  str.toCharArray(s,50);

  x = Xp ;
  y = Yp ;
  for(uint8_t num=0;num<50;num++) {
    if (s[num] == '\n') {
      x = Xp;
      y = y + 8;
      num++;
    }
    else if (s[num] == '\0') {
      break;
    }
    else if (x + 4 > WIDTH) {
      x = Xp;
      y = y + 8;
    }
    putCharacter(x, y, s[num]);
    x = x + 6 ;  // 文字と文字の間は１ドットあける
  }
}

void ATM0130::putCharacter(int16_t x, int16_t y, char c) {
  // 8x5の文字を描画する
  if ((c < 0x20 || c > 0xDF) && c != 192) return;
  for (uint8_t i = 0; i < 5; i++) {
    for (uint8_t j = 0; j < 8; j++) {
      if((pgm_read_byte_near(&(font[c - 0x20][i])) >> j) & 0b1 == 0b1) backScreen[y + j][x + i] = color;
    }
  }
}
