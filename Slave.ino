#include <esp_now.h>
#include <WiFi.h>

// New style callback
void OnDataRecv(const esp_now_recv_info *info, const uint8_t *incomingData, int len) {
  char msg[100];
  memcpy(msg, incomingData, len);
  msg[len] = '\0';  // Null terminate

  Serial.print("Received from: ");
  for (int i = 0; i < 6; i++) {
    Serial.printf("%02X", info->src_addr[i]);
    if (i < 5) Serial.print(":");
  }

  Serial.print(" -> Message: ");
  Serial.println(msg);
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register receive callback
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  // Nothing needed, messages come via callback
}
