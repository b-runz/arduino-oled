
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
static SPISettings ssd1306_spiSettings = SPISettings(8000000UL, MSBFIRST, SPI_MODE0);
static uint8_t ssd1306_buffer[512]  = {
  0xff, 0xff, 0xff, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x7c, 0x7c, 0x83, 0x83, 0x83, 0x83, 0x83, 0x7c, 0x7c, 0x0, 0x0, 0x0, 0x7c, 0x7c, 0x83, 0x83, 0x83, 0x83, 0x83, 0x7c, 0x7c, 0x0, 0x0, 0x60, 0x60, 0x0, 0x0, 0x7c, 0x7c, 0x83, 0x83, 0x83, 0x83, 0x83, 0x7c, 0x7c, 0x0, 0x0, 0x0, 0x7c, 0x7c, 0x83, 0x83, 0x83, 0x83, 0x83, 0x7c, 0x7c, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xe, 0x5, 0xe, 0x0, 0xf, 0x8, 0x0, 0xe, 0x5, 0xe, 0x0, 0xf, 0x5, 0xa, 0x0, 0xe, 0x1, 0x6, 0x1, 0xe, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x3e, 0x3e, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0x3e, 0x3e, 0x0, 0x0, 0x0, 0x3e, 0x3e, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0x3e, 0x3e, 0x0, 0x0, 0xc, 0xc, 0x0, 0x0, 0x3e, 0x3e, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0x3e, 0x3e, 0x0, 0x0, 0x0, 0x3e, 0x3e, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0x3e, 0x3e, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xfe, 0xfe, 0xfe, 0xe, 0xe, 0xce, 0x4e, 0xce, 0xe, 0xce, 0x4e, 0xce, 0xe, 0xe, 0xe, 0xe, 0xce, 0x4e, 0xce, 0xe, 0xce, 0x4e, 0xce, 0xe, 0xe, 0xe, 0xe, 0xce, 0x4e, 0xce, 0xe, 0xce, 0x4e, 0xce, 0xe, 0xe, 0xe, 0xfe, 0xfe, 0xfe, 0x0, 0x0, 0x0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xff, 0xff, 0xff, 0xe0, 0xe0, 0xe7, 0xe5, 0xe7, 0xe0, 0xe7, 0xe5, 0xe7, 0xe0, 0xe1, 0xe1, 0xe0, 0xe7, 0xe5, 0xe7, 0xe0, 0xe7, 0xe5, 0xe7, 0xe0, 0xe1, 0xe1, 0xe0, 0xe7, 0xe5, 0xe7, 0xe0, 0xe7, 0xe5, 0xe7, 0xe0, 0xe0, 0xe0, 0xff, 0xff, 0xff, 0xe0, 0xe0, 0xe0, 0xff, 0xff, 0xff
  };

// typedef struct {
//   uint8_t value; // The byte value
//   unsigned int count; // Number of times the value repeats
// } SequenceSegment;

// static const SequenceSegment sequence[] = {
//   {0xff, 3},
//   {0x07, 122},
//   {0xff, 6},
//   {0x00, 122},
//   {0xff, 6},
//   {0x00, 79},
//   {0xfe, 3},
//   {0x0e, 34},
//   {0xfe, 3},
//   {0x00, 3},
//   {0xff, 6},
//   {0xe0, 79},
//   {0xff, 3},
//   {0xe0, 34},
//   {0xff, 3},
//   {0xe0, 3},
//   {0xff, 3}
// };

// static const unsigned int sequence_length = sizeof(sequence) / sizeof(sequence[0]);

// void init_ssd1306_buffer() {
//   unsigned int index = 0;
//   for (unsigned int i = 0; i < sequence_length; i++) {
//       for (unsigned int j = 0; j < sequence[i].count; j++) {
//           if (index < 512) {
//               ssd1306_buffer[index++] = sequence[i].value;
//           }
//       }
//   }
// }
// static uint8_t ssd1306_buffer[512];

// Initialize the display
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

    // Clear buffer
    // memset(ssd1306_buffer, 0, 512);

    return true;
}

// Send a single command
void ssd1306_command(uint8_t c)
{
    SPI.beginTransaction(ssd1306_spiSettings);
    digitalWrite(OLED_CS, LOW);
    digitalWrite(OLED_DC, LOW); // Command mode
    SPI.transfer(c);
    digitalWrite(OLED_CS, HIGH);
    SPI.endTransaction();
}

// Send a list of commands
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

// Draw a single pixel
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

// Clear the display buffer
void ssd1306_clear_display(void)
{
    memset(ssd1306_buffer, 0, 512);
}

// Update the display with buffer contents
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

void sideUpper(){
  for (size_t i = 0; i < 2; i++)
  {
    SPI.transfer(0b01111100);
  }
}

void topAndBottomUpper(bool withBottom){  
  uint8_t transferByte = withBottom ? 0b10000011 : 0b00000011;
  for (size_t i = 0; i < 5; i++)
  {
    SPI.transfer(transferByte);
  }  
}

void sideLower(){
  for (size_t i = 0; i < 2; i++)
  {
    SPI.transfer(0b00111110);
  }
}

