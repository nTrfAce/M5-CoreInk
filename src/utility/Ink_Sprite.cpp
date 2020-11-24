#include "Ink_Sprite.h"

Ink_Sprite::Ink_Sprite(Ink_eSPI *dev)
{
    _dev = dev;
}

Ink_Sprite::~Ink_Sprite()
{
    deleteSprite();
}

int Ink_Sprite::creatSprite(uint16_t posX,
                            uint16_t posY,
                            uint16_t width,
                            uint16_t height,
                            bool copyFromMem)
{
    if ((posX % 8 != 0) || (width % 8 != 0))
        return -1;

    _posX = posX;
    _posY = posY;
    _width = width;
    _height = height;
    _pixSize = width * height / 8;

    _spriteBuff = (uint8_t *)malloc(sizeof(uint8_t) * _pixSize);
    if (_spriteBuff == nullptr)
        return -1;

    _lastBuff = (uint8_t *)malloc(sizeof(uint8_t) * _pixSize);
    if (_lastBuff == nullptr)
        return -1;

    memset(_spriteBuff, 0xff, sizeof(uint8_t) * _pixSize);
    memset(_lastBuff, 0xff, sizeof(uint8_t) * _pixSize);

    isCreat = true;

    if( copyFromMem )
    {
        for (int y = 0; y < _height; y++)
        {
            for (int x = 0; x < _width; x++)
            {
                int32_t pixNum = _width * y + x;
                if (_dev->getPix(_posX + x, _posY + y) == 0 )
                {
                    _lastBuff[pixNum / 8] &= ~(0x80 >> (pixNum % 8));
                }
                else
                {
                    _lastBuff[pixNum / 8] |= (0x80 >> (pixNum % 8));
                }
            }
        }
    }

    return 0;
}

void Ink_Sprite::clear(int cleanFlag)
{
    if( !isCreat ) return;

    for (int i = 0; i < _pixSize; i++)
    {
        if (cleanFlag & CLEAR_DRAWBUFF)
            _spriteBuff[i] = 0xFF;
        if (cleanFlag & CLEAR_LASTBUFF)
            _lastBuff[i] = 0xFF;
    }
}

void Ink_Sprite::drawPix(uint16_t posX, uint16_t posY, uint8_t pixBit)
{
    if( !isCreat ) return;

    int32_t pixNum = _width * posY + posX;

    if (pixBit == 0)
    {
        _spriteBuff[pixNum / 8] &= ~(0x80 >> (pixNum % 8));
    }
    else
    {
        _spriteBuff[pixNum / 8] |= (0x80 >> (pixNum % 8));
    }
}

void Ink_Sprite::drawPixel(uint16_t posX, uint16_t posY, uint8_t pixBit)
{
  drawPix(posX, posY, pixBit);
}


// from TFT_eSPI.cpp
/***************************************************************************************
** Function name:           drawCircle
** Description:             Draw a circle outline
***************************************************************************************/
// Optimised midpoint circle algorit(hm
void Ink_Sprite::drawCircle(int32_t x0, int32_t y0, int32_t r, uint32_t color)
{
  int32_t  x  = 1;
  int32_t  dx = 1;
  int32_t  dy = r+r;
  int32_t  p  = -(r>>1);

  // These are ordered to minimise coordinate changes in x or y
  // drawPixel can then send fewer bounding box commands
  drawPix(x0 + r, y0, color);
  drawPix(x0 - r, y0, color);
  drawPix(x0, y0 - r, color);
  drawPix(x0, y0 + r, color);

  while(x<r){

    if(p>=0) {
      dy-=2;
      p-=dy;
      r--;
    }

    dx+=2;
    p+=dx;

    // These are ordered to minimise coordinate changes in x or y
    // drawPixel can then send fewer bounding box commands
    drawPixel(x0 + x, y0 + r, color);
    drawPixel(x0 - x, y0 + r, color);
    drawPix(x0 - x, y0 - r, color);
    drawPix(x0 + x, y0 - r, color);
    if (r != x) {
      drawPix(x0 + r, y0 + x, color);
      drawPix(x0 - r, y0 + x, color);
      drawPix(x0 - r, y0 - x, color);
      drawPix(x0 + r, y0 - x, color);
    }
    x++;
  }
}


