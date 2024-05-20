/****************
   ※暫く動かすとリセットが掛かる。要修正。(持って300秒程、短いと10秒以内に発生する)
   ->徐々にスタック(ヒープ)が蓄積されてスタックオーバーフローが発生していると思われる。
   ->電流、電圧の問題の可能性がある。(http://www.netosa.com/blog/2017/10/esp-wroom-02-wdt-reset.html)
   ->画像データの問題の可能性
   ->Serial通信の問題の可能性(https://macsbug.wordpress.com/2016/06/20/esp8266-of-error-message/)]
   ->randomをコメントアウトすると駆動が2,500,000ms以上続いた。
   ->疑似乱数の代わりにmicros()を使用して乱数生成を行う。
   ->駆動50,000,000ms続行
*/

//2022/11/15 2:00 電源耐久試験開始 sleepなし、単三乾電池2本(3.0V)による試験 wifi接続は時間受信最初の1回のみ
//液晶が先に見えなくなる。液晶の電源制御は必須。
//そもそも電圧が3.0Vでは不十分だと思われる。
//deep sleep関連クロックの保持->　https://www.robmiles.com/journal/2020/1/22/esp32-retaining-timing-over-deep-sleep
//NULLの値は0になるため、配列の終止として使用してはならない。
//2024/05/16 マジックナンバーが多すぎる SPI通信の指定がわからない

#include "ATM0130.h"

bool button_press_flag = false;

void animationChecker(){
  for (uint8_t i = 0; i < ANIME_NUM; i++) {
    Serial.printf("anime: %d\n",i);
    uint8_t* a = (uint8_t*)animations[i];
    for (uint8_t j = 0; pgm_read_byte(&(*a)) != ARRAYSTOPPER; j++) {
      Serial.print(pgm_read_byte(&(*a)));
      Serial.print(",");
      Serial.println(pgm_read_byte(&(*(a+1))));
      a+=2;
      if(pgm_read_byte(&(*a)) == ARRAYSTOPPER) break;
      else if(pgm_read_byte(&(*(a+1))) == ARRAYSTOPPER) break;
    }
  }
}

void frame_checker() {
  //運用情報のチェック
  if (millis() - device.last_millis >= 10000) {
    device.last_millis = millis();
    Serial.print("executeTime: ");
    Serial.println(device.last_millis);
    /*
    if (device.isTimeConfigured) device.timeLog();
    else device.readTimeLog();
    */
    /*
    int v = ESP.getVcc();
    Serial.println("VCC=" + String(v / 1000.0) + "V");
    */
  }
  buttonChecker();
  //animationChecker();
}

void buttonChecker() {
  uint8_t button_reader = 0b000;
  /*
  if (digitalRead(BUTTON_A_PIN) == LOW) {
    button_reader |= (1 << BUTTON_A);
  }
  if (digitalRead(BUTTON_B_PIN) == LOW) {
    button_reader |= (1 << BUTTON_B);
  }*/
  device.setButtonState(button_reader);
}

// 割り込み処理
/*
void IRAM_ATTR button_pushed() {
  if (!button_press_flag) {
    button_press_flag = true;
  }
}*/


void setup() {
  /*
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_A_PIN, INPUT_PULLUP);
  pinMode(BUTTON_B_PIN, INPUT_PULLUP);
  */

  // 割り込みを登録 LOWをトリガーとする
  //attachInterrupt(BUTTON_A_PIN, button_pushed, FALLING);
  //attachInterrupt(BUTTON_B_PIN, button_pushed, FALLING);

  Serial.begin(115200);
  Serial.println("\nSYSTEM START");

  //digitalWrite(LED_PIN, HIGH);

  myATM0130.begin();
  myATM0130.clearScreen(BLACK16);
  myATM0130.updateScreen();

  //device.WiFiBegin();

  gameobjects[0] = &background;
  gameobjects[1] = &character;
  gameobjects[2] = &userinterface;
  gameobjects[3] = NULL;
}

void loop() {
  while(true){
    frame_checker();

    /*
    if (device.tryWiFiConnect) {
      Serial.println("tryWiFi");
      device.WiFiConnectCheck();
      if (device.isTimeConfigured) device.WiFiEnd();
    }*/

    if (false/*device.isServerStarted*/) {
      /*
      Serial.println("tryServer");
      device.serverWaitAccess();
      if (device.getButtonState(BUTTON_A, BUTTON_PRESSED)) device.serverEnd();*/
    }
    else {
      //全てgameobjects内でプログラムを実行、background, character, userinterfaceが既に入っている
      for (uint8_t i = 0; gameobjects[i] != NULL; i++) {
        gameobjects[i]->move();
        if (gameobjects[i]->show) gameobjects[i]->draw();
      }
      myATM0130.updateScreen();
    }

    yield();
  }
}
