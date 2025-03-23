



#include   <WiFi.h>
const char* ssid = "@@@@@@@@@@@@@@@";
const char* wifipw = "#############";
        int wifitime, esprestart;

#include   <HTTPClient.h>
#include   <ArduinoJson.h>

#include   "time.h"
#include   "time_routines.h"
      int   timeDay;
const char  degree_symbol[] = "\u00B0";
        
#include   <SPI.h>
#include   <TFT_eSPI.h> //             ! version 2.5.34
            TFT_eSPI tft = TFT_eSPI();
#include   "U8g2_for_TFT_eSPI.h"
            U8g2_for_TFT_eSPI u8f; //  U8g2 font instance
#define     TFT_LED 33

#define     FS_NO_GLOBALS
#include   <FS.h>
#include   "SPIFFS.h"

#include   <TJpg_Decoder.h>
            uint16_t w = 0, h = 0;
            int x, y = 0;
            bool tft_output (int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap) {
                           if (y >= tft.height() ) return 0;
                               tft.pushImage(x, y, w, h, bitmap);
                               return 1;    
                              }

#include   <tableaux.h>
        int meteoCall = (5*60)*1000; //       pour 5 minutes en microsecondes
        int lastMeteoCall;
       bool firstCall = true;          
//          Meteo_France        
            String current = ("https://api.open-meteo.com/v1/forecast?latitude=48.477&longitude=7.6788&current=temperature_2m,relative_humidity_2m,apparent_temperature,is_day,precipitation,weather_code,surface_pressure,wind_speed_10m,wind_direction_10m,wind_gusts_10m&timezone=Europe%2FBerlin");    
//          Weather Forecast
//            String current = ("https://api.open-meteo.com/v1/forecast?latitude=48.477&longitude=7.6788&current=temperature_2m,relative_humidity_2m,apparent_temperature,is_day,weather_code&timezone=Europe%2FBerlin&forecast_days=1");    
            String url;
            String image, temperature, ressenti, humidite;
             float temperatureFormat, ressentiFormat;
               int humiditeFormat;
            String temperatureFormatTxt, ressentiFormatTxt, humiditeFormatTxt;
            String is_day, weather_code, icone;
               int pointeur;

            

void image_display() {

     TJpgDec.getFsJpgSize(&w, &h, "/" + image + ".jpg");
     TJpgDec.drawFsJpg(x, y, "/" + image + ".jpg");
  
}


void startWifi() {

     WiFi.begin(ssid, wifipw);
     Serial.println("Connecting Wifi");
     while (WiFi.status() != WL_CONNECTED) {
            wifitime ++;
            Serial.print("."); delay(100);
            if (wifitime > 20) { ESP.restart(); delay(50); }
           }
     Serial.println();
     Serial.print("Local IP "); Serial.print(WiFi.localIP()); Serial.println();
     Serial.print("Wifi RSSI="); Serial.println(WiFi.RSSI()); Serial.println();
     
}



void setup() {

     Serial.begin(115200); Serial.println("\n \n");
     
     startWifi();
     initTime("CET-1CEST,M3.5.0,M10.5.0/3");   // Set for Berlin
     WiFi.disconnect(); delay(50);    
     
     if (!SPIFFS.begin()) {
         Serial.println("SPIFFS initialisation failed!");
         while (1) yield();
     } else { Serial.println("SPIFFS OK !"); }
     
     tft.init();
     tft.setRotation(3);
     tft.begin();
     u8f.begin(tft); 
     pinMode(TFT_LED, OUTPUT);
     digitalWrite(TFT_LED, HIGH);
     tft.fillScreen(TFT_BLACK);
 
     tft.setSwapBytes(true);
     TJpgDec.setJpgScale(1);
     TJpgDec.setCallback(tft_output);

     x = 0; y = 0; image = "fond"; image_display(); delay(1000);

}


