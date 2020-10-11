/*********************************************************************************
 * The MIT License (MIT)                                                         *
 * <p/>                                                                          *
 * Copyright (c) 2020 Bertrand Martel                                            *
 * <p/>                                                                          *
 * Permission is hereby granted, free of charge, to any person obtaining a copy  *
 * of this software and associated documentation files (the "Software"), to deal *
 * in the Software without restriction, including without limitation the rights  *
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell     *
 * copies of the Software, and to permit persons to whom the Software is         *
 * furnished to do so, subject to the following conditions:                      *
 * <p/>                                                                          *
 * The above copyright notice and this permission notice shall be included in    *
 * all copies or substantial portions of the Software.                           *
 * <p/>                                                                          *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR    *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,      *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER        *
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, *
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN     *
 * THE SOFTWARE.                                                                 *
 *********************************************************************************/
/*******************************************************************************
 * This is a library for the Adafruit 1.8" SPI display.                        *
 * This library works with the Adafruit 1.8" TFT Breakout w/SD card            *
 *   ----> http://www.adafruit.com/products/358                                *
 * The 1.8" TFT shield                                                         *
 *   ----> https://www.adafruit.com/product/802                                *
 * The 1.44" TFT breakout                                                      *
 *   ----> https://www.adafruit.com/product/2088                               *
 * as well as Adafruit raw 1.8" TFT display                                    *
 *   ----> http://www.adafruit.com/products/618                                *
 *   Check out the links above for our tutorials and wiring diagrams           *
 *   These displays use SPI to communicate, 4 or 5 pins are required to        *
 *   interface (RST is optional)                                               *
 *   Adafruit invests time and resources providing this open source code,      *
 *   please support Adafruit and open-source hardware by purchasing            *
 *   products from Adafruit!                                                   *
 *   Written by Limor Fried/Ladyada for Adafruit Industries.                   *
 *   MIT license, all text above must be included in any redistribution        *
 *******************************************************************************/

#include "st7735.h"
#include "common.h"
#include <wiringPiSPI.h>
#include "stdint.h"
#include <stdio.h>
#include <string.h>

uint32_t Adafruit_ST7735::send_spi_data_command(uint8_t * const d, const uint16_t  txlen)
{
	return wiringPiSPIDataRW(0, d, txlen);
}

void Adafruit_ST7735::writecommand(uint8_t c)
{
	digital_write(_rs, LOW);
	send_spi_data_command(&c, 1);
}

void Adafruit_ST7735::writeSPI(uint8_t * const d, uint16_t size)
{
	digital_write(_rs, HIGH);
	send_spi_data_command(d, size);
}

void Adafruit_ST7735::write8bit(uint8_t d)
{
	digital_write(_rs, HIGH);
	send_spi_data_command(&d, 1);

}

// Companion code to the above tables.  Reads and issues
// a series of LCD commands stored in PROGMEM byte array.
void Adafruit_ST7735::commandList(const uint8_t *addr)
{
	uint8_t i;
	uint8_t cmd_number = *(addr++);
	uint8_t  numArgs;
	uint16_t ms;
	for (i = 0; i < cmd_number; i++) {
		writecommand(*(addr++));
		numArgs  = *(addr++);    //   Number of args to follow
		ms       = numArgs & DELAY;          //   If hibit set, delay follows args
		numArgs &= ~DELAY;                   //   Mask out delay bit
		while (numArgs--) {                  //   For each argument...
			write8bit(*(addr++)); //     Read, issue argument
		}
		if (ms) {
			ms = *(addr++); // Read post-command delay time (ms)
			if (ms == 255) ms = 500;    // If 255, delay for 500 ms
			delay_ms(ms);
		}
	}
}

// Initialization code common to both 'B' and 'R' type displays
void Adafruit_ST7735::commonInit(const uint8_t *cmdList)
{
	colstart  = rowstart = 0; // May be overridden in init func

	pin_mode_output(_rs);

	if (_rst) {
		pin_mode_output(_rst);
		digital_write(_rst, HIGH);
		delay_ms(500);
		digital_write(_rst, LOW);
		delay_ms(500);
		digital_write(_rst, HIGH);
		delay_ms(500);
	}

	if (cmdList) commandList(cmdList);
}

