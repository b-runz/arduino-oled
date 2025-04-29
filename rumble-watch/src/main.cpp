#include <Arduino.h>
#include <SPI.h>

// Display dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

// Pin definitions
#define PIN_DC 6
#define PIN_CS 7
#define PIN_RESET 8
#define PIN_MOSI 11
#define PIN_CLK 13

// SSD1306 command definitions
#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR 0x22
#define SSD1306_SETCONTRAST 0x81
#define SSD1306_CHARGEPUMP 0x8D
#define SSD1306_SEGREMAP 0xA0
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_SETMULTIPLEX 0xA8
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF
#define SSD1306_COMSCANDEC 0xC8
#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9
#define SSD1306_SETCOMPINS 0xDA
#define SSD1306_SETVCOMDETECT 0xDB
#define SSD1306_SETSTARTLINE 0x40
#define SSD1306_DEACTIVATE_SCROLL 0x2E

// Display buffer
uint8_t displayBuffer[512];

// Write single byte to SPI
void spiWrite(uint8_t data) {
    SPI.transfer(data);
}

// Send command list to SSD1306
void displaySendCommandList(const uint8_t *commands, uint8_t count) {
    while (count--) {
        spiWrite(pgm_read_byte(commands++));
    }
}

// Send single command to SSD1306
void displaySendCommand(uint8_t command) {
    spiWrite(command);
}

// Update display with buffer contents
void displayUpdate() {
    digitalWrite(PIN_CS, LOW);
    static const uint8_t PROGMEM initCommands[] = {
        SSD1306_PAGEADDR,
        0,                      // Page start
        0xFF,                   // Page end
        SSD1306_COLUMNADDR,
        0                       // Column start
    };
    displaySendCommandList(initCommands, sizeof(initCommands));
    displaySendCommand(SCREEN_WIDTH - 1); // Column end

    uint16_t count = SCREEN_WIDTH * ((SCREEN_HEIGHT + 7) / 8);
    uint8_t *ptr = displayBuffer;
    digitalWrite(PIN_DC, HIGH);
    while (count--) {
        spiWrite(*ptr++);
    }
    digitalWrite(PIN_CS, HIGH);
    SPI.endTransaction();
}

// Clear display buffer
void displayClear() {
    memset(displayBuffer, 0, sizeof(displayBuffer));
}

// Draw pixel in buffer
void displayDrawPixel(int16_t x, int16_t y, uint8_t color) {
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) {
        return;
    }

    uint16_t index = x + (y / 8) * SCREEN_WIDTH;
    uint8_t bit = 1 << (y & 7);
    
    if (color) {
        displayBuffer[index] |= bit;
    } else {
        displayBuffer[index] &= ~bit;
    }
}

// Draw test pattern (top half white, bottom half black)
void displayTestPattern() {
    displayClear();
    for (int16_t x = 0; x < SCREEN_WIDTH; x++) {
        for (int16_t y = 0; y < SCREEN_HEIGHT; y++) {
            displayDrawPixel(x, y, y > SCREEN_HEIGHT / 2);
        }
    }
    displayUpdate();
}

void setup() {
    // Initialize pins
    pinMode(PIN_DC, OUTPUT);
    pinMode(PIN_CS, OUTPUT);
    pinMode(PIN_RESET, OUTPUT);
    
    digitalWrite(PIN_CS, HIGH);
    digitalWrite(PIN_CLK, LOW);

    // Reset sequence
    digitalWrite(PIN_RESET, HIGH);
    delay(1);
    digitalWrite(PIN_RESET, LOW);
    delay(10);
    digitalWrite(PIN_RESET, HIGH);

    // Initialize SPI
    SPI.begin();
    SPI.beginTransaction(SPISettings(8000000UL, MSBFIRST, SPI_MODE0));
    digitalWrite(PIN_CS, LOW);

    // SSD1306 initialization sequence
    static const uint8_t PROGMEM init1[] = {
        SSD1306_DISPLAYOFF,
        SSD1306_SETDISPLAYCLOCKDIV,
        0x80,
        SSD1306_SETMULTIPLEX
    };
    displaySendCommandList(init1, sizeof(init1));
    displaySendCommand(SCREEN_HEIGHT - 1);

    static const uint8_t PROGMEM init2[] = {
        SSD1306_SETDISPLAYOFFSET,
        0x0,
        SSD1306_SETSTARTLINE,
        SSD1306_CHARGEPUMP
    };
    displaySendCommandList(init2, sizeof(init2));
    displaySendCommand(0x14);

    static const uint8_t PROGMEM init3[] = {
        SSD1306_MEMORYMODE,
        0x00,
        SSD1306_SEGREMAP | 0x1,
        SSD1306_COMSCANDEC
    };
    displaySendCommandList(init3, sizeof(init3));

    displaySendCommand(SSD1306_SETCOMPINS);
    displaySendCommand(0x02);
    displaySendCommand(SSD1306_SETCONTRAST);
    displaySendCommand(0x8F);

    displaySendCommand(SSD1306_SETPRECHARGE);
    displaySendCommand(0xF1);

    static const uint8_t PROGMEM init5[] = {
        SSD1306_SETVCOMDETECT,
        0x40,
        SSD1306_DISPLAYALLON_RESUME,
        SSD1306_NORMALDISPLAY,
        SSD1306_DEACTIVATE_SCROLL,
        SSD1306_DISPLAYON
    };
    displaySendCommandList(init5, sizeof(init5));

    digitalWrite(PIN_CS, HIGH);

    // Initial display
    displayTestPattern();
}

void loop() {
}