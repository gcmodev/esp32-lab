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

// User controls. These avoid the SPI pins reserved for the future LoRa radio.
constexpr int BTN_UP_PIN = 32;
constexpr int BTN_DOWN_PIN = 33;
constexpr int BTN_SELECT_PIN = 25;
constexpr int BTN_BACK_PIN = 26;
constexpr int BTN_SEND_PIN = 27;
constexpr int BTN_RX_SIM_PIN = 14;

constexpr unsigned long DEBOUNCE_MS = 25;
constexpr unsigned long NOTICE_MS = 1200;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

struct Button {
  int pin;
  bool raw = HIGH;
  bool stable = HIGH;
  unsigned long changedAt = 0;
  bool pressedEvent = false;

  void begin() {
    pinMode(pin, INPUT_PULLUP);
    raw = stable = digitalRead(pin);
  }

  void update(unsigned long now) {
    const bool current = digitalRead(pin);
    if (current != raw) {
      raw = current;
      changedAt = now;
    }

    if ((now - changedAt) >= DEBOUNCE_MS && stable != raw) {
      stable = raw;
      if (stable == LOW) {
        pressedEvent = true;
      }
    }
  }

  bool takePress() {
    if (!pressedEvent) return false;
    pressedEvent = false;
    return true;
  }
};

Button btnUp{BTN_UP_PIN};
Button btnDown{BTN_DOWN_PIN};
Button btnSelect{BTN_SELECT_PIN};
Button btnBack{BTN_BACK_PIN};
Button btnSend{BTN_SEND_PIN};
Button btnRxSim{BTN_RX_SIM_PIN};

enum class View {
  Home,
  Presets,
  Inbox,
  Channel
};

enum class Notice {
  None,
  Sent,
  Received
};

View view = View::Home;
Notice notice = Notice::None;
unsigned long noticeUntil = 0;

int homeIndex = 0;
int presetIndex = 0;
int channelIndex = 2;
int unreadCount = 0;
String lastRx = "No messages";

const char* HOME_ITEMS[] = {"Presets", "Inbox", "Channel"};
constexpr int HOME_ITEM_COUNT = 3;

const char* PRESETS[] = {
  "OK",
  "WHERE ARE YOU?",
  "ON MY WAY",
  "NEED HELP",
  "CALL ME"
};
constexpr int PRESET_COUNT = 5;

const float CHANNELS_MHZ[] = {
  920.3f,
  920.7f,
  921.1f,
  921.5f,
  921.9f,
  922.3f,
  922.7f
};
constexpr int CHANNEL_COUNT = 7;

void clearText() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
}

void printChannel() {
  display.print(CHANNELS_MHZ[channelIndex], 1);
  display.print(" MHz");
}

void renderHome() {
  clearText();
  display.setCursor(0, 0);
  display.println("LORA HANDHELD  V1");

  display.print("H1  ");
  printChannel();
  if (unreadCount > 0) {
    display.print("  RX:");
    display.print(unreadCount);
  }
  display.println();

  for (int i = 0; i < HOME_ITEM_COUNT; ++i) {
    display.print(homeIndex == i ? "> " : "  ");
    display.println(HOME_ITEMS[i]);
  }

  display.println("SEND = quick OK");
  display.display();
}

void renderPresets() {
  clearText();
  display.setCursor(0, 0);
  display.print("PRESETS  ");
  display.print(presetIndex + 1);
  display.print("/");
  display.println(PRESET_COUNT);
  display.println("----------------");
  display.println(PRESETS[presetIndex]);
  display.println();
  display.println("UP/DN choose");
  display.println("SEND transmit");
  display.println("BACK home");
  display.display();
}

void renderInbox() {
  clearText();
  display.setCursor(0, 0);
  display.println("INBOX");
  display.println("----------------");
  if (lastRx == "No messages") {
    display.println("No messages yet");
  } else {
    display.println("FROM H2");
    display.println(lastRx);
    display.println("RSSI -78 dBm (sim)");
  }
  display.println();
  display.println("BACK home");
  display.display();
}

void renderChannel() {
  clearText();
  display.setCursor(0, 0);
  display.println("CHANNEL");

  display.setTextSize(2);
  display.setCursor(0, 18);
  display.print(CHANNELS_MHZ[channelIndex], 1);
  display.println("MHz");

  display.setTextSize(1);
  display.setCursor(0, 46);
  display.println("UP/DN change");
  display.println("SEL/BACK save");
  display.display();
}

