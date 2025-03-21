// Thêm thư viện
#include <ESP8266WiFi.h>
#include "DHT.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <LittleFS.h>
#include <SD.h>

// Kích thước màn hình OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Địa chỉ I2C của màn hình OLED
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pin kết nối DHT
#define DHTPIN 12
#define DHTTYPE DHT11

// Thiết lập cảm biến DHT
DHT dht(DHTPIN, DHTTYPE);

// Thông số WiFi
const char *ssid = "Nguyen Phong";
const char *password = "29052010";

// Tạo server
WiFiServer server(80);

// Khai bao bien

int value = 0;

void setup()
{
  // Khởi động Serial
  Serial.begin(9600);
  delay(10);

  // Khởi tạo DHT
  dht.begin();

  // Doc giac trị MQ-2
  int value = analogRead(A0);


  // Khởi động OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  { // Địa chỉ I2C của màn hình là 0x3C
    Serial.println(F("Không tìm thấy màn hình OLED!"));
    for (;;)
      ;
  }
  display.display();
  delay(2000);
  display.clearDisplay();

  // Kết nối tới WiFi
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

  // Hiển thị địa chỉ IP trên OLED
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
  // Đọc độ ẩm và nhiệt độ từ cảm biến DHT
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Kiểm tra xem giá trị có đọc thành công không
  if (isnan(h) || isnan(t))
  {
    Serial.println("Không đọc được dữ liệu từ cảm biến DHT11!");
    return;
  }

  // Hiển thị dữ liệu lên màn hình OLED
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

  // Chờ một khoảng thời gian trước khi cập nhật dữ liệu
  delay(2000);

  // Kiểm tra khi có client kết nối
  WiFiClient client = server.available();
  if (!client)
  {
    return;
  }

  // Đợi client gửi yêu cầu
  while (!client.available())
  {
    delay(1);
  }

  // Đọc dòng đầu tiên của yêu cầu
  String req = client.readStringUntil('\r');
  client.flush();

  // Tạo trang HTML hiển thị dữ liệu
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
  s += int(value);
  s += "</html>";


  // Gửi phản hồi tới client
  client.print(s);
  delay(1);
  Serial.println("Client đã ngắt kết nối");
}
