#include "ATM0130.h"

BackGround::BackGround() {
  this->x = 0;
  this->y = 0;
  this->sizex = 120;
  this->sizey = 120;
  this->show = true;
}

BackGround::Cloud::Cloud() {
  this->imageNum = micros() % 7;
  if (this->imageNum <= 2) {
    this->downSpeed = 3 + micros() % 3;
    this->y = 5 + micros() % 20;
  }
  else {
    this->downSpeed = 6 + micros() % 3;
    this->y = 15 + micros() % 15;
  }

  this->x = micros() % 120;
}

void BackGround::Cloud::move() {
  if (myATM0130.frame % downSpeed == 0) this->x--;
  if (this->x <= 0) {
    this->x = 100;
    this->imageNum = micros() % 7;
    if (this->imageNum <= 2) {
      this->downSpeed = 3 + micros() % 3;
      this->y = 5 + micros() % 20;
    }
    else {
      this->downSpeed = 6 + micros() % 3;
      this->y = 15 + micros() % 15;
    }
  }
}

void BackGround::Cloud::draw() {
  if      (this->imageNum == 0)
    myATM0130.drawBlock( this->x % 120, this->y,  0, 16, 16, 16, imagemap);
  else if (this->imageNum == 1)
    myATM0130.drawBlock( this->x % 120, this->y,  0, 32, 16, 16, imagemap);
  else if (this->imageNum == 2)
    myATM0130.drawBlock( this->x % 120, this->y, 16, 16, 16, 16, imagemap);
  else if (this->imageNum == 3)
    myATM0130.drawBlock( this->x % 120, this->y, 16, 32,  8,  8, imagemap);
  else if (this->imageNum == 4)
    myATM0130.drawBlock( this->x % 120, this->y, 24, 32,  8,  8, imagemap);
  else if (this->imageNum == 5)
    myATM0130.drawBlock( this->x % 120, this->y, 16, 40,  8,  8, imagemap);
  else if (this->imageNum == 6)
    myATM0130.drawBlock( this->x % 120, this->y, 24, 40,  8,  8, imagemap);
}


/* BackGround 主処理 */
void BackGround::move() {
  for (uint8_t i = 0; i < CLOUD_NUM; i++) {
    clouds[i].move();
  }
}

void BackGround::drawOutSide() {
  uint8_t i;
  //空
  myATM0130.setColor(0x02bd);
  myATM0130.drawFillRectangle(33, 17, 58, 23);
  myATM0130.drawFillRectangle(61, 17, 86, 23);
  for (i = 33; i <= 86; i += 2) {
    myATM0130.drawDot(i, 24);
    myATM0130.drawDot(i + 1, 25);
  }
  myATM0130.setColor(0x645e);
  for (i = 33; i <= 86; i += 2) {
    myATM0130.drawDot(i + 1, 24);
    myATM0130.drawDot(i, 25);
  }
  myATM0130.drawFillRectangle(33, 26, 58, 39);
  myATM0130.drawFillRectangle(61, 26, 86, 39);

  //雲
  myATM0130.drawBlock( (120 - myATM0130.frame) % 120, 20,  0, 16, 16, 16, imagemap);
  myATM0130.drawBlock( (190 - myATM0130.frame / 2) % 120, 25, 16, 32,  8,  8, imagemap);

  for (i = 0; i < CLOUD_NUM; i++) {
    clouds[i].draw();
  }

  //myATM0130.drawBlock( myATM0130.frame%120, 20,  0, 16, 16, 16, imagemap);
  //myATM0130.drawBlock( (myATM0130.frame/2+20)%120, 30, 16, 32,  8,  8, imagemap);

  //海
  myATM0130.drawBlock(33, 40, 56, 0, 24, 14, imagemap);
  myATM0130.drawFlipBlock(64, 40, 56, 0, 24, 14, imagemap);
  myATM0130.setColor(0x1ddc);
  myATM0130.drawFillRectangle(57, 40, 59, 44);
  myATM0130.drawFillRectangle(61, 40, 63, 44);
  myATM0130.setColor(0x5e9d);
  myATM0130.drawFillRectangle(57, 45, 59, 53);
  myATM0130.drawFillRectangle(61, 45, 63, 53);

}


void BackGround::draw() {
  myATM0130.clearScreen(0xf739);
  drawOutSide();

  myATM0130.setColor(0xf739);
  myATM0130.drawFillRectangle(31,  0,  88, 14);
  myATM0130.drawFillRectangle( 0,  0,  30, 119);
  myATM0130.drawFillRectangle(89,  0, 119, 119);

  myATM0130.setColor(BLACK16);
  myATM0130.drawLine(16, 0, 16, 55);
  myATM0130.drawLine(103, 0, 103, 55);
  myATM0130.drawLine(17, 55, 31, 55);
  myATM0130.drawLine(89, 55, 102, 55);
  myATM0130.drawRectangle(31, 15, 88, 55);
  myATM0130.drawRectangle(32, 16, 87, 54);
  myATM0130.drawRectangle(59, 16, 60, 54);
  myATM0130.drawLine(16, 55, 0, 96);
  myATM0130.drawLine(103, 55, 119, 96);

  
  //植物
  myATM0130.drawBlock(94, 34, 96 + character.love*8,  0, 8, 24, imagemap);

  //ちゃぶ台
  myATM0130.drawBlock(17, 58, 32,  0, 24, 16, imagemap);
  
  //布団
  if(!(character.bodyState == BODY_SLEEP0 || character.bodyState == BODY_SLEEP1))
    myATM0130.drawBlock(70, 55,  0,  0, 32, 16, imagemap);
  //冷蔵庫
  myATM0130.drawBlock( 6, 72, 32, 16, 16, 24, imagemap);
}
