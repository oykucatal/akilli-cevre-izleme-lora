#include <DHT.h>
#include <WiFi.h>
// :round_pushpin: Pin tanımları
#define TRIG_PIN 26
#define ECHO_PIN 27
#define DHTPIN 33
#define DHTTYPE DHT11
#define LDR_PIN 32  // LDR sensörünün bağlı olduğu analog pin
//UART pins for RAK3172
#define RAK_TX_PIN 17  // ESP32 pin connected to RAK3172 RX
#define RAK_RX_PIN 16  // ESP32 pin connected to RAK3172 TX
DHT dht(DHTPIN, DHTTYPE);
// Create a HardwareSerial object for RAK3172
HardwareSerial RAK3172(2);  // creates a serial port object on UART2
// :satellite_antenna: LoRa başlangıç
void setup() {
  Serial.begin(115200);
  Serial.println("LoRa P2P Transmitter - Sensör Verici");
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LDR_PIN, INPUT);
  dht.begin();
  RAK3172.begin(9600, SERIAL_8N1, RAK_RX_PIN, RAK_TX_PIN);
  delay(1000);
  // Configure RAK3172 for LoRa P2P mode
  configureLoRaP2P();
}
// :id: Kısa MAC ID üret (char array'e yaz)
// :id: Kısa MAC ID üret (char array'e yaz)
void getShortMacID(char* buffer, size_t len) {
  uint64_t chipID = ESP.getEfuseMac();
  snprintf(buffer, len, "%02X%02X%02X",
    (uint8_t)(chipID >> 40),
    (uint8_t)(chipID >> 32),
    (uint8_t)(chipID >> 24));
}

// :straight_ruler: Mesafe sensörü okuma
int measureDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH);
  int distance_cm = (int)(duration * 0.0343 / 2);
  return distance_cm;
}
// :thermometer: DHT11 ile sıcaklık
int measureTemperature() {
  float temp = dht.readTemperature();
  if (isnan(temp)) return -999;
  return (int)temp;
}
// :droplet: DHT11 ile nem
int measureHumidity() {
  float hum = dht.readHumidity();
  if (isnan(hum)) return -999;
  return (int)hum;
}
// :bulb: LDR ölçümü
int measureLightLevel() {
  return analogRead(LDR_PIN);  // 0-4095
}
// :bar_chart: Sensör okuma + LoRa gönderim
void readAndDisplaySensors() {
  int distance = measureDistance();
  int temperature = measureTemperature();
  int humidity = measureHumidity();
  int lightLevel = measureLightLevel();
  char nodeID[16];
  getShortMacID(nodeID, sizeof(nodeID));
  Serial.println("----------- Ölçümler -----------");
  Serial.print("Cihaz ID   : ");
  Serial.println(nodeID);
  Serial.print("Mesafe     : ");
  Serial.print(distance);
  Serial.println(" cm");
  if (temperature == -999 || humidity == -999) {
    Serial.println("DHT11 verisi okunamadı!");
  } else {
    Serial.print("Sıcaklık   : ");
    Serial.print(temperature);
    Serial.println(" °C");
    Serial.print("Nem        : ");
    Serial.print(humidity);
    Serial.println(" %");
  }
  Serial.print("Işık Seviye: ");
  Serial.print(lightLevel);
  Serial.println(" / 4095");
  Serial.println("--------------------------------\n");
  // Sensör verilerini tek stringde topla
  char sensorData[64];
  snprintf(sensorData, sizeof(sensorData), "%s$%d$%d$%d$%d", nodeID, distance, temperature, humidity, lightLevel);
  sendLoRaP2PData(sensorData);
}
void loop() {
  readAndDisplaySensors();
  if (RAK3172.available()) {
    String response = RAK3172.readString();
    Serial.print("RAK3172 Response: ");
    Serial.println(response);
  }
  if (Serial.available()) {
    String command = Serial.readString();
    command.trim();
    Serial.println("Sending command: " + command);
    sendATCommand(command.c_str(), 3000);
  }
  delay(5000);
}
// AT komutu gönderme fonksiyonu
void sendATCommand(const char* command, int timeout) {
  Serial.println("Sending: " + String(command));
  while (RAK3172.available()) RAK3172.read();  // boşalt
  RAK3172.println(command);
  unsigned long startTime = millis();
  String response = "";
  while (millis() - startTime < timeout) {
    if (RAK3172.available()) {
      response += RAK3172.readString();
      break;
    }
    delay(10);
  }
  if (response.length() > 0) Serial.println("Response: " + response);
  else Serial.println("No response received");
  Serial.println("---");
}
// RAK3172'yi yapılandır
void configureLoRaP2P() {
  delay(3000);
  Serial.println("Configuring RAK3172 for LoRa P2P...");
  delay(3000);
  sendATCommand("AT", 2000);
  delay(1000);
  sendATCommand("AT+BOOT?", 2000);
  delay(1000);
  sendATCommand("AT+NWM=0", 2000);  // LoRa P2P
  delay(1000);
  sendATCommand("AT+PFREQ=868250000", 2000);  // EU frequency
  delay(1000);
  sendATCommand("AT+PSF=7", 2000);  // Spreading factor
  delay(1000);
  sendATCommand("AT+PBW=125", 2000);  // Bandwidth
  delay(1000);
  sendATCommand("AT+PCR=1", 2000);  // Coding rate
  delay(1000);
  sendATCommand("AT+PPL=8", 2000);  // Preamble
  delay(1000);
  sendATCommand("AT+PTP=14", 2000);  // Power
  Serial.println("LoRa P2P Configuration complete!");
}
// Sensör verisini LoRa ile gönder
void sendLoRaP2PData(const char* data) {
  Serial.print("Sending sensor data via LoRa P2P: ");
  Serial.println(data);
  char hexData[256] = {0};
  for (int i = 0; i < strlen(data); i++) {
    char hexByte[3];
    sprintf(hexByte, "%02X", data[i]);
    strcat(hexData, hexByte);
  }
  char command[300];
  snprintf(command, sizeof(command), "AT+PSEND=%s", hexData);
  sendATCommand(command, 5000);
}