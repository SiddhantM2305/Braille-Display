#include <esp_now.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

// Receiver's MAC address
uint8_t receiverMac[] = {0xFC, 0xE8, 0xC0, 0x7C, 0xDF, 0x20};

// WiFi credentials (only for NTP sync)
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// NTP Client setup
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 19800, 60000);  // +5:30 IST offset = 19800 sec

unsigned long lastSendTime = 0;
int messageCount = 0;

// 📅 Function to get current timestamp
String getTimestamp() {
  timeClient.update();
  return timeClient.getFormattedTime();  // returns HH:MM:SS
}

// 📤 Callback when data is sent
void OnDataSent(const wifi_tx_info_t *info, esp_now_send_status_t status) {
  Serial.println();
  Serial.println(F("--------------------------------------------------"));
  Serial.print(F("📤 Message #"));
  Serial.print(messageCount);
  Serial.println(F(" Delivery Report"));
  Serial.print(F("Time: "));
  Serial.println(getTimestamp());
  Serial.print(F("Status: "));
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "✅ Success" : "❌ Failed");
  Serial.println(F("--------------------------------------------------"));
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println(F("\n========================================="));
  Serial.println(F("   🚀 ESP-NOW Text Sender with Timestamps"));
  Serial.println(F("=========================================\n"));

  // Connect to WiFi (temporarily for NTP)
  Serial.println(F("🌐 Connecting to WiFi for time sync..."));
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\n✅ WiFi connected!");

  // Initialize NTP client
  timeClient.begin();
  timeClient.update();
  Serial.print(F("🕒 Current time synced: "));
  Serial.println(getTimestamp());

  // Switch to STA mode for ESP-NOW
  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);
  Serial.println(F("📡 Switched to ESP-NOW mode.\n"));

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println(F("❌ Error initializing ESP-NOW"));
    return;
  }

  // Register send callback
  esp_now_register_send_cb(OnDataSent);

  // Add peer
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverMac, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println(F("❌ Failed to add peer"));
    return;
  }

  Serial.println(F("✅ Peer added successfully."));
  Serial.println(F("\n💬 Type a message and press ENTER to send:\n"));
}

void loop() {
  if (Serial.available()) {
    String msg = Serial.readStringUntil('\n');
    msg.trim();
    if (msg.length() > 0) {
      messageCount++;
      unsigned long currentTime = millis();
      unsigned long interval = currentTime - lastSendTime;
      lastSendTime = currentTime;

      Serial.println(F("\n--------------------------------------------------"));
      Serial.print(F("🕒 Sending Message #"));
      Serial.println(messageCount);
      Serial.print(F("Time: "));
      Serial.println(getTimestamp());
      Serial.print(F("Interval since last message: "));
      Serial.print(interval / 1000.0, 2);
      Serial.println(F(" seconds"));
      Serial.print(F("Content: \""));
      Serial.print(msg);
      Serial.println(F("\"\n--------------------------------------------------"));

      esp_err_t result = esp_now_send(receiverMac, (uint8_t *)msg.c_str(), msg.length());

      if (result == ESP_OK) {
        Serial.println(F("📨 Message sent... waiting for delivery status"));
      } else {
        Serial.print(F("❌ Send error: "));
        Serial.println(result);
      }
    }
  }
}
