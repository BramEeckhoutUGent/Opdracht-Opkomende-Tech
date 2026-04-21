// Dit is code om te controleren of het 2.8 inch TFT display en zijn SD kaart, DFPlayer mini, knoppen en LED's werken.
// Opmerking! Het kan zijn dat na het uploaden de RST knop kort ingedrukt moet worden als er niets wordt terug gegeven via de serial monitor.
// Er zal telkens vermeld worden bij de code lines wanneer er AI gebruikt werd.


// ------ Installeer de nodige libraries ------

#include <SPI.h>
#include <SD.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>


// ------ Configureer de nodige componenten ------

// ipv het bestandje TFT_eSPI.h aan te passen, schrijf ik het liever voluit. Dit zal ik niet doen bij het schrijven van d ehoofdcode, maar direct integreren in een functie.
// Op deze manier heb ik al meerdere malen mezelf gevangen op het verkeerd verbinden van de poorten
// bekijk https://randomnerdtutorials.com/esp32-s3-devkitc-pinout-guide/ om de juiste pins te gebruiken!

#define TFT_CS   10
#define TFT_DC   9
#define TFT_RST  8
#define SPI_MOSI 11
#define SPI_MISO 13
#define SPI_SCK  12

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST); // De initialisatie werd geschreven door AI, in de hoofdcode heb ik op een andere manier gewerkt

const int button1Pin = 4;
const int button2Pin = 5;
const int ledBlauwPin = 6;
const int ledGroenPin = 7;
const int ledGeelPin = 15;


// ----- Test of het display werkt ------

void setup() { // Visuele controle van de werking van het scherm door het scherm groen te maken.
  Serial.begin(115200);
  while (!Serial) {
    delay(10); 
  }

  Serial.println("\n Start Test!"); // Dit wordt gedaan omdat ik naast visuele feedback ook feedback wil krijgen via de Serialmonitor => dan weet ik op welke plaats in de code ik moet kijken.

  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI); // Initialiseer SPI (Werd door AI als aparte stap gedaan, los van Adafruit_ILI9341 tft = ...

  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(ILI9341_GREEN); // Of gebruik een kleurcode
  Serial.println("Display: Scherm is nu GROEN.");
  tft.setTextColor(ILI9341_BLACK);
  tft.setTextSize(2);
  tft.setCursor(50, 100);
  tft.println("Scherm werkt!");
  
  Serial.println("Test Klaar!");
}


void loop() { // In principe niet nodig, heb deze nu toegevoegd omdat ik soms de melding kreeg dat de ESP32 connect en disconnect. Soms wordt dit opgelost door kort op de RST knop te duwen op het bordje.
              // Op deze manier weet ik dat de controller niet meer werkt als er niets meer geprint wordt.
  Serial.println("Systeem is actief...");
  delay(5000); 
}


// ----- Test of de drukknoppen en de lampjes werken -----

void setup() { // Deze code is gelijkaardig aan oefening 1.4.1
  Serial.begin(115200);
  
  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(ledBlauwPin, OUTPUT);
  digitalWrite(ledBlauwPin, LOW);

  pinMode(buttonPin2, INPUT_PULLUP);
  pinMode(ledGeelPin, OUTPUT);
  digitalWrite(ledGeelPin, LOW);

  pinMode(ledGroenPin, OUTPUT);
  digitalWrite (ledGroenPin, HIGH);
}

void loop() {
  digitalWrite(ledBlauwPin, digitalRead(button1Pin));
  digitalWrite(ledGeelPin, digitalRead(button2Pin));

  if (digitalRead(buttonPin1) == LOW) {
    Serial.println("De rechter knop is ingedrukt dus de Blauwe LED gaat uit");
  }
  if (digitalRead(buttonPin2) == LOW) {
    Serial.println("De linker knop is ingedrukt dus de gele LED gaat uit");
  }
}