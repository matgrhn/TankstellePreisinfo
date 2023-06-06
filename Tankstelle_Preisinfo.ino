/*  siehe Beschreibung neuer Sketch mit aktuellen Preisen */
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>                   
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

#ifndef FRITZBOX
#define FRITZBOX "meinWlan"                                                     //  WLAN Zugangsdaten
#define PASSWORD "xxxxxxxxxxxxxxxxxxxx"                                         //  WLAN Passwort
#endif

// Definieren der PINs fuer das Display
#define TFT_CS       17   
#define TFT_RST       4    // set to -1 and connect to Arduino RESET pin
#define TFT_DC        2       
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// Pin fuer die LEDs in der Tankstelle (z.B. Kassenraumbeleuchtung)
// int led = 3;                                                                  

unsigned long drawTime = 0;
// Definition der Texte und Werte
const String lauftext = 
" ***  Bockwurst im Brot 3,20   ***   Kaffee + Kuchen 4,20   *** "; 
const uint8_t width = 24;  //Länge des Ausschnittes
int offset = 0;

// wir arbeiten mit byte und tun bei der Anzeige nur so, als wären es Kommazahlen. Das spart Speicher.

uint16_t preisE5     = 0;                                                               // = Preis BenzinE5
uint16_t preisE10    = 0;                                                               // = Preis BenzinE10
uint16_t preisDiesel = 0;                                                               // = Preis Diesel
uint16_t preisSplus  = 0;                                                               // = Preis Super+
unsigned long myTime;

/*
 *  Definieren der Farbe fuer den Hintergund und den Vordergrund 
 *  Onlinetool zum umrechnen der Farbwerte
 *  http://www.barth-dev.de/online/rgb565-color-picker/
 *  
 */
//uint16_t TFT_BLUE = 0210; // Hintergrundfarbe                                            
//Zum verwenden die nachfolgenden Zeilen aktivieren.
word RGBtoColor( byte R, byte G, byte B)
{
  return ( ((R & 0xF8) << 8) | ((G & 0xFC) << 3) | (B >> 3) );
}
uint16_t TFT_BLUE = RGBtoColor(65, 105, 255);                                              

#define CHAR_WIDTH 6                                              

// Zeitsteuerung des Preiswechsels
uint32_t aktMillis, lastUpdatePreise, lastUpdateScrollText;
const uint32_t INTERVALL = 12 * 15000UL;                // 12 * 15000 = 180.000 = das sind 180 Sekunden = 3  Minuten
//const uint32_t INTERVALL = 200;                       // 200; // das sind für Testzwecke 0,2 Sekunden
const uint32_t SCROLLINTERVALL = 240/CHAR_WIDTH;        // 240/6 = 0.040 das sind 0.04 Sekunden = 40 MilliSekunden 

// Dein Tankerkoenig ApiKey und die abzufragende TankstellenID         
String TankerkoenigApiKey = "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx";    
String TankstellenID      = "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx";  
                                                                

const String url = 
      "https://creativecommons.tankerkoenig.de/json/detail.php?id=" + TankstellenID + "&apikey=" + TankerkoenigApiKey;
// --------------------------------------------------------------------------
// Freitag, 18. August 2023 um 02:23:14
// root certificate von creativecommons.tankerkoenig.de
// URL/webservice im Browser aufrufen, Klick auf Schlosss im Browser,
// click auf "zertifikat ist sicher", export Root-Cert, dann im 
// editor anpassen wie unten (Zeilenanfang und Ende)
const char* rootCACertificate = \
"-----BEGIN CERTIFICATE-----\n" \
"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\n" \
"-----END CERTIFICATE-----\n" ;

// Not sure if WiFiClientSecure checks the validity date of the certificate. 
// Setting clock just to be sure...
void setClock() {
  configTime(0, 0, "pool.ntp.org");

  Serial.print(F("Waiting for NTP time sync: "));
  time_t nowSecs = time(nullptr);
  while (nowSecs < 8 * 3600 * 2) {
    delay(500);
    Serial.print(F("."));
    yield();
    nowSecs = time(nullptr);
  }

  Serial.println();
  struct tm timeinfo;
  gmtime_r(&nowSecs, &timeinfo);
  Serial.print(F("Current time: "));
  Serial.print(asctime(&timeinfo));
}


WiFiMulti WiFiMulti;

