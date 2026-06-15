// ECE 430 Project 6
// Benjamin Hatto
// SSD1306 Display Control with Buttons and Interrupts
// Board: ST Nucleo-F446RE (would need to modify pin consts to work with arduino)

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "tesseract.h"

#define SCREEN_WIDTH 128 // oled display width (px)
#define SCREEN_HEIGHT 64 // oled display height (px)
#define DISP_ADDR 0x3C // I2C address of oled

// constants for inputs
const uint8_t LBUTTON_PIN = PA10; // arduino D2
const uint8_t RBUTTON_PIN = PB3; // arduino D3
const uint8_t POT_PIN = PA0; // arduino A0 

// image for screen 2
const unsigned char MSU_Logo [] PROGMEM = {
	0xff, 0xff, 0xfc, 0x03, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x3f, 0xff, 0xff, 0xfe, 0x00, 0x00, 
	0x07, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x01, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x03, 0xff, 0xff, 0xc0, 
	0x00, 0x00, 0x03, 0xff, 0xff, 0x00, 0x00, 0x00, 0x07, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x07, 0xff, 
	0xfc, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xf8, 0x00, 0x03, 0xf8, 0x0f, 0xff, 0xf0, 0x00, 0x3f, 0xff, 
	0x9f, 0xff, 0xf0, 0x01, 0xf2, 0x07, 0xff, 0xff, 0xe0, 0x03, 0xa2, 0x3f, 0xff, 0xff, 0xe0, 0x0f, 
	0x32, 0x7f, 0xff, 0xff, 0xc0, 0x19, 0x92, 0xff, 0xff, 0xff, 0xc0, 0x3c, 0x92, 0xff, 0xff, 0xff, 
	0x80, 0x66, 0x53, 0xff, 0xff, 0xff, 0x80, 0xe2, 0x5f, 0xff, 0xff, 0xff, 0x80, 0x99, 0x7c, 0x7f, 
	0xff, 0xff, 0x81, 0x8d, 0xe0, 0x07, 0xff, 0xff, 0x81, 0xe7, 0x80, 0x01, 0xff, 0xff, 0x83, 0x3f, 
	0x00, 0x00, 0x7f, 0xff, 0x03, 0x06, 0x00, 0x00, 0x3f, 0xff, 0x03, 0xfc, 0x00, 0x00, 0x3f, 0xff, 
	0x82, 0x0c, 0x00, 0x00, 0x1f, 0xff, 0x82, 0x38, 0x00, 0x00, 0x1f, 0xff, 0x82, 0x78, 0x00, 0x00, 
	0x0f, 0xff, 0x82, 0xf8, 0x00, 0x00, 0x0f, 0xff, 0x83, 0xf8, 0x00, 0x00, 0x07, 0xff, 0x83, 0xf8, 
	0x00, 0x07, 0xe7, 0xff, 0xc3, 0xf8, 0x00, 0x07, 0xf3, 0xff, 0xc3, 0xf8, 0x00, 0x03, 0xfb, 0xff, 
	0xc3, 0xf8, 0x00, 0x00, 0xf9, 0xff, 0xe1, 0xf8, 0x00, 0x00, 0x19, 0xff, 0xe1, 0xfc, 0x00, 0x00, 
	0x18, 0xff, 0xe1, 0xfc, 0x00, 0x00, 0x18, 0xff, 0xe3, 0xfc, 0x00, 0x00, 0x1f, 0xff, 0xe3, 0xfc, 
	0x00, 0x00, 0x1f, 0xff, 0xe3, 0xfc, 0x00, 0x00, 0x1f, 0xff, 0xe3, 0xfc, 0x00, 0x00, 0x1f, 0xff, 
	0xe7, 0xfc, 0x00, 0x00, 0x1f, 0xff, 0xe7, 0xfc, 0x0f, 0x80, 0x1f, 0xff, 0xcf, 0xfc, 0xff, 0xe0, 
	0x1f, 0xff, 0xdf, 0xff, 0xff, 0xf0, 0x1f, 0xff, 0xbf, 0xff, 0xff, 0xfc, 0x0f, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xcf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

// pointer table of frames
const uint8_t* const ANIMATION_FRAMES[] PROGMEM = {
  frame0,  frame1,  frame2,  frame3,  frame4,  frame5,
  frame6,  frame7,  frame8,  frame9,  frame10, frame11,
  frame12, frame13, frame14, frame15, frame16, frame17,
  frame18, frame19, frame20, frame21, frame22, frame23,
  frame24, frame25, frame26, frame27, frame28, frame29,
  frame30, frame31, frame32, frame33, frame34, frame35,
  frame36, frame37, frame38, frame39, frame40, frame41,
  frame42, frame43, frame44, frame45, frame46, frame47
};

const uint8_t FRAME_COUNT = 48;
const uint16_t FRAME_W = 64;
const uint16_t FRAME_H = 48;

// enum for state logic
enum class State {
  OPTION_1,
  OPTION_2,
  OPTION_3,
  OPTION_4,
  COUNT
};

volatile State currentState = State::OPTION_1;

// button ISRS
void lButtonISR() {
  // Left button pressed
  // debounce logic
  static uint32_t lastPress = 0;
  if (millis() - lastPress < 200) return;  // ignore if <200ms since last press
  lastPress = millis();

  // move back one, wrap around to end
  int next = (int)currentState - 1;
  if (next < 0) next = (int)State::COUNT - 1;
  currentState = (State)next;
}

void rButtonISR() {
  // Right button pressed
  // debounce logic
  static uint32_t lastPress = 0;
  if (millis() - lastPress < 200) return;  // ignore if <200ms since last press
  lastPress = millis();
  
  // move forward one, wrap around to start
  currentState = (State)(((int)currentState + 1) % (int)State::COUNT);
}

// initialize the display
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  Serial.println("Initializing I2C...");
  Wire.setSDA(PB9);
  Wire.setSCL(PB8);
  Wire.begin();

  Serial.println("Initializing SSD1306...");
  if(!display.begin(SSD1306_SWITCHCAPVCC, DISP_ADDR)) { // address for display
    Serial.println(F("SSD1306 initialization failed!"));
    for(;;);
  }
  else {
    Serial.println("Success!");
  }
  Serial.println("Setting up Display...");
  display.clearDisplay();

  // configure button inputs w/ pullup and interrupts
  pinMode(LBUTTON_PIN, INPUT_PULLUP);
  pinMode(RBUTTON_PIN, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(LBUTTON_PIN), lButtonISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(RBUTTON_PIN), rButtonISR, FALLING);

  // configure potentiometer
  pinMode(A0, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  // state machine
  switch (currentState) {
    case State::OPTION_1:
      // clear display on state change
      display.clearDisplay();
      
      // set up text
      display.setTextSize(1);
      display.setTextColor(WHITE);
      
      // Display static text
      display.setCursor(0, 0);
      display.println("Screen 1 - Text");

      display.setCursor(0, 17);
      display.println("Hello, world!");
    
      display.setCursor(0, 30);
      display.println("ECE 430 Project 6");

      display.setCursor(0, 40);
      display.println("SSD1306 OLED Module\nand Adafruit\nGraphics Library");
      
      display.display();
      break;
    case State::OPTION_2:
      // clear display on state change
      display.clearDisplay();

      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      
      // Display static text
      display.println("Screen 2 - Image");

      display.setCursor(64, 17);
      display.println("Go Green!");

      // Display an image
      display.drawBitmap(0, 17, MSU_Logo, 48, 48, WHITE);
      display.display();
      break;
    case State::OPTION_3:
      static uint8_t frameIdx = 0;

      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      display.println("Screen 3 - Animation");
      
      display.drawBitmap(32, 8, ANIMATION_FRAMES[frameIdx], FRAME_W, FRAME_H, WHITE);
      display.display();

      frameIdx = (frameIdx + 1) % FRAME_COUNT;
      delay(50); // ~20 fps
      break;
    case State::OPTION_4:
      int potValue = analogRead(POT_PIN);
      float voltage = (potValue * 3.3) / 1023;

      display.clearDisplay();

      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      
      // Display static text
      display.println("Screen 4 - Reading\nPotentiometer Value");
      
      display.setTextSize(1);
      display.setCursor(0, 17);
      display.print("ADC Value: ");
      display.println(potValue);

      display.print("Voltage: ");
      display.print(voltage);
      display.print(" V");

      display.display();
      delay(50);
      break;

  }
}

