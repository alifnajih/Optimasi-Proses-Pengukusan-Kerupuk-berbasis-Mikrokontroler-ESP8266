
## 🧩 **1️⃣ Tujuan Program**

Program ini digunakan untuk **mengontrol proses pengukusan kerupuk secara otomatis** berdasarkan **suhu yang dibaca sensor DS18B20**, serta **mengirimkan data suhu ke server lokal (web PHP)**.
Sistem juga memiliki:

* LCD untuk tampilan status suhu dan waktu.
* Tombol untuk mengatur waktu pengukusan (*timer start/stop*).
* Relay untuk mengaktifkan elemen pemanas.
* Buzzer sebagai alarm.
* Fitur kontrol otomatis berdasarkan batas **suhu minimum & maksimum** yang dikirim dari server (PHP + MySQL).

---

## ⚙️ **2️⃣ Komponen yang Digunakan**

| No | Komponen          | Fungsi                                               |
| -- | ----------------- | ---------------------------------------------------- |
| 1  | ESP8266 (NodeMCU) | Mikrokontroler utama dan WiFi                        |
| 2  | Sensor DS18B20    | Membaca suhu ruang kukus                             |
| 3  | Relay Module      | Mengontrol elemen pemanas                            |
| 4  | LCD I2C 16x2      | Menampilkan suhu & waktu                             |
| 5  | Buzzer            | Alarm saat batas suhu tercapai                       |
| 6  | Tombol Push       | Memulai / menghentikan timer                         |
| 7  | Server PHP/MySQL  | Menerima data suhu dan mengirim batas suhu (Min/Max) |

---

## 🧠 **3️⃣ Penjelasan Alur Program**

### 🔹 **Setup()**

1. **Inisialisasi pin**

   * Relay → output
   * Buzzer → output
   * Button → input (dengan internal pull-up)
2. **Inisialisasi LCD I2C**

   * Menampilkan tampilan sambutan (*printWelcome()*)
3. **Koneksi ke WiFi**

   * Menampilkan status koneksi di Serial Monitor & LCD.
4. **Tampilan awal LCD**

   ```
   SUHU  => 
   WAKTU =>   MATI
   ```

---

### 🔹 **Loop()**

Program utama berjalan terus-menerus dan memiliki 3 proses utama:

#### ① **Mengirim Data Suhu ke Server**

Fungsi `sendData()` akan:

* Membaca suhu dari DS18B20.
* Menampilkan suhu ke LCD.
* Mengirim nilai suhu ke server melalui `HTTP POST` ke:

  ```
  http://192.168.100.11/pengukusankerupuk/sensorData.php
  ```
* Mengambil nilai batas bawah & atas suhu (`getMin.php` & `getMax.php`) untuk menentukan kapan relay aktif/mati.

---

#### ② **Mengatur Timer Pengukusan**

Tombol di pin D0 (GPIO16) digunakan untuk:

* **Mulai timer** → tekan tombol sekali.
* **Stop timer** → tekan tombol lagi.

Jika timer aktif:

* Menghitung sisa waktu pengukusan (15 menit → bisa diubah dengan variabel `timerDuration`).
* Menampilkan sisa waktu di LCD:

  ```
  WAKTU => 12 min
  ```
* Saat waktu habis, LCD menampilkan:

  ```
  WAKTU => SELESAI
  ```

  dan buzzer berbunyi 5 kali.

---

#### ③ **Kontrol Otomatis Berdasarkan Suhu**

Saat timer aktif:

* Jika suhu **≤ setMin** → buzzer menyala peringatan suhu rendah.
* Jika suhu **≥ setMax** → relay **dimatikan** (menghentikan pemanas).
* Jika suhu **di antara batas Min & Max** → relay **menyala** (pemanas aktif kembali).

---

## 📶 **4️⃣ Komunikasi dengan Server**

Program melakukan koneksi HTTP ke tiga endpoint PHP:

| Fungsi            | URL               | Metode | Data yang Dikirim / Diterima |
| ----------------- | ----------------- | ------ | ---------------------------- |
| Kirim suhu        | `/sensorData.php` | POST   | `api_key`, `temperature`     |
| Ambil batas bawah | `/getMin.php`     | GET    | Mengembalikan nilai `setMin` |
| Ambil batas atas  | `/getMax.php`     | GET    | Mengembalikan nilai `setMax` |

Server kemudian menyimpan data suhu ke database dan mengembalikan konfigurasi suhu terbaru ke alat.

---

## 🔌 **5️⃣ Wiring Rangkaian**

| Komponen              | Pin NodeMCU      | Keterangan                                   |
| --------------------- | ---------------- | -------------------------------------------- |
| **Sensor DS18B20**    | D3 (GPIO0)       | Data (dengan resistor pull-up 4.7kΩ ke 3.3V) |
|                       | 3V3              | VCC                                          |
|                       | GND              | GND                                          |
| **Relay Module**      | D6 (GPIO12)      | Kontrol pemanas                              |
|                       | 5V               | VCC                                          |
|                       | GND              | GND                                          |
| **Buzzer**            | D7 (GPIO13)      | Alarm                                        |
| **Button Start/Stop** | D0 (GPIO16)      | Input tombol (ke GND bila ditekan)           |
| **LCD I2C**           | SDA → D2 (GPIO4) | Jalur data I2C                               |
|                       | SCL → D1 (GPIO5) | Jalur clock I2C                              |
|                       | VCC → 5V         | Daya                                         |
|                       | GND → GND        | Ground bersama                               |

---

## 🔔 **6️⃣ Fungsi Tambahan**

### 🟢 `beep(n)`

Menghasilkan bunyi buzzer sebanyak `n` kali.
Digunakan untuk:

* 1 beep → tombol ditekan.
* 2 beep → timer mulai.
* 5 beep → proses selesai.

---

### 🟢 `printWelcome()`

Menampilkan animasi sambutan bertahap di LCD saat alat menyala:

```
MONITORING PENGUKUSAN KERUPUK
KONTROL SUHU OTOMATIS
By: Alif
Connecting WiFi...
```

---

## 💡 **7️⃣ Kesimpulan Alur Kerja Sistem**

1. Alat menyala → konek WiFi → tampil menu utama.
2. DS18B20 membaca suhu ruang kukus dan menampilkan ke LCD.
3. Data suhu dikirim ke server.
4. Server mengirim batas suhu min & max.
5. User menekan tombol → proses pengukusan dimulai.
6. Timer berjalan → relay hidup/mati otomatis sesuai suhu.
7. Setelah waktu habis → buzzer bunyi, proses selesai.
