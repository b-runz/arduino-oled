
 #include <SPI.h>

#define SSD1306_BLACK 0   ///< Draw 'off' pixels
 #define SSD1306_WHITE 1   ///< Draw 'on' pixels
 #define SSD1306_INVERSE 2 ///< Invert pixels
 
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
 
 #define SSD1306_SETLOWCOLUMN 0x00  ///< Not currently used
 #define SSD1306_SETHIGHCOLUMN 0x10 ///< Not currently used
 #define SSD1306_SETSTARTLINE 0x40  ///< See datasheet
 
 #define SSD1306_EXTERNALVCC 0x01  ///< External display voltage source
 #define SSD1306_SWITCHCAPVCC 0x02 ///< Gen. display voltage from 3.3V
 
 #define SSD1306_RIGHT_HORIZONTAL_SCROLL 0x26              ///< Init rt scroll
 #define SSD1306_LEFT_HORIZONTAL_SCROLL 0x27               ///< Init left scroll
 #define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29 ///< Init diag scroll
 #define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL 0x2A  ///< Init diag scroll
 #define SSD1306_DEACTIVATE_SCROLL 0x2E                    ///< Stop scroll
 #define SSD1306_ACTIVATE_SCROLL 0x2F                      ///< Start scroll
 #define SSD1306_SET_VERTICAL_SCROLL_AREA 0xA3             ///< Set scroll range

 #define OLED_DC 6
 #define OLED_CS 7
 #define OLED_RESET 8
  
 #define SCREEN_WIDTH 128 // OLED display width, in pixels
 #define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Global variables
static SPIClass *ssd1306_spi = &SPI;
static SPISettings ssd1306_spiSettings;
static uint8_t ssd1306_buffer[512];
static uint8_t ssd1306_vccstate;

// Initialize the display
bool ssd1306_begin(SPIClass *spi) {
    // Initialize globals
    ssd1306_spi = spi ? spi : &SPI;
    ssd1306_spiSettings = SPISettings(8000000UL, MSBFIRST, SPI_MODE0);
    ssd1306_vccstate = SSD1306_SWITCHCAPVCC;

    // Setup pins
    pinMode(OLED_DC, OUTPUT);
    pinMode(OLED_CS, OUTPUT);
    digitalWrite(OLED_CS, HIGH); // Deselect

    
    ssd1306_spi->begin();

    pinMode(OLED_RESET, OUTPUT);
    digitalWrite(OLED_RESET, HIGH);
    delay(1);
    digitalWrite(OLED_RESET, LOW);
    delay(10);
    digitalWrite(OLED_RESET, HIGH);


    // Initialization sequence
    ssd1306_spi->beginTransaction(ssd1306_spiSettings);
    digitalWrite(OLED_CS, LOW);

    // Init sequence (mirroring begin() method)
    static const uint8_t init1[] = {SSD1306_DISPLAYOFF, SSD1306_SETDISPLAYCLOCKDIV, 0x80, SSD1306_SETMULTIPLEX};
    digitalWrite(OLED_DC, LOW); // Command mode
    for (uint8_t i = 0; i < sizeof(init1); i++) {
        ssd1306_spi->transfer(init1[i]);
    }
    ssd1306_spi->transfer(SCREEN_HEIGHT - 1);

    static const uint8_t init2[] = {SSD1306_SETDISPLAYOFFSET, 0x0, SSD1306_SETSTARTLINE | 0x0, SSD1306_CHARGEPUMP};
    for (uint8_t i = 0; i < sizeof(init2); i++) {
        ssd1306_spi->transfer(init2[i]);
    }
    ssd1306_spi->transfer((ssd1306_vccstate == SSD1306_EXTERNALVCC) ? 0x10 : 0x14);

    static const uint8_t init3[] = {SSD1306_MEMORYMODE, 0x00, SSD1306_SEGREMAP | 0x1, SSD1306_COMSCANDEC};
    for (uint8_t i = 0; i < sizeof(init3); i++) {
        ssd1306_spi->transfer(init3[i]);
    }

    uint8_t comPins = 0x02;
    digitalWrite(OLED_DC, LOW);
    ssd1306_spi->transfer(SSD1306_SETCOMPINS);
    ssd1306_spi->transfer(comPins);
    ssd1306_spi->transfer(SSD1306_SETCONTRAST);
    ssd1306_spi->transfer(0x8F);

    ssd1306_spi->transfer(SSD1306_SETPRECHARGE);
    ssd1306_spi->transfer((ssd1306_vccstate == SSD1306_EXTERNALVCC) ? 0x22 : 0xF1);

    static const uint8_t init5[] = {SSD1306_SETVCOMDETECT, 0x40, SSD1306_DISPLAYALLON_RESUME, SSD1306_NORMALDISPLAY, SSD1306_DEACTIVATE_SCROLL, SSD1306_DISPLAYON};
    for (uint8_t i = 0; i < sizeof(init5); i++) {
        ssd1306_spi->transfer(init5[i]);
    }

    digitalWrite(OLED_CS, HIGH);
    ssd1306_spi->endTransaction();

    // Clear buffer
    memset(ssd1306_buffer, 0, 512);

    return true;
}

