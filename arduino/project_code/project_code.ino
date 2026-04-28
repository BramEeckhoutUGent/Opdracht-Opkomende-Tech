// De code voor het tonen van GIF's werd deels gebaseerd op de Youtube Tutorial: https://youtu.be/omUWkUqFYrQ
// De code voor het afspelen van audio bestanden via de DFPlayer mini werd deels gebaseerd op de Youtube Tutorial: https://www.youtube.com/watch?v=P42ICrgAtS4 
// De code voor dit project werd zo veel mogelijk zelf geschreven, AI werd gebruikt om de code te verbeteren.
// Code die door AI geschreven werd, wordt aangegeven in de lijn.


// ------ Installeren van libraries ------

#include "esp_flash.h"
#include "esp_partition.h"
#include <bb_spi_lcd.h>
#include <AnimatedGIF.h>
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"


// ------ GIF bestanden ------

#include "gif_files/JasjePixel.h" // Dit is de gif voor deze taak
#define GifData JasjePixel // Dit stuurt de GIF naar het scherm


// ------ Definieer pinnen -------
// --- 2.8 Inch TFT Display ---
#define TFT_CS    10
#define TFT_DC    9
#define TFT_RST   8
#define TFT_LED   -1 // waarde is -1 want deze sturen we niet aan door een poort, maar verbinden met 3.3V poort
#define TFT_MISO  13
#define TFT_MOSI  11
#define TFT_CLK   12

// --- DFPlayer Mini ---
#define DF_RX 18 
#define DF_TX 17

// --- Drukknoppen ---
#define Volgende_knop 16
#define Actie_knop 15

// --- LED's ---



// ------ Start code -------
// --- Objecten ---
BB_SPI_LCD tft;
SoftwareSerial dfSS(DF_RX, DF_TX);
DFRobotDFPlayerMini myDFPlayer;
AnimatedGIF gif;

// --- Status managment ---
// Deze code werd gegenereerd door AI (gemini)
int Huidige_taak = 0;
bool Wordt_actie_getoond = false; // Zijn we momenteel de actie-afbeelding aan het tonen?
bool Taak_gedaan = false;

// --- GIF data ---
#define GIF_COUNT 3 // Het aantal taakjes van de Planda-beer
const uint8_t* gifData[GIF_COUNT] = {gif1, gif2, gif3};
const size_t gifSizes[GIF_COUNT] = {sizeof(gif1), sizeof(gif2), sizeof(gif3)};

// --- Debounce variabelen ---
// Deze code werd gegenereerd door AI (gemini)
int lastNextState = -1, nextState = -1;
unsigned long lastNextDebounce = 0;
int lastActionState = -1, actionState = -1;
unsigned long lastActionDebounce = 0;


// -----

void setup() {
  Serial.begin(115200);
  dfSS.begin(9600);
  
  pinMode(Volgende_knop, INPUT_PULLUP);
  pinMode(Actie_knop, INPUT_PULLUP);

  tft.begin(LCD_ILI9341, FLAGS_NONE, 40000000, TFT_CS, TFT_DC, TFT_RST, TFT_LED, TFT_MISO, TFT_MOSI, TFT_CLK);
  tft.setRotation(LCD_ORIENTATION_90);
  tft.fillScreen(TFT_BLACK);

  if (!myDFPlayer.begin(dfSS)) {
    Serial.println("DFPlayer error");
  } else {
    myDFPlayer.volume(20); // Geluid van 0 tot 30
  }

  loadGif(Huidige_taak); // Dit toont de eerste taak op het scherm
}

void loop() {
  if (Taak_gedaan) return;

  // --- Optie 1: Ga door naar de volgende taak ---
  if (isButtonPressed(Volgende_knop, nextState, lastNextState, lastNextDebounce)) { // Deze code werd gegenereerd door AI (gemini)
    Huidige_taak++;                                                                 // |||||||||||||||||||||||||||||||||||||||||||
    Wordt_actie_getoond = false;                                                    // |||||||||||||||||||||||||||||||||||||||||||
    
    if (Huidige_taak < GIF_COUNT) {
      Serial.printf("Naar taak %d\n", Huidige_taak + 1);
      tft.fillScreen(TFT_BLACK);
      loadGif(Huidige_taak);
    } else {
      Serial.println("Taak is gedaan");
      tft.fillScreen(TFT_BLACK);
      Taak_gedaan = true;
    }
  }

  // --- Optie 2: Geef extra instructies/extra motivatie ---
  if (!Taak_gedaan && isButtonPressed(Actie_knop, actionState, lastActionState, lastActionDebounce)) { // Deze code werd gegenereerd door AI (gemini)
    if (!Wordt_actie_getoond) {                                                                        // |||||||||||||||||||||||||||||||||||||||||||
      Serial.println("Kindje wil extra hulp");                                                         // |||||||||||||||||||||||||||||||||||||||||||
      Wordt_actie_getoond = true;                                                                      // |||||||||||||||||||||||||||||||||||||||||||
      
      myDFPlayer.play(1); // Speel geluidje
      
      tft.fillScreen(TFT_BLACK);
      showActionImage(); // Hint wordt getoond 
    }
  }

  // --- Rendering ---
  if (!Wordt_actie_getoond && !Taak_gedaan) {
    gif.playFrame(false, NULL); // Speel de GIF af
  }
}


// ------ Functie om een nieuwe GIF klaar te zetten ------

void loadGif(int index) {
  gif.close();
  gif.begin(GIF_PALETTE_RGB565_BE);
  if (gif.open((uint8_t *)gifData[index], gifSizes[index], GIFDraw)) {
    gif.setDrawType(GIF_DRAW_COOKED);
    gif.allocFrameBuf(GIFAlloc);
  }
}


// ------ Functie om de statische actie-afbeelding te tonen ------

void showActionImage() {
  gif.close();
  gif.begin(GIF_PALETTE_RGB565_BE);
  if (gif.open((uint8_t *)???, sizeof(???), GIFDraw)) {
    gif.setDrawType(GIF_DRAW_COOKED);
    gif.allocFrameBuf(GIFAlloc);
    gif.playFrame(false, NULL);
  }
}


// ------ knop-checker ------
// Deze code werd gegenereerd door AI (gemini)
bool isButtonPressed(int pin, int &state, int &lastState, unsigned long &lastDebounce) {
  int reading = digitalRead(pin);
  if (reading != lastState) lastDebounce = millis();
  if ((millis() - lastDebounce) > 50) {
    if (reading != state) {
      state = reading;
      return state == LOW;
    }
  }
  lastState = reading;
  return false;
}


// ------ GIF helper functies (Nodig voor bb_spi_lcd) ------

void *GIFAlloc(uint32_t u32Size) { return malloc(u32Size); }
void GIFFree(void *p) { free(p); }
void GIFDraw(GIFDRAW *pDraw) {
  if (pDraw->y == 0) tft.setAddrWindow(pDraw->iX, pDraw->iY, pDraw->iWidth, pDraw->iHeight);
  tft.pushPixels((uint16_t *)pDraw->pPixels, pDraw->iWidth);
}


// ------ TEST -------
# wanneer knop ingedrukt wordt speelt bevestigings geluid af en begint 
het lampje van de knop te pinken totdat er een volgende actie wordt geactiveert 
om naar de volgende stap te gaan, standaard branden de ledlichtjes dus