#include <WiFi.h>
#include "routes.h"

#include <SPI.h>
#include <TFT_eSPI.h>

#include <Arduino_JSON.h>


const int BUTTON_UP_PIN = 33;
const int BUTTON_DOWN_PIN = 21;
const int BUTTON_SELECT_PIN = 32;
const int BUTTON_BACK_PIN = 35;

const int POT_PIN = 34;

const int RX_PIN = 16;
const int TX_PIN = 17;

int buttonStateSelect = 0;
int buttonStateUp = 0;
int buttonStateDown = 0;
int buttonStateBack = 0;

int potVolume;

int cursorSelector = 1;
int selectPage;

int previous_buttonUp = HIGH;
int previous_buttonDown = HIGH;
int previous_buttonSelect = LOW;
int previous_buttonBack = LOW;

long time_ = 0;
long debounce = 200;

char* ssid =     "NS_Business";
char* password = "Hello112?";

Routes route;
TFT_eSPI tft = TFT_eSPI();

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
  Serial.println(Serial2.readString());

  pinMode(BUTTON_UP_PIN, INPUT_PULLUP);
  pinMode(BUTTON_BACK_PIN, INPUT_PULLUP);
  pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP);
  pinMode(BUTTON_SELECT_PIN, INPUT_PULLUP);
  
  pinMode(POT_PIN, INPUT_PULLUP);

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);

  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) delay(1500);
  Serial.print("Connected To:");
  Serial.println(ssid);
}

void drawLine(int x1, int y1, int x2, int y2, uint32_t color) {
  tft.drawLine(x1, y1, x2, y2, color);
}

void displayMenuItem(int x, int y, int textSize, String label) {
  tft.setCursor(x, y);
  tft.setTextSize(textSize);
  tft.println(label);
}

void handleMainMenu() {
  tft.init();
  tft.fillScreen(TFT_BLACK);
  displayMenuItem(10, 10, 2, "MENU");
  drawLine(0, 30, 350, 30, TFT_WHITE);
  
  get_volume();
  
  if (cursorSelector == 1) {
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(3);
    tft.setCursor(90, 50);
    tft.println("DISCOVER");
  
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(5);
    tft.setCursor(100, 90);
    tft.println("HOME");
  
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(3);
    tft.setCursor(75, 150);
    tft.println("FAVOURITES");
  
    if (buttonStateSelect == LOW && previous_buttonSelect == LOW && millis() - time_ > debounce) {
      selectPage = 1;
      time_ = millis(); 
    }
  }

  if (cursorSelector == 0) {
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(3);
    tft.setCursor(120, 50);
    tft.println("HOME");
  
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(4);
    tft.setCursor(55, 90);
    tft.println("FAVOURITES");

    if (buttonStateSelect == LOW && previous_buttonSelect == LOW && millis() - time_ > debounce) {
      selectPage = 2;
      time_ = millis();
    }
  }

  if (cursorSelector == 2) {
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(5);
    tft.setCursor(60, 90);
    tft.println("DISCOVER");
  
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(3);
    tft.setCursor(120, 150);
    tft.println("HOME");

    if (buttonStateSelect == LOW && previous_buttonSelect == LOW && millis() - time_ > debounce) {
      selectPage = 3;
      time_ = millis();
    }
  }
  
  
  drawLine(0, 190, 350, 190, TFT_WHITE);

  String songInfo = getSongInfo();
  if (!songInfo.isEmpty()) {
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(1);
    tft.setCursor(10, 210);
    tft.println(songInfo);
  }
}

void handleStationPage(String title, String station1, String station2, String station3) {  
  tft.setCursor(10, 10);
  tft.println(title);
  drawLine(0, 30, 350, 30, TFT_WHITE); 

  displayMenuItem(90, 50, 3, station1);
  displayMenuItem(60, 90, 4, station2);
  displayMenuItem(80, 150, 3, station3);

  drawLine(0, 190, 350, 190, TFT_WHITE);
  
  if (buttonStateBack == HIGH && previous_buttonBack == LOW && millis() - time_ > debounce) {
    selectPage = 0;
    Serial.println("BACK BUTTON IS INGEDRUKT");
    time_ = millis();
  }
  previous_buttonBack = buttonStateBack;
};

