#ifndef _INK_SPRITE_H_
#define _INK_SPRITE_H_

#include "Ink_eSPI.h"

#define CLEAR_DRAWBUFF  0x01
#define CLEAR_LASTBUFF  0x02

// Swap any type
template <typename T> static inline void
swap_coord(T& a, T& b) { T t = a; a = b; b = t; }


class Ink_Sprite : private Ink_eSPI
{
public:
public:
    Ink_Sprite(Ink_eSPI *dev);
    ~Ink_Sprite();

    int creatSprite(uint16_t posX,
                    uint16_t posY,
                    uint16_t width = 200,
                    uint16_t height = 200,
                    bool copyFromMem = true);

    int pushSprite();
    int deleteSprite();

    void clear( int cleanFlag = CLEAR_DRAWBUFF );
    void drawPix(uint16_t posX,uint16_t posY,uint8_t pixBit);
    void drawPixel(uint16_t posX,uint16_t posY,uint8_t pixBit);
    void drawCircle(int32_t x, int32_t y, int32_t r, uint32_t color);
    void fillCircle(int16_t x0, int16_t y0, int16_t r, uint8_t color);
    void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color);
    void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color);
    void drawLine(int16_t xs, int16_t ys, int16_t xe, int16_t ye, uint8_t color);
    void drawFastVLine(int16_t x, int16_t y, int16_t h, uint8_t color);
    void drawFastHLine(int16_t x, int16_t y, int16_t w, uint8_t color);
    void FillRect(uint16_t posX,
                  uint16_t posY,
                  uint16_t width,
                  uint16_t height,
                  uint8_t pixBit);

    void drawFullBuff(uint8_t* buff, bool bitMode = true);
    void drawBuff(uint16_t posX,
                  uint16_t posY,
                  uint16_t width,
                  uint16_t height,
                  uint8_t* imageDataptr);

    void drawChar(uint16_t posX,uint16_t posY,char charData,Ink_eSPI_font_t* fontPtr = &AsciiFont8x16);
    void drawString(uint16_t posX,uint16_t posY,const char* charData,Ink_eSPI_font_t* fontPtr = &AsciiFont8x16);

    uint8_t* getSpritePtr(){ return _spriteBuff;}

    uint16_t width(){ return _width;}
    uint16_t height(){ return _height;}
    uint16_t posX(){ return _posX;}
    uint16_t posY(){ return _posY;}

private:
    Ink_eSPI *_dev;
    bool isCreat = false;
    uint16_t _posX,_posY;
    uint16_t _width,_height;
    uint16_t _pixSize;

    uint8_t *_spriteBuff = nullptr;
    uint8_t *_lastBuff = nullptr;
};


#endif
