# InfluxDB Kurulumu ve ChirpStack Entegrasyonu

Bu döküman, SSH anahtarıyla bir Ubuntu sunucusuna bağlanarak `.tar.gz` formatında InfluxDB kurulumu, servis dosyası ile 7/24 çalışır hale getirilmesi, arayüz üzerinden yapılandırma adımları ve ChirpStack ile entegrasyon sürecini kapsamaktadır.

---

# 1. SSH Key ile Sunucuya Bağlantı

Sunucuya güvenli bağlantı şu komutla sağlandı:

ssh srvadmin@<ip adresi>

Adından şifre girilerek sunucuya giriş sağlandı.

<ssh_bağlantısı.png>

# 2. .tar.gz ile InfluxDB Kurulumu
Aşağıdaki adımlarla .tar.gz formatında InfluxDB indirildi ve kuruldu:

<kurulum.png>

wget https://dl.influxdata.com/influxdb/releases/influxdb2-<versiyon>-linux-amd64.tar.gz
tar -xvzf influxdb2-<versiyon>-linux-amd64.tar.gz
Kurulum tamamlandıktan sonra influx version komutu ile doğrulama yapıldı.

# 3. InfluxDB için Systemd Servisi Oluşturma
Kurulan InfluxDB’yi 7/24 çalışır hâle getirmek için aşağıdaki gibi bir servis dosyası oluşturuldu:

sudo nano /etc/systemd/system/influxdb.service

<servis.png>

sudo systemctl daemon-reexec
sudo systemctl enable influxdb
sudo systemctl start influxdb
Sunucu her yeniden başladığında InfluxDB otomatik olarak devreye girer.

# 4. Web Arayüzüne Erişim (Kendi Bilgisayarından)
Tarayıcı üzerinden aşağıdaki gibi InfluxDB arayüzüne erişildi:

http://<ip_adresi>:8086
Sunucu dışarıya açıldığı için farklı cihazlardan yönetim arayüzüne giriş mümkün oldu.
<influx.png>

# 5. İlk Yapılandırma: Kullanıcı, Org, Bucket, Token

İlk kurulum ekranında:

Kullanıcı adı ve şifre belirlendi

organization ismi girildi

lora_sensor adlı bucket oluşturuldu

<bucket.png>

API Token Key üretildi

<token.png>

# 6. ChirpStack ile Entegrasyon
InfluxDB'den oluşturulan API Token Key, ChirpStack arayüzüne aşağıdaki şekilde eklendi:

<chirpstack.png>