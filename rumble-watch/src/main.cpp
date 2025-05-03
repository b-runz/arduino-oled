/**************************************************************************
 This is an example for our Monochrome OLEDs based on SSD1306 drivers

 Pick one up today in the adafruit shop!
 ------> http://www.adafruit.com/category/63_98

 This example is for a 128x32 pixel display using SPI to communicate
 4 or 5 pins are required to interface.

 Adafruit invests time and resources providing this open
 source code, please support Adafruit and open-source
 hardware by purchasing products from Adafruit!

 Written by Limor Fried/Ladyada for Adafruit Industries,
 with contributions from the open source community.
 BSD license, check license.txt for more information
 All text above, and the splash screen below must be
 included in any redistribution.
 **************************************************************************/

 #include <SPI.h>
 #include <Wire.h>
 #include <Adafruit_SSD1306.h>
 
 #define SCREEN_WIDTH 128 // OLED display width, in pixels
 #define SCREEN_HEIGHT 32 // OLED display height, in pixels
 
 /* Comment out above, uncomment this block to use hardware SPI */
 #define OLED_DC 6
 #define OLED_CS 7
 #define OLED_RESET 8
 Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
                          &SPI, OLED_DC, OLED_RESET, OLED_CS);
 
 void displayTestPattern(bool flip)
 {
     display.clearDisplay();
     for (int16_t x = 0; x < SCREEN_WIDTH; x++)
     {
         if (flip)
         {
             for (int16_t y = 0; y < SCREEN_HEIGHT; y++)
             {
                 display.drawPixel(x, y, y > SCREEN_HEIGHT / 2);
             }
         }
         else
         {
             for (int16_t y = 0; y < SCREEN_HEIGHT; y++)
             {
                 display.drawPixel(x, y, y < SCREEN_HEIGHT / 2);
             }
         }
     }
     display.display();
 }
 
 void setup()
 {
 
     pinMode(9, INPUT);
 
     // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
     display.begin(SSD1306_SWITCHCAPVCC);
 
     bool flip = 0;
     while (1)
     {
         while (digitalRead(9) == 0)
         {
             delay(100);
         }
 
         displayTestPattern(flip);
         flip = !flip;
         
         delay(200);
     }
 }
 