#include <WiFi.h>
#include <ezTime.h>
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <Adafruit_NeoPixel.h>

#define LCD_ADDRESS 0x27
#define LCD_COLUMNS 20
#define LCD_ROWS 4
#define LED_PIN 5
#define LED_COUNT 39

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS);

const char* ssid = "moaiwlan";
const char* password = "Ossi1Paavo234";

String apiKey = "07689267308b7cfbbad02f8d175c27c9";
String city = "Helsinki";
String weatherURL = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "&appid=" + apiKey + "&units=metric";

Timezone Helsinki;

float currentTemp = 0.0;
String weatherDesc = "";

// ---------------------------
// WEATHER FETCH
// ---------------------------
void getWeather() {
  HTTPClient http;
  http.begin(weatherURL);
  int httpCode = http.GET();

  if (httpCode > 0) {
    String payload = http.getString();

    DynamicJsonDocument doc(2048);
    deserializeJson(doc, payload);

    currentTemp = doc["main"]["temp"];
    weatherDesc = doc["weather"][0]["description"].as<String>();
  }

  http.end();
}

// ---------------------------
// FUNNY REMINDERS
// ---------------------------
String getReminder(float temp, String desc) {
  desc.toLowerCase();

  if (desc.indexOf("heavy") >= 0 && desc.indexOf("rain") >= 0) return "You WILL get wet.";
  if (desc.indexOf("rain") >= 0) return "Take your umbrella!";
  if (desc.indexOf("snow") >= 0) return "Winter is coming!";
  if (desc.indexOf("thunder") >= 0) return "Boom boom sky time.";
  if (desc.indexOf("fog") >= 0) return "Visibility? none.";
  if (desc.indexOf("wind") >= 0) return "Hold onto your snacks.";
  if (desc.indexOf("scattered") >= 0) return "Sky mood: confused.";
  if (desc.indexOf("storm") >= 0) return "Angry sky today.";
  if (temp < 0) return "Dont lick metal.";
  if (temp > 25) return "Drink your water!";
  if (desc.indexOf("cloud") >= 0) return "Lonkerotaivas.";
  if (desc.indexOf("clear sky") >= 0) return "Blue sky unlocked ";
  if (desc.indexOf("mist") >= 0) return "Visibility: NONE";

  return "Good luck, legend.";
}

// ---------------------------
// LED GRADIENT COLORS
// ---------------------------
void getGradientColors(float temp, uint32_t &colorA, uint32_t &colorB) {
  if (temp < 0) {
    colorA = strip.Color(0, 20, 153);
    colorB = strip.Color(102, 100, 255);
  }
  else if (temp < 10) {
    colorA = strip.Color(128, 128, 128);
    colorB = strip.Color(255, 255, 5);
  }
  else if (temp < 20) {
    colorA = strip.Color(255, 51, 100);
    colorB = strip.Color(51, 255, 50);
  }
  else if (temp < 25) {
    colorA = strip.Color(255, 10, 0);
    colorB = strip.Color(255, 127, 0);
  }
  else {
    colorA = strip.Color(255, 0, 0);
    colorB = strip.Color(255, 50, 0);
  }
}

// ---------------------------
// LED GRADIENT ANIMATION
// ---------------------------
void animateTempGradient(float temp) {
  static int offset = 0;
  offset++;
  
  uint32_t colorA, colorB;
  getGradientColors(temp, colorA, colorB);

  uint8_t aR = (colorA >> 16) & 0xFF;
  uint8_t aG = (colorA >> 8) & 0xFF;
  uint8_t aB = colorA & 0xFF;

  uint8_t bR = (colorB >> 16) & 0xFF;
  uint8_t bG = (colorB >> 8) & 0xFF;
  uint8_t bB = colorB & 0xFF;

  for (int i = 0; i < LED_COUNT; i++) {
    float wave = (sin((i + offset) * 0.05) + 1.0) / 2.0;

    uint8_t r = aR + (bR - aR) * wave;
    uint8_t g = aG + (bG - aG) * wave;
    uint8_t b = aB + (bB - aB) * wave;

    strip.setPixelColor(i, r, g, b);
  }

  strip.show();
}

