
// Designed for Maple Leaf Mini with TFT+SD Reader
// Katja's Marvellous Magical Motivational Mirror
//
// At the touch of a bottom, a motivational message is read from
// the SD card and displayed on screen.

#include <SPI.h>
#include <SD.h>

#include <Adafruit_GFX_AS.h>      // Core graphics library, with extra fonts.
#include <Adafruit_ILI9341_STM.h> // STM32 DMA Hardware-specific library

// Use the UsrButton to enter test mode
#define testPin  32
// Input pin for button presses.
#define bttnPin  20
// Output pin for the LED illuminating the TFT Display
#define tftLedPin 14
// SD card SPI interface chip select pin
#define sdcs 10
// TFT Display SPI interface chip select pin
#define tftcs 7
// TFT Display Reset pin
#define tftrst 8
// TFT Display D/C pin
#define tftdc 9

unsigned int colors[] =  {
  // ILI9341_BLACK,
  // ILI9341_NAVY,
  // ILI9341_DARKGREEN,
  // ILI9341_DARKCYAN,
  // ILI9341_MAROON,
  // ILI9341_PURPLE,
  // ILI9341_OLIVE,
  ILI9341_LIGHTGREY,
  // ILI9341_DARKGREY,
  // ILI9341_BLUE,
  ILI9341_GREEN,
  ILI9341_CYAN,
  // ILI9341_RED,
  // ILI9341_MAGENTA,
  ILI9341_YELLOW,
  ILI9341_WHITE,
  ILI9341_ORANGE,
  ILI9341_GREENYELLOW,
  ILI9341_PINK
};

const int colors_len = sizeof(colors)/sizeof(unsigned int);

#define ON 1
#define OFF 0

static bool testMode = OFF;

volatile byte bttnState = 0;

// File handle for input files.
File root;

int lines = 10;
char dsplyBffr[10][31] = {
  " ",
  " ",
  "Though a program be",
  "but three lines longs,",
  "someday it will have",
  "to be maintained.",
  " ",
  "-- The Tao of Progamming"
  " ",
  " "
};

char fileName[14] = "initial";

// Keep reading next file until a valid quote file is read
// one that has the file name 'FILE' for the first four chars.
void validQuoteFile(File root, File &entry) {
  char *nptr;
  do {
    entry = root.openNextFile();

    if (!entry) 
      root.rewindDirectory();
    else {
      nptr = entry.name();
      Serial.print("Reading ");
      Serial.println(entry.name());
    }
    
  } while (
          !entry ||
          entry.isDirectory() || 
          !(nptr[0] == 'F' && nptr[1] == 'I' && nptr[2] == 'L' && nptr[3] == 'E')
          );  
}


// Global variables
Adafruit_ILI9341_STM tft = Adafruit_ILI9341_STM(tftcs, tftdc, tftrst);

// Read the next file into the display buffer.
void loadDisplayBuffer() {
  char chr;
  int len = 0;
  File entry;
  int skipCount;
  
 if (testMode) 
   skipCount = 1;
 else
   skipCount = (int)((millis() & 0xFFFF) % 5) + 1;

  // Randomly skip some files
  for (int i=0; i<skipCount; i++) {
    validQuoteFile(root, entry);
  }
  
  Serial.print("File is: ");
  Serial.print(entry.name());

  if (testMode) {
    char *ptr = entry.name();
    char *fptr = &fileName[0];
    while (*fptr++ = *ptr++);
    *fptr = '\0';
  }
  Serial.print("\n");

  lines = 0;
  while(entry.available() && lines < 10) {
    chr = entry.read();
    if (chr == '\r') continue;
    if (chr == '\n' || len >= 30) {
      dsplyBffr[lines][len] = '\0';
      len = 0;
      lines++;
    }
    else {
      dsplyBffr[lines][len++] = chr;
    }
  }
  entry.close();
}

// This function is called on the button interrupt.
void interruptFunction() {
  if (!bttnState) bttnState = ON;
}

