#include <ESP8266WiFi.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <LittleFS.h>
#include <SD.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define DHTPIN 12
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

const char *ssid = "SSID";
const char *password = "PASSWORD";

WiFiServer server(80);
int gasValue = 0;

void setup()
{
  Serial.begin(9600);
  delay(10);
  dht.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  { 
    //I2C Oled 0x3C
    Serial.println(F("Không tìm thấy màn hình OLED!"));
    for (;;)
      ;
  }
  display.display();
  delay(2000);
  display.clearDisplay();

  Serial.println();
  Serial.println();
  Serial.print("Kết nối tới mạng ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Đã kết nối WiFi");

  // Khởi động server
  server.begin();
  Serial.println("Khởi động Server");

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("WiFi connected");
  display.println(WiFi.localIP());
  Serial.println(WiFi.localIP());
  display.display();
  delay(2000);
}

void loop()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  int gasValue = analogRead(A0); 
  Serial.println(gasValue);
  if (isnan(h) || isnan(t))
  {
    Serial.println("Không đọc được dữ liệu từ cảm biến DHT11!");
    return;
  }
  if(isnan(gasValue)
  {
    Serial.println("Không đọc được dữ liệu từ cảm biến MQ-2")
  }
    
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(26, 0);
  display.println("Control");
  display.setTextSize(1);
  display.setCursor(26, 20);
  display.print("Temp: ");
  display.print(t);
  display.println(" C");
  display.setCursor(26, 30);
  display.print("Hum:  ");
  display.print(h);
  display.println(" %");
  display.setCursor(26, 40);
  display.print("Gas:  ");
  display.print(value);
  display.display();
  delay(2000);

  WiFiClient client = server.available();
  if (!client)
  {
    return;
  }

  while (!client.available())
  {
    delay(1);
  }
  String req = client.readStringUntil('\r');
  client.flush();

  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
  s += "<!DOCTYPE HTML>";
  s += "<html>";
  s += "<h1>Control</h1>";
  s += "<p>Temperature: ";
  s += String(t);
  s += " &#8451;</p>";
  s += "<p>Humidity: ";
  s += String(h);
  s += " %</p>";
  s += "</html>";
  s += "<p>Gas: ";
  s += int(gasValue);
  s += "</html>";

  client.print(s);
  delay(1);
  Serial.println("Client đã ngắt kết nối");
}
