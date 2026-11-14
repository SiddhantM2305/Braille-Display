#include <esp_now.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

// ======================= TIME SETUP ==========================
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 19800, 60000); // IST timezone (+5:30)

String getTimestamp() {
  timeClient.update();
  return timeClient.getFormattedTime();  // Returns "HH:MM:SS"
}

// ======================= BRAILLE SETUP ==========================
const int braillePins[2][6] = {
  {12, 13, 14, 27, 26, 25},  // Cell 1
  {33, 32, 21, 19, 18, 5}    // Cell 2
};

// Braille alphabet (a–z)
const uint8_t brailleMap[26] = {
  0b000001, 0b000011, 0b001001, 0b011001, 0b010001, 0b001011, 0b011011, 0b010011, 0b001010, 0b011010,
  0b000101, 0b000111, 0b001101, 0b011101, 0b010101, 0b001111, 0b011111, 0b010111, 0b001110, 0b011110,
  0b100101, 0b100111, 0b111010, 0b101101, 0b111101, 0b110101
};

void clearBrailleCells() {
  for (int cell = 0; cell < 2; cell++)
    for (int pin = 0; pin < 6; pin++)
      digitalWrite(braillePins[cell][pin], LOW);
}

void displayChar(int cell, char c) {
  clearBrailleCells();  // Clear both before showing new characters

  if (c >= 'a' && c <= 'z') {
    uint8_t pattern = brailleMap[c - 'a'];
    for (int i = 0; i < 6; i++) {
      int bit = (pattern >> i) & 1;
      digitalWrite(braillePins[cell][i], bit);
    }
  }
}

void displayText(String text) {
  text.toLowerCase();
  char c1 = (text.length() > 0) ? text[0] : ' ';
  char c2 = (text.length() > 1) ? text[1] : ' ';

  Serial.printf("Braille Display: '%c' '%c'\n", c1, c2);

  displayChar(0, c1);
  displayChar(1, c2);
}

// ======================= ESP-NOW SETUP ==========================
void OnDataRecv(const esp_now_recv_info *info, const uint8_t *incomingData, int len) {
  char msg[150];
  memcpy(msg, incomingData, len);
  msg[len] = '\0';

  Serial.println();
  Serial.println(F("--------------------------------------------------"));
  Serial.print(F("📥 Received Message at "));
  Serial.println(getTimestamp());
  Serial.print(F("Message: "));
  Serial.println(msg);
  Serial.println(F("--------------------------------------------------"));

  displayText(String(msg));  // Convert received message to Braille
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println(F("\n==========================================="));
  Serial.println(F("  🟢 ESP-NOW Receiver + Braille Display"));
  Serial.println(F("===========================================\n"));

  // Setup Braille pins
  for (int cell = 0; cell < 2; cell++)
    for (int pin = 0; pin < 6; pin++) {
      pinMode(braillePins[cell][pin], OUTPUT);
      digitalWrite(braillePins[cell][pin], LOW);
    }

  // Connect to WiFi temporarily for NTP sync
  Serial.println(F("🌐 Connecting to WiFi for NTP sync..."));
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\n✅ WiFi connected!");
  timeClient.begin();
  timeClient.update();
  Serial.print(F("🕒 Time synced: "));
  Serial.println(getTimestamp());

  // Switch to STA for ESP-NOW
  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);
  Serial.println(F("📡 Switched to ESP-NOW mode.\n"));

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println(F("❌ Error initializing ESP-NOW"));
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);
  Serial.println(F("✅ Ready to receive Braille messages...\n"));
}

void loop() {
  // Nothing here — callback handles everything
}
