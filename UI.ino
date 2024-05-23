#include "ATM0130.h"

//UIの定義はGameObjct.hの中。買える事。

UI::UI() {
  /*
    moves[0] = &UI::moveTouch;
    moves[1] = &UI::moveEat;
    moves[2] = &UI::moveInfo;
    moves[3] = &UI::moveSave;
    moves[4] = &UI::movePowor;
    moves[5] = &UI::moveSetting;*/

  this->x = 104;
  this->y = 104;
  this->sizex = 120;
  this->sizey = 16;
  this->show = true;

  this->menuOpen = false;
  this->UIcursor = 0;
  this->secondCursor = 0;
}



void UI::moveTouch() {
  if (device.getButtonState(BUTTON_A, BUTTON_PRESSED)) {
    if (this->secondCursor == 0) {
      if (character.state == STATE_SLEEP) this->secondCursor++;
      character.changeState(STATE_STROKE);
      character.happiness++;
    }
    if (this->secondCursor == 1) {
      if (character.state == STATE_SLEEP) character.changeState(STATE_NORMAL);
      else character.changeState(STATE_SLEEP);
      this->menuOpen = !(this->menuOpen);
    }
    else if (this->secondCursor == 2) this->menuOpen = !(this->menuOpen);
  }
  else if (device.getButtonState(BUTTON_B, BUTTON_PRESSED)) {
    if (character.state != STATE_STROKE || character.state != STATE_STROKE_HAPPY) this->secondCursor++;
  }
  if ((character.state == STATE_SLEEP || character.state == STATE_DIE) && this->secondCursor == 0) this->secondCursor++;
  this->secondCursor = secondCursor % 3;
}

void UI::foodDecision() {
  uint8_t num = micros() % 100;
  if (num == 0) foodType = FOOD_ANKONAN;
  else {
    foodType = micros() % (FOOD_IMAGENUM - 1);
  }
}

void UI::moveEat() {
  if (character.state != STATE_EAT) {
    if (device.getButtonState(BUTTON_A, BUTTON_PRESSED)) {
      if (this->secondCursor == 0) {
        foodDecision();
        character.changeState(STATE_EAT);
        character.eat(foodType);
      }
      if (this->secondCursor == 1) {
        foodDecision();
        //character.stock(foodType);
      }
      else if (this->secondCursor == 2) this->menuOpen = !(this->menuOpen);
    }
    else if (device.getButtonState(BUTTON_B, BUTTON_PRESSED)) {
      this->secondCursor++;
    }
  }
  if ((character.state == STATE_SLEEP || character.state == STATE_DIE) && this->secondCursor == 0) this->secondCursor++;
  this->secondCursor = secondCursor % 3;
}

void UI::moveInfo() {
  if (device.getButtonState(BUTTON_A, BUTTON_PRESSED)) {
    this->menuOpen = !(this->menuOpen);
  }
}

void UI::moveSave() {
  /*device.save(
    &character.life,
    &character.stomach,
    &character.favorability,
    &character.hiddenFavorability
    );*/
  if (device.getButtonState(BUTTON_A, BUTTON_PRESSED)) {
    this->menuOpen = !(this->menuOpen);
  }
}

void UI::movePowor() {
  if (device.getButtonState(BUTTON_A, BUTTON_PRESSED)) {
    this->menuOpen = !(this->menuOpen);
  }
}

void UI::moveSetting() {
  this->secondCursor++;
  if (device.getButtonState(BUTTON_A, BUTTON_PRESSED)) {
    this->menuOpen = !(this->menuOpen);
  }
}

/* UIの主処理 */
void UI::move() {
  if (menuOpen) {
    if      (this->UIcursor == ICON_HAND   && character.state != STATE_DIE) this->moveTouch();
    else if (this->UIcursor == ICON_FOOD  && character.state != STATE_DIE) this->moveEat();
    else if (this->UIcursor == ICON_INFO   ) this->moveInfo();
    else if (this->UIcursor == ICON_SAVE   ) this->moveSave();
    else if (this->UIcursor == ICON_POWER  ) this->movePowor();
    else if (this->UIcursor == ICON_SETTING) this->moveSetting();
    else {
      if (device.getButtonState(BUTTON_A, BUTTON_PRESSED)) this->menuOpen = !(this->menuOpen);
    }
  }
  else {
    if (device.getButtonState(BUTTON_A, BUTTON_PRESSED)) {
      this->menuOpen = !this->menuOpen;
      this->secondCursor = 0;
    }
    else if (device.getButtonState(BUTTON_B, BUTTON_PRESSED)) {
      this->UIcursor++;
      this->UIcursor = UIcursor % 7;
    }
  }
}

