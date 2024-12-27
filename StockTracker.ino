#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <stdlib.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char* ssid = "Universal LAN";
const char* password = "Connecto Patronum";

const char* apiKey = "7f17d44856dda6ede4f17113696ac107";  // Replace with your MarketStack API key

const char *stocks[] = {"MAHINDRA","JIOFIN","RAYLFSTYL"};
const char *tick[] = {"M%26M.XNSE","JIOFIN.XNSE","RAYMONDLSL.XNSE"};
const char* stocks1[] = {"PFIZER", "ULTRATECH", "RELIANCE", "ADI-BIRLA", "GRASIM", "BOM-DYEING", "RAYMOND", "LICI", "TATA STEEL", "MAHINDRA", "JIOFIN", "RAYLFSTYL"};
const char* ticks1[] = {"PFIZER.BSE", "ULTRACEMCO.BSE", "RELIANCE.BSE", "ABCAPITAL.BSE", "GRASIM.BSE", "BOMDYEING.BSE", "RAYMOND.BSE", "LICI.BSE", "TATASTEEL.BSE","M%26M.XNSE","JIOFIN.XNSE","RAYMONDLSL.XNSE"};
int units[] = {50,15,144,133,95,60,28,15,335,48,72,22};
float trprices[]={0,0,0,0,0,0,0,0,0,0,0,0};

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

void fetchStockPrice1(int x, int y, int i) {
  String stockSymbol = ticks1[i];
  String httpRequestAddress = "https://www.alphavantage.co/query?function=GLOBAL_QUOTE&symbol=" + stockSymbol + "&apikey=J8IQQ0AGFK2BVHHC";
  HTTPClient http;

  http.begin(httpRequestAddress);
  int httpCode = http.GET();

  if (httpCode > 0) {
    String payload = http.getString();

    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, payload);

    if (error) {
      resetDisplay();
      display.setTextSize(1);
      display.println("JSON Parsing Failed");
      display.display();
      http.end();
      return;
    }

    if (!doc["Global Quote"].isNull()) {
      JsonObject quote = doc["Global Quote"];
      float price = quote["05. price"].as<float>();
      trprices[i] = price;

      const char* changePercent = quote["10. change percent"]; // Change percent

      resetDisplay();
      display.setTextSize(2);
      display.setCursor(x, y);
      display.println(stocks1[i]);

      // Display stock price and percentage change
      display.setTextSize(1);
      display.setCursor(x, y + 16);
      display.print("Price: ");
      display.println(price);
      display.setCursor(x, y + 32);
      display.print("Change: ");
      display.println(changePercent);
    } else {
      resetDisplay();
      display.setTextSize(1);
      display.println("No Data Found");
    }
  } else {
    resetDisplay();
    display.setTextSize(1);
    display.println("HTTP Error: ");
    display.println(httpCode);
  }

  display.display();
  http.end();
}

void fetchStockPrice(int i) {
  String stockSymbol = ticks1[i];
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
      trprices[i] = float(price);

      resetDisplay();
      display.setTextSize(2);
      display.println(stocks1[i]);
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

void displaySumOnScreen(int k) {
  float sum;
  int i=0;
  for(i=0;i<k;i++){
    sum+=(units[i] * trprices[i]);
  }
  resetDisplay(); // Clear the display and set cursor to top-left
  display.setTextSize(2); // Set larger text size for better visibility
  display.setCursor(0, 0); // Position cursor
  display.println("Total = "); // Title for the sum
  display.setCursor(0, 20); // Move cursor below the title
  display.print(sum); // Display the calculated sum
  display.println(" INR"); // Append currency units
  display.display(); // Push changes to the display
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
  float sum=0;
  int i=0;
  while(i<=12){
    if(i<9){
      fetchStockPrice1(0, 0, i);
      delay(10000);
      i++;
    }
    else if(i>8 && i<12){
      fetchStockPrice(i);
      delay(10000);
      i++;
    }
    else{
      displaySumOnScreen(12);
      delay(20000);
    }
  }
}

