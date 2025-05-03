 
 #include "Adafruit_SSD1306.h"
 #include "splash.h"
 
 // SOME DEFINES AND STATIC VARIABLES USED INTERNALLY -----------------------
 
 #define SSD1306_SELECT digitalWrite(csPin, LOW);       ///< Device select
 #define SSD1306_DESELECT digitalWrite(csPin, HIGH);    ///< Device deselect
 #define SSD1306_MODE_COMMAND digitalWrite(dcPin, LOW); ///< Command mode
 #define SSD1306_MODE_DATA digitalWrite(dcPin, HIGH);   ///< Data mode

 
 #define SPI_TRANSACTION_START spi->beginTransaction(spiSettings) ///< Pre-SPI
 #define SPI_TRANSACTION_END spi->endTransaction()                ///< Post-SPI
 
 #define TRANSACTION_START                                                      \
 SPI_TRANSACTION_START;                                                   \
SSD1306_SELECT;      

 #define TRANSACTION_END                                                        \
 SSD1306_DESELECT;                                                          \ 
 SPI_TRANSACTION_END;                                                     
 
 Adafruit_SSD1306::Adafruit_SSD1306(uint8_t w, uint8_t h, SPIClass *spi_ptr,
                                    int8_t dc_pin, int8_t rst_pin, int8_t cs_pin,
                                    uint32_t bitrate)
     : spi(spi_ptr ? spi_ptr : &SPI),
       dcPin(dc_pin), csPin(cs_pin),
       rstPin(rst_pin) {
        
   spiSettings = SPISettings(bitrate, MSBFIRST, SPI_MODE0);
 }
 Adafruit_SSD1306::~Adafruit_SSD1306(void) {

 }
 
 inline void Adafruit_SSD1306::SPIwrite(uint8_t d) {
     (void)spi->transfer(d);
 }

 void Adafruit_SSD1306::ssd1306_command1(uint8_t c) {
  SSD1306_MODE_COMMAND
  SPIwrite(c);
 }
 void Adafruit_SSD1306::ssd1306_commandList(const uint8_t *c, uint8_t n) {
  SSD1306_MODE_COMMAND
  while (n--)
    SPIwrite(pgm_read_byte(c++));
 }
 void Adafruit_SSD1306::ssd1306_command(uint8_t c) {
   TRANSACTION_START
   ssd1306_command1(c);
   TRANSACTION_END
 }
 bool Adafruit_SSD1306::begin(uint8_t vcs, uint8_t addr, bool reset,
                              bool periphBegin) {
   vccstate = vcs;
 
   pinMode(dcPin, OUTPUT); // Set data/command pin as output
   pinMode(csPin, OUTPUT); // Same for chip select

    SSD1306_DESELECT
    spi->begin();
 
    pinMode(rstPin, OUTPUT);
    digitalWrite(rstPin, HIGH);
    delay(1);                   // VDD goes high at start, pause for 1 ms
    digitalWrite(rstPin, LOW);  // Bring reset low
    delay(10);                  // Wait 10 ms
    digitalWrite(rstPin, HIGH); // Bring out of reset
 
   TRANSACTION_START
 
   // Init sequence
   static const uint8_t PROGMEM init1[] = {SSD1306_DISPLAYOFF,         // 0xAE
                                           SSD1306_SETDISPLAYCLOCKDIV, // 0xD5
                                           0x80, // the suggested ratio 0x80
                                           SSD1306_SETMULTIPLEX}; // 0xA8
   ssd1306_commandList(init1, sizeof(init1));
   ssd1306_command1(SSD1306_LCDHEIGHT - 1);
 
   static const uint8_t PROGMEM init2[] = {SSD1306_SETDISPLAYOFFSET, // 0xD3
                                           0x0,                      // no offset
                                           SSD1306_SETSTARTLINE | 0x0, // line #0
                                           SSD1306_CHARGEPUMP};        // 0x8D
   ssd1306_commandList(init2, sizeof(init2));
 
   ssd1306_command1((vccstate == SSD1306_EXTERNALVCC) ? 0x10 : 0x14);
 
   static const uint8_t PROGMEM init3[] = {SSD1306_MEMORYMODE, // 0x20
                                           0x00, // 0x0 act like ks0108
                                           SSD1306_SEGREMAP | 0x1,
                                           SSD1306_COMSCANDEC};
   ssd1306_commandList(init3, sizeof(init3));
 
   uint8_t comPins = 0x02;  
 
   ssd1306_command1(SSD1306_SETCOMPINS);
   ssd1306_command1(comPins);
   ssd1306_command1(SSD1306_SETCONTRAST);
   ssd1306_command1(0x8F);
 
   ssd1306_command1(SSD1306_SETPRECHARGE); // 0xd9
   ssd1306_command1((vccstate == SSD1306_EXTERNALVCC) ? 0x22 : 0xF1);
   static const uint8_t PROGMEM init5[] = {
       SSD1306_SETVCOMDETECT, // 0xDB
       0x40,
       SSD1306_DISPLAYALLON_RESUME, // 0xA4
       SSD1306_NORMALDISPLAY,       // 0xA6
       SSD1306_DEACTIVATE_SCROLL,
       SSD1306_DISPLAYON}; // Main screen turn on
   ssd1306_commandList(init5, sizeof(init5));
 
   TRANSACTION_END
 
   return true; // Success
 }
 
 // DRAWING FUNCTIONS -------------------------------------------------------
 
 /*!
     @brief  Set/clear/invert a single pixel. This is also invoked by the
             Adafruit_GFX library in generating many higher-level graphics
             primitives.
     @param  x
             Column of display -- 0 at left to (screen width - 1) at right.
     @param  y
             Row of display -- 0 at top to (screen height -1) at bottom.
     @param  color
             Pixel color, one of: SSD1306_BLACK, SSD1306_WHITE or
             SSD1306_INVERSE.
     @return None (void).
     @note   Changes buffer contents only, no immediate effect on display.
             Follow up with a call to display(), or with other graphics
             commands as needed by one's own application.
 */
 void Adafruit_SSD1306::drawPixel(int16_t x, int16_t y, uint16_t color) {
   if ((x >= 0) && (x < SSD1306_LCDWIDTH) && (y >= 0) && (y < SSD1306_LCDHEIGHT)) {
     // Pixel is in-bounds. Rotate coordinates if needed.

     switch (color) {
     case SSD1306_WHITE:
       buffer[x + (y / 8) * SSD1306_LCDWIDTH] |= (1 << (y & 7));
       break;
     case SSD1306_BLACK:
       buffer[x + (y / 8) * SSD1306_LCDWIDTH] &= ~(1 << (y & 7));
       break;
     case SSD1306_INVERSE:
       buffer[x + (y / 8) * SSD1306_LCDWIDTH] ^= (1 << (y & 7));
       break;
     }
   }
 }
 
 /*!
     @brief  Clear contents of display buffer (set all pixels to off).
     @return None (void).
     @note   Changes buffer contents only, no immediate effect on display.
             Follow up with a call to display(), or with other graphics
             commands as needed by one's own application.
 */
 void Adafruit_SSD1306::clearDisplay(void) {
   memset(buffer, 0, 512);
 }
 void Adafruit_SSD1306::display(void) {
  digitalWrite(csPin, LOW);
  static const uint8_t PROGMEM initCommands[] = {
      SSD1306_PAGEADDR,
      0,    // Page start
      0xFF, // Page end
      SSD1306_COLUMNADDR,
      0 // Column start
  };
  ssd1306_commandList(initCommands, sizeof(initCommands));
  ssd1306_command1(SSD1306_LCDWIDTH - 1); // Column end

  uint16_t count = SSD1306_LCDWIDTH * ((SSD1306_LCDHEIGHT + 7) / 8);
  uint8_t *ptr = buffer;
  digitalWrite(dcPin, HIGH);
  while (count--)
  {
    SPIwrite(*ptr++);
  }
  digitalWrite(csPin, HIGH);
  SPI.endTransaction();
 }
 