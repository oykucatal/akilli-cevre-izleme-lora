# Akıllı Çevre İzleme Sistemi - LoRa Gateway Modülü

Bu klasör, çevresel sensör verilerini alan LoRa Gateway modülünün yazılımını içermektedir. Gateway tarafında, ESP32 mikrodenetleyici kullanılarak RAK3172 LoRa modülü ile haberleşme sağlanır. Alınan veriler UART üzerinden ESP32’ye aktarılır ve Wi-Fi bağlantısı üzerinden sunucuya iletilmek üzere hazırlanabilir.

---

## Proje Amacı

- Uzun menzilli, düşük güç tüketimli kablosuz haberleşme ile çevresel verilerin toplanması.
- LoRa P2P haberleşmesiyle sensör düğümlerinden gelen verilerin alınması.
- Verilerin işlenerek daha sonra merkezi sunuculara (örneğin: InfluxDB, Flask, MQTT) iletilmesi için temel gateway yapısının kurulması.

---

##  Donanım Bileşenleri

| Bileşen              | Model / Tip                       |
|----------------------|-----------------------------------|
| Mikrodenetleyici     | ESP32 DevKit v1                   |
| LoRa Modülü          | RAK3172 (AT komutları ile kontrol edilen) |
| Güç bağlantısı       | USB 5V (ESP32 üzerinden)          |
| Sensörler (node tarafında) | DHT11/22, MQ-135, BH1750 vb.   |

---

##  Donanım Bağlantıları

### RAK3172 ↔ ESP32 UART Bağlantısı:

| RAK3172 Pin | ESP32 GPIO | Açıklama                   |
|-------------|-------------|----------------------------|
| TXD         | GPIO 16     | RAK3172’den veri alımı     |
| RXD         | GPIO 17     | ESP32’den veri gönderimi   |
| GND         | GND         | Ortak toprak hattı         |
| VCC         | 3.3V        | Güç bağlantısı (**5V verilmemeli**) |

> Not: UART2 kullanılıyor, ESP32’de `HardwareSerial(2)` olarak tanımlanmış.

---

##  Klasör Yapısı

```
LoRa_to_gateway/
│
├── LoRa_Receiver.ino       → ESP32 kodu (RAK3172’den AT komutlarıyla veri alma)
├── sendATCommand.h         → AT komutu gönderme yardımcı fonksiyonu
└── README.md               → Proje açıklaması (bu dosya)
```

---

##  Sistem Akışı

```
[Sensor Node (LoRa Sender)]
  →→→  LoRa P2P iletişimi
[ESP32 + RAK3172 (Gateway)]
  →→→  UART ile veri alımı
  →→→  Wi-Fi ile sunucuya gönderim (opsiyonel)
[InfluxDB / Flask / MQTT (Sunucu)]
  →→→  Grafana ile görselleştirme
```

---

##  Kullanılan AT Komutları (RAK3172 için)

| Komut                          | Açıklama                                    |
|--------------------------------|---------------------------------------------|
| `AT`                           | Cihaza bağlantı kontrolü                    |
| `AT+VER=?`                     | Donanım/sürüm bilgisi                       |
| `AT+DEVEUI=?`                  | Cihaz EUI bilgisini gösterir               |
| `AT+NWM=0`                     | LoRaWAN yerine P2P modunu aktif eder       |
| `AT+P2P=868250000:7:125:0:10:14` | P2P frekans ve parametre tanımı (EU868)    |
| `AT+PRECV=0`                   | Cihazı sürekli dinleme moduna alır         |

> Not: AT komutları arası `delay()` süreleri uygulanarak, cihazın cevap verebilmesi sağlanmalıdır.

---

##  Kodun Açıklaması

### `LoRa_Receiver.ino`
- UART2 üzerinden RAK3172 ile haberleşir.
- `sendATCommand()` fonksiyonu ile AT komutları gönderilir.
- `Serial2.readString()` ile gelen veri okunur.
- Gelecek sürümde gelen veri JSON parse edilerek MQTT veya HTTP ile sunucuya gönderilebilir.

### `sendATCommand.h`
```cpp
void sendATCommand(String command, unsigned long timeout) {
  RAK3172.println(command);
  delay(100);
  unsigned long t = millis();
  while (millis() - t < timeout) {
    if (RAK3172.available()) {
      Serial.write(RAK3172.read());
    }
  }
}
```

---

##  Veri Aktarımı (Sonraki Adım)

> Bu aşama henüz kodda yok ancak ileride eklenebilir:

- **WiFi bağlantısı kurulup** ESP32, gelen veriyi bir MQTT sunucusuna gönderebilir.
- Alternatif olarak HTTP POST isteği ile veriler Flask gibi bir backend'e yönlendirilebilir.
- Bu veriler sunucu tarafında **InfluxDB**'ye yazılarak **Grafana** ile görselleştirilebilir.

---

##  Grafana Görselleştirme (Varsayım)

Grafana tarafında, veri şu şekilde görselleştirilebilir:
- Sensör bazlı paneller (örn: sıcaklık, nem)
- Cihaz kimliğine göre filtreleme
- Zaman serisi grafikler

---

##  Kurulum Adımları

1. Arduino IDE’yi açın.
2. ESP32 kart tanımını yükleyin (`ESP32 Dev Module`).
3. `LoRa_Receiver.ino` dosyasını açın.
4. RX ve TX pinlerini ESP32’ye göre ayarlayın.
5. Kodu ESP32'ye yükleyin.
6. Seri Monitör üzerinden RAK3172'nin verdiği AT yanıtlarını ve gelen veriyi gözlemleyin.

---

## Geliştirme Notları

- LoRa bağlantısı kurulduğunda, diğer node cihazlarından gelen veri otomatik olarak seri porta düşer.
- Şimdilik yalnızca veri alımı yapılmaktadır. Gönderim veya doğrulama mekanizması (CRC, kimlik kontrolü) eklenmemiştir.
- İleride gelen veri içeriği parse edilerek bulut tabanlı servislere aktarılabilir.

---

##  Önerilen Geliştirmeler

- Gelen verinin JSON olarak ayrıştırılması
- MQTT üzerinden veri gönderimi
- Web dashboard'da veri canlı izleme
- OTA güncelleme desteği (ESP32 için)

---

## Lisans

Bu proje MIT lisansı ile yayımlanmıştır. Ticari ve kişisel kullanım serbesttir.

---

##  Katkıda Bulunanlar

Proje, öğrenme ve uygulama amacıyla geliştirilmiştir. Katkılar için pull request gönderebilirsiniz.