void setup() {

  Serial.begin(115200);
  // Serial.setDebugOutput(true);
  Serial.println();
  Serial.println();
  Serial.println("Hello. I´m the display of your Tankstelle");
  
  

 
  // Init TFT-Display ------------------------------------------------------
  tft.initR(INITR_BLACKTAB);          // TFT 1.8"    128*160                      
  //tft.initR(INITR_144GREENTAB);     // TFT 1.44"                        
  tft.setRotation(0);           // DREHEN DES DISPLAYS um 180° mit (0) oder (2)
  tft.setSPISpeed(40000000);                                
  tft.setTextWrap(false);                                       
  tft.fillScreen(TFT_BLUE);
  tft.drawRect(0,116,tft.width(),13,ST7735_WHITE); // x, y, weite, hoehe, Farbe
  
  randomSeed(analogRead(A0));   // unbenutzter Analogeingang 
  
  Serial.println("lesepreise und schreibeNeu im Setup");
  lesepreise();
  
  tft.fillScreen(TFT_BLUE);
  schreibeNeu();
}
// --------------------------------------------Setup-Ende --------

  void loop() {    
  //digitalWrite(3,HIGH);                                                      
    aktMillis = millis(); 
                                                                               
  if (aktMillis - lastUpdatePreise >= INTERVALL) {                      
    lastUpdatePreise = aktMillis;
    Serial.println("lesepreise und schreibeNeu im Loop");
    lesepreise();
    
    schreibeNeu();
  }


  // Zeichnen des Rahmens um den Lauftext
  tft.drawRect(0,116,tft.width(),13,ST7735_WHITE); // x, y, weite, hoehe, Farbe

//Lauftext ausgeben
  if (aktMillis - lastUpdateScrollText >= SCROLLINTERVALL) {             
    lastUpdateScrollText = aktMillis;
    //Serial.println(scroll());
    if(offset< CHAR_WIDTH * lauftext.length()){
      offset++;
      scrollText(offset);      
    }else{
      offset = 0;
    }
  }
                              
  // Gute Fahrt
  tft.setTextSize(2);
  tft.setCursor(0, 140);
  tft.print("Gute Fahrt!");

 }      // ------------------------------------ Ende Loop  ----------

//Scrolltext erstellen und ausgeben  
void scrollText(int offset){
    String t = "";
      tft.setCursor(-offset, 119);
      tft.setTextSize(1);
      tft.print(lauftext);
      tft.drawLine(0,116,0, 116+13, ST7735_WHITE);  // Senkrechte Linie links neu zeichnen weil vom Text gelöscht wurde
      tft.drawLine(tft.width()-1, 116, tft.width()-1, 116+13, ST7735_WHITE);    // Senkrechte Linie rechts neu zeichnen
}

// uint16_t zu scheinbarem Floatstring
char *byteToFloat(uint16_t in) {
static char preisStr[5];
   preisStr[4] ='\0';
   preisStr[1] ='.';  // oder ','                                  // Hier kann im Preis "Punkt" oder "Komma" auswählen
   preisStr[3] = in % 10 +'0';
   in /= 10;
   preisStr[2] = in % 10 +'0';
   in /= 10;
   preisStr[0] = in % 10 +'0';
   // Serial.println(preisStr);
   return preisStr;
}
// Ausgabe eine Zeile
void schreibPreis(int y, char* beschriftung, uint16_t preisZahl) {
  int abstandLinksText = 5;
  char *preis = byteToFloat(preisZahl);
  Serial.print(beschriftung); Serial.print(": ");Serial.print(preis); Serial.print(" ");    // Serielle Ausgabe 
  
  //Kraftstoffart klein schreiben
  tft.setTextSize(1);
  tft.setCursor(abstandLinksText, y);
  tft.print(beschriftung);
  
  //Preis gross schreiben
  tft.setTextSize(2);
  int abstandLinksPreis = 70; //durch Experimente ermittelt
  tft.setCursor(abstandLinksPreis, y);
  tft.print(preis);
  
  //eine kleine 9 schreiben
  tft.setTextSize(1);
  int abstandLinks9 = 120; //durch Experimente ermittelt
  tft.setCursor(abstandLinks9,y);
  tft.print("9");
}


//schreib die Preise

