/*
  Created by Fabrizio Di Vittorio (fdivitto2013@gmail.com) - <http://www.fabgl.com>
  Copyright (c) 2019-2020 Fabrizio Di Vittorio.
  All rights reserved.

  This file is part of FabGL Library.

  FabGL is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  FabGL is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with FabGL.  If not, see <http://www.gnu.org/licenses/>.
 */


#pragma once



/**
 * @file
 *
 * @brief This file contains fabgl::VGADirectController definition.
 */


#include <stdint.h>
#include <stddef.h>
#include <atomic>

#include "rom/lldesc.h"
#include "driver/gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "fabglconf.h"
#include "fabutils.h"
#include "devdrivers/swgenerator.h"
#include "displaycontroller.h"
#include "dispdrivers/vgabasecontroller.h"



#define VGAD_LinesCount 2



namespace fabgl {



/**
 * @brief Represents an base abstract class for direct draw VGA controller
 *
 * A direct draw VGA controller draws paint the screen in real time, for each scanline. Every two scanlines an interrupt is generated.
 * Put your drawing code inside this interrupt, implementing abstract method drawScanline().
 *
 * See DirectVGA.ino example.
 */
class VGADirectController : public VGABaseController {

public:

  VGADirectController();

  // unwanted methods
  VGADirectController(VGADirectController const&) = delete;
  void operator=(VGADirectController const&)  = delete;


  /**
   * @brief Returns the singleton instance of VGADirectController class
   *
   * @return A pointer to VGADirectController singleton object
   */
  static VGADirectController * instance() { return s_instance; }

  // abstract method of BitmappedDisplayController
  NativePixelFormat nativePixelFormat()               { return NativePixelFormat::PALETTE16; }

  // import "modeline" v3rsion of setResolution
  using VGABaseController::setResolution;

  void setResolution(VGATimings const& timings, int viewPortWidth = -1, int viewPortHeight = -1, bool doubleBuffered = false);

  void readScreen(Rect const & rect, RGB888 * destBuf);

  virtual void IRAM_ATTR drawScanline(uint8_t * dest, int scanLine) = 0;

private:

  void init();

  void onSetupDMABuffer(lldesc_t volatile * buffer, bool isStartOfVertFrontPorch, int scan, bool isVisible, int visibleRow);
  void allocateViewPort();
  void freeViewPort();


  // abstract method of BitmappedDisplayController
  void setPixelAt(PixelDesc const & pixelDesc, Rect & updateRect);

  // abstract method of BitmappedDisplayController
  void drawEllipse(Size const & size, Rect & updateRect);

  // abstract method of BitmappedDisplayController
  void clear(Rect & updateRect);

  // abstract method of BitmappedDisplayController
  void VScroll(int scroll, Rect & updateRect);

  // abstract method of BitmappedDisplayController
  void HScroll(int scroll, Rect & updateRect);

  // abstract method of BitmappedDisplayController
  void drawGlyph(Glyph const & glyph, GlyphOptions glyphOptions, RGB888 penColor, RGB888 brushColor, Rect & updateRect);

  // abstract method of BitmappedDisplayController
  void invertRect(Rect const & rect, Rect & updateRect);

  // abstract method of BitmappedDisplayController
  void copyRect(Rect const & source, Rect & updateRect);

  // abstract method of BitmappedDisplayController
  void swapFGBG(Rect const & rect, Rect & updateRect);

  // abstract method of BitmappedDisplayController
  void rawDrawBitmap_Native(int destX, int destY, Bitmap const * bitmap, int X1, int Y1, int XCount, int YCount);

  // abstract method of BitmappedDisplayController
  void rawDrawBitmap_Mask(int destX, int destY, Bitmap const * bitmap, void * saveBackground, int X1, int Y1, int XCount, int YCount);

  // abstract method of BitmappedDisplayController
  void rawDrawBitmap_RGBA2222(int destX, int destY, Bitmap const * bitmap, void * saveBackground, int X1, int Y1, int XCount, int YCount);

  // abstract method of BitmappedDisplayController
  void rawDrawBitmap_RGBA8888(int destX, int destY, Bitmap const * bitmap, void * saveBackground, int X1, int Y1, int XCount, int YCount);

  // abstract method of BitmappedDisplayController
  void rawFillRow(int y, int x1, int x2, RGB888 color);

  void rawFillRow(int y, int x1, int x2, uint8_t colorIndex);

  void rawInvertRow(int y, int x1, int x2);

  void rawCopyRow(int x1, int x2, int srcY, int dstY);

  void swapRows(int yA, int yB, int x1, int x2);

  // abstract method of BitmappedDisplayController
  void absDrawLine(int X1, int Y1, int X2, int Y2, RGB888 color);

  // abstract method of BitmappedDisplayController
  int getBitmapSavePixelSize() { return 1; }

  static void I2SInterrupt(void * arg);


  static VGADirectController * s_instance;
  static volatile int          s_scanLine;
  static lldesc_t volatile *   s_frameResetDesc;


  volatile uint8_t *          m_lines[VGAD_LinesCount];

};



} // end of namespace








