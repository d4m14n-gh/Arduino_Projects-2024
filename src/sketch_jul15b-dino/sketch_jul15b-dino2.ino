#include <SPI.h>
#include <Wire.h>
//#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define i2c_Address 0x3c
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1   //   QT-PY / XIAO
#define FLOOR 60

//pinout
#define XP A1
#define YP A0
#define LED 5
#define BUZZER 7
#define POT A2



Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
static const unsigned char PROGMEM dino[] = {
	B00000000, B00001111, B11000000, B00000000,
	B00000000, B00011111, B11110000, B00000000,
	B00000000, B00011111, B11100000, B00000000,
	B00000000, B00110111, B11110000, B00000000,
	B00000000, B00111111, B11110000, B00000000,
	B00000000, B00111111, B11110000, B00000000,
  B00000000, B00111111, B11110000, B00000000,
  B00000000, B00111110, B00000000, B00000000,
  B10000000, B00111111, B11000000, B00000000,
  B10000000, B01111100, B00000000, B00000000,
  B11000000, B11111100, B00000000, B00000000,
  B11100001, B11111111, B00000000, B00000000,
  B11110011, B11111101, B00000000, B00000000,
  B11111111, B11111100, B00000000, B00000000,
  B11111111, B11111100, B00000000, B00000000,
  B01111111, B11111100, B00000000, B00000000,
  B00111111, B11111000, B00000000, B00000000,
  B00011111, B11110000, B00000000, B00000000,
  B00001111, B11100000, B00000000, B00000000,
  B00000111, B01100000, B00000000, B00000000,
  B00000110, B00100000, B00000000, B00000000,
  B00000100, B00100000, B00000000, B00000000,
	B00000110, B00110000, B00000000, B00000000
  };

unsigned long t=0;

void setup() {
  pinMode(8, INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(LED, OUTPUT);

  delay(250); 
  display.begin(i2c_Address, true); 
  display.display();
  delay(1000);
  t=millis();
  randomSeed(0);
}


void drawString(String text, int posx=0, int posy=0);
float sign(float x){if(x>=0)return 1;else return -1;}

float cx=240, cx2=350;
int hg=18, hg2=25;
void drawMap(){
  display.drawLine(0, FLOOR, SCREEN_WIDTH, FLOOR, 1);
  display.fillRoundRect(cx-hg/4, FLOOR-hg+5, hg/2, hg, hg/9+1, 0);
  display.drawRoundRect(cx-hg/4, FLOOR-hg+5, hg/2, hg, hg/9+1, 1);
  display.fillRoundRect(cx2-hg2/4, FLOOR-hg2+5, hg2/2, hg2, hg2/4, 0);
  display.drawRoundRect(cx2-hg2/4, FLOOR-hg2+5, hg2/2, hg2, hg2/4, 1);
}

float x=44, y=-32, vx=1, vy=0;
bool buzzz=false;

float points=0;

void loop() {
  float dx=analogRead(XP)/1023.0-0.5;
  float dy=analogRead(YP)/1023.0-0.5;
  float dt=(float)(millis()-t)/10.0;
  dt*=(analogRead(POT)/1023.0);
  const int fp=-(FLOOR-5);
  t=millis();

  if(abs(dy)>0.15&&y==fp)
    vy=2.0f;

  y+=vy*dt;
  vx=1+sqrt(points)/25;
  cx-=vx*dt;
  if(cx<0){
    cx=cx2+random(90, 232);
    hg=random(15, 35);
  }
  cx2-=vx*dt;
  if(cx2<0){
    if(random(3)==1)
      cx2=cx+random(5, 15);
    else
      cx2=cx+random(90, 432);
    hg2=random(15, 35);
  }

  if(y<fp){
    y=fp;
    vy=0;
    buzzz=true;
  }
  else if(y>fp)
    vy-=dt/20.0;


  vx*=0.5;
  for(int i=0;i<round(dt);i++)
    vy*=0.995;

  points+=vx*dt/10.0;

  display.clearDisplay();
  drawMap();
  display.drawBitmap(x-16, -y-11-5, dino, 32, 22, 1);
  //display.fillCircle(round(x), round(-y), 5, 1);
  analogWrite(LED, map(analogRead(POT),0,1023,0,100));
  drawString("points: "+String( round(points) ));
  if(analogRead(POT)<500)
    display.invertDisplay(true);
  else
    display.invertDisplay(false);
  display.display();

  if(abs(x-cx)<6&&y<fp+hg-5||abs(x-cx2)<6&&y<fp+hg2-5){
    //delay(3000);
    buzzz=true;
    //display.drawCircle(round(x), round(-y), 7, 1);
    display.display();
    delay(1500);
    t=millis();
    cx=152;
    cx2=252;
    points=0;
    y=fp;
  }

  if(digitalRead(8)==HIGH||buzzz){
    //display.drawCircle(round(x), round(-y), 7, 1);
    display.display();
    tone(BUZZER, 1850, 10);
    buzzz=false;
  }
  delay(5);
}

void drawString(String text, int posx=0, int posy=0){
  display.setTextSize(1);
  display.setTextColor(1);
  display.setCursor(posx, posy);
  for (int i = 0; i < text.length(); i++)
    display.write(text[i]);
  display.write('\n');
}
