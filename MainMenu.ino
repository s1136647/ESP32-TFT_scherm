#include <WiFi.h>
#include "routes.h"

#include <SPI.h>
#include <TFT_eSPI.h>

#include <Arduino_JSON.h>


const int BUTTON_UP_PIN = 33;
const int BUTTON_DOWN_PIN = 21;
const int BUTTON_SELECT_PIN = 32;
const int BUTTON_BACK_PIN = 5;

const int POT_PIN = 34;

int buttonStateSelect = 0;
int buttonStateUp = 0;
int buttonStateDown = 0;
int buttonStateBack = 0;

int potVolume;

int cursorSelector = 1;
int selectPage;
int volumeDraaiknop = 1;

int homeIndex = 0;
int favouritesIndex = 0;

JSONVar radioArr;
JSONVar favoritesArr;
JSONVar name;
JSONVar nummerName;

int previous_buttonUp = HIGH;
int previous_buttonDown = HIGH;
int previous_buttonSelect = LOW;
int previous_buttonBack = HIGH;

long time_ = 0;
long debounce = 200;

char* ssid =     "NS_Business";
char* password = "Hello112?";

Routes route;
TFT_eSPI tft = TFT_eSPI();

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_UP_PIN, INPUT_PULLUP);
  pinMode(BUTTON_BACK_PIN, INPUT_PULLUP);
  pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP);
  pinMode(BUTTON_SELECT_PIN, INPUT_PULLUP);
  
  pinMode(POT_PIN, INPUT_PULLUP);

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  // tft.setTextColor(0x030303);
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE);

  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) delay(1500);
  Serial.print("Connected To:");
  Serial.println(ssid);
  radioArr = JSON.parse(route.AllStations());
  favoritesArr = JSON.parse(route.GetFavorites(1));   
}

void drawLine(int x1, int y1, int x2, int y2, uint32_t color) {
  tft.drawLine(x1, y1, x2, y2, color);
}

void displayMenuItem(int x, int y, int textSize, String label) {
  tft.setTextColor(TFT_WHITE);
  tft.drawString(String(label), x, y, textSize);
}

void handleMainMenu() {
  displayMenuItem(10, 10, 1, "MENU");
  tft.drawLine(0, 30, 350, 30, TFT_WHITE);
  
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
      tft.fillScreen(TFT_BLACK);
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

    tft.setTextSize(3);
    if (buttonStateSelect == LOW && previous_buttonSelect == LOW && millis() - time_ > debounce) {
      selectPage = 2;
      tft.fillScreen(TFT_BLACK);
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
      tft.fillScreen(TFT_BLACK);
      time_ = millis();
    }
  }
  
  
  tft.drawLine(0, 190, 350, 190, TFT_WHITE);

  // String songInfo = getSongInfo();
  // displayMenuItem(10, 210,1, songInfo);
}

void handleStationPage(String title, String station1, String station2, String station3) {  
  tft.setCursor(10, 10);
  tft.println(title);
  drawLine(0, 30, 350, 30, TFT_WHITE); 

  get_volume();  
  
  displayMenuItem(40, 50, 1, station1);
  displayMenuItem(40, 90, 1, station2);
  displayMenuItem(40, 150, 1, station3);

  tft.drawLine(0, 190, 350, 190, TFT_WHITE);
  
  if (buttonStateBack == LOW && previous_buttonBack == LOW && millis() - time_ > debounce) {
    selectPage = 0;  
    tft.fillScreen(TFT_BLACK);
    time_ = millis();
  }
  previous_buttonBack = buttonStateBack;
};

void handleHomePage() {

  if (buttonStateUp == LOW && previous_buttonUp == LOW && millis() - time_ > debounce) {
    homeIndex++;
    tft.fillScreen(TFT_BLACK);
  }
  previous_buttonUp = buttonStateUp;

  if (buttonStateDown == LOW && previous_buttonDown == LOW && millis() - time_ > debounce) {
    homeIndex--;
    tft.fillScreen(TFT_BLACK);
  }
  previous_buttonDown = buttonStateDown;

  if (buttonStateSelect == LOW && previous_buttonSelect == LOW && millis() - time_ > debounce) {
    route.Reset();
    String radioId = route.PutRadioInfo(radioArr[homeIndex + 1]["id"]);
    Serial.println("Stuur id");
    time_ = millis();
    tft.fillScreen(TFT_BLACK);
  }
  Serial.println(radioArr[homeIndex]["id"]);
  
  tft.setTextColor(TFT_WHITE);
  String station1 = radioArr[homeIndex]["name"];
  String station2 = radioArr[homeIndex+1]["name"];
  String station3 = radioArr[homeIndex+2]["name"];
  handleStationPage("HOME", station1, station2, station3);

  

  // String songInfo = getSongInfo();
  // displayMenuItem(10, 210,1, songInfo);
}

