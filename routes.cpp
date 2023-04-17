#include <HTTPClient.h>
#include <Arduino.h>
#include <WiFi.h>
#include "routes.h"
#include <Arduino_JSON.h>


String Routes::request(String url) {
      
      http.begin(url);
      int httpCode = http.GET();
      if (httpCode > 0) {
        if (httpCode == HTTP_CODE_OK) {
          
          return http.getString();
        }
      }
      return "Error" + String(httpCode) ;
}

  // Retrieve all radio stations
    String Routes::AllStations() {
      String payload;
      payload = request(baseURL + "stations");
      JSONVar getRadioInfo = JSON.parse(payload);
      String radioArr = JSON.stringify(getRadioInfo["data"]);
      return radioArr;
    }

     // Retrieve data of a station
    String Routes::GetRandomRadio() {      
        String payload;
        payload = request(baseURL + "stations/discover");
        JSONVar output = JSON.parse(payload);
        String name = JSON.stringify(output["data"]);        
        return name;
    }

    // Get all favorites
    String Routes::GetFavorites(const int user_id) {      
      String payload;
      payload = request(baseURL + "users/" + user_id + "/favourites");
      JSONVar parsedFavorites= JSON.parse(payload);
      String favoritesArr = JSON.stringify(parsedFavorites["data"]);
      return favoritesArr;     
    }

    String Routes::PutRadioInfo(const int stationID) {
      http.begin(baseURL + "stations/" + stationID);
      int httpCode = http.PUT("playing=1");
      if (httpCode > 0) {
        if (httpCode == HTTP_CODE_OK) {
          return http.getString();
        }
      }
      return "Error";
    }

    String Routes::Reset(){
      String payload;
      payload = request(baseURL + "stations/clear");
      return "Done";
    }

    // / PUT /api/users/{user_id}/settings?volume={volume}
    // Update the volume
    String Routes::UpdateVolume(const int user_id, const int volume) {
    http.begin(baseURL + "users/" +  user_id  + "/settings");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String payload = "volume=";
    payload.concat(volume);

      Serial.println(payload); 
      int httpCode = http.PUT(payload);
      if (httpCode > 0) {
        if (httpCode == HTTP_CODE_OK) {
          Serial.println(http.getString());
          return http.getString();
        }
      }
      Serial.println("ik ben hier");
      return "Error";
    }

    String Routes::GetPlayingStation() {      
        String payload;
        payload = request(baseURL + "stations?is_playing=1" );       
        JSONVar output = JSON.parse(payload);                 
        String name = JSON.stringify(output["data"][0]);   
        return name;
    }