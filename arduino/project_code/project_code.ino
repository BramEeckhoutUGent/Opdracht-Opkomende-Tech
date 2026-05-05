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
// --- Taakjes ---
#include "gif_files/Taak_1_Opruimen.h"
#include "gif_files/Taak_2_Tandenpoetsen.h"
#include "gif_files/Taak_3_Jasje.h"
#define GifData Taak_1_Opruimen
#define GifData Taak_2_Tandenpoetsen
#define GifData Taak_3_Jasje

// --- Hints ---
#include "gif_files/Hint_1.h"
#include "gif_files/Hint_2.h"
#include "gif_files/Hint_3.h"
#define GifData Hint_1
#define GifData Hint_2
#define GifData Hint_3


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
#define Blauwe_knop 16
#define Orange_knop 15

// --- LED's ---
#define Blauwe_led 2 
#define Orange_led 3
#define Groene_led 4


// ------ Definieer variabelen -------
// --- Objecten ---
BB_SPI_LCD tft;
SoftwareSerial dfSS(DF_RX, DF_TX);
DFRobotDFPlayerMini myDFPlayer;
AnimatedGIF gif;

// --- Status managment ---
// [!] Deze code werd gegenereerd door AI (gemini)
int Huidige_taak = 0;
bool Wordt_actie_getoond = false; // Zijn we momenteel de actie-afbeelding aan het tonen?
bool Taak_gedaan = false;

// --- GIF data voor de taakjes ---
#define GIF_COUNT 3
const uint8_t* taakgifData[GIF_COUNT] = {Taak_1_Opruimen, Taak_2_Tandenpoetsen, Taak_3_Jasje};
const size_t taakgifSizes[GIF_COUNT] = {sizeof(Taak_1_Opruimen), sizeof(Taak_2_Tandenpoetsen), sizeof(Taak_3_Jasje)};

// --- GIF data voor de hints ---
const uint8_t* hintgifData[GIF_COUNT] = {Hint_1, Hint_2, Hint_3};
const size_t hintgifSizes[GIF_COUNT] = {sizeof(Hint_1), sizeof(Hint_2), sizeof(Hint_3)};

// --- Debounce variabelen ---
// [!] Deze code werd gegenereerd door AI (gemini)
int lastNextState = -1, nextState = -1;
unsigned long lastNextDebounce = 0;
int lastActionState = -1, actionState = -1;
unsigned long lastActionDebounce = 0;

// --- LED timers en status ---
unsigned long Knipper_Blauw_Start = 0;
unsigned long Knipper_Orange_Start = 0;
unsigned long Groen_Start = 0;
bool Knipper_Blauw_On = false;
bool Knipper_Orange_On = false;
bool Groen_On = false;


// ------ Start Code ------
void setup() {
  Serial.begin(115200);
  dfSS.begin(9600);
  
  pinMode(Blauwe_knop, INPUT_PULLUP);
  pinMode(Orange_knop, INPUT_PULLUP);
  
  pinMode(Blauwe_led, OUTPUT);
  pinMode(Orange_led, OUTPUT);
  pinMode(Groene_led, OUTPUT);
  digitalWrite(Blauwe_led, HIGH);
  digitalWrite(Orange_led, HIGH);
  digitalWrite(Groene_led, LOW);
  
  tft.begin(LCD_ILI9341, FLAGS_NONE, 40000000, TFT_CS, TFT_DC, TFT_RST, TFT_LED, TFT_MISO, TFT_MOSI, TFT_CLK);
  tft.setRotation(LCD_ORIENTATION_90);
  tft.fillScreen(TFT_BLACK);

  if (!myDFPlayer.begin(dfSS)) {
    Serial.println("DFPlayer error");
  } else {
    myDFPlayer.volume(20); // Geluid van 0 tot 30
  }

  loadtaakGif(Huidige_taak); // Dit toont de eerste taak op het scherm
}

