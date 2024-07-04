//https://www.mgo-tec.com/blog-entry-esp32-wifi-ap-selector-arduino.html/2

#include "ATM0130.h"

Device::Device() {
  pinMode(ANALOG_BUTTON_PIN, ANALOG);
  pinMode(WAKEUP_BUTTON_PIN, INPUT_PULLUP);
  pinMode(DISPLAY_POWER_PIN, OUTPUT);
  this->last_millis = 0;
  
  this->ssid[0] = String("");
  this->pass[0] = String("");
  this->ssid[1] = String("");
  this->pass[1] = String("");
  this->ssid[2] = String("");
  this->pass[2] = String("");
  
  /*
  this->ssid[0] = String("xg100n-3f3275-1");
  this->pass[0] = String("117fd2a99b576");
  this->ssid[1] = String("PXL_1028");
  this->pass[1] = String("kodaidesu");
  this->ssid[2] = String("PXL_1028");
  this->pass[2] = String("kodaidesu");
  */
  //readSettings();

  this->isServerStarted = false;
  
  this->isTimeConfigured = false;
  this->isWiFiConnected = false;
  this->tryWiFiConnect = false;
  this->APscanLastTime = 0;

  this->button_state    = 0b00000000;
  this->button_pressed  = 0b00000000;
  this->button_pressing = 0b00000000;
  this->button_release  = 0b00000000;
  digitalWrite(DISPLAY_POWER_PIN, LOW);
}

void Device::setButtonState(uint8_t states) {
  this->button_pressed  = ~this->button_state   & states;
  this->button_pressing = this->button_pressing & states;
  this->button_pressing = this->button_pressing | this->button_state;
  this->button_release  = this->button_state    & ~states;

  // 今回の状態を保存
  this->button_state    = states;
}

bool Device::getButtonState(uint8_t button, uint8_t state) {
  if      (state == BUTTON_NOTPRESS) return (bool)( ((~this->button_state) >> button) & 0b1 );
  else if (state == BUTTON_PRESSED ) return (bool)( (this->button_pressed  >> button) & 0b1 );
  else if (state == BUTTON_PRESSING) return (bool)( (this->button_pressing >> button) & 0b1 );
  else if (state == BUTTON_RELEASE ) return (bool)( (this->button_release  >> button) & 0b1 );

  return false;
}

void handleRootGet() {
  String html = "";
  html += "<h1>WiFi Settings</h1>";
  html += "<form method='post'>";
  html += "  <input type='text' name='ssid' placeholder='ssid'><br>";
  html += "  <input type='text' name='pass' placeholder='pass'><br>";
  html += "  <input type='submit'><br>";
  html += "</form>";
  //server.send(200, "text/html", html);
}

void handleRootPost() {/*
  String getssid = server.arg("ssid");
  String getpass = server.arg("pass");

  SPIFFS.begin();
  File f = SPIFFS.open("/wifi_setting.txt", "w");
  f.println(getssid);
  f.println(getpass);
  f.close();
  SPIFFS.end();

  Serial.println("getSSID: " + getssid);
  Serial.println("getPASS: " + getpass);
  

  myATM0130.clearScreen(BLACK16);
  myATM0130.setColor(GREEN16);
  myATM0130.putStr(0,00,"setting changed\0");
  myATM0130.updateScreen();
  
  String html = "";
  html += "<h1>WiFi Settings</h1>";
  html += getssid + "<br>";
  html += getpass + "<br>";
  server.send(200, "text/html", html);
  */
}

void Device::serverBegin(){
  if(this->tryWiFiConnect) this->WiFiEnd();
  
  WiFi.mode(WIFI_AP);
  WiFi.softAP(APssid, APpass);
  
  this->timeOut_time = 0;
  server.begin();
  Serial.println("Serevr start");
  this->isServerStarted = true;
}

void Device::serverWaitAccess(){
  this->scan_AP();
  this->client_connect();
}

