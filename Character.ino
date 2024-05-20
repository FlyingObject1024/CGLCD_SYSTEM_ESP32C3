#include "ATM0130.h"

Character::Character() {
  this->x = 60;
  this->y = 60;
  this->sizex = 32;
  this->sizey = 32;
  this->show = true;
  this->life = 80;
  this->stomach = 50;
  this->stockNum = 0;
  this->sleepiness = 100;
  this->happiness = 50;
  this->favorability = 50;
  this->love = 0;
  this->loopTime = 60;
  this->beforeState = ARRAYSTOPPER - 1;
  this->state = ARRAYSTOPPER;
  this->behavior = 0;
  if (this->life <= 0) {
    changeState(STATE_DIE);
  }
  else {
    changeState(STATE_NORMAL);
    this->eyeState = EYE_NORMAL;
    this->bodyState = BODY_NORMAL1;
    this->mouthState = MOUTH_NORMAL;
    this->flip = false;
  }
}

void Character::eat(uint8_t foodType){
  uint8_t foodNum = (pgm_read_byte(&(foodLevel[foodType])) * 6);
  if(this->stomach + foodNum >= 100) this->stomach = 100;
  else this->stomach += foodNum;
  
  this->sleepiness--;
  this->life++;
  if(this->life >= 100) this->life = 100;
}

void Character::statusChanger() {
  Serial.printf("\nCharacter:\nlife: %d, stomach: %d\nsleepiness: %d, favorability: %d,love: %d\nstate: %d\n\n", life, stomach, sleepiness, favorability, love, state);  
  
  if (this->state == STATE_SLEEP) {
    this->sleepiness++;
    if (this->love >= 0
        && this->stomach >= 90 
        && this->happiness  >= 90 
        && this->life >= 80 
        && this->sleepiness >= 80){
          this->favorability++;
          if(this->favorability >= 90) this->favorability = 90;
    }
    if (this->sleepiness >= 100){
      this->sleepiness = 100;
      changeState(STATE_NORMAL);
    }
  }
  else {
    this->sleepiness--;
    //満腹度に関連する計算
    if (this->stomach > 0) {
      if (this->stomach >= 90 && this->sleepiness >= 10) {
        this->life++;
        this->happiness++;
        if (this->happiness  >= 90 && this->life >= 80) this->favorability++;
        if(this->favorability >= 90){
          this->favorability = 50;
          this->love++;
          if(this->love >= 2) love = 2;
        }
      }
      this->stomach--;
      this->happiness--;
      //if(this->stomach <= 10) changeState(STATE_HUNGRY);
    }
    else {
      this->life--;
      this->happiness -= 2;
    }

    if(this->life >= 100) this->life = 100;
        
    if (this->happiness <= 0) {
      this->happiness = 0;
      changeState(STATE_CALL);
      this->favorability--;
      if (this->favorability <= 10 && this->life <= 20){
        this->favorability = 50;
          this->love--;
          if(this->love <= -2) love = -2;
      }
    }

    if (this->sleepiness <= 5) {
      changeState(STATE_SLEEPY);
      if (this->sleepiness <= 0) this->sleepiness = 0;
    }
  }
  
  if(this->life <= 0){
    this->life = 0;
    changeState(STATE_DIE);
  }
}

void Character::changeState(uint8_t state) {
  this->beforeState = this->state;
  this->state = state;
  if (this->state != this->beforeState) {
    if      (this->state == STATE_SLEEP) animation = (uint8_t*)animations[ANIME_SLEEPING];
    else if (this->state == STATE_SLEEPY) animation = (uint8_t*)animations[ANIME_SLEEPY];
    else if (this->state == STATE_STROKE) animation = (uint8_t*)animations[ANIME_STROKE];
    else if (this->state == STATE_STROKE_HAPPY) animation = (uint8_t*)animations[ANIME_STROKE_HAPPY];
    else if (this->state == STATE_EAT) {
      animation = (uint8_t*)animations[ANIME_EAT];
    }
    else if (this->state == STATE_DIE) animation = (uint8_t*)animations[ANIME_DIE];
    else {
      animation = (uint8_t*)animations[ANIME_NORMAL];
    }
    mouthState = MOUTH_NORMAL;
    this->animationNum = 0;
  }
}

void Character::moveAnimation() {
  this->bodyState = pgm_read_byte(&(*(this->animation + 2 * this->animationNum)));
  this->eyeState = pgm_read_byte(&(*(this->animation + 2 * this->animationNum + 1)));

  if (this->state == STATE_EAT) {
    if (this->animationNum % 4 / 2 % 2 == 0) this->mouthState = MOUTH_OPEN;
    else this->mouthState = MOUTH_CLOSE;
  }

  this->animationNum++;

  if (pgm_read_byte(&(*(animation + 2 * animationNum))) == FLIPBODY) {
    this->flip = !this->flip;
  }
  if (pgm_read_byte(&(*(animation + 2 * animationNum + 1))) == ARRAYSTOPPER) {
    if (this->state == STATE_STROKE) {
      changeState(STATE_STROKE_HAPPY);
      this->happiness++;
    }
    else if(this->state == STATE_STROKE_HAPPY || this->state == STATE_EAT) {
      changeState(STATE_NORMAL);
      this->happiness++;
    }
    this->animationNum = 0;
  }
  Serial.printf("animationNum: %d,body: %d,eye: %d\n", animationNum, this->bodyState, this->eyeState);
}

