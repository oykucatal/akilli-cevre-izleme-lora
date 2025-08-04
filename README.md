# Akıllı Çevre İzleme Sistemi - LoRa Gateway Modülü

Bu klasör, çevresel sensör verilerini LoRa P2P protokolü kullanarak alacak bir gateway modülü için geliştirilen yazılımı içermektedir. Gateway tarafında ESP32 mikrodenetleyicisi kullanılarak, UART haberleşmesi yoluyla RAK3172 LoRa modülüyle iletilen veriler alınmakta ve gerekirse sunucuya aktarılmaya hazırlanmaktadır.

---

## 📍 Projenin Genel Amacı

* Uzun menzilli, düşük güç tüketimli haberleşme ile farklı lokasyonlardan veri toplamak.
* LoRa P2P protokolüyle sensör düğümlerinden gelen verileri kablosuz olarak almak.
* Alınan verileri çözümleyerek sunucuya (MQTT, HTTP, WebSocket vs.) iletmek üzerine altyapı oluşturmak.
* Sistemi InfluxDB + Grafana gibi bir ortamda görselleştirilebilir hale getirmek.

---

## 🔧 Kullanılan Donanım ve Bağlantılar

| Donanım Bileşeni | Model / Tip      | Görevi                         |
| ---------------- | ---------------- | ------------------------------ |
| Mikrodenetleyici | ESP32 DevKit     | UART, Wi-Fi ve genel kontrol   |
| LoRa Modülü      | RAK3172          | P2P veri alışverişi            |
| Sensörler        | (Node tarafında) | Veri oluşturur (temp, nem vb.) |

### UART Bağlantıları:

* RAK3172 TX  →  ESP32 GPIO16 (RX)
* RAK3172 RX  →  ESP32 GPIO17 (TX)
* RAK3172 VCC →  3.3V
* RAK3172 GND →  GND

> Not: RAK3172 5V toleranslı değildir. 3.3V besleme kullanılmalıdır.

---

## 📁 Proje Klasör Yapısı

```
LoRa_to_gateway/
├── LoRa_Receiver.ino      # ESP32 ana kodu
├── sendATCommand.h        # AT komutu gönderimi için yardımcı fonksiyon
└── README.md              # Bu dosya
```

---

## 🚀 Sistem Akışı ve Veri Yolculuğu

```
[Sensor Node (ESP32 + RAK3172 + Sensör)]
     ↦  LoRa P2P
[Gateway (ESP32 + RAK3172)]
     ↦  UART ile veri alımı
     ↦  Wi-Fi ile MQTT/HTTP ile sunucuya gönderim
[Sunucu: InfluxDB]
     ↦  [Grafana] ile veri görselleştirme
```

---

## 📃 RAK3172 ile Kullanılan AT Komutları

| Komut                            | Açıklama                    |
| -------------------------------- | --------------------------- |
| `AT`                             | Bağlantı testi              |
| `AT+VER=?`                       | Sürüm bilgisi               |
| `AT+DEVEUI=?`                    | Cihaz EUI                   |
| `AT+NWM=0`                       | P2P modunu aktif eder       |
| `AT+P2P=868250000:7:125:0:10:14` | Frekans ve özellikler ayarı |
| `AT+PRECV=0`                     | Sürekli dinleme moduna alır |

---

## 📊 Kodun Temel Mantığı

* ESP32, `HardwareSerial(2)` ile UART2 kullanarak RAK3172 ile haberleşir.
* `sendATCommand()` fonksiyonuyla AT komutları gönderilir.
* `Serial2.readString()` ile gelen veri okunur ve `Serial` portundan yazdırılır.
* Gelişmiş versiyonlarda bu veri çözülerek MQTT veya HTTP üzerinden gönderilir.

---

## 📝 Kurulum Adımları

1. Arduino IDE kurun, ESP32 kart tanımını ekleyin.
2. RX (16), TX (17) pinlerine UART2 tanımı yapın.
3. `LoRa_Receiver.ino` dosyasını ESP32'ye yükleyin.
4. Seri monitörü 115200 baud ile açın.
5. Gelen verileri ve AT cevaplarını gözlemleyin.

---

## 🌐 Sunucuya Veri Aktarımı (Opsiyonel Katman)

* ESP32 Wi-Fi ile ağa bağlanır.
* Gelen veriler MQTT broker'ına veya bir REST API'ye gönderilir.
* Sunucuda bu veriler InfluxDB'ye kaydedilir.
* Grafana paneli ile izlenebilir.

---

## 🔄 Geliştirme Önerileri

* JSON çözümleme ve verilerin düzenli formatlanması
* Hata kontrolü (CRC, zaman damgası, kimlik)
* Web arayüz entegrasyonu (Grafik, uyarı, log)
* OTA güncelleme desteği

---

## 📅 Lisans

Bu proje MIT lisansı ile sunulmuştur. Kullanım ve geliştirme özgürdür.

---

## 👨‍💻 Katkı ve İletişim


