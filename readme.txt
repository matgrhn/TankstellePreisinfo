Mit dem Sketch Tankstelle_Preisinfo kann auf einem arduino der aktuellen Tagespreis von einer individuellen Tankstelle anzeigt werden - für die Modelleisenbahn oder auf einem Display zuhause.

----------------------------------------------------------------------------------------------------------------------------------
Vielen Dank an https://tankerkoenig.de, wo Daten aller deutschen Tankstellen in Echtzeit kostenlos abgefragt werden können.
Um den Dienst zu nutzen, wird ein persönlicher Zugangsschlüssel (API-KEY) und der Key der gewünschten Tankstelle benötigt.

Registrieren kostenlos beim Tankerkoenig mit dem Aufruf
https://creativecommons.tankerkoenig.de  unter dem Reiter: API-KEY.

Als Ergebnis nach dem Aufruf bekommt man seinen eigenen 36stelligen ApiKey,
den man
      im Sketch in Zeile 66 im String TankerkoenigApiKey = "xxxxxxxx-xx…"
      als auch im folgenden Aufruf der Tankstellenliste eintragen muss.

Ermitteln einer TankstellenID aus der Tankstellenliste:
Aus der Liste die TankstellenID ermitteln, von der man die aktuellen Preise anzeigen möchte.  Mehr als 14000 Tankstellen aus dem Bundesgebiet sind in der Datenbank beim Tankerkoenig verfügbar.
Für die Auswahl von Tankstellen kann man den Breitengrad (?lat=xx.xxx), den Längengrad (&lng=x.xxxx) und den Radius (&rad=x) angeben:

Der URL-Aufruf lautet (im Browser testen),eventuell bei der Gelegenheit gleich das Root-Zertifikat speichern - siehe unten.

https://creativecommons.tankerkoenig.de/json/list.php?lat=xx.xxx&lng=x.xxxx&rad=x &sort=price&type=diesel&apikey=xx ( hier den TankerkoenigApiKey 36stellig eingeben.)

Als Ergebnis nach dem Aufruf bekommt man eine Liste der ermittelten Tankstellen. Von einer ausgewählten Tankstelle ist im Sketch im String die TankstellenID = "xxxxxxxx-xx…" einzutragen.

----------------------------------------------------------------------------------------------------------------------------------

Benötigt wird:
der Sketch,
ein ESP32 (mit WLAN),
ein TFT-Display (z.B. 1.8Zoll, 128 x 160 Pixel, Z.B: ST7735S).

ESP32 in der arduino Entwicklungsumgebung einbinden bzw. kompilierbar machen, siehe auch
https://www.az-delivery.de/blogs/azdelivery-blog-fur-arduino-und-raspberry-pi/esp32-jetzt-mit-boardverwalter-installieren

Unter Preferences bei "Addition board manager URLSs" diese Quelle einstellen:
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json

Dann IDE neustarten, ESP32 verbinden und Port auswählen (den aktiven Port findet man im Windows-Gerätemanager)