void UI::drawMenubar() {
  //白地
  myATM0130.setColor(WHITE16);
  myATM0130.drawFillRectangle(1, 104, 119, 119);
  myATM0130.drawLine(0, 104, 0, 120);
  myATM0130.drawLine(119, 104, 119, 120);

  myATM0130.setColor(BLACK16);
  myATM0130.drawLine(1, 103, 118, 103);
  myATM0130.drawDot(0, 104);
  //icon描画
  for (uint8_t i = 0; i < 6; i++) myATM0130.drawBlock_16px(1 + i * 15, 104, icon[i]);
  if (character.state == STATE_DIE) {
    myATM0130.drawBlock_16px(1, 104, icon[ICON_NO]);
    myATM0130.drawBlock_16px(16, 104, icon[ICON_NO]);
  }
  //カーソル表示
  if (this->UIcursor != 6) {
    myATM0130.setColor(RED16);
    myATM0130.drawLine(  1 + this->UIcursor * 15, 106,  1 + this->UIcursor * 15, 117);
    myATM0130.drawLine( 16 + this->UIcursor * 15, 106, 16 + this->UIcursor * 15, 117);
    myATM0130.drawLine(  3 + this->UIcursor * 15, 104, 14 + this->UIcursor * 15, 104);
    myATM0130.drawLine(  3 + this->UIcursor * 15, 119, 14 + this->UIcursor * 15, 119);
    myATM0130.drawDot(   2 + this->UIcursor * 15, 105);
    myATM0130.drawDot(   2 + this->UIcursor * 15, 118);
    myATM0130.drawDot(  15 + this->UIcursor * 15, 105);
    myATM0130.drawDot(  15 + this->UIcursor * 15, 118);
  }

  this->drawClock(106, 106);
}


void UI::drawTouch() {
  if      (secondCursor == 0) {
    if (character.state != STATE_STROKE && character.state != STATE_STROKE_HAPPY) {
      myATM0130.drawBlock_16px(character.x + 8, character.y - 16, icon[ICON_POINT1]);
    }
  }
  if (secondCursor == 1) {
    myATM0130.drawBlock_16px(82, 38, icon[ICON_POINT1]);
  }
  else if (secondCursor == 2) {
    myATM0130.drawBlock_16px(50, 99, icon[ICON_POINT1]);
  }

  myATM0130.setColor(WHITE16);
  myATM0130.drawFillRectangle(1, 116, 119, 119);
  myATM0130.drawLine(0, 116, 0, 120);
  myATM0130.drawLine(119, 116, 119, 120);

  myATM0130.setColor(BLACK16);
  myATM0130.drawLine(1, 115, 118, 115);
  myATM0130.drawDot(0, 116);
  myATM0130.drawDot(119, 116);
}

void UI::drawEat() {
  if (character.state == STATE_EAT) {
    myATM0130.drawBlock_16px(character.x + 2, character.y + 12, foods[foodType], character.animationNum , 0, 0, 0);
    myATM0130.drawBlock_16px(character.x + 2, character.y + 12, icon[ICON_SUPPORT]);
  }
  else {
    if      (secondCursor == 0) {
      myATM0130.drawBlock_16px(character.x + 8, character.y - 16, icon[ICON_POINT1]);
    }
    else if (secondCursor == 1) {
      myATM0130.drawBlock_16px(8, 55, icon[ICON_POINT1]);
    }
    else if (secondCursor == 2) {
      myATM0130.drawBlock_16px(50, 99, icon[ICON_POINT1]);
    }
  }
  myATM0130.setColor(WHITE16);
  myATM0130.drawFillRectangle(1, 116, 119, 119);
  myATM0130.drawLine(0, 116, 0, 120);
  myATM0130.drawLine(119, 116, 119, 120);

  myATM0130.setColor(BLACK16);
  myATM0130.drawLine(1, 115, 118, 115);
  myATM0130.drawDot(0, 116);
  myATM0130.drawDot(119, 116);
}

void UI::drawClock(int16_t x, int16_t y) {
  float Rad;
  float sinRad;
  float cosRad;
  //盤
  myATM0130.setColor(WHITE16);
  myATM0130.drawFillCircle(x, y, 13);

  if (device.isTimeConfigured) {
    myATM0130.setColor(BLACK16);
  }
  else {
    if      (device.isWiFiConnected) myATM0130.setColor(GREEN16);
    else if (device.tryWiFiConnect) myATM0130.setColor(YELLOW16);
    else myATM0130.setColor(RED16);
  }
  myATM0130.drawCircle(x, y, 13);
  myATM0130.drawCircle(x, y, 12);

  device.getTime();
  //長針
  myATM0130.setColor(BLACK16);
  Rad = (360 * ((device.getMinute() % 60) / 60.0) - 90) / (180 / PI);
  sinRad = sin(Rad);
  cosRad = cos(Rad);
  myATM0130.drawLine(x  , y  , x + cosRad * 9, y + sinRad * 9);
  myATM0130.drawLine(x - 1, y  , x + cosRad * 9, y + sinRad * 9);
  myATM0130.drawLine(x + 1, y  , x + cosRad * 9, y + sinRad * 9);
  myATM0130.drawLine(x  , y - 1, x + cosRad * 9, y + sinRad * 9);
  myATM0130.drawLine(x  , y + 1, x + cosRad * 9, y + sinRad * 9);

  //短針
  myATM0130.setColor(RED16);
  //Rad = (360 * ((device.getHour() % 12) / 12.0) - 90) / (180 / PI);
  Rad = (360 * ((( (device.getHour() % 12) * 5 + device.getMinute() / 12 ) % 60) / 60.0) - 90) / (180 / PI);
  sinRad = sin(Rad);
  cosRad = cos(Rad);
  myATM0130.drawLine(x  , y  , x + cosRad * 5, y + sinRad * 5);
  myATM0130.drawLine(x - 1, y  , x + cosRad * 5, y + sinRad * 5);
  myATM0130.drawLine(x + 1, y  , x + cosRad * 5, y + sinRad * 5);
  myATM0130.drawLine(x  , y - 1, x + cosRad * 5, y + sinRad * 5);
  myATM0130.drawLine(x  , y + 1, x + cosRad * 5, y + sinRad * 5);

  //秒針
  myATM0130.setColor(BLUE16);
  Rad = (360 * ((device.getSecond() % 60) / 60.0) - 90) / (180 / PI);
  sinRad = sin(Rad);
  cosRad = cos(Rad);
  myATM0130.drawLine(x, y, x + cosRad * 11, y + sinRad * 11);

}