/***************************************************************************************
** Function name:           fillCircle
** Description:             draw a filled circle
***************************************************************************************/
// Optimised midpoint circle algorithm, changed to horizontal lines (faster in sprites)
// Improved algorithm avoids repetition of lines
void Ink_Sprite::fillCircle(int16_t x0, int16_t y0, int16_t r, uint8_t color)
{
  int32_t  x  = 0;
  int32_t  dx = 1;
  int32_t  dy = r+r;
  int32_t  p  = -(r>>1);

  drawFastHLine(x0 - r, y0, dy+1, color);

  while(x<r){

    if(p>=0) {
      drawFastHLine(x0 - x, y0 + r, dx, color);
      drawFastHLine(x0 - x, y0 - r, dx, color);
      dy-=2;
      p-=dy;
      r--;
    }

    dx+=2;
    p+=dx;
    x++;

    drawFastHLine(x0 - r, y0 + x, dy+1, color);
    drawFastHLine(x0 - r, y0 - x, dy+1, color);
  }
}

/***************************************************************************************
** Function name:           drawTriangle
** Description:             Draw a triangle outline using 3 arbitrary points
***************************************************************************************/
// Draw a triangle
void Ink_Sprite::drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color)
{
  drawLine(x0, y0, x1, y1, color);
  drawLine(x1, y1, x2, y2, color);
  drawLine(x2, y2, x0, y0, color);
}



/***************************************************************************************
** Function name:           fillTriangle
** Description:             Draw a filled triangle using 3 arbitrary points
***************************************************************************************/
// Fill a triangle - original Adafruit function works well and code footprint is small
void Ink_Sprite::fillTriangle ( int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color)
{
  int16_t a, b, y, last;

  // Sort coordinates by Y order (y2 >= y1 >= y0)
  if (y0 > y1) {
    swap_coord(y0, y1); swap_coord(x0, x1);
  }
  if (y1 > y2) {
    swap_coord(y2, y1); swap_coord(x2, x1);
  }
  if (y0 > y1) {
    swap_coord(y0, y1); swap_coord(x0, x1);
  }

  if (y0 == y2) { // Handle awkward all-on-same-line case as its own thing
    a = b = x0;
    if (x1 < a)      a = x1;
    else if (x1 > b) b = x1;
    if (x2 < a)      a = x2;
    else if (x2 > b) b = x2;
    drawFastHLine(a, y0, b - a + 1, color);
    return;
  }

  int16_t
  dx01 = x1 - x0,
  dy01 = y1 - y0,
  dx02 = x2 - x0,
  dy02 = y2 - y0,
  dx12 = x2 - x1,
  dy12 = y2 - y1,
  sa   = 0,
  sb   = 0;

  // For upper part of triangle, find scanline crossings for segments
  // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
  // is included here (and second loop will be skipped, avoiding a /0
  // error there), otherwise scanline y1 is skipped here and handled
  // in the second loop...which also avoids a /0 error here if y0=y1
  // (flat-topped triangle).
  if (y1 == y2) last = y1;  // Include y1 scanline
  else         last = y1 - 1; // Skip it

  for (y = y0; y <= last; y++) {
    a   = x0 + sa / dy01;
    b   = x0 + sb / dy02;
    sa += dx01;
    sb += dx02;

    if (a > b) swap_coord(a, b);
    drawFastHLine(a, y, b - a + 1, color);
  }

  // For lower part of triangle, find scanline crossings for segments
  // 0-2 and 1-2.  This loop is skipped if y1=y2.
  sa = dx12 * (y - y1);
  sb = dx02 * (y - y0);
  for (; y <= y2; y++) {
    a   = x1 + sa / dy12;
    b   = x0 + sb / dy02;
    sa += dx12;
    sb += dx02;

    if (a > b) swap_coord(a, b);
    drawFastHLine(a, y, b - a + 1, color);
  }
}





    
    
/***************************************************************************************
** Function name:           drawLine
** Description:             draw a line between 2 arbitrary points
***************************************************************************************/
// Bresenham's algorithm - thx wikipedia - speed enhanced by Bodmer to use
// an efficient FastH/V Line draw routine for line segments of 2 pixels or more
void Ink_Sprite::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t color)
{

  bool steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    swap_coord(x0, y0);
    swap_coord(x1, y1);
  }

  if (x0 > x1) {
    swap_coord(x0, x1);
    swap_coord(y0, y1);
  }

  int32_t dx = x1 - x0, dy = abs(y1 - y0);;

  int32_t err = dx >> 1, ystep = -1, xs = x0, dlen = 0;

  if (y0 < y1) ystep = 1;

  // Split into steep and not steep for FastH/V separation
  if (steep) {
    for (; x0 <= x1; x0++) {
      dlen++;
      err -= dy;
      if (err < 0) {
        err += dx;
        if (dlen == 1) drawPixel(y0, xs, color);
        else drawFastVLine(y0, xs, dlen, color);
        dlen = 0;
        y0 += ystep; xs = x0 + 1;
      }
    }
    if (dlen) drawFastVLine(y0, xs, dlen, color);
  }
  else
  {
    for (; x0 <= x1; x0++) {
      dlen++;
      err -= dy;
      if (err < 0) {
        err += dx;
        if (dlen == 1) drawPixel(xs, y0, color);
        else drawFastHLine(xs, y0, dlen, color);
        dlen = 0;
        y0 += ystep; xs = x0 + 1;
      }
    }
    if (dlen) drawFastHLine(xs, y0, dlen, color);
  }
}


