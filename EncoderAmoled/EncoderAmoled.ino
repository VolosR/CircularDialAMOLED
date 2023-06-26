#include "rm67162.h"
#include <TFT_eSPI.h>
#include "font.h"
#include "yt.h"
#include "sevenSeg.h"
#include <RotaryEncoder.h>

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);

#define up 21
#define down 0
#define led 38
#define rotButton 1

#define PIN_IN1 46
#define PIN_IN2 45

RotaryEncoder encoder(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::TWO03);
bool deb = 0;

unsigned short grays[101]={0};
bool ledON = false;

unsigned short foreground= TFT_WHITE;
unsigned short background= TFT_BLACK;

double rad=0.01745;

float x[360]; //outer point
float y[360];
float px[360]; //ineer point
float py[360];
float lx[360]; //long line 
float ly[360];
float shx[360]; //short line 
float shy[360];
float tx[360]; //text
float ty[360];

int angle=0;
int value=0;
int chosenFont;
int chosenColor;
int r=280;
int sx=-170;
int sy=268;
int inc=1;


void setup()
{
  pinMode(up, INPUT_PULLUP);
  pinMode(down, INPUT_PULLUP);
  pinMode(rotButton, INPUT_PULLUP);
  pinMode(led, OUTPUT);
  sprite.createSprite(240,536);
  sprite.setSwapBytes(1);
  
  
  rm67162_init();  // amoled lcd initialization
  lcd_brightness(200);
  
  for(int i=0;i<360;i++)
  {
       x[i]=(r*cos(rad*i))+sx;
       y[i]=(r*sin(rad*i))+sy;
       px[i]=((r-14)*cos(rad*i))+sx;
       py[i]=((r-14)*sin(rad*i))+sy;

       lx[i]=((r-40)*cos(rad*i))+sx;
       ly[i]=((r-40)*sin(rad*i))+sy;

       shx[i]=((r-26)*cos(rad*i))+sx;
       shy[i]=((r-26)*sin(rad*i))+sy;

      tx[i]=((r+28)*cos(rad*i))+sx;
      ty[i]=((r+28)*sin(rad*i))+sy;
  }

  for(int i=0;i<101;i++)
  grays[i]=tft.color565(50+(i*2), 50+(i*2), 50+(i*2));

  draw();
}

int a=0;

void draw()
{
 sprite.setTextDatum(3);
 sprite.fillSprite(background);
 
 for(int i=0;i<120;i++)
 {
   a=angle+(i*3);
   if(a>359)
   a=(angle+(i*3))-360;
   int pos=ty[a];

  if(i%6==0 && ty[a]>0 && ty[a]<536 ){
   
    if(pos<sy){
    chosenFont=map(pos,0,sy,0,59);
    chosenColor=map(pos,0,sy,0,100);
    }
    if(pos>=sy)
    {
    chosenFont=map(pos,sy,536,59,0);
    chosenColor=map(pos,sy,536,100,0);
    }
  
  sprite.setFreeFont(myfonts[chosenFont]); 
  sprite.drawWedgeLine(x[a],y[a],lx[a],ly[a],3,3,grays[chosenColor],background);
  
  sprite.setTextColor(grays[chosenColor],background);
  sprite.drawString(String((i/6)*5),tx[a],ty[a]);
  
  }
  else if(i%3==0)
  {
  sprite.drawWedgeLine(x[a],y[a],shx[a],shy[a],2,2,grays[chosenColor],background);
  }
  else{
  sprite.drawWedgeLine(x[a],y[a],px[a],py[a],2,2,grays[chosenColor],background);}

 }

 //sprite.pushImage(120,480,120,50,yt); 

sprite.setTextColor(grays[30],background);
 sprite.setFreeFont(myfonts[4]);
 sprite.setTextDatum(5);
 sprite.drawString("ANGLE",240,50);
 sprite.drawString("VALUE",240,460);

 sprite.setTextColor(grays[50],background);
 sprite.setFreeFont(&DSEG14_Classic_Regular_28);
 
 sprite.drawString(String(value/10.00),240,490); 
 sprite.drawString(String(angle),240,82); 
 
 

 sprite.setTextColor(0x93E4,background);
 sprite.drawString("VOLOS",240,16); 
 sprite.setFreeFont(myfonts[6]);
 sprite.drawString("INCREMENT:"+String(inc), 240,526);

 sprite.drawWedgeLine(-20,sy,40,sy,8,1,TFT_RED);
 
 lcd_PushColors(0, 0, 240,536, (uint16_t*)sprite.getPointer());
 }


 void readButtons(){
  if (digitalRead(up) == 0) {
    angle=angle+2;
    if(angle>=360)
    angle=0;
  }

  if (digitalRead(down) == 0) {
    angle--;
    if(angle<0)
    angle=359;
  } 

   if (digitalRead(rotButton) == 0) {
    if(deb==0)
    {
    deb=1;
    inc=inc*2;
    if(inc==16)
    inc=1;
    draw();
    delay(100);
    }
  } else deb=0;
}

int prev=0;

void loop()
{
  readButtons();
  readEncoder();
  value=map(angle,0,359,1000,0);
  

  if(angle!=prev){
  draw();
  prev=angle;
  } 
}

void readEncoder() {

  static int pos = 0;
  encoder.tick();

  int newPos = encoder.getPosition();
  if (pos != newPos) {
    
    if(newPos>pos)
    angle=angle+inc;
    if(newPos<pos)
    angle=angle-inc;
    
    pos = newPos;
  } 
 if(angle<0)
    angle=359;

  if(angle>=360)
    angle=0;
}