----------------------------------------------------------------------------------------------------------------------------------
Für die Kommunikation zwischen ESP32 und TFT-Display wird die SPI-Schnittstelle verwendet. 
Dafür benötigt man in der arduino Entwicklungsumgebung eine Bilbliothek, z.B. die von Bodmer:
https://github.com/Bodmer/TFT_eSPI
----------------------------------------------------------------------------------------------------------------------------------
Für die SSL-Verbindung: 
Server certificate:
Ein CACertificate, gültig bis zum 11.Sept.2024,  ist bereits im Sketch eingefügt.
Wenn das Zertifikat abläuft, ein neues beschaffen, das geht so:
die komplette URL vom Tankerkönig, also 
https://creativecommons.tankerkoenig.de/json/list.php?lat=xx.xxx&lng=x.xxxx&rad=x &sort=price&type=diesel&apikey=xx ...
im Browser eingeben. Wenn das Ergebnis im JSON-Format angezeigt wird, 
also in etwa
{"ok":true,"license":"CC BY 4.0 -  https:\/\/creativecommons.tankerkoenig.de","data":"MTS-K","status":"ok","station":{"id":"34d2...ef","name":"AVIA Tankstelle","brand":"AVIA","street":"M\u00fchlenweg","houseNumber":"17","postCode":26209,"place":"Hatten","openingTimes":[{"text":"Mo-....
hat es geklappt. Dann in der Brower-URL-Zeile auf das Schloss klicken und durcharbeiten über "die Verbindung ist sicher"... "Zertifikat anzeigen.." (je nach Browser unterschiedliche), bis an die Stelle wo bei "Zertifikatshierachie "Root" ausgewählt werden kann. Markieren, Exportieren und mit einem Editor anzeigen, sieht in etwa so aus:

-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
....
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE----


Das so anpassen, dass es zum Arduino-Code passt, also Anführungszeichen vorn+hinten, \ usw., am Ende ein Semikolon:
"-----BEGIN CERTIFICATE-----\n" \
"MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw\n" \
"TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\n" \
"cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4\n" \
"WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu\n" \
...
"emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=\n" \
"-----END CERTIFICATE-----\n" ;

Dann in den Sketch einsetzen.
----------------------------------------------------------------------------------------------------------------------------------
Für das JSON-Parsing (Daten von Tankerkönig aufbereiten):
Bibliothek ArduinoJson von Benoit Blanchon (Benoit hat auch sehr informative Youtubevideos zu dem Thema erstellt)
----------------------------------------------------------------------------------------------------------------------------------

Verdrahtung siehe Verdrahtung.pdf
----------------------------------------------------------------------------------------------------------------------------------

WiFi Internetzugang:
z.B. die FRITZ!Box-Bezeichnung und den WLAN-Netzwerkschlüssel bei #define Fritzbox einstellen.
----------------------------------------------------------------------------------------------------------------------------------

Kompiliert wird der Sketch mit:
Arduino ID:
Unter Werkzeuge eintragen:
ESP32 Board: „DOIT ESP32 DEVKIT V1“
Boardverwalter: „ESP32 Arduino“ -> „DOIT ESP32 DEVKIT V1“.

Wenn es beim Kompilieren noch seltsame Fehler gibt, fehlt vielleicht noch eine Bibliothek, die eingebunden werden muss. 
Einige Bibliotheken lassen sich über den Library-Manager aktivieren, die folgenden musste ich mir sich als Zip-Datei auf Github suchen und herunterladen:
Adafruit-GFX-Library-Master
Adafruit-ST7735-Library-Master
Adafruit_BusIO https://github.com/adafruit/Adafruit_BusIO

Google mal nach diesen Bibliotheken, das sind Kandidaten für fehlende Bibliotheken:
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>                   
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

Mit dem ESP32 gibt es immer mal Schwierigkeiten
beim Hochladen. Nach "Connecting..." kommt dann irgendwann ein Fehlercode 2. Es soll helfen, denn Button "Boot" zu drücken oder En und Boot zusammen. Bei mir hat das
Problem ein 10my Kondensator gelöst, den ich zwischen dem Pin EN und GND gesetzt habe (siehe https://www.youtube.com/watch?v=SlAG6DFLnBE).


----------------------------------------------------------------------------------------------------------------------------------

Lauftext Anpassung:
Seinen „persönlichen“ Lauftext kann man im String lauftext = „***   in Zeile 30 – 31 anpassen.
----------------------------------------------------------------------------------------------------------------------------------
SerMon:
Fehlerhinweise und weitere Informationen werden im seriellen Monitor aufgezeichnet (oben rechts in der IDE klicken, dann sieht man das Protokoll unten), z.B.:

    10:40:09.235 -> Hello. I´m the display of your Tankstelle
    10:40:09.328 -> Waiting for WiFi to connect... connected
    10:40:15.467 -> Waiting for NTP time sync: ...
    10:40:16.948 -> Current time: Wed May 31 08:40:16 2023
    10:40:18.143 -> [HTTPS] begin...
    10:40:18.143 -> [HTTPS] GET...
    10:40:19.039 -> [HTTPS] GET... code: 200
    10:40:19.039 -> {"ok":true,"license":"CC BY 4.0 -  https:\/\/creativecommons.tankerkoenig.de",
                "data":"MTS-K","status":"ok","station":{"id":"ac50c5f1-7.....840b6a",.....
    10:40:19.087 -> Benzin: 1.85 Super: 1.83 Diesel: 1.66 Super+: 0.00 
    10:43:09.201 -> Benzin: 1.85 Super: 1.83 Diesel: 1.66 Super+: 0.00 
    10:46:09.202 -> Benzin: 1.85 Super: 1.83 Diesel: 1.66 Super+: 0.00 
    10:49:09.226 -> Benzin: 1.85 Super: 1.83 Diesel: 1.66 Super+: 0.00
       
