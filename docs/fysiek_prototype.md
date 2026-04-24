# Fysiek prototype

In dit bestand kun je meer info vinden over hoe alles te verbinden.


## wiring diagrama

![Opstelling Planda-beer](<../imgs/Opstelling Planda-beer.jpg>)

## Installeren ESP32-S3 N16R8 Devboard
Programmeren van de ESP32-S3 gebeurt eenvoudig via de USB-poort. Volg onderstaande stappen om alles correct in te stellen in de Arduino IDE.

### Stap 1: Installeer de ESP32 Board Manager
* Open de Arduino IDE
* Ga naar File → Preferences
* Zoek het veld “Additional Boards Manager URLs”

Voeg de volgende URL toe:

https://espressif.github.io/arduino-esp32/package_esp32_index.json

* Klik op OK
* Ga naar Tools → Board → Boards Manager
* Zoek naar “ESP32”
* Installeer “ESP32 by Espressif Systems”

### Stap 3: Configureer het juiste board
Ga naar Tools en selecteer de volgende gegevens:

* Board: ESP32S3 Dev Module
* USB CDC On Boot: "Enabled"
* Flash Mode: "QIO 80MHz
* Flash Size: "16MB (128Mb)
* Partition Scheme: "16M Flash (3MB APP/9.9MB FATFS)"
* PSRAM: "OPI PSRAM"
