#ifndef DEVICE_H_
#define DEVICE_H_

#define JST     3600* 9
#define CONFIG_LWIP_SNTP_UPDATE_DELAY 3600000

#define ANALOG_BUTTON_PIN A2
#define WAKEUP_BUTTON_PIN D9
#define DISPLAY_POWER_PIN D3

#define BUTTON_A 0
#define BUTTON_B 1
#define BUTTON_M 2

#define BUTTON_NOTPRESS 0
#define BUTTON_PRESSED  1
#define BUTTON_PRESSING 2
#define BUTTON_RELEASE  3

#define LFSR_SEED 0xCAFE

class Device{
  private:
    volatile uint8_t button_state;
    volatile uint8_t button_pressed;
    volatile uint8_t button_pressing;
    volatile uint8_t button_release;
    
    String APssid = "noah-game-proto";
    String APpass = "yakoshin";
    
    String ssid[3];
    String pass[3];

    String settings = "/wifi_setting.txt";
    
    struct tm *tm;
    
  public:  
    bool isDebugMode = true;
    bool isServerStarted = false;
  
    bool tryWiFiConnect;
    bool isWiFiConnected;
    uint8_t tryTimeConfigure;
    bool isTimeConfigured;
    bool isWiFiConnect_failed;
  
    volatile unsigned long last_millis;

    unsigned long timeOut_time;
    unsigned long wait_time;
    uint8_t trynumber;


    //天気予報のデフォルトは東京(気象庁)
    uint8_t area_num = 18;
    
    //緯度経度
    float longitude = 139.7414;
    float latitude = 35.6581;
    float today = 0;
    //日の出・日の入り時刻
    float sunriseTime = 0;
    float sunsetTime = 0;
 
    uint8_t ssid_num;
    String ssid_rssi_str[30];
    String ssid_str[30];
    String Selected_SSID_str = " ";
    String Sel_SSID_PASS_str = " ";
 
    uint32_t scanLastTime = 0;
    boolean First_Scan_Set = true;
    uint32_t APscanLastTime;
    const uint32_t scan_interval = 30000;

    Device();
    void setButtonState(uint8_t states);
    bool getButtonState(uint8_t button,uint8_t state);

    void serverBegin();
    void serverWaitAccess();
    void scan_AP();
    void client_connect();
    void html_send(
      boolean sta_connected, 
      String message1, 
      String message2, 
      String color, 
      String html_res_head, 
      String html_tag1, 
      String html_tag2
    );
    String HTML_Select_Box_str(
      String button_id, 
      String Sel_Ssid
    );
    void client_GET_requests();
    void favicon_response();
    void serverEnd();
    String getServerssid();
    String getServerpass();
    
    void WiFiBegin();
    void WiFiConnectCheck();
    void WiFiEnd();
    
    void setTime();
    void getTime();

    float dCalc();
    float eCalc();
    void setSunriseAndSunsetTime();

    void fetchWeatherForecast();
    void analyzeWeatherForecast(String json_string);
    
    uint8_t getHour();
    uint8_t getMinute();
    uint8_t getSecond();
    bool readSettings();
    bool writeSettings();

    bool save(uint8_t life, uint8_t stomach, int8_t favorability, int8_t hiddenFavorability);

    void sleep();
    void wakeup();
    void timeLog();
    void readTimeLog();
    
    uint16_t reg = LFSR_SEED;
    uint16_t LFSR_bit;

    uint16_t LFSR();

    /* 設定用の文字列 */
    String req_str = "";
    String html_res_head = (
        String("HTTP/1.1 200 OK\r\n")
       +String("Content-type:text/html\r\n")
       +String("Connection:close\r\n\r\n")
    );
    String html_tag1 = (
       String("<!DOCTYPE html>\r\n<html>\r\n<head>\r\n")
      +String("<meta name='viewport' content='initial-scale=1.5'>\r\n")
      +String("</head>\r\n\r\n")
      +String("<body style='background:#000; color:#fff; font-size:100%;'>\r\n")
      +String("ESP32<br>\r\n")
      +String("Access Point Selector<br>\r\n")
    );
    String html_tag2 = "\r\n</body>\r\n</html>\r\n\r\n";
};

#endif