// Send a single command
void ssd1306_command(uint8_t c) {
    ssd1306_spi->beginTransaction(ssd1306_spiSettings);
    digitalWrite(OLED_CS, LOW);
    digitalWrite(OLED_DC, LOW); // Command mode
    ssd1306_spi->transfer(c);
    digitalWrite(OLED_CS, HIGH);
    ssd1306_spi->endTransaction();
}

// Send a list of commands
void ssd1306_command_list(const uint8_t *c, uint8_t n) {
    ssd1306_spi->beginTransaction(ssd1306_spiSettings);
    digitalWrite(OLED_CS, LOW);
    digitalWrite(OLED_DC, LOW); // Command mode
    while (n--) {
        ssd1306_spi->transfer(pgm_read_byte(c++));
    }
    digitalWrite(OLED_CS, HIGH);
    ssd1306_spi->endTransaction();
}

// Draw a single pixel
void ssd1306_draw_pixel(int16_t x, int16_t y, uint16_t color) {
    if ((x >= 0) && (x < SCREEN_WIDTH) && (y >= 0) && (y < SCREEN_HEIGHT)) {
        uint16_t index = x + (y / 8) * SCREEN_WIDTH;
        uint8_t bit = 1 << (y & 7);
        switch (color) {
            case SSD1306_WHITE:
                ssd1306_buffer[index] |= bit;
                break;
            case SSD1306_BLACK:
                ssd1306_buffer[index] &= ~bit;
                break;
            case SSD1306_INVERSE:
                ssd1306_buffer[index] ^= bit;
                break;
        }
    }
}

// Clear the display buffer
void ssd1306_clear_display(void) {
    memset(ssd1306_buffer, 0, 512);
}

// Update the display with buffer contents
void ssd1306_display(void) {
    ssd1306_spi->beginTransaction(ssd1306_spiSettings);
    digitalWrite(OLED_CS, LOW);

    // Set page and column addresses
    static const uint8_t initCommands[] = {SSD1306_PAGEADDR, 0, 0xFF, SSD1306_COLUMNADDR, 0};
    digitalWrite(OLED_DC, LOW); // Command mode
    for (uint8_t i = 0; i < sizeof(initCommands); i++) {
        ssd1306_spi->transfer(initCommands[i]);
    }
    ssd1306_spi->transfer(SCREEN_WIDTH - 1); // Column end

    // Send buffer data
    uint16_t count = SCREEN_WIDTH * ((SCREEN_HEIGHT + 7) / 8);
    uint8_t *ptr = ssd1306_buffer;
    digitalWrite(OLED_DC, HIGH); // Data mode
    while (count--) {
        ssd1306_spi->transfer(*ptr++);
    }

    digitalWrite(OLED_CS, HIGH);
    ssd1306_spi->endTransaction();
}
 
 void displayTestPattern(bool flip)
 {
     ssd1306_clear_display();
     for (int16_t x = 0; x < SCREEN_WIDTH; x++)
     {
         if (flip)
         {
             for (int16_t y = 0; y < SCREEN_HEIGHT; y++)
             {
                 ssd1306_draw_pixel(x, y, y > SCREEN_HEIGHT / 2);
             }
         }
         else
         {
             for (int16_t y = 0; y < SCREEN_HEIGHT; y++)
             {
                ssd1306_draw_pixel(x, y, y < SCREEN_HEIGHT / 2);
             }
         }
     }
     ssd1306_display();
 }
 
 void setup()
 {
 
     pinMode(9, INPUT);
 
     // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
     ssd1306_begin(&SPI);
 
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
 