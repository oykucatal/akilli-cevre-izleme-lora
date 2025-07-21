#include <DHT.h>

// 📍 Pin tanımları
#define TRIG_PIN 26
#define ECHO_PIN 27

#define DHTPIN 32
#define DHTTYPE DHT11

#define LDR_PIN 33  // LDR sensörünün bağlı olduğu analog pin

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LDR_PIN, INPUT);

  dht.begin();
}

void loop() {
  // 📏 HC-SR04 ile mesafe ölçümü
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  float distance_cm = duration * 0.0343 / 2;

  // 🌡️ DHT11 ile sıcaklık ve nem
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // 💡 LDR ile ışık seviyesi
  int ldr_value = analogRead(LDR_PIN);  // 0-4095 arasında değer döner (ESP32 ADC çözünürlüğü yüksek)

  // 📤 Seri monitöre yazdırma
  Serial.println("----------- Ölçümler -----------");
  Serial.print("Mesafe     : "); Serial.print(distance_cm); Serial.println(" cm");

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("DHT11 verisi okunamadı!");
  } else {
    Serial.print("Sıcaklık   : "); Serial.print(temperature); Serial.println(" °C");
    Serial.print("Nem        : "); Serial.print(humidity); Serial.println(" %");
  }

  Serial.print("Işık Seviye: "); Serial.print(ldr_value); Serial.println(" / 4095");
  Serial.println("--------------------------------\n");

  delay(1000);
}