void Device::scan_AP(){
   if((this->First_Scan_Set == true) 
      ||((millis() - this->scanLastTime) > this->scan_interval) 
     )
   {
    Serial.println("scan start");
 
    // WiFi.scanNetworks will return the number of networks found
    this->ssid_num = WiFi.scanNetworks();
    Serial.println("scan done\n");
    if (this->ssid_num == 0) {
      Serial.println("no networks found\n");
    } 
    else{
      Serial.printf("%d networks found\n\n", ssid_num);
      for(uint8_t i = 0; i < this->ssid_num; ++i) {
        this->ssid_str[i] = WiFi.SSID(i);
        String wifi_auth_open = ((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
        this->ssid_rssi_str[i] = this->ssid_str[i] + " (" + WiFi.RSSI(i) + "dBm)" + wifi_auth_open;
        Serial.printf("%d: %s\n", i, this->ssid_rssi_str[i].c_str());
        delay(10);
      }
    }
    Serial.println("");
    this->scanLastTime = millis();
    this->First_Scan_Set = false;
  }
}

void Device::client_connect(){
  client = server.available();
  if (client) {
    Serial.println("new client");

    req_str = "";
    while (client.connected()){
      myATM0130.setColor(GREEN16);
      myATM0130.putStr(0, 89, "CONNECTED");
      myATM0130.updateScreen();
      device.getButtonState(BUTTON_A, BUTTON_PRESSED);
      while(client.available()){
        //待機
        req_str = client.readStringUntil('\n');
        if(req_str.indexOf("\r") == 0) break;
        Serial.println(req_str);
        this->client_GET_requests();
        device.getButtonState(BUTTON_A, BUTTON_PRESSED);
      }
    }
  }
}

void Device::html_send(
  boolean sta_connected, 
  String message1, 
  String message2, 
  String color, 
  String html_res_head, 
  String html_tag1, 
  String html_tag2
  ){
  client.print(html_res_head);
  client.print(html_tag1);
  client.print(HTML_Select_Box_str("!xxxx", message1));
  client.printf("<p style='color:%s; font-size:80%%'>%s</p>\r\n", color.c_str(), message2.c_str());
  if(sta_connected == true){
    client.print("<span style='font-size:80%'>ESP32 STA IP = ");
    client.print("</span>\r\n");
  }
  client.print(html_tag2);
 
  Serial.print(html_res_head);
  Serial.print(html_tag1);
  Serial.print(HTML_Select_Box_str("!xxxx", message1));
  Serial.printf("<p style='color:%s; font-size:80%%'>%s</p>\r\n", color.c_str(), message2.c_str());
  if(sta_connected == true){
    Serial.print("<span style='font-size:80%%'>ESP32 STA IP = ");
    Serial.print("</span>\r\n");
  }
  Serial.print(html_tag2);
  
}

String Device::HTML_Select_Box_str(
  String button_id, 
  String Sel_Ssid
  ){
  String str = "";
  String selected_str = "";
  str += "<form name='F_ssid_select'>\r\n";
  str += "  <select name='ssid_select'>\r\n";
  for(int i=0; i<ssid_num; i++){
    if(Selected_SSID_str == ssid_str[i]){
      selected_str = " selected";
    }else{
      selected_str = "";
    }
    str += "    <option value=" + ssid_str[i] +  selected_str + ">" + ssid_rssi_str[i] + "</option>\r\n";
  }
  str += "</select><br>\r\n";
  str += "Password<br><input type='password' name='pass1'>\r\n";
  str += "<br><button type='submit' name='ssid_sel_submit' value='send' style='background-color:#AFA;' onclick='document.getElementById(\"ssid_sel_txt\").innerHTML=document.F_ssid_select.ssid_select.value;'>STA Connection GO!</button>\r\n";
  str += "</form><br>\r\n";
  str += "<form name='F_connection_close'>\r\n";
  str += "  <button type='submit' name='connection_close' value='send' style='background-color:#FAA;' onclick='document.getElementById(\"ssid_sel_txt\").innerHTML=\"ESP32 STA connection close\";'>STA Connection Close</button>\r\n";
  str += "</form>\r\n";
  str += "<br>  (Selected SSID)<br><span id='ssid_sel_txt'  style='font-size:80%;'>";
  str += Sel_Ssid;
  str += "</span>\r\n";
  return str;
}

void Device::client_GET_requests(){
  if(req_str.indexOf("GET / ") >= 0){
    Serial.println("--------------- GET Request Receive from Clinet");
    while(client.available()){
      char c = client.read();
      Serial.write(c);
    }
    Serial.println("--------------- GET Request Receive Finish");
    html_send(false, "ESP32 STA connection close", "ESP32 STA connection close", "#FFF", html_res_head, html_tag1, html_tag2);
 
    delay(10);
    client.stop();
    Serial.println("client disonnected");
    delay(10);
    req_str = "";
  }
  else if(req_str.indexOf("GET /?") >= 0){
    Serial.println("--------------- SUBMIT Receive from Clinet");
    int16_t getTXT_pass = req_str.indexOf("pass1=");
    int16_t getTXT_select = req_str.indexOf("ssid_select=");
    int16_t getTXT_close = req_str.indexOf("connection_close=");
 
    if(getTXT_pass > 0){
      Sel_SSID_PASS_str = req_str.substring(getTXT_pass + 6, req_str.indexOf("&ssid_sel_submit"));
      ssid[0] = Sel_SSID_PASS_str;
    }
    if(getTXT_select > 0){
      Selected_SSID_str = req_str.substring(getTXT_select + 12, req_str.indexOf("&pass1"));
      pass[0] = Sel_SSID_PASS_str;
    }
    if(getTXT_close < 0){
      Serial.printf("Selected_SSID_str = %s\r\n", Selected_SSID_str.c_str());
      Serial.printf("Sel_SSID_PASS_str = %s\r\n", Sel_SSID_PASS_str.c_str());
 
      while(client.available()){
        char c = client.read();
        Serial.write(c);
      }
 
      Serial.println("-------------- SUBMIT Request Receive Finish");
      String str_w = Selected_SSID_str + "\r\n" + Sel_SSID_PASS_str;
    }
    else{
      html_send(false, "---", "ESP32 STA Closed!", "#F00", html_res_head, html_tag1, html_tag2);
      WiFi.disconnect(false); //false=WiFi_ON , true=WiFi_OFF
    }
 
    delay(10);
    client.stop();
    Serial.println("client disonnected");
    delay(10);
    req_str = "";
  }
  else if(req_str.indexOf("GET /favicon") >= 0){
    favicon_response();
    req_str = "";
  }
}

void Device::favicon_response(){
  Serial.println(F("-----------------------Favicon GET Request Received"));
  while(client.available()){
    Serial.write(client.read());
  }
 
  client.print(F("HTTP/1.1 404 Not Found\r\n"));
  client.print(F("Connection:close\r\n\r\n"));
 
  delay(10);
  client.stop();
  delay(10);
 
  Serial.println(F("-----------------Client.stop (by Favicon Request)"));
}

void Device::serverEnd(){
  server.close();
  this->isServerStarted = false;
  readSettings();
}

String Device::getServerssid(){
  return this->APssid;
}

String Device::getServerpass(){
  return this->APpass;
}

void Device::WiFiBegin() {
  this->trynumber = 0;
  WiFi.mode(WIFI_STA);
  WiFi.begin(
    this->ssid[this->trynumber], 
    this->pass[this->trynumber]
  );
  
  this->tryWiFiConnect = true;
  this->timeOut_time = millis();
}

void Device::WiFiConnectCheck(){
  if(this->tryWiFiConnect){
    if(this->wait_time + 500 <= millis()){
      if (WiFi.status() != WL_CONNECTED){
        Serial.print("Connecting to ");
        Serial.println(this->ssid[this->trynumber]);
        if(this->timeOut_time + 10000 <= millis()){
          Serial.println("TIMEOUT");
          WiFi.disconnect();
          this->trynumber++;
          if(this->ssid[this->trynumber].length() <= 0 || this->trynumber >= 3){
            this->isWiFiConnected = false;
            this->tryWiFiConnect = false;
            this->isWiFiConnect_failed = true;
            this->trynumber = 0;
          }
          else{
            WiFi.begin(
              this->ssid[this->trynumber], 
              this->pass[this->trynumber]
            );
            Serial.print("Reconnect to");
            Serial.println(this->ssid[this->trynumber]);
            this->timeOut_time = millis();
          }
        }
      }    
      else{
        this->isWiFiConnected = true;
        this->tryWiFiConnect = false;
        this->setTime();
        Serial.print("CONNECTED.\nSSID=");
        Serial.println(WiFi.SSID());
      }
      this->wait_time = millis();
    }
  }
}

void Device::WiFiEnd(){
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  this->tryWiFiConnect = false;
  this->isWiFiConnected = false;
}

void Device::fetchWeatherForecast(){
  //region_codes[this->area_num];
  return;
}

void Device::analyzeWeatherForecast(String json_string){  
  String todayWeather;
  String todayMaxTemp;
  String todayMinTemp;
  String todayPop;

  // 天気を取得
  int weatherIndex = json_string.indexOf("\"weathers\"");
  if (weatherIndex != -1) {
    int start = json_string.indexOf("[", weatherIndex);
    int end = json_string.indexOf("]", weatherIndex);
    if (start != -1 && end != -1) {
      todayWeather = json_string.substring(start + 2, json_string.indexOf(",", start) - 1);
    }
  }

  // 最高気温・最低気温を取得
  int tempIndex = json_string.indexOf("\"temps\"");
  if (tempIndex != -1) {
    int start = json_string.indexOf("[", tempIndex);
    int end = json_string.indexOf("]", tempIndex);
    if (start != -1 && end != -1) {
      todayMaxTemp = json_string.substring(start + 2, json_string.indexOf(",", start) - 1);
      todayMinTemp = json_string.substring(json_string.indexOf(",", start) + 2, json_string.indexOf(",", json_string.indexOf(",", start) + 1) - 1);
    }
  }

  // 降水確率を取得
  int popIndex = json_string.indexOf("\"pops\"");
  if (popIndex != -1) {
    int start = json_string.indexOf("[", popIndex);
    int end = json_string.indexOf("]", popIndex);
    if (start != -1 && end != -1) {
      todayPop = json_string.substring(start + 2, json_string.indexOf(",", start) - 1);
    }
  }

  Serial.println("Today's Weather: " + todayWeather);
  Serial.println("Today's Max Temp: " + todayMaxTemp + "°C");
  Serial.println("Today's Min Temp: " + todayMinTemp + "°C");
  Serial.println("Today's POP: " + todayPop + "%");
}

void Device::setTime(){
  if (this->isWiFiConnected){
    if     (this->tryTimeConfigure>=4){
      this->isWiFiConnected = false;
      this->isTimeConfigured = false;
      this->tryTimeConfigure = 0;
      return;
    }
    else if(this->tryTimeConfigure==0){
      configTime(JST, 0, "ntp.nict.jp", "time.google.com", "ntp.jst.mfeed.ad.jp");
      this->timeOut_time = millis();
    }
    else if(this->timeOut_time + 500 <= millis()){
      configTime(JST, 0, "ntp.nict.jp", "time.google.com", "ntp.jst.mfeed.ad.jp");
      this->timeOut_time = millis();
    }
    this->tryTimeConfigure++;
  }
}


float Device::dCalc(){
  float d;
  d = + 0.33281
      - 22.984 * cos(this->today) - 0.34990 * cos(2 * this->today) - 0.13980 * cos(3 * this->today)
      + 3.7872 * sin(this->today) + 0.03250 * sin(2 * this->today) + 0.07187 * sin(3 * this->today);
  return RAD(d);    
}

float Device::eCalc(){
  float e;
  e = + 0.0072 * cos(this->today) - 0.0528 * cos(2 * this->today) - 0.0012 * cos(3 * this->today)
      - 0.1229 * sin(this->today) - 0.1565 * sin(2 * this->today) - 0.0041 * sin(3 * this->today);
  return e; 
}

void Device::setSunriseAndSunsetTime(){
  float y = RAD(this->latitude);
  float d = this->dCalc();
  float e = this->eCalc();
  float t = DEG(acos( (sin(RAD(-0.899)) - sin(d) * sin(y)) / (cos(d) * cos(y)) ) );
  
  this->sunriseTime = ( -t + 180.0 - this->longitude + 135.0) / 15.0 - e;
  this->sunsetTime = ( t + 180.0 - this->longitude + 135.0) / 15.0 - e;
  Serial.printf("sunrise: %f\n", sunriseTime);
  Serial.printf("sunset: %f\n", sunsetTime);
}

void Device::getTime(){
  time_t t;
  
  t = time(NULL);
  tm = localtime(&t);
  
  Serial.printf("%04d/%02d/%02d %02d:%02d:%02d\n",
        tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday,
        tm->tm_hour, tm->tm_min, tm->tm_sec);

  if(tm->tm_year >= 122){
    this->isTimeConfigured = true;
  }

  // 日付をラジアンに変換
  this->today = (((tm->tm_mon+1)*30.4 + tm->tm_mday) + 0.5) * 2 * PI / 365; 
  this->setSunriseAndSunsetTime();
}

uint8_t Device::getHour(){
  return tm->tm_hour;  
}

uint8_t Device::getMinute(){
  return tm->tm_min;
}

uint8_t Device::getSecond(){
  Serial.printf("%04d/%02d/%02d %02d:%02d:%02d\n",
        tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday,
        tm->tm_hour, tm->tm_min, tm->tm_sec);
  return tm->tm_sec;
}

bool Device::readSettings(){
  String area_str;
  SPIFFS.begin();
  File f = SPIFFS.open("/setting.txt","r");
  if(f == NULL) return false;

  area_str = f.readStringUntil('\n');
  ssid[0] = f.readStringUntil('\n');
  pass[0] = f.readStringUntil('\n');
  ssid[1] = f.readStringUntil('\n');
  pass[1] = f.readStringUntil('\n');
  ssid[2] = f.readStringUntil('\n');
  pass[2] = f.readStringUntil('\n');
  
  f.close();
  SPIFFS.end();

  this->area_num = area_str.toInt();
  ssid[0].trim();
  pass[0].trim();
  ssid[1].trim();
  pass[1].trim();
  ssid[2].trim();
  pass[2].trim();
  
  return true;
}

bool Device::writeSettings(){
  SPIFFS.begin();
  File f = SPIFFS.open("/setting.txt","w");
  if(f == NULL) return false;
  f.println(this->area_num);
  if(ssid[0] != ""){
    f.println(this->ssid[0]);
    f.println(this->pass[0]);    
  }
  else{
    f.println("");
    f.println("");
  }
  if(ssid[1] != ""){
    f.println(this->ssid[1]);
    f.println(this->pass[1]);    
  }
  else{
    f.println("");
    f.println("");
  }
  if(ssid[2] != ""){
    f.println(this->ssid[2]);
    f.println(this->pass[2]);    
  }
  else{
    f.println("");
    f.println("");
  }
  f.close();
  SPIFFS.end();
  
  return true;
}

bool Device::save(uint8_t life, uint8_t stomach, int8_t favorability, int8_t hiddenFavorability){  
  /*
  SPIFFS.begin();
  File f = SPIFFS.open("/character_data","w");
  if(f == NULL) return false;
  f.println(life);
  f.println(stomach);
  f.println(favorability);
  f.println(hiddenFavorability);
  f.close();
  SPIFFS.end();
  return true;
  */
  return false;
}

void Device::sleep(){
  //digitalWrite(LED_PIN,LOW);
}

void Device::wakeup(){
  
}

void Device::timeLog(){
  String timelog = String(tm->tm_year+1900) +"/"+ String(tm->tm_mon+1) + "/" + String(tm->tm_mday) + " " + String(tm->tm_hour) + ":" + String(tm->tm_min) + ":" + String(tm->tm_sec);
  Serial.println(timelog);
  /*
  SPIFFS.begin();
  File f = SPIFFS.open("/time_log.txt", "w");
  f.println(timelog);
  f.close();
  SPIFFS.end();*/
}

void Device::readTimeLog(){
  String timelog;
  /*
  SPIFFS.begin();
  File f = SPIFFS.open("/time_log.txt","r");
  if(f == NULL) return;
  
  timelog = f.readStringUntil('\n');
  f.close();
  SPIFFS.end();
  Serial.println("Last Timelog: " + timelog);
  */
}

uint16_t Device::LFSR(){
  //arduinoの乱数生成器は信用できないため乱数生成は自前で行う
  //Wikipediaの線形帰還シフトレジスタ > フィボナッチLFSRを使用
  LFSR_bit = (
    (reg & 0x0001)
    ^((reg & 0x0004) >> 2)
    ^((reg & 0x0008) >> 3)
    ^((reg & 0x0020) >> 5)
    );
  reg = (reg >> 1) | (LFSR_bit << 15);
  return reg;
}