void handleFavouritesPage() {
  tft.drawLine(0, 30, 350, 30, TFT_WHITE);

  get_volume();  
  
  if (buttonStateUp == LOW && previous_buttonUp == LOW && millis() - time_ > debounce) {
    favouritesIndex++;
    tft.fillScreen(TFT_BLACK);
  }
  previous_buttonUp = buttonStateUp;

  if (buttonStateDown == LOW && previous_buttonDown == LOW && millis() - time_ > debounce) {
    favouritesIndex--;
    tft.fillScreen(TFT_BLACK);
  }
  previous_buttonDown = buttonStateDown;

  if (buttonStateSelect == LOW && previous_buttonSelect == LOW && millis() - time_ > debounce) {
    route.Reset();
    String radioId = route.PutRadioInfo(favoritesArr[favouritesIndex]["id"]);
    Serial.println("Stuur id");
    time_ = millis();
    tft.fillScreen(TFT_BLACK);
  }

  String station1 = favoritesArr[favouritesIndex]["name"];
  String station2 = favoritesArr[favouritesIndex+1]["name"];
  String station3 = favoritesArr[favouritesIndex+2]["name"];
  handleStationPage("FAVOURITES", station1, station2, station3);

  // String songInfo = getSongInfo();
  // displayMenuItem(10, 210,1, songInfo);
}

void handleDiscoverPage() {
  displayMenuItem(10, 10, 1, "DISCOVER");
  tft.drawLine(0, 30, 350, 30, TFT_WHITE);

  get_volume();  

  // String station;
  String station = name["name"];

  if (buttonStateSelect == LOW && previous_buttonSelect == LOW && millis() - time_ > debounce) {
    route.Reset();
    name = JSON.parse(route.GetRandomRadio());
    Serial.println(station);
    String radioId = route.PutRadioInfo(name["id"]);
    Serial.println("Stuur id");
    time_ = millis();
    tft.fillScreen(TFT_BLACK);
  }

  tft.drawLine(0, 190, 350, 190, TFT_WHITE);
  
  // String songInfo = getSongInfo();
  // displayMenuItem(10, 210, 1, songInfo);

  if (buttonStateBack == LOW && previous_buttonBack == LOW && millis() - time_ > debounce) {
    selectPage = 0;    
    tft.fillScreen(TFT_BLACK);
    time_ = millis();
  }
  previous_buttonBack = buttonStateBack;
  tft.drawString(station, 30, 90, 1);
}

// String getSongInfo() {
//   String songInfo = nummerName["now_playing"];
//   nummerName = JSON.parse(route.GetPlayingStation());
//   Serial.println(songInfo);
//   return songInfo;
// }



void get_volume() {
  potVolume = analogRead(POT_PIN);
  potVolume = map(potVolume, 0, 1023, 0, 100);
  potVolume = potVolume / 4;
  potVolume = potVolume / 10;
  if (potVolume != volumeDraaiknop) {
    tft.fillScreen(TFT_BLACK);
    selectPage = selectPage;
    volumeDraaiknop = potVolume;    
  }
  char charBuffer[4];
  itoa(potVolume, charBuffer, 10);
  
  if (buttonStateSelect == LOW && previous_buttonSelect == LOW && millis() - time_ > debounce) {
    route.UpdateVolume(1, potVolume);
    Serial.println("VOLUME GESTUURD");
    Serial.println(potVolume);
  }
  Serial.println(potVolume);

  displayMenuItem(270, 10, 1, charBuffer);
}

void loop() {  
  tft.init();
  buttonStateUp = digitalRead(BUTTON_UP_PIN);
  buttonStateDown = digitalRead(BUTTON_DOWN_PIN);
  buttonStateSelect = digitalRead(BUTTON_SELECT_PIN);
  buttonStateBack = digitalRead(BUTTON_BACK_PIN);

  Serial.println(buttonStateBack);

  if (buttonStateUp == LOW && previous_buttonUp == LOW && millis() - time_ > debounce) {
    ++cursorSelector;
    time_ = millis(); 
    tft.fillScreen(TFT_BLACK);
  }
  previous_buttonUp = buttonStateUp;

  if (buttonStateDown == LOW && previous_buttonDown == LOW && millis() - time_ > debounce) {
    --cursorSelector;
    time_ = millis(); 
    tft.fillScreen(TFT_BLACK);
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