// ---------------------------
// SCROLLING TEXT
// ---------------------------
unsigned long lastScroll = 0;
int scrollIndex = 0;
bool scrollForward = true;
String scrollTextBuffer = "";

void updateScrollingText(int row, int speed) {
  if (scrollTextBuffer.length() <= 20) {
    String line = scrollTextBuffer;
    while (line.length() < 20) line += " ";
    lcd.setCursor(0, row);
    lcd.print(line);
    return;
  }

  if (millis() - lastScroll >= speed) {
    lastScroll = millis();

    int maxIndex = scrollTextBuffer.length() - 20;

    lcd.setCursor(0, row);
    lcd.print(scrollTextBuffer.substring(scrollIndex, scrollIndex + 20));

    if (scrollForward) {
      scrollIndex++;
      if (scrollIndex >= maxIndex) scrollForward = false;
    } else {
      scrollIndex--;
      if (scrollIndex <= 0) scrollForward = true;
    }
  }
}

// ---------------------------
// SETUP
// ---------------------------
void setup() {
  strip.begin();
  strip.show();
  lcd.init();
  lcd.backlight();
  lcd.clear();

  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(200);

  waitForSync();
  Helsinki.setLocation("Europe/Helsinki");

  getWeather();

  // --- DRAW LCD IMMEDIATELY AT BOOT ---
  String timeStr = Helsinki.dateTime("H:i:s");
  while (timeStr.length() < 20) timeStr += " ";
  lcd.setCursor(0, 0);
  lcd.print(timeStr);

  String tempStr = String(currentTemp) + " C";
  while (tempStr.length() < 20) tempStr += " ";
  lcd.setCursor(0, 1);
  lcd.print(tempStr);

  String descStr = weatherDesc;
  while (descStr.length() < 20) descStr += " ";
  lcd.setCursor(0, 2);
  lcd.print(descStr);

  scrollTextBuffer = getReminder(currentTemp, weatherDesc);
  String bootScroll = scrollTextBuffer.substring(0, (scrollTextBuffer.length() < 20 ? scrollTextBuffer.length() : 20));
  while (bootScroll.length() < 20) bootScroll += " ";
  lcd.setCursor(0, 3);
  lcd.print(bootScroll);
}

// ---------------------------
// MAIN LOOP
// ---------------------------
unsigned long lastWeather = 0;
unsigned long lastClock = 0;

void loop() {
  events();

  // Päivitä sää 1 minuutin välein
  if (millis() - lastWeather > 60000) {
    lastWeather = millis();
    getWeather();

    String tempStr = String(currentTemp) + " C";
    while (tempStr.length() < 20) tempStr += " ";
    lcd.setCursor(0, 1);
    lcd.print(tempStr);

    String descStr = weatherDesc;
    while (descStr.length() < 20) descStr += " ";
    lcd.setCursor(0, 2);
    lcd.print(descStr);
  }

  // Päivitä kellonaika 1 sekunnin välein (ei vilkkumista)
  if (millis() - lastClock > 1000) {
    lastClock = millis();

    String timeStr = Helsinki.dateTime("H:i:s");
    while (timeStr.length() < 20) timeStr += " ";

    lcd.setCursor(0, 0);
    lcd.print(timeStr);
  }

  // Scroll-teksti
  String newScroll = getReminder(currentTemp, weatherDesc);
  if (newScroll != scrollTextBuffer) {
    scrollIndex = 0;
    scrollForward = true;
  }
  scrollTextBuffer = newScroll;
  updateScrollingText(3, 200);

  // LED-animaatio
  animateTempGradient(currentTemp);
}