void handleHomePage() {
  // tft.init();
  tft.fillScreen(TFT_BLACK);
  handleStationPage("HOME", "3FM", "SLAM", "Sky Radio");
  String songInfo = getSongInfo();
  if (!songInfo.isEmpty()) {
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(1);
    tft.setCursor(10, 210);
    tft.println(songInfo);
  }
}

void handleFavouritesPage() {
  tft.fillScreen(TFT_BLACK);
  Serial.println(162);

  String radioId = route.PutRadioInfo(162);
  // handleStationPage("FAVOURITES", "3FM", "SLAM", "Sky Radio");

  // JSONVar favoritesArr = JSON.parse(route.GetFavorites(1));  
  // Get's 3 radio stations from the array 
  // int i = 0;
  
  // if (buttonStateUp == LOW && previous_buttonUp == LOW && millis() - time_ > debounce) {
  //   i += 1;
  // }
  // if (buttonStateDown == LOW && previous_buttonDown == LOW && millis() - time_ > debounce) {
  //   i -= 1;
  // }

  // CHECK EFFE HOE DIT GOED MOET MET DEZE LIBRARY https://github.com/arduino-libraries/Arduino_JSON/blob/master/
  // Serial.print(favoritesArr);
  // String station1 = favoritesArr[i]["name"];
  // String station2 = favoritesArr[i+1]["name"];
  // String station3 = favoritesArr[i+2]["name"];
  // handleStationPage("FAVOURITES", station1, station2, station3);

  handleStationPage("FAVOURITES", "SLAM", "Groningen1", "3FM");

  // String songInfo = getSongInfo();
  // if (!songInfo.isEmpty()) {
  //   tft.setTextColor(TFT_WHITE);
  //   tft.setTextSize(1);
  //   tft.setCursor(10, 210);
  //   tft.println(songInfo);
  // }
}

void handleDiscoverPage() {
  tft.fillScreen(TFT_BLACK);
  handleStationPage("DISCOVER", "3FM", "SLAM", "Sky Radio");
  String songInfo = getSongInfo();
  if (!songInfo.isEmpty()) {
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(1);
    tft.setCursor(10, 210);
    tft.println(songInfo);
  }
}

String getSongInfo() {
  // String songInfo = Serial2.readString();
  // Serial.println(songInfo);
  String songInfo = "STATION: SLAM!NONSTOP";
  if (songInfo.indexOf("STATION") != -1) {
    songInfo.replace(" ", "");
    songInfo.replace("STATION:", "");
    // Serial.println(songInfo);
    return songInfo;
  }
  return "";
}

void print_song() {
  String songInfo = getSongInfo();
  if (!songInfo.isEmpty()) {
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(1);
    tft.setCursor(10, 210);
    tft.println(songInfo);
  }
}

void get_volume() {
  potVolume = analogRead(POT_PIN);
  potVolume = map(potVolume, 0, 1023, 0, 100);
  potVolume = potVolume / 4;
  char charBuffer[4];
  itoa(potVolume, charBuffer, 10);

  displayMenuItem(270, 10, 2, charBuffer);
  Serial2.write(charBuffer);
}

void loop() {  
  tft.init();
  buttonStateUp = digitalRead(BUTTON_UP_PIN);
  buttonStateDown = digitalRead(BUTTON_DOWN_PIN);
  buttonStateSelect = digitalRead(BUTTON_SELECT_PIN);
  buttonStateBack = digitalRead(BUTTON_BACK_PIN);
  
  print_song();
  get_volume();

  if (buttonStateUp == LOW && previous_buttonUp == LOW && millis() - time_ > debounce) {
    ++cursorSelector;
    time_ = millis(); 
  }
  previous_buttonUp = buttonStateUp;

  if (buttonStateDown == LOW && previous_buttonDown == LOW && millis() - time_ > debounce) {
    --cursorSelector;
    time_ = millis(); 
  }
  previous_buttonDown = buttonStateDown;

  switch (selectPage) {
    case 1:
      handleHomePage();
      break;
    case 2:
      handleFavouritesPage();
      break;
    case 3:
      handleDiscoverPage();
      break;
    default:
      handleMainMenu();
  }
}