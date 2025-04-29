#include <Arduino.h>

#include <SPI.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_DC 6
#define dcPin 6
#define OLED_CS 7
#define csPin 7
#define OLED_RESET 8
#define rstPin 8
#define mosiPin 11
#define clkPin 13

//SPI defs
#define SSD1306_MEMORYMODE 0x20          ///< See datasheet
#define SSD1306_COLUMNADDR 0x21          ///< See datasheet
#define SSD1306_PAGEADDR 0x22            ///< See datasheet
#define SSD1306_SETCONTRAST 0x81         ///< See datasheet
#define SSD1306_CHARGEPUMP 0x8D          ///< See datasheet
#define SSD1306_SEGREMAP 0xA0            ///< See datasheet
#define SSD1306_DISPLAYALLON_RESUME 0xA4 ///< See datasheet
#define SSD1306_DISPLAYALLON 0xA5        ///< Not currently used
#define SSD1306_NORMALDISPLAY 0xA6       ///< See datasheet
#define SSD1306_INVERTDISPLAY 0xA7       ///< See datasheet
#define SSD1306_SETMULTIPLEX 0xA8        ///< See datasheet
#define SSD1306_DISPLAYOFF 0xAE          ///< See datasheet
#define SSD1306_DISPLAYON 0xAF           ///< See datasheet
#define SSD1306_COMSCANINC 0xC0          ///< Not currently used
#define SSD1306_COMSCANDEC 0xC8          ///< See datasheet
#define SSD1306_SETDISPLAYOFFSET 0xD3    ///< See datasheet
#define SSD1306_SETDISPLAYCLOCKDIV 0xD5  ///< See datasheet
#define SSD1306_SETPRECHARGE 0xD9        ///< See datasheet
#define SSD1306_SETCOMPINS 0xDA          ///< See datasheet
#define SSD1306_SETVCOMDETECT 0xDB       ///< See datasheet
#define SSD1306_SETSTARTLINE 0x40  ///< See datasheet
#define SSD1306_DEACTIVATE_SCROLL 0x2E                    ///< Stop scroll

void SPIwrite(uint8_t d)
{
  (void)SPI.transfer(d);
}

void ssd1306_commandList(const uint8_t *c, uint8_t n)
{
  while (n--)
    SPIwrite(pgm_read_byte(c++));
}

void ssd1306_command1(uint8_t c)
{
  SPIwrite(c);
}

uint8_t buffer[512];

#define WIDTH 128
#define HEIGHT 32

void display_(void) {
  digitalWrite(csPin, LOW);
  static const uint8_t PROGMEM dlist1[] = {
      SSD1306_PAGEADDR,
      0,                      // Page start address
      0xFF,                   // Page end (not really, but works here)
      SSD1306_COLUMNADDR, 0}; // Column start address
  ssd1306_commandList(dlist1, sizeof(dlist1));
  ssd1306_command1(WIDTH - 1); // Column end address

  uint16_t count = WIDTH * ((HEIGHT + 7) / 8);
  uint8_t *ptr = buffer;
  digitalWrite(dcPin, HIGH);
  while (count--)
    SPIwrite(*ptr++);
  digitalWrite(csPin, HIGH);                                                         
  SPI.endTransaction();     
}

void clearDisplay(){
  memset(buffer, 0, 512);
}


void SSD1306_DrawPixel(int16_t x, int16_t y, uint8_t color) {
  if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) {
      return; // Out of bounds
  }

  uint16_t index = x + (y / 8) * WIDTH;
  uint8_t bit = 1 << (y & 7);

  if (color) {
      buffer[index] |= bit;  // Set pixel (white)
  } else {
      buffer[index] &= ~bit; // Clear pixel (black)
  }
}

void SSD1306_TestPattern(void) {
  clearDisplay();
  // Draw a simple pattern: top half white, bottom half black
  for (int16_t x = 0; x < WIDTH; x++) {
      for (int16_t y = 0; y < HEIGHT; y++) {
          if (y < HEIGHT / 2) {
              SSD1306_DrawPixel(x, y, 1); // Top half white
          } else {
              SSD1306_DrawPixel(x, y, 0); // Bottom half black
          }
      }
  }
  display_();
}

void setup()
{
  clearDisplay();
  SPI.begin();
  pinMode(OLED_DC, OUTPUT);  // Set data/command pin as output
  pinMode(OLED_CS, OUTPUT);  // Same for chip select
  digitalWrite(csPin, HIGH); ///< Device deselect
  digitalWrite(clkPin, LOW); // Clock low

  pinMode(rstPin, OUTPUT);
  digitalWrite(rstPin, HIGH);
  delay(1);                   // VDD goes high at start, pause for 1 ms
  digitalWrite(rstPin, LOW);  // Bring reset low
  delay(10);                  // Wait 10 ms
  digitalWrite(rstPin, HIGH); // Bring out of reset

  // SPI_TRANSACTION_START;
  SPISettings spiSettings = SPISettings(8000000UL, MSBFIRST, SPI_MODE0);
  SPI.beginTransaction(spiSettings);
  digitalWrite(csPin, LOW); ///< Device select

  // SPI commands
  static const uint8_t PROGMEM init1[] = {SSD1306_DISPLAYOFF,         // 0xAE
                                          SSD1306_SETDISPLAYCLOCKDIV, // 0xD5
                                          0x80,                       // the suggested ratio 0x80
                                          SSD1306_SETMULTIPLEX};      // 0xA8
  ssd1306_commandList(init1, sizeof(init1));
  ssd1306_command1(32 - 1);

  static const uint8_t PROGMEM init2[] = {SSD1306_SETDISPLAYOFFSET,   // 0xD3
                                          0x0,                        // no offset
                                          SSD1306_SETSTARTLINE | 0x0, // line #0
                                          SSD1306_CHARGEPUMP};        // 0x8D
  ssd1306_commandList(init2, sizeof(init2));

  ssd1306_command1(0x14); // Sends 0x14

  static const uint8_t PROGMEM init3[] = {SSD1306_MEMORYMODE, // 0x20
                                          0x00,               // 0x0 act like ks0108
                                          SSD1306_SEGREMAP | 0x1,
                                          SSD1306_COMSCANDEC};
  ssd1306_commandList(init3, sizeof(init3));

  uint8_t comPins = 0x02;
  uint8_t contrast = 0x8F;

  ssd1306_command1(SSD1306_SETCOMPINS);
  ssd1306_command1(comPins);
  ssd1306_command1(SSD1306_SETCONTRAST);
  ssd1306_command1(contrast);

  ssd1306_command1(SSD1306_SETPRECHARGE); // 0xd9
  ssd1306_command1(0xF1);
  static const uint8_t PROGMEM init5[] = {
      SSD1306_SETVCOMDETECT, // 0xDB
      0x40,
      SSD1306_DISPLAYALLON_RESUME, // 0xA4
      SSD1306_NORMALDISPLAY,       // 0xA6
      SSD1306_DEACTIVATE_SCROLL,
      SSD1306_DISPLAYON}; // Main screen turn on
  ssd1306_commandList(init5, sizeof(init5));

  digitalWrite(csPin, HIGH);


  // Clear the buffer
  clearDisplay();
  SSD1306_TestPattern();


}

void loop()
{
}