/* Character 主処理 */
void Character::move() {
  if (myATM0130.frame % 2 == 0) {
    this->loopTime--;
    if (this->loopTime % 10 == 0) statusChanger();
    if (this->loopTime == 0) {
      this->loopTime = 60;
    }
  }
  if (this->state == STATE_NORMAL) {
    if (micros() % 100 <= 10) {
      uint8_t vx = micros() % 11 - 5;
      if (vx >= 0) this->flip = !this->flip;
      this->x += vx;
      this->y += micros() % 11 - 5;
      if (this->x < 0) this->x = 0;
      else if (this->x >= 88) this->x = 87;
      if (this->y < 50) this->y = 50;
      else if (this->y >= 88) this->y = 87;
    }
  }
  moveAnimation();

  if (this->happiness >= 100) this->happiness = 100;
  this->bodyState %= BODY_IMAGENUM;
  this->eyeState %= EYE_IMAGENUM;
}

void Character::drawEye() {
  if (this->eyeState == EYE_HIDE
      || this->bodyState == BODY_SAMPLE
      || this->bodyState == BODY_BACK
      || this->bodyState == BODY_DIE   ) {
    return;
  }
  else if (this->bodyState == BODY_LIE) {
    if (this->flip) {
      myATM0130.drawRotateBlock_4px(x + 6, y + 23, character_eye[this->eyeState * 2]);
      myATM0130.drawRotateBlock_4px(x + 6, y + 30, character_eye[this->eyeState * 2 + 1]);
    }
    else {
      myATM0130.drawRotateFlipBlock_4px(x + 22, y + 23, character_eye[this->eyeState * 2]);
      myATM0130.drawRotateFlipBlock_4px(x + 22, y + 30, character_eye[this->eyeState * 2 + 1]);
    }
  }
  else if (this->bodyState == BODY_SLEEP0
           || this->bodyState == BODY_SLEEP1) {
    this->x = 70;
    this->y = 39;
    this->flip = false;
    if (this->flip) {
      myATM0130.drawRotateBlock_4px(x + 6, y + 21, character_eye[this->eyeState * 2]);
      myATM0130.drawRotateBlock_4px(x + 6, y + 27, character_eye[this->eyeState * 2 + 1]);
    }
    else {
      myATM0130.drawRotateFlipBlock_4px(x + 22, y + 21, character_eye[this->eyeState * 2]);
      myATM0130.drawRotateFlipBlock_4px(x + 22, y + 27, character_eye[this->eyeState * 2 + 1]);
    }
  }
  else if (this->bodyState == BODY_SIT) {
    if (this->flip) {
      myATM0130.drawFlipBlock_4px(x + 17, y + 17, character_eye[this->eyeState * 2]);
      myATM0130.drawFlipBlock_4px(x + 10, y + 17, character_eye[this->eyeState * 2 + 1]);
    }
    else {
      myATM0130.drawBlock_4px(x + 11, y + 17, character_eye[this->eyeState * 2]);
      myATM0130.drawBlock_4px(x + 18, y + 17, character_eye[this->eyeState * 2 + 1]);
    }
  }
  else if (this->state == STATE_STROKE || this->state == STATE_STROKE_HAPPY) {
    if (this->flip) {
      myATM0130.drawFlipBlock_16px(x + 8, y - 4 + animationNum % 4 / 2 % 2, icon[ICON_STROKE]);
      myATM0130.drawFlipBlock_4px(x + 17, y + 10 + animationNum % 4 / 2 % 2, character_eye[this->eyeState * 2]);
      myATM0130.drawFlipBlock_4px(x + 10, y + 10 + animationNum % 4 / 2 % 2, character_eye[this->eyeState * 2 + 1]);
    }
    else {
      myATM0130.drawBlock_16px(x + 9, y - 4 + animationNum % 4 / 2 % 2, icon[ICON_STROKE]);
      myATM0130.drawBlock_4px(x + 11, y + 10 + animationNum % 4 / 2 % 2, character_eye[this->eyeState * 2]);
      myATM0130.drawBlock_4px(x + 18, y + 10 + animationNum % 4 / 2 % 2, character_eye[this->eyeState * 2 + 1]);
    }
  }
  else {
    if (this->flip) {
      myATM0130.drawFlipBlock_4px(x + 17, y + 10, character_eye[this->eyeState * 2]);
      myATM0130.drawFlipBlock_4px(x + 10, y + 10, character_eye[this->eyeState * 2 + 1]);
    }
    else {
      myATM0130.drawBlock_4px(x + 11, y + 10, character_eye[this->eyeState * 2]);
      myATM0130.drawBlock_4px(x + 18, y + 10, character_eye[this->eyeState * 2 + 1]);
    }
  }
}

void Character::drawMouth() {
  int8_t yplus = 12;
  if (this->bodyState == BODY_STROKE) yplus = 13;

  if (this->flip) myATM0130.drawFlipBlock_4px(x + 14, y + yplus, character_mouth[this->mouthState]);
  else myATM0130.drawBlock_4px(x + 14, y + yplus, character_mouth[this->mouthState]);
}

void Character::draw() {
  myATM0130.drawBlock_32px(x, y, character_image[this->bodyState], this->flip);
  drawEye();
  drawMouth();
}
