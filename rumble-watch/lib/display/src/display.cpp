#include "display.h"

SPISettings ssd1306_spiSettings = SPISettings(8000000UL, MSBFIRST, SPI_MODE0);
static uint8_t ssd1306_buffer[512];

typedef struct {
  uint8_t value; // The byte value
  unsigned int count; // Number of times the value repeats
} SequenceSegment;

static const SequenceSegment sequence[] = {
  {0xff, 3},
  {0x07, 122},
  {0xff, 6},
  {0x00, 122},
  {0xff, 6},
  {0x00, 79},
  {0xfe, 3},
  {0x0e, 34},
  {0xfe, 3},
  {0x00, 3},
  {0xff, 6},
  {0xe0, 79},
  {0xff, 3},
  {0xe0, 34},
  {0xff, 3},
  {0xe0, 3},
  {0xff, 3}
};

static const unsigned int sequence_length = sizeof(sequence) / sizeof(sequence[0]);

void init_ssd1306_buffer() {
  unsigned int index = 0;
  for (unsigned int i = 0; i < sequence_length; i++) {
      for (unsigned int j = 0; j < sequence[i].count; j++) {
          if (index < 512) {
              ssd1306_buffer[index++] = sequence[i].value;
          }
      }
  }
}
bool ssd1306_begin()
{
  // Setup pins
  pinMode(OLED_DC, OUTPUT);
  pinMode(OLED_CS, OUTPUT);
  digitalWrite(OLED_CS, HIGH); // Deselect

  SPI.begin();

  pinMode(OLED_RESET, OUTPUT);
  digitalWrite(OLED_RESET, HIGH);
  delay(1);
  digitalWrite(OLED_RESET, LOW);
  delay(10);
  digitalWrite(OLED_RESET, HIGH);

  // Initialization sequence
  SPI.beginTransaction(ssd1306_spiSettings);
  digitalWrite(OLED_CS, LOW);

  // Init sequence (mirroring begin() method)
  static const uint8_t init1[] = {SSD1306_DISPLAYOFF, SSD1306_SETDISPLAYCLOCKDIV, 0x80, SSD1306_SETMULTIPLEX};
  digitalWrite(OLED_DC, LOW); // Command mode
  for (uint8_t i = 0; i < sizeof(init1); i++)
  {
    SPI.transfer(init1[i]);
  }
  SPI.transfer(SCREEN_HEIGHT - 1);

  static const uint8_t init2[] = {SSD1306_SETDISPLAYOFFSET, 0x0, SSD1306_SETSTARTLINE | 0x0, SSD1306_CHARGEPUMP};
  for (uint8_t i = 0; i < sizeof(init2); i++)
  {
    SPI.transfer(init2[i]);
  }
  SPI.transfer(0x14);

  static const uint8_t init3[] = {SSD1306_MEMORYMODE, 0x00, SSD1306_SEGREMAP | 0x1, SSD1306_COMSCANDEC};
  for (uint8_t i = 0; i < sizeof(init3); i++)
  {
    SPI.transfer(init3[i]);
  }

  uint8_t comPins = 0x02;
  digitalWrite(OLED_DC, LOW);
  SPI.transfer(SSD1306_SETCOMPINS);
  SPI.transfer(comPins);
  SPI.transfer(SSD1306_SETCONTRAST);
  SPI.transfer(0x8F);

  SPI.transfer(SSD1306_SETPRECHARGE);
  SPI.transfer(0xF1);

  static const uint8_t init5[] = {SSD1306_SETVCOMDETECT, 0x40, SSD1306_DISPLAYALLON_RESUME, SSD1306_NORMALDISPLAY, SSD1306_DEACTIVATE_SCROLL, SSD1306_DISPLAYON};
  for (uint8_t i = 0; i < sizeof(init5); i++)
  {
    SPI.transfer(init5[i]);
  }

  digitalWrite(OLED_CS, HIGH);
  SPI.endTransaction();

  return true;
}

void ssd1306_command(uint8_t c)
{
  SPI.beginTransaction(ssd1306_spiSettings);
  digitalWrite(OLED_CS, LOW);
  digitalWrite(OLED_DC, LOW); // Command mode
  SPI.transfer(c);
  digitalWrite(OLED_CS, HIGH);
  SPI.endTransaction();
}

void ssd1306_command_list(const uint8_t *c, uint8_t n)
{
  SPI.beginTransaction(ssd1306_spiSettings);
  digitalWrite(OLED_CS, LOW);
  digitalWrite(OLED_DC, LOW); // Command mode
  while (n--)
  {
    SPI.transfer(pgm_read_byte(c++));
  }
  digitalWrite(OLED_CS, HIGH);
  SPI.endTransaction();
}

void ssd1306_draw_pixel(int16_t x, int16_t y, uint16_t color)
{
  if ((x >= 0) && (x < SCREEN_WIDTH) && (y >= 0) && (y < SCREEN_HEIGHT))
  {
    uint16_t index = x + (y / 8) * SCREEN_WIDTH;
    uint8_t bit = 1 << (y & 7);
    switch (color)
    {
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

void ssd1306_clear_display(void)
{
  memset(ssd1306_buffer, 0, 512);
}

void ssd1306_display(void)
{
  SPI.beginTransaction(ssd1306_spiSettings);
  digitalWrite(OLED_CS, LOW);

  // Set page and column addresses
  static const uint8_t initCommands[] = {SSD1306_PAGEADDR, 0, 0xFF, SSD1306_COLUMNADDR, 0};
  digitalWrite(OLED_DC, LOW); // Command mode
  for (uint8_t i = 0; i < sizeof(initCommands); i++)
  {
    SPI.transfer(initCommands[i]);
  }
  SPI.transfer(SCREEN_WIDTH - 1); // Column end

  // Send buffer data
  uint16_t count = SCREEN_WIDTH * ((SCREEN_HEIGHT + 7) / 8);
  uint8_t *ptr = ssd1306_buffer;
  digitalWrite(OLED_DC, HIGH); // Data mode
  while (count--)
  {
    SPI.transfer(*ptr++);
  }

  digitalWrite(OLED_CS, HIGH);
  SPI.endTransaction();
}