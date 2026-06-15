#include <LiquidCrystal.h>

constexpr uint8_t rs = 12, en = 13, d4 = 5, d5 = 4, d6 = 3, d7 = 2;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  // Initialize with LCD's number of columns and rows:
  lcd.begin(16, 2);
  
  // Print an initial static welcome message
  lcd.setCursor(0, 0);          // Column 0, Row 0
  lcd.print("ECE 430 Proj 6");
  lcd.setCursor(0, 1);          // Column 0, Row 1
  lcd.print("Display Demo!");
  delay(4000);                  // Hold message for 4 seconds
}

void loop() {
  // Clear the screen to show changing content
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Module 1: LCD");
  lcd.setCursor(0, 1);
  lcd.print("ECE 430 Proj 6");
  delay(2000);

  // Centering can be done manually (or algorithmically)
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("    HD44780");
  lcd.setCursor(3, 1);
  lcd.print("4-Bit Mode");
  delay(2000);
}