void renderNotice(const char* title, const char* message) {
  clearText();
  display.setTextSize(2);
  display.setCursor(0, 6);
  display.println(title);
  display.setTextSize(1);
  display.setCursor(0, 36);
  display.println(message);
  display.setCursor(0, 54);
  printChannel();
  display.display();
}

void renderCurrent() {
  switch (view) {
    case View::Home: renderHome(); break;
    case View::Presets: renderPresets(); break;
    case View::Inbox: renderInbox(); break;
    case View::Channel: renderChannel(); break;
  }
}

void enterView(View next) {
  view = next;
  switch (view) {
    case View::Home: Serial.println("UI:HOME"); break;
    case View::Presets: Serial.println("UI:PRESETS"); break;
    case View::Inbox:
      unreadCount = 0;
      Serial.println("UI:INBOX");
      break;
    case View::Channel: Serial.println("UI:CHANNEL"); break;
  }
  renderCurrent();
}

void showNotice(Notice type, const char* message) {
  notice = type;
  noticeUntil = millis() + NOTICE_MS;
  renderNotice(type == Notice::Sent ? "SENT" : "RECEIVED", message);
}

void sendMessage(const char* message) {
  // Phase 1 uses a simulated transport. Phase 2 will replace this with SX1262.
  Serial.print("TX:");
  Serial.println(message);
  showNotice(Notice::Sent, message);
}

void simulateReceive() {
  lastRx = "PING FROM H2";
  unreadCount++;
  Serial.println("RX:PING FROM H2");
  showNotice(Notice::Received, "PING FROM H2");
}

void updateButtons() {
  const unsigned long now = millis();
  btnUp.update(now);
  btnDown.update(now);
  btnSelect.update(now);
  btnBack.update(now);
  btnSend.update(now);
  btnRxSim.update(now);
}

void handleControls() {
  // The RX simulator is always active, even while a SENT notice is visible.
  if (btnRxSim.takePress()) {
    simulateReceive();
    return;
  }

  if (notice != Notice::None) {
    if ((long)(millis() - noticeUntil) >= 0) {
      notice = Notice::None;
      enterView(View::Home);
    }
    return;
  }

  if (btnSend.takePress()) {
    if (view == View::Presets) {
      sendMessage(PRESETS[presetIndex]);
    } else {
      sendMessage(PRESETS[0]);
    }
    return;
  }

  if (btnBack.takePress()) {
    enterView(View::Home);
    return;
  }

  if (view == View::Home) {
    if (btnUp.takePress()) {
      homeIndex = (homeIndex + HOME_ITEM_COUNT - 1) % HOME_ITEM_COUNT;
      renderHome();
    }
    if (btnDown.takePress()) {
      homeIndex = (homeIndex + 1) % HOME_ITEM_COUNT;
      renderHome();
    }
    if (btnSelect.takePress()) {
      if (homeIndex == 0) enterView(View::Presets);
      if (homeIndex == 1) enterView(View::Inbox);
      if (homeIndex == 2) enterView(View::Channel);
    }
    return;
  }

  if (view == View::Presets) {
    if (btnUp.takePress()) {
      presetIndex = (presetIndex + PRESET_COUNT - 1) % PRESET_COUNT;
      renderPresets();
    }
    if (btnDown.takePress()) {
      presetIndex = (presetIndex + 1) % PRESET_COUNT;
      renderPresets();
    }
    return;
  }

  if (view == View::Channel) {
    if (btnUp.takePress()) {
      channelIndex = (channelIndex + CHANNEL_COUNT - 1) % CHANNEL_COUNT;
      renderChannel();
    }
    if (btnDown.takePress()) {
      channelIndex = (channelIndex + 1) % CHANNEL_COUNT;
      renderChannel();
    }
    if (btnSelect.takePress()) {
      Serial.print("CHANNEL:");
      Serial.println(CHANNELS_MHZ[channelIndex], 1);
      enterView(View::Home);
    }
  }
}
}

void setup() {
  Serial.begin(115200);
  delay(200);

  btnUp.begin();
  btnDown.begin();
  btnSelect.begin();
  btnBack.begin();
  btnSend.begin();
  btnRxSim.begin();

  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);

  Serial.println("BOOT:LORA_HANDHELD_V1");

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println("OLED_INIT_FAILED");
    while (true) {
      delay(1000);
    }
  }

  Serial.println("OLED_OK");
  enterView(View::Home);
  Serial.println("LORA_READY");
}

void loop() {
  updateButtons();
  handleControls();
  delay(2);
}
