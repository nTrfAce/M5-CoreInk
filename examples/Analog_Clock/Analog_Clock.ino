// This sketch displays an analog clock on an M5Stack CoreIce
//
// use examples/RTC_BM8563 to set the time


#include "M5CoreInk.h"

Ink_Sprite InkPageSprite(&M5.M5Ink);

uint16_t CLOCK_RADIUS = 0;
const uint8_t HOUR_HAND_LENGTH_PERCENT = 40;
const uint8_t MINUTE_HAND_LENGTH_PERCENT = 65;
const uint8_t INK = 0;
const uint8_t PAPER = 1;

RTC_TimeTypeDef RTC_TimeStruct;

uint8_t minutes = 0;  // limit minutes to integer numbers so the minute hand snaps to the marks
float hours = 0;      // hours is a float for more resolution between steps
float minute_radians;
float hour_radians;


void setup()
{
  M5.begin();
  digitalWrite(LED_EXT_PIN,LOW);
  if( !M5.M5Ink.isInit())
  {
      Serial.printf("Ink Init faild");
      while (1) delay(100);   
  }
  M5.M5Ink.clear(1);

  //create ink refresh Sprite
  if( InkPageSprite.creatSprite(0,0,200,200,true) != 0 )
  {
      Serial.printf("Ink Sprite create failed");
  }
}

void loop()
{  
  uint16_t HEIGHT = InkPageSprite.height();
  uint16_t WIDTH  = InkPageSprite.width();

  // if the display isn't square, make the clock fit the smaller dimension
  if (HEIGHT >= WIDTH)
  {
    CLOCK_RADIUS = (WIDTH/2)-1;  
  } else {
    CLOCK_RADIUS = (HEIGHT/2)-1;
  }

  uint16_t CENTER_X = WIDTH/2;
  uint16_t CENTER_Y = HEIGHT/2;

  M5.rtc.GetTime(&RTC_TimeStruct);

  minutes = RTC_TimeStruct.Minutes + (RTC_TimeStruct.Seconds / 60.0);
  minute_radians = ((minutes / 60.0) * 2 * PI);

  hours = RTC_TimeStruct.Hours + (RTC_TimeStruct.Minutes / 60.0)  + (RTC_TimeStruct.Seconds / 3600.0);
  if (hours > 13)
  {
    hours = hours - 12;
  }
  hour_radians = ((hours / 12.0) * 2 * PI);
  
  InkPageSprite.clear();
  
  // draw the watch outline circle
  InkPageSprite.fillCircle(100, 100, CLOCK_RADIUS, INK);
  
  // erase the center of the watch face
  InkPageSprite.fillCircle(100, 100, CLOCK_RADIUS * 0.98, PAPER);


  // draw hour marks
  for (uint8_t a = 1; a < 12; a = a + 1)
  {
    draw_clock_mark((a / 12.0 * 2 * PI) + PI, CLOCK_RADIUS, CLOCK_RADIUS * 0.20, CENTER_X, CENTER_Y, INK);
  }

  // draw minute marks
  for (uint8_t a = 1; a < 60; a = a + 1)
  {
    draw_clock_mark((a / 60.0 * 2 * PI) + PI, CLOCK_RADIUS, CLOCK_RADIUS * 0.10, CENTER_X, CENTER_Y, INK);
  }

  // draw 12 o'clock mark
  InkPageSprite.drawTriangle(
    CLOCK_RADIUS-(CLOCK_RADIUS*.04)+1, CLOCK_RADIUS*2*0.01,
    CLOCK_RADIUS+(CLOCK_RADIUS*.04)+1, CLOCK_RADIUS*2*0.01,
    CLOCK_RADIUS+1, CLOCK_RADIUS*.21,
    INK);
  
  // center dot
  InkPageSprite.fillCircle(100, 100, 3, INK);
  
  draw_clock_hand(HOUR_HAND_LENGTH_PERCENT * 0.01 * CLOCK_RADIUS, CENTER_X, CENTER_Y, hour_radians, INK);
  draw_clock_hand(MINUTE_HAND_LENGTH_PERCENT * 0.01 * CLOCK_RADIUS, CENTER_X, CENTER_Y, minute_radians, INK);

  InkPageSprite.pushSprite();

  M5.shutdown(60);
  //M5.shutdown(RTC_TimeTypeDef(10,2,0));
  //M5.rtc.SetAlarmIRQ(RTC_TimeTypeDef(10,2,0));
  
  M5.update();
}



void draw_clock_hand(uint16_t Length, uint16_t center_x, uint16_t center_y, float Radians, uint8_t color)
{
  uint16_t x = Length * sin(Radians) * -1;
  uint16_t y = Length * cos(Radians);

  InkPageSprite.drawLine(center_x - x, center_y - y, center_x, center_y, color);
}


void draw_clock_mark(float Radians, uint8_t diameter, uint8_t Length, uint8_t center_x, uint8_t center_y, uint8_t color)
{
  uint16_t x = center_x + ((diameter - Length) * sin(Radians)) * -1;
  uint16_t y = center_y + ((diameter - Length) * cos(Radians));

  float x2 = center_x + ((diameter - 1) * sin(Radians)) * -1;
  float y2 = center_y + ((diameter - 1) * cos(Radians)) ;


  InkPageSprite.drawLine(x, y, x2, y2, color);
}