void loop() {

     printLocalTime();

     if (oldTimeTxt != timeTxt) {
         oldTimeTxt = timeTxt;
         x = 10; y = 70; image = "heure"; image_display();     
         u8f.setFont(u8g2_font_7Segments_26x42_mn);
         u8f.setFontMode(1);    
         u8f.setForegroundColor(TFT_BLACK);
         u8f.setCursor(33,174);
         u8f.print(timeTxt);
         u8f.setForegroundColor(TFT_LIGHTGREY);
         u8f.setCursor(30,170);
         u8f.print(timeTxt);
         u8f.setFont(u8g2_font_profont29_mf);
         u8f.setFontMode(1);    
         u8f.setForegroundColor(TFT_BLACK);
         u8f.setCursor(103,174);
         u8f.print("H");
         u8f.setForegroundColor(TFT_LIGHTGREY);
         u8f.setCursor(100,170);
         u8f.print("H");
        }
        
     if (oldJourTxt != jourTxt) {
         oldJourTxt = jourTxt;
         x = 215; y = 175; image = "jour"; image_display();
         u8f.setFont(u8g2_font_profont29_mf);
         u8f.setFontMode(1);     
         u8f.setForegroundColor(TFT_BLACK);
         u8f.setCursor(283,223);
         u8f.print(jourTxt);
         u8f.setForegroundColor(TFT_LIGHTGREY);
         u8f.setCursor(280,220);
         u8f.print(jourTxt);
        }

     if (oldDateTxt != dateTxt) {
         oldDateTxt = dateTxt;
         x = 10; y = 230; image = "date"; image_display();
         u8f.setFont(u8g2_font_fur35_tn);
         u8f.setFontMode(1);    
         u8f.setForegroundColor(TFT_BLACK);
         u8f.setCursor(43,294);
         u8f.print(dateTxt);
         u8f.setForegroundColor(TFT_LIGHTGREY);
         u8f.setCursor(40,290);
         u8f.print(dateTxt);
        }

     if (oldMoisTxt != moisTxt) {
         oldMoisTxt = moisTxt;
         x = 140; y = 250; image = "mois"; image_display();
         u8f.setFont(u8g2_font_profont29_mf);
         u8f.setFontMode(1);    
         u8f.setForegroundColor(TFT_BLACK);
         u8f.setCursor(158,294);
         u8f.print(moisTxt);
         u8f.setForegroundColor(TFT_LIGHTGREY);
         u8f.setCursor(155,290);
         u8f.print(moisTxt);
        }


     if ((millis() - lastMeteoCall) > meteoCall || firstCall == true) {
         firstCall = false;
         lastMeteoCall = millis();          
         meteo_datas_load();     
Serial.print("------------------> Icone "); Serial.println(icone);
         x = 325; y = 5; image = icone; image_display();
         x = 215; y = 5; image = "temp"; image_display();
         u8f.setFont(u8g2_font_profont12_mf);
         u8f.setFontMode(1);     
         u8f.setForegroundColor(TFT_LIGHTGREY);
         u8f.setCursor(235,25);
         u8f.print(" Température");
         u8f.setCursor(235,75);
         u8f.print("  Ressenti");
         u8f.setCursor(235,125);
         u8f.print("  Humidité");         
         
         u8f.setFont(u8g2_font_profont29_mf);
         u8f.setFontMode(1);    
         u8f.setForegroundColor(TFT_BLACK);
         u8f.setCursor(227,53);
         u8f.print(temperatureFormatTxt); u8f.print("\u00B0");
         u8f.setForegroundColor(TFT_LIGHTGREY);
         u8f.setCursor(225,50);
         u8f.print(temperatureFormatTxt); u8f.print("\u00B0");
         u8f.setForegroundColor(TFT_BLACK);
         u8f.setCursor(227,103);
         u8f.print(ressentiFormatTxt); u8f.print("\u00B0");
         u8f.setForegroundColor(TFT_LIGHTGREY);
         u8f.setCursor(225,100);
         u8f.print(ressentiFormatTxt); u8f.print("\u00B0");
         u8f.setForegroundColor(TFT_BLACK);
         u8f.setCursor(242,153);
         u8f.print(humiditeFormatTxt + " %");
         u8f.setForegroundColor(TFT_LIGHTGREY);
         u8f.setCursor(240,150);
         u8f.print(humiditeFormatTxt + " %");

      }
      
delay(9999);

}


//
