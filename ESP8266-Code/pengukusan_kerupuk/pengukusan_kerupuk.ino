//*************************************************************************//
//           OPTIMASI PROSES PENGUKUSAN KERUPUK dengan SISTEM              //
//            BERBASIS MICROKONTROLER DI PABRIK KERUPUK PLOSO              //
//*************************************************************************//

#include <OneWire.h>
#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <LiquidCrystal_I2C.h>
#include <DallasTemperature.h>
#include <ESP8266HTTPClient.h>
//*************************************************************************//
const char *wifi_ssid = "Alif Najih";              // nama wifi
const char *wifi_password = "123456789";  // sandi wifi
//*************************************************************************//
const char *IP_HOST = "192.168.100.12";  // IP Address server
String PROJECT_API_KEY = "dhani"; 
//*************************************************************************//
unsigned long lastMillis = 0;
//*************************************************************************//
const int relay1 = 12;
const int buzzer = 13;
//*************************************************************************//
OneWire oneWire(0);
DallasTemperature sensor(&oneWire);
LiquidCrystal_I2C lcd(0x27, 16, 2);
//*************************************************************************//
const int buttonPin = 16;
//*************************************************************************//
int buttonState = HIGH;
int lastButtonState = HIGH;
bool timerRunning = false;
unsigned long startTime;
unsigned long timerDuration = 15;
unsigned long lastDebounceTime = 0;
//*************************************************************************//
int setMin, setMax;
//*************************************************************************//
float tempC;
//*************************************************************************//
//                            Setup Function                               //
//*************************************************************************//
void setup() {
  //======================================================
  Serial.begin(115200);
  delay(500);
  pinMode(relay1, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  digitalWrite(relay1, HIGH);
  digitalWrite(buzzer, LOW);
  //======================================================
  lcd.init();
  lcd.backlight();
  delay(1000);
  printWelcome();
  //======================================================
  Serial.print("\nConnecting to ");
  Serial.println(wifi_ssid);
  Serial.println("");
  WiFi.begin(wifi_ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("");
  //======================================================
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" Connected Wifi ");
  delay(1500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SUHU  => ");
  lcd.setCursor(0, 1);
  lcd.print("WAKTU => ");
  lcd.setCursor(8, 1);
  lcd.print("  MATI   ");
  delay(500);
  //======================================================
}
//*************************************************************************//

//*************************************************************************//
//                             Loop Function                               //
//*************************************************************************//
void loop() {
  
  //======================================================
  sendData();
  //======================================================
  int reading = digitalRead(buttonPin);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }
  //======================================================
  if ((millis() - lastDebounceTime) > 50) {
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == LOW) {
        beep(1);
        if (timerRunning) {
          // Hentikan timer
          timerRunning = false;
          lcd.setCursor(8, 1);
          lcd.print("  MATI  ");
        } else {
          // Mulai timer
          startTime = millis();
          timerRunning = true;
          beep(2);
        }
      }
    }
  }

  lastButtonState = reading;
  //======================================================
  if (timerRunning) {
    unsigned long currentTime = millis();
    unsigned long elapsedTime = currentTime - startTime;
    unsigned long remainingTime = (timerDuration * 60000) - elapsedTime;
    unsigned long remainingMinutes = remainingTime / 60000;

    lcd.setCursor(8, 1);
    lcd.print(remainingMinutes);
    lcd.print(" min    ");

    // Jika waktu telah habis
    if (elapsedTime >= timerDuration * 60000) {
      timerRunning = false;
      lcd.setCursor(8, 1);
      lcd.print(" SELESAI ");
      beep(5);
    }

    if (tempC <= setMin) {
      digitalWrite(buzzer, HIGH);
      delay (1500);
      digitalWrite(buzzer, LOW);
      delay(1500);
      return;
    } else if (tempC >= setMax) {
      digitalWrite(relay1, LOW);
    } else {
      digitalWrite(relay1, HIGH);
      digitalWrite(buzzer, LOW);
    }
  }
  //======================================================
}
//*************************************************************************//
//                          Send Data Function                             //
//*************************************************************************//
void sendData() {
  //======================================================
  sensor.requestTemperatures();
  tempC = sensor.getTempCByIndex(0);
  //======================================================
  if (millis() - lastMillis > 1000) {
    WiFiClient client;
    HTTPClient tempHttp;
    const int port = 80;
    if (!client.connect(IP_HOST, port)) {
      Serial.print(F("Not connect to server\n"));
      delay(1000);
      return;
    }
    //======================================================
    if (tempC != DEVICE_DISCONNECTED_C) {
      lcd.setCursor(8, 0);
      lcd.print(String(tempC, 1) + " 'C");
      String temperature = String(tempC, 2);
      String temperature_data;
      temperature_data = "api_key=" + PROJECT_API_KEY;
      temperature_data += "&temperature=" + temperature;

      String tempUrl = "http://" + String(IP_HOST) + "/pengukusankerupuk/sensorData.php";
      tempHttp.begin(client, tempUrl);
      tempHttp.addHeader("Content-Type", "application/x-www-form-urlencoded");
      int httpResponseCode = tempHttp.POST(temperature_data);
      String payload;
      payload = tempHttp.getString();
      payload.trim();
      if (payload.length() > 0) {
        Serial.println(payload + "\n");
      }
      tempHttp.end();
      Serial.print(F("Nilai Suhu => "));
      Serial.print(tempC);
      Serial.print(F("\n"));
    }
    //======================================================
    String minUrl;
    HTTPClient httpMin;
    minUrl = "http://" + String(IP_HOST) + "/pengukusankerupuk/getMin.php";
    httpMin.begin(client, minUrl);
    httpMin.GET();
    String valueMin = httpMin.getString();
    setMin = valueMin.toInt();
    Serial.print(F("Nilai minimum  => "));
    Serial.print(setMin);
    Serial.print(F("\n"));
    httpMin.end();
    //======================================================
    String maxUrl;
    HTTPClient httpMax;
    maxUrl = "http://" + String(IP_HOST) + "/pengukusankerupuk/getMax.php";
    httpMax.begin(client, maxUrl);
    httpMax.GET();
    String valueMax = httpMax.getString();
    setMax = valueMax.toInt();
    Serial.print(F("Nilai maksimum => "));
    Serial.print(setMax);
    Serial.print(F("\n"));
    Serial.print(F("\n"));
    httpMax.end();
    //======================================================
    lastMillis = millis();
    //======================================================
  }
}
//*************************************************************************//

//*************************************************************************//
//                            Beep Function                                //
//*************************************************************************//
void beep(uint8_t n) {
  while (n > 0) {
    digitalWrite(buzzer, HIGH);
    delay(100);
    digitalWrite(buzzer, LOW);
    delay(100);
    n--;
  }
}
//*************************************************************************//

//*************************************************************************//
//                        Print Welcome Function                           //
//*************************************************************************//
void printWelcome() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("   MONITORING   ");
  lcd.setCursor(0, 1);
  lcd.print("  PENGUKUSAN KERUPUK  ");
  delay(1500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("    KONTROL    ");
  lcd.setCursor(0, 1);
  lcd.print(" SUHU OTOMATIS ");
  delay(1500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("   By :    ");
  lcd.setCursor(0, 1);
  lcd.print(" DHANI SETIAWAN ");
  delay(1500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting Wifi.");
  lcd.setCursor(0, 1);
  delay(1000);
  lcd.print("     ......     ");
  delay(1500);
}
//*************************************************************************//