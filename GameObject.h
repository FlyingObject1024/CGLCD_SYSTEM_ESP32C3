#ifndef GAMEOBJECT_H_
#define GAMEOBJECT_H_

#define CLOUD_NUM 10

enum{
  STATE_NORMAL,
  STATE_HUNGRY,
  STATE_STARVE,
  STATE_SLEEPY,
  STATE_SLEEP,
  STATE_STROKE,
  STATE_STROKE_HAPPY,
  STATE_DIE,
  STATE_CALL,
  STATE_EAT,
  STATE_STORAGE_EAT,
};

class GameObject{
  public:
    bool show;
    int16_t x;
    int16_t y;
    uint8_t sizex;
    uint8_t sizey;
    
    virtual void move();
    virtual void draw();

};

class BackGround : public GameObject{
  private:
    class Cloud{
      public:
        uint8_t imageNum;
        int16_t x;
        int16_t y;
        bool show;
        uint8_t downSpeed;
        Cloud();
        void move();
        void draw();
    };
  public:
    BackGround();
    Cloud clouds[CLOUD_NUM];
    void drawOutSide();
    void move();
    void draw();
};

class Character : public GameObject{
  private:
    uint8_t stockNum;
    uint8_t stockItems[4] = {FOOD_IMAGENUM,FOOD_IMAGENUM,FOOD_IMAGENUM,FOOD_IMAGENUM};//冷蔵庫
    uint8_t* animation;
    void moveAnimation();
    
  public:
    int8_t life;    //体力 0になると倒れる
    int8_t stomach; //満腹度
    
    int8_t sleepiness;   //眠気(低いと眠い)    
    int8_t happiness;    //楽しさ
    int8_t favorability; //一時好感度
    //happiness = 100*favorability/(time+favorability)
    int8_t love;//真の好感度

    //0になったらパラメータ変動判定が走る
    uint8_t loopTime;

    uint8_t storage[4] = {FOOD_RICEBALL, FOOD_IMAGENUM, FOOD_IMAGENUM, FOOD_IMAGENUM};
    
    Character();
    void eat(uint8_t foodType);
    void stock(uint8_t foodType);
    void statusChanger();
    void move();
    void draw();

    uint8_t state;
    uint8_t beforeState;
    void changeState(uint8_t state);

    uint8_t behavior;
    uint8_t animationNum;
    
    uint8_t eyeState;//現在の目の状態
    uint8_t bodyState;//現在の体の状態
    uint8_t mouthState;//現在の口の状態
    bool flip;//表示を反転させるフラグ
    void drawEye();
    void drawMouth();
};

class UI : public GameObject{
  private:
    bool menuOpen;
    int8_t UIcursor;
    int8_t secondCursor;

    uint8_t foodType;
    void foodDecision();

    void moveTouch();
    void moveEat();
    void moveInfo();
    void moveSave();
    void movePowor();
    void moveSetting();
    
    void drawMenubar();
    
    void drawTouch();
    void drawEat();
    void drawInfo();
    void drawSave();
    void drawPowor();
    void drawSetting();

    //void (UI::*moves[6])(void);
    
  public:
    UI();
    void move();
    void draw();
    
    void drawClock(int16_t x,int16_t y);
};

GameObject* gameobjects[4];
BackGround background;
Character character;
UI userinterface;

#endif