void UI::drawInfo() {
  myATM0130.setColor(WHITE16);
  myATM0130.drawFillRectangle(1, 85, 118, 119);
  myATM0130.drawLine(0, 85, 0, 120);
  myATM0130.drawLine(119, 85, 119, 120);
  myATM0130.setColor(BLACK16);
  myATM0130.drawLine(1, 84, 118, 84);
  myATM0130.drawDot(0, 85);
  myATM0130.drawDot(119, 85);

  myATM0130.drawLine(68, 84, 68, 119);

  myATM0130.drawBlock_32px(1, 86, character_image[0]);
  myATM0130.drawBlock_16px(34, 86, icon[ICON_GRAYMUSICNOTE]);
  myATM0130.drawBlock_16px(34, 86, icon[ICON_MUSICNOTE], (uint8_t)(16 - 16.0 * (((double)character.happiness) / 100.0)), 0, 0, 0);
  myATM0130.drawBlock_16px(34, 102, icon[ICON_GRAYSTMACH]);
  myATM0130.drawBlock_16px(34, 102, icon[ICON_STMACH], (uint8_t)(16 - 16.0 * (((double)character.stomach) / 100.0)), 0, 0, 0);
  myATM0130.drawBlock_16px(52, 94, icon[ICON_GRAYSLEEP]);
  myATM0130.drawBlock_16px(52, 94, icon[ICON_SLEEP], (uint8_t)(16 - 16.0 * (((double)character.sleepiness) / 100.0)), 0, 0, 0);

  //冷蔵庫
  myATM0130.drawBlock(70, 92, 32, 16, 16, 24, imagemap);


  //デバッグ用
  myATM0130.setColor(BLUE16);
  myATM0130.putStr(0,  1, String("LT:" + String(character.loopTime) + " S:" + String(character.state) + " AN:" + String(character.animationNum)));
  myATM0130.setColor(GREEN16);
  myATM0130.putStr(0, 9, String("HP:" + String(character.life)+ " LOVE:") + String(character.love));
  myATM0130.setColor(RED16);
  myATM0130.putStr(0, 17, String("Fav:" + String(character.favorability)+ " LOVE:") + String(character.love));
  
}

void UI::drawSave() {
  //moveSave();
}

void UI::drawSetting() {
  if (!device.isServerStarted) {
    device.serverBegin();
  }
  myATM0130.clearScreen(BLACK16);
  myATM0130.setColor(GREEN16);
  myATM0130.putStr(0, 1, "Please Access to\0");
  myATM0130.putStr(0, 9, "SSID:\0");
  myATM0130.putStr(0, 17, device.getServerssid());
  myATM0130.putStr(0, 25, "PASS:\0");
  myATM0130.putStr(0, 33, device.getServerpass());
  myATM0130.setColor(WHITE16);
  myATM0130.putStr(0, 49, "and search URL:\nhttp://192.168.4.1/\0");
  myATM0130.setColor(RED16);
  myATM0130.putStr(0, 73, "EXIT:\n PRESS LEFT BUTTON\0");
  this->menuOpen = false;
}

void UI::draw() {
  if (this->menuOpen) {
    if      (this->UIcursor == ICON_HAND   && character.state != STATE_DIE) this->drawTouch();
    else if (this->UIcursor == ICON_FOOD   && character.state != STATE_DIE) this->drawEat();
    else if (this->UIcursor == ICON_INFO   ) this->drawInfo();
    else if (this->UIcursor == ICON_SAVE   ) this->drawSave();
    else if (this->UIcursor == ICON_POWER  ) ;
    else if (this->UIcursor == ICON_SETTING) this->drawSetting();
  }
  else {
    if (device.isServerStarted) {
      device.serverEnd();
    }
    this->drawMenubar();
  }
}
