/*********************************************************************
 This is a library for our Monochrome OLEDs based on SSD1322 drivers

 Pick one up today in the adafruit shop!
 ------> http://www.adafruit.com/category/63_98

 These displays use SPI to communicate, 4 or 5 pins are required to
 interface

 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 Written by Limor Fried/Ladyada  for Adafruit Industries.
 BSD license, check license.txt for more information
 All text above, and the splash screen below must be included in any redistribution
 *********************************************************************/

#include <avr/pgmspace.h>
#ifndef __SAM3X8E__
#include <util/delay.h>
#endif
#include <stdlib.h>

#include <Wire.h>

#include "Adafruit_GFX.h"
#include "Adafruit_SSD1322.h"

// the memory buffer for the LCD

static uint8_t buffer[SSD1322_LCDHEIGHT * SSD1322_LCDWIDTH / 8] = { 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x01, 0x80, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00,
		0x00, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00,
		0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x10, 0x08, 0x40, 0x01, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x01, 0xE0, 0x00, 0x00,
		0x00, 0x00, 0x20, 0xF8, 0x08, 0x04, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00,
		0x01, 0x00, 0x03, 0x10, 0x08, 0x40, 0x01, 0x00, 0x0D, 0xF0, 0x01, 0x00,
		0x07, 0xF1, 0x80, 0x00, 0x01, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x13, 0x08,
		0x08, 0x84, 0x04, 0x00, 0x03, 0xE0, 0x01, 0x70, 0x00, 0x80, 0x1C, 0x10,
		0x08, 0x40, 0x01, 0xFC, 0x35, 0x10, 0x00, 0x80, 0x00, 0x11, 0x80, 0x00,
		0x03, 0xE4, 0x00, 0x00, 0x00, 0x00, 0x04, 0xA0, 0x08, 0xA4, 0x07, 0x3C,
		0x0C, 0x20, 0x1F, 0x80, 0x00, 0x00, 0x04, 0x50, 0x08, 0x78, 0x3E, 0x00,
		0x29, 0xD0, 0x04, 0x40, 0x03, 0x11, 0x80, 0x00, 0x07, 0xEF, 0xC0, 0x00,
		0x00, 0x00, 0x41, 0x10, 0x08, 0xA4, 0x3D, 0x44, 0x08, 0x20, 0x02, 0x00,
		0x00, 0xFC, 0x07, 0x10, 0x0E, 0xC0, 0x02, 0x00, 0x29, 0x20, 0x04, 0x40,
		0x1C, 0x11, 0x80, 0x00, 0x0F, 0xEF, 0xFE, 0x00, 0x00, 0x00, 0x22, 0x80,
		0x0C, 0xA4, 0x05, 0x74, 0x08, 0x20, 0x02, 0x80, 0x3F, 0x80, 0x1C, 0x50,
		0x38, 0x40, 0x03, 0xE0, 0x25, 0xE0, 0x08, 0x20, 0x00, 0x11, 0x80, 0x00,
		0x0F, 0xE7, 0xFF, 0xE0, 0x00, 0x00, 0x00, 0xF8, 0x38, 0xA4, 0x05, 0x44,
		0x0F, 0xA0, 0x04, 0x80, 0x01, 0x00, 0x66, 0x10, 0x08, 0x70, 0x06, 0x20,
		0x3D, 0x08, 0x09, 0x18, 0x07, 0x11, 0x80, 0x00, 0x07, 0xE7, 0x3F, 0xFE,
		0x00, 0x00, 0x17, 0x80, 0x08, 0xA4, 0x09, 0x44, 0x08, 0x20, 0x09, 0xF0,
		0x01, 0xE0, 0x0D, 0x3E, 0x0D, 0x90, 0x0B, 0xA0, 0x21, 0x90, 0x11, 0x0E,
		0x19, 0x11, 0x80, 0x00, 0x07, 0xE7, 0x07, 0xFF, 0xF0, 0x00, 0x11, 0xC0,
		0x0E, 0xA4, 0x09, 0x78, 0x08, 0x20, 0x0E, 0x80, 0x02, 0x20, 0x14, 0xD0,
		0x18, 0xA0, 0x12, 0x20, 0x21, 0x60, 0x22, 0x00, 0x11, 0x11, 0x80, 0x00,
		0x07, 0xE7, 0x00, 0xFF, 0xFE, 0x00, 0x22, 0xA0, 0x19, 0x24, 0x11, 0x02,
		0x08, 0x20, 0x00, 0x90, 0x04, 0x20, 0x24, 0x10, 0x68, 0x40, 0x22, 0x20,
		0x21, 0x20, 0x04, 0x40, 0x1F, 0x11, 0x80, 0x00, 0x07, 0xF7, 0x00, 0x1F,
		0xFE, 0x00, 0x24, 0x98, 0x61, 0x24, 0x21, 0x02, 0x0F, 0xE0, 0x08, 0x88,
		0x08, 0x20, 0x04, 0x10, 0x08, 0xA0, 0x43, 0xA0, 0x21, 0x5E, 0x09, 0xE0,
		0x10, 0x11, 0x80, 0x00, 0x03, 0xF7, 0x00, 0x03, 0xFE, 0x00, 0x28, 0x8E,
		0x02, 0x04, 0x41, 0x02, 0x00, 0x20, 0x08, 0x84, 0x31, 0x40, 0x04, 0x10,
		0x0B, 0x18, 0x02, 0x20, 0x21, 0x80, 0x0E, 0x20, 0x00, 0x11, 0x80, 0x00,
		0x03, 0xF7, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x80, 0x04, 0x04, 0x00, 0xFC,
		0x00, 0x00, 0x11, 0x80, 0x00, 0x80, 0x04, 0x10, 0x18, 0x0E, 0x04, 0x60,
		0x21, 0x00, 0x00, 0x00, 0x00, 0x71, 0x80, 0x00, 0x03, 0xF3, 0x00, 0x00,
		0x1C, 0x00, 0x00, 0x80, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
		0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x04, 0x20, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x21, 0x80, 0x00, 0x01, 0xF3, 0x00, 0x00, 0x1C, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0xC0,
		0x01, 0xF3, 0x80, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0xF8, 0x01, 0xF3, 0x80, 0x00,
		0x3C, 0x00, 0x0F, 0x30, 0x00, 0x01, 0xFB, 0x00, 0x00, 0x03, 0xE0, 0x00,
		0x27, 0xCC, 0x03, 0x00, 0x00, 0x3F, 0x00, 0x03, 0x00, 0x00, 0x18, 0x00,
		0x00, 0x01, 0x80, 0x7F, 0x00, 0xF3, 0x80, 0x00, 0x3C, 0x00, 0x19, 0xB0,
		0x00, 0x00, 0x1B, 0x00, 0x00, 0x03, 0x00, 0x00, 0x66, 0x60, 0x00, 0x00,
		0x00, 0x0C, 0x00, 0x03, 0x00, 0x00, 0x18, 0x00, 0x00, 0x01, 0x80, 0x7F,
		0xC0, 0xF3, 0x80, 0x00, 0x38, 0x00, 0x18, 0x3E, 0x3C, 0xF8, 0x33, 0xE3,
		0xCF, 0x83, 0x07, 0x1E, 0xF6, 0x6C, 0xF3, 0x7C, 0x7C, 0x0C, 0x3C, 0x73,
		0xE7, 0xC7, 0x99, 0xE3, 0xF8, 0xC1, 0x80, 0x3F, 0xF8, 0xF3, 0x80, 0x00,
		0x38, 0x00, 0x1E, 0x33, 0x66, 0xCC, 0x63, 0x36, 0x6C, 0xC3, 0xE9, 0xB3,
		0x66, 0x6D, 0x9B, 0x66, 0xCC, 0x0C, 0x66, 0xDB, 0x36, 0x6C, 0xDB, 0x36,
		0x6D, 0x81, 0x80, 0x3F, 0xFE, 0xF9, 0x80, 0x00, 0x38, 0x00, 0x07, 0xB3,
		0x7E, 0xCC, 0x63, 0x37, 0xEC, 0xC3, 0x07, 0xBC, 0x67, 0xCD, 0xE3, 0x66,
		0xCC, 0x0C, 0x7E, 0xC3, 0x36, 0x6C, 0xDB, 0x36, 0x6D, 0x81, 0x80, 0x1D,
		0xFF, 0x79, 0x80, 0x00, 0x78, 0x00, 0x01, 0xB3, 0x60, 0xCC, 0xC3, 0x36,
		0x0C, 0xC3, 0x0D, 0x8F, 0x66, 0xCC, 0x7B, 0x66, 0xCC, 0x0C, 0x60, 0xC3,
		0x36, 0x6C, 0xDB, 0x36, 0x6D, 0x81, 0x80, 0x1C, 0x3F, 0x79, 0x80, 0x00,
		0x70, 0x00, 0x19, 0xB3, 0x66, 0xCD, 0x83, 0x36, 0x6C, 0xC3, 0x0D, 0xB3,
		0x66, 0x6D, 0x9B, 0x66, 0xCC, 0x0C, 0x66, 0xDB, 0x36, 0x6C, 0xDB, 0x36,
		0x67, 0x01, 0x80, 0x1C, 0x07, 0x79, 0x80, 0x00, 0x70, 0x00, 0x0F, 0x33,
		0x3C, 0xCD, 0xFB, 0x33, 0xCC, 0xC3, 0xE7, 0x9E, 0x36, 0x3C, 0xF3, 0x66,
		0x7C, 0x0C, 0x3C, 0x73, 0x36, 0x67, 0x99, 0xE3, 0xE7, 0x01, 0x80, 0x0E,
		0x00, 0x39, 0x80, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8C, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x04, 0x66, 0x01, 0x80, 0x0E, 0x00, 0x39, 0xC0, 0x00,
		0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03,
		0xDC, 0x01, 0x80, 0x06, 0x00, 0x39, 0xC0, 0x00, 0x60, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x07,
		0x00, 0x18, 0xC0, 0x00, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x03, 0x00, 0x18, 0xC0, 0x00,
		0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x01, 0x80, 0x03, 0x00, 0x1C, 0xC0, 0x00, 0xE0, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x01,
		0x80, 0x1C, 0xFF, 0xFF, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x01, 0x80, 0x0C, 0xFF, 0xFF,
		0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0xE7, 0xF8, 0x00, 0xF0, 0x60,
		0x3F, 0xCF, 0xC1, 0xC1, 0xC7, 0x0F, 0x87, 0x80, 0x0C, 0x00, 0x00, 0x00,
		0x00, 0x01, 0x80, 0x00, 0x80, 0x0C, 0xFF, 0xFF, 0xC0, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x1F, 0xE7, 0xFC, 0x03, 0xFC, 0x60, 0x3F, 0xCF, 0xF1, 0xC1,
		0xCF, 0x9F, 0xCF, 0xC0, 0x1C, 0x00, 0x00, 0x00, 0x33, 0x01, 0x80, 0x00,
		0x80, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x06,
		0x0C, 0x03, 0x0C, 0x60, 0x30, 0x0C, 0x31, 0xE3, 0xDD, 0xD8, 0xD8, 0xC0,
		0x3C, 0x00, 0x00, 0x00, 0x33, 0x01, 0x80, 0x00, 0x00, 0x04, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x06, 0x0C, 0x06, 0x06, 0x60,
		0x30, 0x0C, 0x19, 0xE3, 0xD8, 0xC0, 0xD8, 0xC0, 0x6C, 0x0E, 0x03, 0x1C,
		0x33, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x1F, 0xE6, 0x0C, 0x06, 0x06, 0x60, 0x3F, 0xCC, 0x19, 0xA2,
		0xD8, 0xC3, 0x80, 0xC0, 0x4C, 0x1B, 0x07, 0x36, 0x44, 0x01, 0x87, 0xE0,
		0x00, 0x02, 0x3F, 0x0C, 0x00, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x1F, 0xE7,
		0xF8, 0x06, 0x06, 0x60, 0x3F, 0xCC, 0x19, 0xB6, 0xD8, 0xC3, 0x81, 0x80,
		0x0C, 0x03, 0x0F, 0x06, 0x00, 0x01, 0x86, 0x00, 0x00, 0x06, 0x31, 0x80,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x07, 0xF0, 0xF6, 0x06, 0x60,
		0x30, 0x0C, 0x19, 0xB6, 0xD8, 0xC0, 0xC3, 0x0F, 0x0C, 0x06, 0x0B, 0x06,
		0x00, 0x01, 0x86, 0x03, 0xC3, 0xCF, 0x31, 0x8C, 0x78, 0xCD, 0x86, 0xC0,
		0x00, 0x00, 0x18, 0x06, 0x38, 0xF6, 0x06, 0x60, 0x30, 0x0C, 0x19, 0x9C,
		0xD8, 0xD8, 0xC6, 0x0F, 0x0C, 0x03, 0x03, 0x0C, 0x00, 0x01, 0x86, 0x04,
		0x66, 0x66, 0x31, 0x8C, 0xCC, 0xCE, 0xCD, 0xC0, 0x00, 0x00, 0x18, 0x06,
		0x1C, 0x03, 0x0C, 0x60, 0x30, 0x0C, 0x31, 0x9C, 0xDD, 0xDC, 0xCC, 0x00,
		0x0C, 0x03, 0x03, 0x18, 0x00, 0x01, 0x87, 0xE1, 0xE7, 0x06, 0x3F, 0x0C,
		0xE0, 0xCC, 0xCC, 0xC0, 0x00, 0x00, 0x1F, 0xE6, 0x0C, 0x03, 0xFC, 0x7F,
		0x3F, 0xCF, 0xF1, 0x9C, 0xCF, 0x8F, 0x9F, 0xC0, 0x0C, 0x1B, 0x63, 0x30,
		0x00, 0x01, 0x86, 0x03, 0x63, 0xC6, 0x33, 0x0C, 0x78, 0xCC, 0xCC, 0xC0,
		0x00, 0x00, 0x1F, 0xE6, 0x0E, 0x00, 0xF0, 0x7F, 0x3F, 0xCF, 0xC1, 0x88,
		0xC7, 0x07, 0x1F, 0xC0, 0x0C, 0x0E, 0x63, 0x3E, 0x00, 0x01, 0x86, 0x06,
		0x60, 0xE6, 0x31, 0x8C, 0x1C, 0xCC, 0xCC, 0xC0, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x86, 0x06, 0x66, 0x66, 0x31, 0x8C,
		0xCC, 0xCC, 0xCD, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x01, 0x87, 0xE3, 0xE3, 0xC3, 0x30, 0xCC, 0x78, 0xCC, 0xC6, 0xC0,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0xC0, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x07, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x01, 0xBF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xBF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x01, 0xC7, 0x9C, 0x47, 0x04, 0x3C, 0x01, 0x00,
		0x04, 0x38, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x04, 0x5E, 0x00,
		0x10, 0x11, 0xA0, 0x00, 0x00, 0x28, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00,
		0x02, 0x24, 0x22, 0xE8, 0x8C, 0x22, 0x01, 0x00, 0x0C, 0x44, 0x44, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x14, 0x04, 0x61, 0x00, 0x00, 0x11, 0xA0, 0x00,
		0x00, 0x20, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x28, 0x20, 0x48,
		0x14, 0x21, 0x3B, 0x9C, 0x14, 0x40, 0x82, 0xA7, 0x22, 0x05, 0x8E, 0x53,
		0x94, 0xE4, 0xA0, 0x39, 0x53, 0x91, 0xBC, 0x8A, 0x21, 0xE8, 0xEF, 0x27,
		0x22, 0x18, 0xE7, 0x60, 0x00, 0x2F, 0x3C, 0xAF, 0x14, 0x21, 0x45, 0x22,
		0x04, 0x78, 0x80, 0xC8, 0xA2, 0x06, 0x51, 0x64, 0x55, 0x14, 0x98, 0x45,
		0x94, 0x51, 0xA2, 0x89, 0x42, 0x29, 0x08, 0xA0, 0x94, 0x25, 0x14, 0x90,
		0x00, 0x40, 0xA2, 0x08, 0xA4, 0x21, 0x45, 0x18, 0x04, 0x44, 0x8E, 0x87,
		0x94, 0x04, 0x4F, 0x43, 0xD5, 0xF4, 0x86, 0x7D, 0x13, 0xD1, 0xA2, 0x89,
		0x5A, 0x28, 0xC8, 0xA3, 0x94, 0x21, 0x14, 0x90, 0x00, 0x80, 0xA2, 0x08,
		0xBE, 0x21, 0x45, 0x04, 0x04, 0x44, 0x82, 0x88, 0x94, 0x04, 0x51, 0x44,
		0x55, 0x04, 0x81, 0x41, 0x14, 0x51, 0xA2, 0x89, 0x42, 0x28, 0x28, 0xA4,
		0x94, 0x25, 0x14, 0x90, 0x01, 0x08, 0xA2, 0x08, 0x84, 0x22, 0x45, 0x22,
		0x04, 0x44, 0x44, 0x89, 0x88, 0x06, 0x53, 0x44, 0xD5, 0x15, 0x21, 0x45,
		0x14, 0xD1, 0xBC, 0x78, 0x81, 0xE9, 0xCF, 0x23, 0x88, 0x98, 0xE4, 0x90,
		0x03, 0xE7, 0x1C, 0x07, 0x04, 0x3C, 0x39, 0x9C, 0x04, 0x38, 0x38, 0x86,
		0x88, 0x05, 0x8D, 0x43, 0x54, 0xE5, 0x1E, 0x39, 0x13, 0x51, 0x80, 0x00,
		0x80, 0x00, 0x08, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x04, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x03, 0x00, 0x00, 0x08, 0x00,
		0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x10, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

// the most basic function, set a single pixel
void Adafruit_SSD1322::drawPixel(int16_t x, int16_t y, uint16_t color) {
	if ((x < 0) || (x >= width()) || (y < 0) || (y >= height()))
		return;

	// check rotation, move pixel around if necessary
	switch (getRotation()) {
	case 1:
		swap(x, y)
		;
		x = WIDTH - x - 1;
		break;
	case 2:
		x = WIDTH - x - 1;
		y = HEIGHT - y - 1;
		break;
	case 3:
		swap(x, y)
		;
		y = HEIGHT - y - 1;
		break;
	}

	// x is which column
	switch (color) {
	case WHITE:
		buffer[x + (y / 8) * SSD1322_LCDWIDTH] |= (1 << (y & 7));
		break;
	case BLACK:
		buffer[x + (y / 8) * SSD1322_LCDWIDTH] &= ~(1 << (y & 7));
		break;
	case INVERSE:
		buffer[x + (y / 8) * SSD1322_LCDWIDTH] ^= (1 << (y & 7));
		break;
	}

}

Adafruit_SSD1322::Adafruit_SSD1322(int8_t SID, int8_t SCLK, int8_t DC,
		int8_t RST, int8_t CS) :
		Adafruit_GFX(SSD1322_LCDWIDTH, SSD1322_LCDHEIGHT) {
	cs = CS;
	rst = RST;
	dc = DC;
	sclk = SCLK;
	sid = SID;
	hwSPI = false;
}

// constructor for hardware SPI - we indicate DataCommand, ChipSelect, Reset 
Adafruit_SSD1322::Adafruit_SSD1322(int8_t DC, int8_t RST, int8_t CS) :
		Adafruit_GFX(SSD1322_LCDWIDTH, SSD1322_LCDHEIGHT) {
	dc = DC;
	rst = RST;
	cs = CS;
	hwSPI = true;
}

// initializer for I2C - we only indicate the reset pin!
Adafruit_SSD1322::Adafruit_SSD1322(int8_t reset) :
		Adafruit_GFX(SSD1322_LCDWIDTH, SSD1322_LCDHEIGHT) {
	sclk = dc = cs = sid = -1;
	rst = reset;
}

void Adafruit_SSD1322::begin(uint8_t i2caddr, bool reset) {
	_i2caddr = i2caddr;

	// set pin directions
	if (sid != -1) {
		pinMode(dc, OUTPUT);
		pinMode(cs, OUTPUT);
#ifndef ESP8266    					//Added for compatibility with ESP8266 board
		csport = portOutputRegister(digitalPinToPort(cs));
		cspinmask = digitalPinToBitMask(cs);
		dcport = portOutputRegister(digitalPinToPort(dc));
		dcpinmask = digitalPinToBitMask(dc);
#endif
		if (!hwSPI) {
			// set pins for software-SPI
			pinMode(sid, OUTPUT);
			pinMode(sclk, OUTPUT);
#ifndef ESP8266    					//Added for compatibility with ESP8266 board
			clkport = portOutputRegister(digitalPinToPort(sclk));
			clkpinmask = digitalPinToBitMask(sclk);
			mosiport = portOutputRegister(digitalPinToPort(sid));
			mosipinmask = digitalPinToBitMask(sid);
#endif
		}
		if (hwSPI) {
			SPI.begin();
#ifdef __SAM3X8E__
			SPI.setClockDivider (9); // 9.3 MHz
#elif defined ESP8266    					//Added for compatibility with ESP8266 board
			SPI.setClockDivider (SPI_CLOCK_DIV2); // 26/2 = 13 MHz (freq ESP8266 26 MHz)
#else
			SPI.setClockDivider(SPI_CLOCK_DIV2); // 8 MHz
#endif
		}
	} else {
		// I2C Init
		Wire.begin();
#ifdef __SAM3X8E__
		// Force 400 KHz I2C, rawr! (Uses pins 20, 21 for SDA, SCL)
		TWI1->TWI_CWGR = 0;
		TWI1->TWI_CWGR = ((VARIANT_MCK / (2 * 400000)) - 4) * 0x101;
#endif
	}

	if (reset) {
		// Setup reset pin direction (used by both SPI and I2C)
		pinMode(rst, OUTPUT);
		digitalWrite(rst, HIGH);
		// VDD (3.3V) goes high at start, lets just chill for a ms
		delay(1);
		// bring reset low
		digitalWrite(rst, LOW);
		// wait 10ms
		delay(10);
		// bring out of reset
		digitalWrite(rst, HIGH);
		// turn on VCC (9V?)
	}

#if defined SSD1322_256_64

	// Init sequence for 256x64 OLED module
	ssd1322_command(SSD1322_SETCOMMANDLOCK);// 0xFD
	ssd1322_command(0x12);// Unlock OLED driver IC

	ssd1322_command(SSD1322_DISPLAYOFF);// 0xAE

	ssd1322_command(SSD1322_SETCLOCKDIVIDER);// 0xB3
	ssd1322_command(0x91);// 0xB3

	ssd1322_command(SSD1322_SETMUXRATIO);// 0xCA
	ssd1322_command(0x3F);// duty = 1/64

	ssd1322_command(SSD1322_SETDISPLAYOFFSET);// 0xA2
	ssd1322_command(0x00);

	ssd1322_command(SSD1322_SETSTARTLINE);// 0xA1
	ssd1322_command(0x00);

	ssd1322_command(SSD1322_SETREMAP);// 0xA0
	ssd1322_command(0x14);//Horizontal address increment,Disable Column Address Re-map,Enable Nibble Re-map,Scan from COM[N-1] to COM0,Disable COM Split Odd Even
	ssd1322_command(0x11);//Enable Dual COM mode

	ssd1322_command(SSD1322_FUNCTIONSEL);// 0xAB
	ssd1322_command(0x01);// selection external vdd

	ssd1322_command(SSD1322_DISPLAYENHANCE);// 0xB4
	ssd1322_command(0xA0);// enables the external VSL
	ssd1322_command(0xFD);// 0xfFD,Enhanced low GS display quality;default is 0xb5(normal),

	ssd1322_command(SSD1322_SETCONTRASTCURRENT);// 0xC1
	ssd1322_command(0xFF);// 0xFF - default is 0x7f

	ssd1322_command(SSD1322_MASTERCURRENTCONTROL);// 0xC7
	ssd1322_command(0x0F);// default is 0x0F

	ssd1322_command(SSD1322_SETPHASELENGTH);// 0xB1
	ssd1322_command(0xE2);// default is 0x74

	ssd1322_command(SSD1322_DISPLAYENHANCEB);// 0xD1
	ssd1322_command(0x82);// Reserved;default is 0xa2(normal)
	ssd1322_command(0x20);//

	ssd1322_command(SSD1322_SETPRECHARGEVOLTAGE);// 0xBB
	ssd1322_command(0x1F);// 0.6xVcc

	ssd1322_command(SSD1322_SETSECONDPRECHARGEPERIOD);// 0xB6
	ssd1322_command(0x08);// default

	ssd1322_command(SSD1322_SETVCOMH);// 0xBE
	ssd1322_command(0x07);// 0.86xVcc;default is 0x04

	ssd1322_command(SSD1322_NORMALDISPLAY);// 0xA6

#endif

	ssd1322_command(SSD1322_DISPLAYON);// 0xAF
}

void Adafruit_SSD1322::invertDisplay(uint8_t i) {
	if (i) {
		ssd1322_command(SSD1322_INVERSEDISPLAY);
	} else {
		ssd1322_command(SSD1322_NORMALDISPLAY);
	}
}

// startscrollright
// Activate a right handed scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F) 
void Adafruit_SSD1322::startscrollright(uint8_t start, uint8_t stop) {
	ssd1322_command(SSD1322_RIGHT_HORIZONTAL_SCROLL);
	ssd1322_command(0X00);
	ssd1322_command(start);
	ssd1322_command(0X00);
	ssd1322_command(stop);
	ssd1322_command(0X00);
	ssd1322_command(0XFF);
	ssd1322_command(SSD1322_ACTIVATE_SCROLL);
}

// startscrollleft
// Activate a right handed scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F) 
void Adafruit_SSD1322::startscrollleft(uint8_t start, uint8_t stop) {
	ssd1322_command(SSD1322_LEFT_HORIZONTAL_SCROLL);
	ssd1322_command(0X00);
	ssd1322_command(start);
	ssd1322_command(0X00);
	ssd1322_command(stop);
	ssd1322_command(0X00);
	ssd1322_command(0XFF);
	ssd1322_command(SSD1322_ACTIVATE_SCROLL);
}

// startscrolldiagright
// Activate a diagonal scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F) 
void Adafruit_SSD1322::startscrolldiagright(uint8_t start, uint8_t stop) {
	ssd1322_command(SSD1322_SET_VERTICAL_SCROLL_AREA);
	ssd1322_command(0X00);
	ssd1322_command(SSD1322_LCDHEIGHT);
	ssd1322_command(SSD1322_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL);
	ssd1322_command(0X00);
	ssd1322_command(start);
	ssd1322_command(0X00);
	ssd1322_command(stop);
	ssd1322_command(0X01);
	ssd1322_command(SSD1322_ACTIVATE_SCROLL);
}

// startscrolldiagleft
// Activate a diagonal scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F) 
void Adafruit_SSD1322::startscrolldiagleft(uint8_t start, uint8_t stop) {
	ssd1322_command(SSD1322_SET_VERTICAL_SCROLL_AREA);
	ssd1322_command(0X00);
	ssd1322_command(SSD1322_LCDHEIGHT);
	ssd1322_command(SSD1322_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL);
	ssd1322_command(0X00);
	ssd1322_command(start);
	ssd1322_command(0X00);
	ssd1322_command(stop);
	ssd1322_command(0X01);
	ssd1322_command(SSD1322_ACTIVATE_SCROLL);
}

void Adafruit_SSD1322::stopscroll(void) {
	ssd1322_command(SSD1322_DEACTIVATE_SCROLL);
}

// Dim the display
// dim = true: display is dimmed
// dim = false: display is normal
void Adafruit_SSD1322::dim(boolean dim) {
	uint8_t contrast;

	if (dim) {
		contrast = 0; // Dimmed display
	}
//	else {
//		if (_vccstate == SSD1322_EXTERNALVCC) {
//			contrast = 0x9F;
//		} else {
//			contrast = 0xCF;
//		}
//	}
	// the range of contrast to too small to be really useful
	// it is useful to dim the display
	ssd1322_command(SSD1322_SETCONTRASTCURRENT);
	ssd1322_command(contrast);
}

void Adafruit_SSD1322::ssd1322_command(uint8_t c) {
	if (sid != -1) {
		// SPI
#ifdef ESP8266    					//Added for compatibility with ESP8266 board
		digitalWrite(cs, HIGH);
		digitalWrite(dc, LOW);
		digitalWrite(cs, LOW);
		fastSPIwrite(c);
		digitalWrite(cs, HIGH);
#else
		*csport |= cspinmask;
		*dcport &= ~dcpinmask;
		*csport &= ~cspinmask;
		fastSPIwrite(c);
		*csport |= cspinmask;
#endif
	} else {
		// I2C
		uint8_t control = 0x00;   // Co = 0, D/C = 0
		Wire.beginTransmission(_i2caddr);
		WIRE_WRITE(control);
		WIRE_WRITE(c);
		Wire.endTransmission();
	}
}

void Adafruit_SSD1322::ssd1322_data(uint8_t c) {
	if (sid != -1) {
		// SPI
#ifdef ESP8266    					//Added for compatibility with ESP8266 board
		digitalWrite(cs, HIGH);
		digitalWrite(dc, HIGH);
		digitalWrite(cs, LOW);
		fastSPIwrite(c);
		digitalWrite(cs, HIGH);
#else
		*csport |= cspinmask;
		*dcport |= dcpinmask;
		*csport &= ~cspinmask;
		fastSPIwrite(c);
		*csport |= cspinmask;
#endif
	} else {
		// I2C
		uint8_t control = 0x40;   // Co = 0, D/C = 1
		Wire.beginTransmission(_i2caddr);
		WIRE_WRITE(control);
		WIRE_WRITE(c);
		Wire.endTransmission();
	}
}

void Adafruit_SSD1322::display(void) {
//	ssd1322_command(SSD1322_SETCOLUMNADDR);
//	ssd1322_command(0);   // Column start address (0 = reset)
//	ssd1322_command(SSD1322_LCDWIDTH - 1); // Column end address (127 = reset)
//
//	ssd1322_command(SSD1322_PAGEADDR);
//	ssd1322_command(0); // Page start address (0 = reset)
//#if SSD1322_LCDHEIGHT == 64
//	ssd1322_command(7); // Page end address
//#endif
//#if SSD1322_LCDHEIGHT == 32
//	ssd1322_command(3); // Page end address
//#endif
//#if SSD1322_LCDHEIGHT == 16
//	ssd1322_command(1); // Page end address
//#endif
//
//	if (sid != -1) {
//		// SPI
//		*csport |= cspinmask;
//		*dcport |= dcpinmask;
//		*csport &= ~cspinmask;
//
//		for (uint16_t i = 0; i < (SSD1322_LCDWIDTH * SSD1322_LCDHEIGHT / 8);
//				i++) {
//			fastSPIwrite (buffer[i]);
//			//ssd1306_data(buffer[i]);
//		}
//		*csport |= cspinmask;
//	} else {
//		// save I2C bitrate
//		//Serial.println(TWBR, DEC);
//		//Serial.println(TWSR & 0x3, DEC);
//
//		// I2C
//		for (uint16_t i = 0; i < (SSD1322_LCDWIDTH * SSD1322_LCDHEIGHT / 8);
//				i++) {
//			// send a bunch of data in one xmission
//			Wire.beginTransmission(_i2caddr);
//			WIRE_WRITE(0x40);
//			for (uint8_t x = 0; x < 16; x++) {
//				WIRE_WRITE(buffer[i]);
//				i++;
//			}
//			i--;
//			Wire.endTransmission();
//		}
//	}
}

// clear everything
void Adafruit_SSD1322::clearDisplay(void) {
	memset(buffer, 0, (SSD1322_LCDWIDTH * SSD1322_LCDHEIGHT / 8));
}

inline void Adafruit_SSD1322::fastSPIwrite(uint8_t d) {

	if (hwSPI) {
		(void) SPI.transfer(d);
	} else {
		for (uint8_t bit = 0x80; bit; bit >>= 1) {
			*clkport &= ~clkpinmask;
			if (d & bit)
				*mosiport |= mosipinmask;
			else
				*mosiport &= ~mosipinmask;
			*clkport |= clkpinmask;
		}
	}
	//*csport |= cspinmask;
}

void Adafruit_SSD1322::drawFastHLine(int16_t x, int16_t y, int16_t w,
		uint16_t color) {
	boolean bSwap = false;
	switch (rotation) {
	case 0:
		// 0 degree rotation, do nothing
		break;
	case 1:
		// 90 degree rotation, swap x & y for rotation, then invert x
		bSwap = true;
		swap(x, y)
		;
		x = WIDTH - x - 1;
		break;
	case 2:
		// 180 degree rotation, invert x and y - then shift y around for height.
		x = WIDTH - x - 1;
		y = HEIGHT - y - 1;
		x -= (w - 1);
		break;
	case 3:
		// 270 degree rotation, swap x & y for rotation, then invert y  and adjust y for w (not to become h)
		bSwap = true;
		swap(x, y)
		;
		y = HEIGHT - y - 1;
		y -= (w - 1);
		break;
	}

	if (bSwap) {
		drawFastVLineInternal(x, y, w, color);
	} else {
		drawFastHLineInternal(x, y, w, color);
	}
}

void Adafruit_SSD1322::drawFastHLineInternal(int16_t x, int16_t y, int16_t w,
		uint16_t color) {
	// Do bounds/limit checks
	if (y < 0 || y >= HEIGHT) {
		return;
	}

	// make sure we don't try to draw below 0
	if (x < 0) {
		w += x;
		x = 0;
	}

	// make sure we don't go off the edge of the display
	if ((x + w) > WIDTH) {
		w = (WIDTH - x);
	}

	// if our width is now negative, punt
	if (w <= 0) {
		return;
	}

	// set up the pointer for  movement through the buffer
	register uint8_t *pBuf = buffer;
	// adjust the buffer pointer for the current row
	pBuf += ((y / 8) * SSD1322_LCDWIDTH);
	// and offset x columns in
	pBuf += x;

	register uint8_t mask = 1 << (y & 7);

	switch (color) {
	case WHITE:
		while (w--) {
			*pBuf++ |= mask;
		}
		;
		break;
	case BLACK:
		mask = ~mask;
		while (w--) {
			*pBuf++ &= mask;
		}
		;
		break;
	case INVERSE:
		while (w--) {
			*pBuf++ ^= mask;
		}
		;
		break;
	}
}

void Adafruit_SSD1322::drawFastVLine(int16_t x, int16_t y, int16_t h,
		uint16_t color) {
	bool bSwap = false;
	switch (rotation) {
	case 0:
		break;
	case 1:
		// 90 degree rotation, swap x & y for rotation, then invert x and adjust x for h (now to become w)
		bSwap = true;
		swap(x, y)
		;
		x = WIDTH - x - 1;
		x -= (h - 1);
		break;
	case 2:
		// 180 degree rotation, invert x and y - then shift y around for height.
		x = WIDTH - x - 1;
		y = HEIGHT - y - 1;
		y -= (h - 1);
		break;
	case 3:
		// 270 degree rotation, swap x & y for rotation, then invert y
		bSwap = true;
		swap(x, y)
		;
		y = HEIGHT - y - 1;
		break;
	}

	if (bSwap) {
		drawFastHLineInternal(x, y, h, color);
	} else {
		drawFastVLineInternal(x, y, h, color);
	}
}

void Adafruit_SSD1322::drawFastVLineInternal(int16_t x, int16_t __y,
		int16_t __h, uint16_t color) {

	// do nothing if we're off the left or right side of the screen
	if (x < 0 || x >= WIDTH) {
		return;
	}

	// make sure we don't try to draw below 0
	if (__y < 0) {
		// __y is negative, this will subtract enough from __h to account for __y being 0
		__h += __y;
		__y = 0;

	}

	// make sure we don't go past the height of the display
	if ((__y + __h) > HEIGHT) {
		__h = (HEIGHT - __y);
	}

	// if our height is now negative, punt
	if (__h <= 0) {
		return;
	}

	// this display doesn't need ints for coordinates, use local byte registers for faster juggling
	register uint8_t y = __y;
	register uint8_t h = __h;

	// set up the pointer for fast movement through the buffer
	register uint8_t *pBuf = buffer;
	// adjust the buffer pointer for the current row
	pBuf += ((y / 8) * SSD1322_LCDWIDTH);
	// and offset x columns in
	pBuf += x;

	// do the first partial byte, if necessary - this requires some masking
	register uint8_t mod = (y & 7);
	if (mod) {
		// mask off the high n bits we want to set
		mod = 8 - mod;

		// note - lookup table results in a nearly 10% performance improvement in fill* functions
		// register uint8_t mask = ~(0xFF >> (mod));
		static uint8_t premask[8] = { 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC,
				0xFE };
		register uint8_t mask = premask[mod];

		// adjust the mask if we're not going to reach the end of this byte
		if (h < mod) {
			mask &= (0XFF >> (mod - h));
		}

		switch (color) {
		case WHITE:
			*pBuf |= mask;
			break;
		case BLACK:
			*pBuf &= ~mask;
			break;
		case INVERSE:
			*pBuf ^= mask;
			break;
		}

		// fast exit if we're done here!
		if (h < mod) {
			return;
		}

		h -= mod;

		pBuf += SSD1322_LCDWIDTH;
	}

	// write solid bytes while we can - effectively doing 8 rows at a time
	if (h >= 8) {
		if (color == INVERSE) { // separate copy of the code so we don't impact performance of the black/white write version with an extra comparison per loop
			do {
				*pBuf = ~(*pBuf);

				// adjust the buffer forward 8 rows worth of data
				pBuf += SSD1322_LCDWIDTH;

				// adjust h & y (there's got to be a faster way for me to do this, but this should still help a fair bit for now)
				h -= 8;
			} while (h >= 8);
		} else {
			// store a local value to work with
			register uint8_t val = (color == WHITE) ? 255 : 0;

			do {
				// write our value in
				*pBuf = val;

				// adjust the buffer forward 8 rows worth of data
				pBuf += SSD1322_LCDWIDTH;

				// adjust h & y (there's got to be a faster way for me to do this, but this should still help a fair bit for now)
				h -= 8;
			} while (h >= 8);
		}
	}

	// now do the final partial byte, if necessary
	if (h) {
		mod = h & 7;
		// this time we want to mask the low bits of the byte, vs the high bits we did above
		// register uint8_t mask = (1 << mod) - 1;
		// note - lookup table results in a nearly 10% performance improvement in fill* functions
		static uint8_t postmask[8] = { 0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F,
				0x7F };
		register uint8_t mask = postmask[mod];
		switch (color) {
		case WHITE:
			*pBuf |= mask;
			break;
		case BLACK:
			*pBuf &= ~mask;
			break;
		case INVERSE:
			*pBuf ^= mask;
			break;
		}
	}
}
