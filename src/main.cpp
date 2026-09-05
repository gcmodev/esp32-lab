#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

namespace {
constexpr int SCREEN_WIDTH = 128;
constexpr int SCREEN_HEIGHT = 64;
constexpr int OLED_RESET = -1;
constexpr uint8_t OLED_ADDRESS = 0x3C;

constexpr int I2C_SDA_PIN = 21;
constexpr int I2C_SCL_PIN = 22;
constexpr int BUTTON_PIN = 27;

constexpr unsigned long DEBOUNCE_MS = 25;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

bool stableButtonState = HIGH;
bool lastRawButtonState = HIGH;
unsigned long lastDebounceAt = 0;

void renderStatus(bool pressed) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Haekal ESP32 Lab");
  display.println("----------------");

  display.setTextSize(2);
  display.setCursor(0, 24);
  display.println("ONLINE");

  display.setTextSize(1);
  display.setCursor(0, 52);
  display.print("Button: ");
  display.print(pressed ? "PRESSED" : "RELEASED");

  display.display();
}

void reportButton(bool pressed) {
  Serial.println(pressed ? "BUTTON:PRESSED" : "BUTTON:RELEASED");
  renderStatus(pressed);
}
}

void setup() {
  Serial.begin(115200);
  delay(200);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);

  Serial.println("BOOT:ESP32_LAB");

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println("OLED_INIT_FAILED");
    while (true) {
      delay(1000);
    }
  }

  stableButtonState = digitalRead(BUTTON_PIN);
  lastRawButtonState = stableButtonState;

  renderStatus(stableButtonState == LOW);

  Serial.println("OLED_OK");
  Serial.println("LAB_READY");
}

void loop() {
  const bool rawState = digitalRead(BUTTON_PIN);

  if (rawState != lastRawButtonState) {
    lastRawButtonState = rawState;
    lastDebounceAt = millis();
  }

  if ((millis() - lastDebounceAt) >= DEBOUNCE_MS && rawState != stableButtonState) {
    stableButtonState = rawState;
    reportButton(stableButtonState == LOW);
  }

  delay(2);
}
