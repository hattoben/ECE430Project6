// ECE 430 Project 6
// Benjamin Hatto
// SSD1306 "Hello World" Sample

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // oled display width (px)
#define SCREEN_HEIGHT 64 // oled display height (px)

#define DISP_ADDR 0x3C

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
  delay(2000);
  Serial.println("Setting up Display...");
  display.clearDisplay();

  display.setTextSize(1);
  display.setCursor(0, 10);
  // Display static text
  display.println("Hello, world!");
  display.display();
}

void loop() {
  // put your main code here, to run repeatedly:
}