void loop() {
  if (Taak_gedaan) return;
  unsigned long nu = millis();

  // --- Optie 1: Ga door naar de volgende taak (blauwe knop + blauwe LED knipperen + groene LED) ---
  if (isButtonPressed(Blauwe_knop, nextState, lastNextState, lastNextDebounce)) { // Deze code werd gegenereerd door AI (gemini)
    Huidige_taak++;                                                                 // |||||||||||||||||||||||||||||||||||||||||||
    Wordt_actie_getoond = false;                                                    // |||||||||||||||||||||||||||||||||||||||||||
    
    Knipper_Blauw_On = true;
    Knipper_Blauw_Start = nu;
    Groen_On = true;
    Groen_Start = nu;
    digitalWrite(Groene_led, HIGH);

    if (Huidige_taak < GIF_COUNT) {
      Serial.printf("Naar taak %d\n", Huidige_taak + 1);
      tft.fillScreen(TFT_BLACK);
      loadtaakGif(Huidige_taak);
    } else {
      Serial.println("Taak is gedaan");
      tft.fillScreen(TFT_BLACK);
      Taak_gedaan = true;
    }
  }

  // --- Optie 2: Geef extra instructies (oranje knop + oranje LED) ---
  if (!Taak_gedaan && isButtonPressed(Orange_knop, actionState, lastActionState, lastActionDebounce)) { // Deze code werd gegenereerd door AI (gemini)
    if (!Wordt_actie_getoond) {                                                                        // |||||||||||||||||||||||||||||||||||||||||||
      Serial.println("Kindje wil extra hulp");                                                         // |||||||||||||||||||||||||||||||||||||||||||
      Wordt_actie_getoond = true;                                                                      // |||||||||||||||||||||||||||||||||||||||||||
      
      Knipper_Orange_On = true;
      Knipper_Orange_Start = nu;

      myDFPlayer.play(1); // Speel geluidje
      
      tft.fillScreen(TFT_BLACK);
      loadhintGif(Huidige_taak); // Hint wordt getoond 
    }
  }

if (Knipper_Blauw_On) {
    if (nu - Knipper_Blauw_Start < 3000) {
      if ((nu / 200) % 2 == 0) digitalWrite(Blauwe_led, HIGH);
      else digitalWrite(Blauwe_led, LOW);
    } else {
      Knipper_Blauw_On = false;
      digitalWrite(Blauwe_led, HIGH);
    }
  }

if (Knipper_Orange_On) {
    if (nu - Knipper_Orange_Start < 3000) {
      if ((nu / 200) % 2 == 0) digitalWrite(Orange_led, HIGH);
      else digitalWrite(Orange_led, LOW);
    } else {
      Knipper_Orange_On = false;
      digitalWrite(Orange_led, HIGH);
    }
  }

  if (Groen_On) {
    if (nu - Groen_Start >= 2000) {
      Groen_On = false;
      digitalWrite(Groene_led, LOW);
    }
  }

  // --- Rendering ---
  if (!Taak_gedaan) {
    gif.playFrame(false, NULL); // Speel de GIF af
  }
}


// ------ Functie om een nieuwe GIF klaar te zetten ------
void loadtaakGif(int index) {
  gif.close();
  gif.begin(GIF_PALETTE_RGB565_BE);
  if (gif.open((uint8_t *)taakgifData[index], taakgifSizes[index], GIFDraw)) {
    gif.setDrawType(GIF_DRAW_COOKED);
    gif.allocFrameBuf(GIFAlloc);
  }
}

void loadhintGif(int index) {
  gif.close();
  gif.begin(GIF_PALETTE_RGB565_BE);
  if (gif.open((uint8_t *)hintgifData[index], hintgifSizes[index], GIFDraw)) {
    gif.setDrawType(GIF_DRAW_COOKED);
    gif.allocFrameBuf(GIFAlloc);
  }
}

// ------ knop-checker ------
// [!] Deze code werd gegenereerd door AI (gemini)
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