void topAndBottomLower(bool withTop){
  uint8_t transferByte = withTop ? 0b11000001 : 0b11000000;
  for (size_t i = 0; i < 5; i++)
  {
    SPI.transfer(transferByte);
  }  
}

void blankSide(){
  for (size_t i = 0; i < 2; i++)
  {
    SPI.transfer(0x00);
  }
}

void printOne(){
  for (size_t i = 0; i < 7; i++)
  {
    SPI.transfer(0x00);
  }
  sideUpper();
  for (size_t i = 0; i < 7; i++)
  {
    SPI.transfer(0x00);
  }
  sideLower();
}

void printTwo(){
  blankSide();
  topAndBottomUpper(true);
  sideUpper();
  sideLower();
  topAndBottomLower(true);
  for (size_t i = 0; i < 2; i++)
  {
    SPI.transfer(0x00);
  }
}

void printThree(){
  blankSide();
  topAndBottomUpper(true);
  sideUpper();
  blankSide();
  topAndBottomLower(true);
  sideLower();
}

void printFour(){
  sideUpper();
  for (size_t i = 0; i < 5; i++){
    SPI.transfer(0b10000000);
  }
  sideUpper();
  blankSide();
  for (size_t i = 0; i < 5; i++){
    SPI.transfer(0b0000001);
  }
  sideLower();
}

void printFive(){
  sideUpper();
  topAndBottomUpper(true);
  for (size_t i = 0; i < 4; i++){
    SPI.transfer(0b0000000);
  }
  topAndBottomLower(true);
  sideLower();
}

void printSix(){
  sideUpper();
  topAndBottomUpper(true);
  blankSide();
  sideLower();
  topAndBottomLower(true);
  sideLower();
}

void printSeven(){
  blankSide();
  topAndBottomUpper(false);
  sideUpper();
  for (size_t i = 0; i < 7; i++){
    SPI.transfer(0b0000000);
  }
  sideLower();
}

void printEight(){
  sideUpper();
  topAndBottomUpper(true);
  sideUpper();

  sideLower();
  topAndBottomLower(true);
  sideLower();
}

void printNine(){
  sideUpper();
  topAndBottomUpper(true);
  sideUpper();

  for (size_t i = 0; i < 2; i++)
  {
    SPI.transfer(0b0000000);
  }
  topAndBottomLower(true);
  sideLower();
}

void printNaught(){
  sideUpper();
  topAndBottomUpper(false);
  sideUpper();

  sideLower();
  topAndBottomLower(false);
  sideLower();
}

void initDigit(uint8_t digit){
  uint8_t columnStart = 0x13;
  uint8_t columnEnd = 0x1B;

  switch (digit)
  {
  case 2:
    columnStart = 0x1F; // Start column = 31
    columnEnd = 0x27; // End column = 39
    break;

  case 3:
    columnStart = 0x2E; // Start column = 46
    columnEnd = 0x36; // End column = 54
    break;

  case 4:
    columnStart = 0x3A; // Start column = 58
    columnEnd = 0x42; // End column = 66 
    break;
  
  default:
    break;
  }

  digitalWrite(OLED_CS, LOW);

  // Set Page Addressing Mode
  digitalWrite(OLED_DC, LOW); // Command mode
  SPI.transfer(0x20); // Set Memory Addressing Mode
  SPI.transfer(0x00); // Horizontal Addressing Mode

  // Set column range (19–27)
  SPI.transfer(0x21); // Set Column Address
  SPI.transfer(columnStart);
  SPI.transfer(columnEnd); 

  // Set page range (1–2)
  SPI.transfer(0x22); // Set Page Address
  SPI.transfer(0x01); // Start page = 1
  SPI.transfer(0x02); // End page = 2

  // Write data to set pixel at row 0 (COM0)
  digitalWrite(OLED_DC, HIGH); // Data mode
}

void printClockDigit(uint8_t digit){
  switch (digit)
      {
          case 0:
              printNaught();
              break;
          
          case 1:
              printOne();
              break;
          
          case 2:
              printTwo();
              break;
          
          case 3:
              printThree();
              break;
          
          case 4:
              printFour();
              break;
          
          case 5:
              printFive();
              break;
          
          case 6:
              printSix();
              break;
          
          case 7:
              printSeven();
              break;
          
          case 8:
              printEight();
              break;
          
          case 9:
              printNine();
              break;
          
          default:
              break;
      }
}

void printHour(uint8_t hour){
  uint8_t onesDigit = hour % 10 ;
  uint8_t hoursDigit = hour / 10;

  initDigit(1);
  printClockDigit(hoursDigit);
  initDigit(2);
  printClockDigit(onesDigit);
}

void setup()
{

    pinMode(9, INPUT);

    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    ssd1306_begin();

    // ssd1306_display();
    // memset(ssd1306_buffer, 0, 512);
    // init_ssd1306_buffer();
    ssd1306_display();

    initDigit(1);
    // ssd1306_display();

    // memset(ssd1306_buffer, 0xff, 512);
    // ssd1306_display();

    uint8_t counter = 1;
    printHour(0);
    while (1)
    {
      while (digitalRead(9) == 0)
      {
          delay(100);
      }
      delay(200);

      if(counter == 24)
      {
        counter = 0;
      }
      printHour(counter);
      

      counter++;
    }
}