// Initialization for ST7735R screens (green or red tabs)
int Adafruit_ST7735::initR(uint8_t options)
{
	int err_code = wiringPiSPISetup(0, 32000000);
	_width = ST7735_TFTWIDTH_128;
	_height = ST7735_TFTHEIGHT_160;
	commonInit(Rcmd1);
	if (options == INITR_GREENTAB) {
		commandList(Rcmd2green);
		colstart = 2;
		rowstart = 1;
	} else if ((options == INITR_144GREENTAB) || (options == INITR_HALLOWING)) {
		_height = ST7735_TFTHEIGHT_128;
		_width = ST7735_TFTWIDTH_128;
		commandList(Rcmd2green144);
		colstart = 2;
		rowstart = 3; // For default rotation 0
	} else if (options == INITR_MINI160x80) {
		_height = ST7735_TFTWIDTH_80;
		_width = ST7735_TFTHEIGHT_160;
		commandList(Rcmd2green160x80);
		colstart = 24;
		rowstart = 0;
	} else {
		// colstart, rowstart left at default '0' values
		commandList(Rcmd2red);
	}
	commandList(Rcmd3);
	// if black, change MADCTL color filter
	if (options == INITR_BLACKTAB) {
		writecommand(ST7735_MADCTL);
		write8bit(0xC0);
	}
	return err_code;
}

void Adafruit_ST7735::setAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{

	writecommand(ST7735_CASET); // Column addr set
	write8bit(0x00);
	write8bit(x0 + colstart); // XSTART
	write8bit(0x00);
	write8bit(x1 + colstart); // XEND

	writecommand(ST7735_RASET); // Row addr set
	write8bit(0x00);
	write8bit(y0 + rowstart); // YSTART
	write8bit(0x00);
	write8bit(y1 + rowstart); // YEND

	writecommand(ST7735_RAMWR); // write to RAM
}

// fill a rectangle
void Adafruit_ST7735::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
	// rudimentary clipping (drawChar w/big text requires this)
	if ((x >= _width) || (y >= _height)) return;
	if ((x + w - 1) >= _width)  w = _width  - x;
	if ((y + h - 1) >= _height) h = _height - y;

	setAddrWindow(x, y, x + w - 1, y + h - 1);

	uint8_t hi = color >> 8, lo = color;
	uint8_t data[TX_RX_BUF_LENGTH] = {};
	uint32_t count = 0;
	for (y = h; y > 0; y--) {
		for (x = w; x > 0; x--) {
			if (count == TX_RX_BUF_LENGTH) {
				writeSPI(data, count);
				count = 0;
			}
			data[count++] = hi;
			data[count++] = lo;
		}
	}
	writeSPI(data, count);
}

void Adafruit_ST7735::writePixel(uint16_t x, uint16_t y, uint16_t color) {
	if ((x >= 0) && (x < _width) && (y >= 0) && (y < _height)) {
		uint8_t data[2] = {};
		data[0] = color >> 8;
		data[1] = color;
		setAddrWindow(x, y, 1, 1);
		writeSPI(data, 2);
	}
}

void Adafruit_ST7735::fillScreen(uint16_t color)
{
	fillRect(0, 0,  _width, _height, color);
}

void Adafruit_ST7735::drawString(const char *c)
{
	drawString(c, textcolor, 1);
}

void Adafruit_ST7735::drawString(const char *c, uint16_t color, uint8_t size)
{
	if (color != textcolor){
		textcolor = color;
	}
	for (uint16_t i = 0; i < strlen(c); i++){
		write(c[i]);
	}
}

void Adafruit_ST7735::drawChar(char c)
{
	write(c);
}

void Adafruit_ST7735::write(uint8_t c) {
	if (c == '\n') {              // Newline?
		cursor_x = 0;               // Reset x to zero,
		cursor_y += textsize_y * 8; // advance y one line
	} else if (c != '\r') {       // Ignore carriage returns
		if (wrap && ((cursor_x + textsize_x * 6) > _width)) { // Off right?
			cursor_x = 0;                                       // Reset x to zero,
			cursor_y += textsize_y * 8; // advance y one line
		}
		drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize_x, textsize_y);
		cursor_x += textsize_x * 6; // Advance x one char
		if (cursor_x+6 >= _width) {
			cursor_x = 0;
			cursor_y +=textsize_y * 8;
		}
	}
}

