/*
 *  © 2023, Neil McKechnie. All rights reserved.
 *  
 *  This file is part of DCC++EX API
 *
 *  This is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  It is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CommandStation.  If not, see <https://www.gnu.org/licenses/>.
 */

/* 
 * This driver provides a more immediate interface into the OLED display
 * than the one installed through the config.h file.  When an LCD(...) call
 * is made, the text is output immediately to the specified display line,
 * without waiting for the next 2.5 second refresh.  However, if the line 
 * specified is off the screen then the text in the bottom line will be 
 * overwritten.  There is however a special case that if line 255 is specified, 
 * the existing text will scroll up and the new line added to the bottom
 * line of the screen.
 * 
 * To install, use the following command in myHal.cpp:

 *    OLEDDisplay::create(address, width, height);
 * 
 * where address is the I2C address (0x3c or 0x3d),
 * width is the width in pixels of the display, and
 * height is the height in pixels of the display.
 * 
 * Valid width and height are 128x32 (SSD1306 controller), 
 * 128x64 (SSD1306) and 132x64 (SH1106).  The driver uses
 * a 5x7 character set in a 6x8 pixel cell.
 */


#ifndef IO_OLEDDISPLAY_H
#define IO_OLEDDISPLAY_H

#include "IODevice.h"
#include "DisplayInterface.h"
#include "SSD1306Ascii.h"
#include "version.h"

class OLEDDisplay : public IODevice, DisplayInterface {
private:
  // Here we define the device-specific variables.  
  uint8_t _height; // in pixels
  uint8_t _width;  // in pixels
  SSD1306AsciiWire *oled;
  uint8_t _rowNo = 0;   // Row number being written by caller
  uint8_t _colNo = 0;  // Position in line being written by caller
  uint8_t _numRows;
  uint8_t _numCols;
  char *_buffer = NULL;
  uint8_t *_rowGeneration = NULL;
  uint8_t *_lastRowGeneration = NULL;
  uint8_t _rowNoToScreen = 0; 
  uint8_t _charPosToScreen = 0;

public:
  //  Static function to handle "OLEDDisplay::create(...)" calls.
  static void create(I2CAddress i2cAddress, int width = 128, int height=64) {
    /* if (checkNoOverlap(i2cAddress)) */ new OLEDDisplay(i2cAddress, width, height);
  } 

protected:
  // Constructor
  OLEDDisplay(I2CAddress i2cAddress, int width, int height) {
    _I2CAddress = i2cAddress;
    _width = width;
    _height = height;
    _numCols = _width / 6;    // character block 6 x 8 
    _numRows = _height / 8; 

    _charPosToScreen = _numCols;

    // Allocate arrays
    _buffer = (char *)calloc(_numRows*_numCols, sizeof(char));
    _rowGeneration = (uint8_t *)calloc(_numRows, sizeof(uint8_t));
    _lastRowGeneration = (uint8_t *)calloc(_numRows, sizeof(uint8_t));
    // Fill buffer with spaces
    memset(_buffer, ' ', _numCols*_numRows);

    // Create OLED driver
    oled = new SSD1306AsciiWire();

    // Clear the entire screen
    oled->clearNative();
    
    addDevice(this);
  }
  
  // Device-specific initialisation
  void _begin() override {
    // Initialise device
    if (oled->begin(_I2CAddress, _width, _height)) {
      // Store pointer to this object into CS display hook, so that we
      // will intercept any subsequent calls to lcdDisplay methods.
      DisplayInterface::lcdDisplay = this;

      DIAG(F("OLEDDisplay installed on address %s"), _I2CAddress.toString());

      // Set first two lines on screen
      LCD(0,F("DCC++ EX v%S"),F(VERSION));
      LCD(1,F("Lic GPLv3"));

      // Force all rows to be redrawn
      for (uint8_t row=0; row<_numRows; row++)
        _rowGeneration[row]++;
    }
  }

  void _loop(unsigned long) override {

    // Loop through the buffer and if a row has changed
    // (rowGeneration[row] is changed) then start writing the
    // characters from the buffer, one character per entry, 
    // to the screen until that row has been refreshed.

    // First check if the OLED driver is still busy from a previous 
    // call.  If so, don't to anything until the next entry.
    if (!oled->isBusy()) {
      // Check if we've just done the end of a row or just started
      if (_charPosToScreen >= _numCols) {
        // Move to next line
        if (++_rowNoToScreen >= _numRows)
          _rowNoToScreen = 0; // Wrap to first row

        if (_rowGeneration[_rowNoToScreen] != _lastRowGeneration[_rowNoToScreen]) {
          // Row content has changed, so start outputting it
          _lastRowGeneration[_rowNoToScreen] = _rowGeneration[_rowNoToScreen];
          oled->setRowNative(_rowNoToScreen);
          _charPosToScreen = 0;  // Prepare to output first character on next entry
        } else {
          // Row not changed, don't bother writing it.
        }
      } else {
        // output character at current position
        oled->writeNative(_buffer[_rowNoToScreen*_numCols+_charPosToScreen++]);
      }  
    }
    return;
  }
  
  /////////////////////////////////////////////////
  // DisplayInterface functions
  // 
  /////////////////////////////////////////////////
  DisplayInterface* loop2(bool force) override {
    (void)force;   // suppress compiler warning
    return NULL;
  }

  // Position on nominated line number (0 to number of lines -1)
  // Clear the line in the buffer ready for updating
  void setRow(byte line) override {
    if (line == 255) {
      // LCD(255, "xxx") - scroll the contents of the buffer
      // and put the new line at the bottom of the screen
      for (int row=1; row<_numRows; row++) {
        strncpy(&_buffer[(row-1)*_numCols], &_buffer[row*_numCols], _numCols);
        _rowGeneration[row-1]++;
      }
      line = _numRows-1;
    } else if (line >= _numRows) 
      line = _numRows - 1;  // Overwrite bottom line.

    _rowNo = line;
    // Fill line with blanks
    for (_colNo = 0; _colNo < _numCols; _colNo++)
      _buffer[_rowNo*_numCols+_colNo] = ' ';
    _colNo = 0;
    // Mark that the buffer has been touched.  It will be 
    // sent to the screen on the next loop entry.
    _rowGeneration[_rowNo]++;
  }

  // Write blanks to all of the screen (blocks until complete)
  void clear () override {
    // Clear buffer
    for (_rowNo = 0; _rowNo < _numRows; _rowNo++) {
      setRow(_rowNo);
    }
    _rowNo = 0;
  }

  // Write one character
  size_t write(uint8_t c) override {
    // Write character to buffer (if space)
    if (_colNo < _numCols)
      _buffer[_rowNo*_numCols+_colNo++] = c;
    return 1;
  }

  // Display information about the device.
  void _display() {
    DIAG(F("OLEDDisplay Configured addr %s"), _I2CAddress.toString());
  }

};

#endif // IO_OLEDDISPLAY_H