void schreibeNeu() {
//Hintergrund schwarz füllen
  //tft.fillScreen(TFT_BLUE);                                                   // auskommentiert
  tft.setTextColor(ST7735_WHITE, TFT_BLUE);
  int y = 10;
  int zeilenHoehe = 28;                                                 //Pixel pro Zeile; durch Experimente ermittelt
 

  schreibPreis(y, "Benzin-E5", preisE5);
  y += zeilenHoehe;
  schreibPreis(y, "Benzin-E10", preisE10);
  y += zeilenHoehe;
  schreibPreis(y, "Diesel", preisDiesel);
  y += zeilenHoehe;
  schreibPreis(y, "Super+", preisSplus);
  Serial.println("");
}


void lesepreise() {
  // Init WiFi --------------
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(FRITZBOX, PASSWORD);   
  
  // wait for WiFi connection
  Serial.print("Waiting for WiFi to connect...");
  while ((WiFiMulti.run() != WL_CONNECTED)) {
    Serial.print(".");
  }
  Serial.println(" connected");
  
   setClock();  
  
  WiFiClientSecure *client = new WiFiClientSecure;
  if(client) {
    client -> setCACert(rootCACertificate);
    {
      // Add a scoping block for HTTPClient https 
      HTTPClient https;  
      Serial.print("[HTTPS] begin...\n");
      if (https.begin(*client, url)) {      
        Serial.print("[HTTPS] GET...\n");
        // start connection and send HTTP header
        int httpCode = https.GET();
        // httpCode will be negative on error
        if (httpCode > 0) {
          // HTTP header has been send and Server response header has been handled
          Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
          // file found at server
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
           {
           String payload = https.getString();
           Serial.println(payload);
          // Stream& input;
          StaticJsonDocument<1024> doc;
          DeserializationError error = deserializeJson(doc, payload);
          if (error) {
            Serial.print("deserializeJson() failed: ");
            Serial.println(error.c_str());
            return;
                     }
          bool ok = doc["ok"];                                                    // true
          const char* license = doc["license"];                                   // "CC BY 4.0 -  https://creativecommons.tankerkoenig.de"
          const char* data = doc["data"];                                         // "MTS-K"
          const char* status = doc["status"];                                     // "ok"
          JsonObject station = doc["station"];
          const char* station_id = station["id"];                                 // 
          const char* station_name = station["name"];                             // 
          const char* station_brand = station["brand"];                           // 
          const char* station_street = station["street"];                         // 
          const char* station_houseNumber = station["houseNumber"];               // 
          int station_postCode = station["postCode"];                             // 
          const char* station_place = station["place"];                           // 
          for (JsonObject station_openingTime : station["openingTimes"].as<JsonArray>())
            {
            const char* station_openingTime_text = station_openingTime["text"];   // "Mo-Fr", "Samstag", "Sonntag, ...
            const char* station_openingTime_start = station_openingTime["start"]; // "07:00:00", "08:00:00", ...
            const char* station_openingTime_end = station_openingTime["end"];     // "20:00:00", "20:00:00", "19:00:00"
            }
          bool station_wholeDay = station["wholeDay"];                            // false
          bool station_isOpen = station["isOpen"];                                // true
          float station_e5 = station["e5"];                                       // 1.919
          float station_e10 = station["e10"];                                     // 1.859
          float station_diesel = station["diesel"];                               // 1.699
          float station_lat = station["lat"];                                     // 53.05154
          float station_lng = station["lng"];                                     // 8.264041
          // station["state"] is null
          Serial.print("e5:");           
          Serial.print(station_e5);
          Serial.print(" e10:");           
          Serial.print(station_e10);
          Serial.print(" Diesel:");           
          Serial.print(station_diesel); 
          Serial.print(" Station:"); 
          Serial.println(station_brand);    
          preisE5     =  station_e5 * 100;
          preisE10    =  station_e10 * 100;
          preisDiesel =  station_diesel * 100;   
          preisSplus  =  0; 
          }       // Ende HTTPS-Code = OK 
        }         // Ende HTTP-Code > 0
          else {
          Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
               }
        https.end();
      }           // Ende von HTTPS.begin
       else {
       Serial.printf("[HTTPS] Unable to connect\n");
      }
      // End extra scoping block
    }             // Ende von HTTP Client
  
      delete client;
    } else {
      Serial.println("Unable to create client");
    }
}
