#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char* ssid = "Universal LAN";
const char* password = "Connecto Patronum";
const char* apiKey = "43dedfe1de07b2012cb0b60cec9f41b7";  // Replace with your MarketStack API key

const char *stocks[] = {"MAHINDRA","JIOFIN","RAY-LIFSTYL"};
const char *tick[] = {"M&M.XBOM","JIOFIN.XNSE","RAYMONDLSL.XNSE"};

void connectWiFi() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Connecting to WiFi...");
  display.display();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Connected to WiFi");
  display.display();
  delay(2000);
}

void resetDisplay() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
}

void fetchStockPrice(int i) {
  String stockSymbol = tick[i];
  String url = "http://api.marketstack.com/v1/eod/latest?access_key=" + String(apiKey) + "&symbols=" + stockSymbol;

  HTTPClient http;
  http.begin(url);
  int httpCode = http.GET();

  if (httpCode > 0) {
    String payload = http.getString();

    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, payload);

    if (error) {
      resetDisplay();
      display.setTextSize(1);
      display.println("JSON Parsing Failed");
      display.display();
      http.end();
      return;
    }

    if (doc.containsKey("data") && doc["data"].size() > 0) {
      float price = doc["data"][0]["close"].as<float>();

      resetDisplay();
      display.setTextSize(2);
      display.println(stocks[i]);
      display.setTextSize(1);
      display.setCursor(0, 16);
      display.print("Price: ");
      display.print(price, 2);
      display.println(" INR");
    } else {
      resetDisplay();
      display.setTextSize(1);
      display.println("No data available");
    }
  } else {
    resetDisplay();
    display.setTextSize(1);
    display.println("HTTP Error:");
    display.println(httpCode);
  }

  display.display();
  http.end();
}

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Stock Prices Tracker");
  display.display();

  connectWiFi();
}

void loop() {
  int i=0;
  for(i=0;i<3;i++){
    fetchStockPrice(i);
    delay(5000);
  }
}
