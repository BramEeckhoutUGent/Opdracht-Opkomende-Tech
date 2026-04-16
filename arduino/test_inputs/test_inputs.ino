// Dit is code om te controleren of het 2.8 inch TFT display en zijn SD kaart, DFPlayer mini, knoppen en LED's werken.
// Opmerking! Het kan zijn dat na het uploaden de RST knop kort ingedrukt moet worden als er niets wordt terug gegeven via de serial monitor.

#include <SPI.h>
#include <SD.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

// ipv het bestandje TFT_eSPI.h aan te passen, schrijf ik het liever voluit
// Op deze manier heb ik al meerdere malen mezelf gevangen op het verkeerd verbinden

#define TFT_CS   10
#define TFT_DC   41
#define TFT_RST  15
#define SD_CS    9
#define SPI_MOSI 11
#define SPI_MISO 13
#define SPI_SCK  12

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

// Volgende code werd deels geschreven door Gemini: lijn 30-58

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10); 
  }

  Serial.println("\n--- Start Test ---");

  // Initialiseer SPI (Werd door AI als stap gegeven, moet nog onderzoeken wat dit doet)
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);

  // Visuele controle van de werking van het scherm door het scherm groen te maken.
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(ILI9341_GREEN); // Of gebruik de kleurcode: 
  Serial.println("Display: Scherm is nu GROEN.");
  tft.setTextColor(ILI9341_BLACK);
  tft.setTextSize(2);
  tft.setCursor(50, 100);
  tft.println("Scherm werkt!");

  // Controle of de SD kaart op het scherm niet kapot is, hierop zullen later de GIF's staan.
  // Momenteel zal deze niet werken, er moet nog gesoldeerd worden
  Serial.print("SD Kaart status: ");
  if (SD.begin(SD_CS)) {
    Serial.println("GEVONDEN! :)");
    uint32_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("Grootte: %u MB\n", cardSize);
  } else {
    Serial.println("NIET GEVONDEN!");
  }
  
  Serial.println("--- Test Klaar ---");
}

// In principe niet nodig, heb deze nu toegevoegd omdat ik soms de melding kreeg dat de ESP32 connect en disconnect.

void loop() {
  Serial.println("Systeem is actief...");
  delay(5000); 
}