/***************************************************************************************
** Function name:           drawFastVLine
** Description:             draw a vertical line
***************************************************************************************/
void Ink_Sprite::drawFastVLine(int16_t x, int16_t y, int16_t h, uint8_t color)
{
  //if (_vpOoB) return;

  //x+= _xDatum;
  //y+= _yDatum;

  // Clipping
  //if ((x < _vpX) || (x >= _vpW) || (y >= _vpH)) return;

  //if (y < _vpY) { h += y - _vpY; y = _vpY; }

  //if ((y + h) > _vpH) h = _vpH - y;

  if (h < 1) return;

  //begin_tft_write();

  //setWindow(x, y, x, y + h - 1);
  FillRect(x, y, 1, h, color);
  //pushBlock(color, h);

  //end_tft_write();
}


/***************************************************************************************
** Function name:           drawFastHLine
** Description:             draw a horizontal line
***************************************************************************************/
void Ink_Sprite::drawFastHLine(int16_t x, int16_t y, int16_t w, uint8_t color)
{
  //if (_vpOoB) return;

  //x+= _xDatum;
  //y+= _yDatum;

  // Clipping
  //if ((y < _vpY) || (x >= _vpW) || (y >= _vpH)) return;

  //if (x < _vpX) { w += x - _vpX; x = _vpX; }

  //if ((x + w) > _vpW) w = _vpW - x;

  if (w < 1) return;

  //begin_tft_write();

  //setWindow(x, y, x + w - 1, y);

  //pushBlock(color, w);


  FillRect(x, y, w, 1, color);

  //end_tft_write();
}




void Ink_Sprite::FillRect(uint16_t posX,
                          uint16_t posY,
                          uint16_t width,
                          uint16_t height,
                          uint8_t pixBit)
{
    if( !isCreat ) return;

    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
            drawPix(posX + x, posY + y, pixBit);
}

void Ink_Sprite::drawFullBuff(uint8_t *buff, bool bitMode)
{
    if( !isCreat ) return;

    for (int i = 0; i < _pixSize; i++)
    {
        _spriteBuff[i] = (bitMode) ? buff[i] : ~buff[i];
    }
}

void Ink_Sprite::drawChar(uint16_t posX, uint16_t posY, char charData, Ink_eSPI_font_t *fontPtr)
{
    if( !isCreat ) return;

    charData -= 0x20;

    for (int y = 0; y < fontPtr->_height; y++)
    {
        for (int x = 0; x < fontPtr->_width; x++)
        {
            uint8_t mark = 0x80 >> (y * fontPtr->_width + x) % 8;
            if ((fontPtr->_fontptr[charData * fontPtr->_fontSize + (y * fontPtr->_width + x) / 8]) & mark)
            {
                drawPix(x + posX, y + posY, 0);
            }
            else
            {
                drawPix(x + posX, y + posY, 1);
            }
        }
    }
}

void Ink_Sprite::drawString(uint16_t posX, uint16_t posY, const char *charData, Ink_eSPI_font_t *fontPtr)
{
    if( !isCreat ) return;

    int _posX = posX;
    while (*charData != '\0')
    {
        drawChar(_posX, posY, *charData, fontPtr);
        _posX += fontPtr->_width;

        charData++;
    }
}

void Ink_Sprite::drawBuff(uint16_t posX,
                          uint16_t posY,
                          uint16_t width,
                          uint16_t height,
                          uint8_t *imageDataptr)
{
    if( !isCreat ) return;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            uint8_t mark = 0x80 >> (y * width + x) % 8;
            if ((imageDataptr[(y * width + x) / 8]) & mark)
            {
                drawPix(x + posX, y + posY, 1);
            }
            else
            {
                drawPix(x + posX, y + posY, 0);
            }
        }
    }
}

int Ink_Sprite::pushSprite()
{
    if( !isCreat ) return -1;

    if (_dev->getMode() != INK_PARTIAL_MODE)
    {
        _dev->clearDSRAM();
        _dev->switchMode(INK_PARTIAL_MODE);
    }
    _dev->setDrawAddr(_posX, _posY, _width, _height);
    _dev->drawBuff(_lastBuff, _spriteBuff, _pixSize);

    return 0;
}

int Ink_Sprite::deleteSprite()
{
    if( !isCreat ) return -1;

    free(_spriteBuff);
    free(_lastBuff);

    return 0;
}