void setup() {  
  unsigned long catchTestTime;
  int pinRead;
  
  Serial.begin(9600);
  
  // Configure the MLM user button to to be input - used to trigger test mode.
  pinMode(testPin, INPUT);
  // Configure the Button input to have a pulldown (in case of disconnection).
  pinMode(bttnPin, INPUT_PULLDOWN);
  // Configure TFT LED pin as an output and initially set it off.
  pinMode(tftLedPin, OUTPUT);
  digitalWrite(tftLedPin, LOW);
  

  // Initialise the TFT
  tft.begin();
  tft.setRotation(3);

  tft.fillScreen(ILI9341_BLACK);
  digitalWrite(tftLedPin, HIGH);

  tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
  tft.drawCentreString("Katja's", 160, 30, 4);

  tft.setTextColor(ILI9341_YELLOW);
  delay(300);
  tft.drawCentreString(" Marvellous", 160, 70, 4);

  tft.setTextColor(ILI9341_ORANGE);
  delay(300);
  tft.drawCentreString("Magical", 160, 110, 4);

  tft.setTextColor(ILI9341_RED);
  delay(300);
  tft.drawCentreString(" Motivational", 150, 150, 4);

  tft.setTextColor(ILI9341_PINK);
  delay(300);
  tft.drawCentreString("Mirror", 160, 190, 4);
  Serial.println("Started");

  // Three seconds to press the test pin after the message display.
  catchTestTime = millis() + 3000;
  while (millis() < catchTestTime) {
    if (digitalRead(testPin) == ON) {
      testMode = true;
      tft.setTextColor(ILI9341_WHITE);
      tft.drawCentreString("TEST MODE ON", 160, 0, 2);
      break;
    }
  }

  delay(2000);
  tft.fillScreen(ILI9341_BLACK);
  delay(200);
  digitalWrite(tftLedPin, LOW);

  delay(200);
  Serial.println("Initialising SD Card");
  // Initialise the SD Card
  if(!SD.begin(sdcs)) {
    digitalWrite(tftLedPin, HIGH);
    delay(500);
    tft.drawCentreString("I seem to have lost my memory!", 160, 110, 2);  
    delay(7000);       
    digitalWrite(tftLedPin, LOW);
  }
  else {
    // Attach button interrupt.
    root = SD.open("/");
    attachInterrupt(bttnPin, interruptFunction, CHANGE);
    Serial.println("Setup complete");
  }
  delay(500);
  tft.fillScreen(ILI9341_BLACK);  
}

// Main function 
void mainFunction() {
  if (bttnState) {
    Serial.print("Display motivational message\n");
    int y = (5 - (lines/2)) * 24;

    tft.setTextColor( colors[ (int)((millis() & 0xFFFF) % colors_len) ] );

    digitalWrite(tftLedPin, HIGH);
    for (int i=0; i<lines; i++) { 
      tft.drawCentreString(dsplyBffr[i], 160, y, 4);
      y += 24;
    }
    delay(200);
    
    Serial.print("Load the next message\n");
    loadDisplayBuffer();
    Serial.print("Message loaded.\n");

    delay(7500);
    
    digitalWrite(tftLedPin, LOW);
    tft.fillScreen(ILI9341_BLACK);

    bttnState = OFF;
    Serial.print("End of message display\n");
  }  
  delay(1000);
}

// Test Mode function - only run if the Test Button (user) is pressed within 3 seconds of
// the start up message.
void testModeFunction() {    
    digitalWrite(tftLedPin, HIGH);
    Serial.print("Display All Messages\n");
    while (true) {
      int y = (5 - (lines/2)) * 24;

      tft.setTextColor( ILI9341_WHITE );
      tft.drawCentreString(fileName, 160, 0, 2);
      tft.setTextColor( colors[ (int)((millis() & 0xFFFF) % colors_len) ] );

      for (int i=0; i<lines; i++) { 
        tft.drawCentreString(dsplyBffr[i], 160, y, 4);
        y += 24;
      }
      delay(200);
    
      Serial.print("Load the next message\n");
      loadDisplayBuffer();
      Serial.print("Message loaded.\n");

      delay(5000);
      tft.fillScreen(ILI9341_BLACK);
    }
    Serial.print("End of message display\n");
}

void loop() {
  // put your main code here, to run repeatedly:
  if (testMode) {
    testModeFunction();
  }
  else {
    mainFunction();
  }

  Serial.print(".");
}