void Adafruit_ST7735::drawChar(uint16_t x, uint16_t y, unsigned char c,
							uint16_t color, uint16_t bg, uint8_t size_x,
							uint8_t size_y)
{
	if ((x >= _width) ||              // Clip right
			(y >= _height) ||             // Clip bottom
			((x + 6 * size_x - 1) < 0) || // Clip left
			((y + 8 * size_y - 1) < 0))   // Clip top
		return;

	//if (!_cp437 && (c >= 176))
	//  c++; // Handle 'classic' charset behavior

	for (int8_t i = 0; i < 5; i++) { // Char bitmap = 5 columns
		uint8_t line = font[c * 5 + i];
		for (int8_t j = 0; j < 8; j++, line >>= 1) {
			if (line & 1) {
				if (size_x == 1 && size_y == 1){
					writePixel(x + i, y + j, color);
				}
				else{
					fillRect(x + i * size_x, y + j * size_y, size_x, size_y, color);
				}
			} else if (bg != color) {
				if (size_x == 1 && size_y == 1){
					writePixel(x + i, y + j, bg);
				}
				else{
					fillRect(x + i * size_x, y + j * size_y, size_x, size_y, bg);
				}
			}
		}
	}
	if (bg != color) { // If opaque, draw vertical line for last column
		if (size_x == 1 && size_y == 1){
			//writeFastVLine(x + 5, y, 8, bg);
		}
		else{
			fillRect(x + 5 * size_x, y, size_x, 8 * size_y, bg);
		}
	}
}

void Adafruit_ST7735::draw_bitmap_st7735(uint16_t pos_x, uint16_t pos_y, const uint16_t *image, uint16_t bitmap_width, uint16_t bitmap_height)
{

	int i = bitmap_width * (bitmap_height - 1);

	setAddrWindow(pos_x, pos_y - bitmap_height + 1, pos_x + bitmap_width - 1, pos_y);
	uint8_t data[TX_RX_BUF_LENGTH] = {};
	uint32_t count = 0;
	for (uint16_t y = 0; y < bitmap_height; y++) {

		for (uint16_t x = 0; x < bitmap_width; x++) {

			if (count == TX_RX_BUF_LENGTH) {
				writeSPI(data, count);
				count = 0;
			}
			data[count++] = image[i];
			data[count++] = (image[i] >> 8);
			i++;
		}
		i = i - 2 * bitmap_width;
	}
	writeSPI(data, count);
}

void Adafruit_ST7735::reset_bitmap_stream() {
	stream_x_pos = 0;
	stream_y_pos = 0;
}

void Adafruit_ST7735::draw_bitmap_st7735_stream(const uint8_t *image, unsigned long length)
{
	int i = _width * 2 - 1;

	int x0 = stream_x_pos;
	uint32_t y0 = stream_y_pos;
	uint32_t x1 = (((stream_x_pos + length) / 2) % _width);
	uint32_t y1 = stream_y_pos + (length / _width) / 2;

	setAddrWindow(x0, y0, _width - 1, _height);
	uint8_t data[TX_RX_BUF_LENGTH] = {};
	uint16_t pow = 1;
	uint32_t count = 0;
	for (uint16_t y = y0; y < (y1 + 1); y++) {

		if (y == y1) {
			x0 = x1;
			if (x0 == 0) {
				break;
			}
		}

		for (int x = _width; x >= (x0 + 1); x--) {

			if (count == TX_RX_BUF_LENGTH) {
				writeSPI(data, count);
				count = 0;
			}

			data[count++] = image[i];

			if ((y == y1) && ((x1 % 2) != 0)) {

			}
			else {
				data[count++] = image[i - 1];
			}
			i -= 2;
		}
		pow++;
		i = _width * 2 * pow - 1;
		x0 = 0;
	}
	writeSPI(data, count);

	stream_x_pos = x1;
	stream_y_pos = y1;
}

Adafruit_ST7735::Adafruit_ST7735(uint32_t dc, uint32_t rst){
	_rs   = dc;
	_rst  = rst;
}

void Adafruit_ST7735::setCursor(int16_t x, int16_t y) {
	cursor_x = x;
	cursor_y = y;
}

void Adafruit_ST7735::setTextBackground(uint16_t bg) {
	textbgcolor = bg;
}

void Adafruit_ST7735::setTextColor(uint16_t color) {
	textcolor = color;
}