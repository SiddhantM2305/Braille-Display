#include <esp_now.h>
#include <WiFi.h>

// Replace with receiver's MAC address
uint8_t receiverMac[] = {0xFC, 0xE8, 0xC0, 0x7C, 0xDF, 0x20};

// New style callback for send status
void OnDataSent(const wifi_tx_info_t *info, esp_now_send_status_t status) {
  Serial.print("Delivery: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register send callback
  esp_now_register_send_cb(OnDataSent);

  // Add peer (receiver)
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverMac, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  Serial.println("Type a message and press ENTER to send:");
}

void loop() {
  if (Serial.available()) {
    String msg = Serial.readStringUntil('\n');  // read input until Enter key
    msg.trim(); // remove newline / spaces
    if (msg.length() > 0) {
      esp_now_send(receiverMac, (uint8_t *)msg.c_str(), msg.length());
    }
  }
}
