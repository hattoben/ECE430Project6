// ECE 430 Project 6
// Benjamin Hatto
// SSD1306 Display Control with Buttons and Interrupts
// Board: ST Nucleo-F446RE (would need to modify pin consts to work with arduino)

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // oled display width (px)
#define SCREEN_HEIGHT 64 // oled display height (px)
#define DISP_ADDR 0x3C // I2C address of oled

// constants for interrupts
const uint8_t LBUTTON_PIN = PA10;  // arduino D2
const uint8_t RBUTTON_PIN = PB3;   // arduino D3

// enum for state logic
enum class State {
  OPTION_1,
  OPTION_2,
  OPTION_3,
  COUNT
};

volatile State currentState = State::OPTION_1;

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
    Serial.println(F("SSD1306 allocation failed"));
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
}

void loop() {
  // put your main code here, to run repeatedly:
  while (currentState == State::OPTION_1) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 20);
    // Display static text
    display.println("Hello, world!");
    display.display();
  }
  while (currentState == State::OPTION_2){
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 20);
    // Display static text
    display.println("Another Screen");
    display.display();
  }
  while (currentState == State::OPTION_3){
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 20);
    // Display static text
    display.println("Last Screen!");
    display.display();
  }
}

