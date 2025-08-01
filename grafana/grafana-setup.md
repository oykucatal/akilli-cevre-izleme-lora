# 📊 Grafana Panel Kurulumu
#######################################################









#######################################################
 1. Sıcaklık Sensörü Paneli
Görev: 
Cihazlardan gelen sıcaklık verisini °C cinsinden zaman serisi olarak grafiğe döker.
Measurement (Ölçüm Adı):** `device_frmpayload_data_temperature`  
Field: `value`

 Açıklama:
Bu ölçüm alanı, sensörlerin gönderdiği sıcaklık verilerini InfluxDB’de saklar. Veriler `value` adlı sayısal alanda tutulur.

 Kullanılan Flux Sorgusu:
from(bucket: "lora_sensor")
  |> range(start: -24h)
  |> filter(fn: (r) => r._measurement == "device_frmpayload_data_temperature")
  |> filter(fn: (r) => r._field == "value")
<img width="1920" height="917" alt="sıcalık" src="https://github.com/user-attachments/assets/558e8e6d-c721-424c-bf1c-c34b9ffbd540" />



2. Nem Sensörü Paneli
Açıklama:
Nem sensörleri, ölçtükleri ortam nem oranını yüzde cinsinden (örneğin 45%) InfluxDB’ye gönderir. Bu veriler `device_frmpayload_data_humidity` adlı measurement altında `value` alanında tutulur.  
Grafana, bu verileri InfluxDB'den alır ve zamana göre çizilen grafik ile kullanıcıya sunar.
Görev: 
Bu panel, LoRaWAN cihazlarından gelen **bağıl nem (%)** değerlerini zaman serisi grafiği olarak görselleştirir. Her cihazın nem verisi ayrı bir renk ile çizilir.
Measurement (Ölçüm Adı):  
`device_frmpayload_data_humidity`
Field: 
`value`

from(bucket: "lora_sensor")
  |> range(start: -24h)
  |> filter(fn: (r) => r._measurement == "device_frmpayload_data_humidity")
  |> filter(fn: (r) => r._field == "value")
 
<img width="1917" height="926" alt="nem" src="https://github.com/user-attachments/assets/ecb822b7-ccd9-4731-afe8-7d45dbb2c38b" />


  
  3. Mesafe Sensörü Paneli
Görev:
Ortamda algılanan nesneye olan uzaklığı zaman serisi grafikle gösterir.
Measurement: device_frmpayload_data_distance
Field: value
Açıklama:
Mesafe sensörlerinden gelen veriler bu measurement içinde tutulur. Veriler cm veya metre birimindedir (cihaza göre değişebilir).


from(bucket: "lora_sensor")
  |> range(start: -24h)
  |> filter(fn: (r) => r._measurement == "device_frmpayload_data_distance")
  |> filter(fn: (r) => r._field == "value")
<img width="1918" height="926" alt="mesafe" src="https://github.com/user-attachments/assets/42cdfe2c-536d-481f-a9c0-f9d56f1158b6" />
