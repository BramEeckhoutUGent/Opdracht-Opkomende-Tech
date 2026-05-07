// De code voor het tonen van GIF's werd deels gebaseerd op de Youtube Tutorial: https://youtu.be/omUWkUqFYrQ
// De code voor het afspelen van audio bestanden via de DFPlayer mini werd deels gebaseerd op de Youtube Tutorial: https://www.youtube.com/watch?v=P42ICrgAtS4 
// De code voor dit project werd zo veel mogelijk zelf geschreven, AI werd gebruikt om de code te verbeteren.
// Code die door AI geschreven werd, wordt aangegeven in de lijn.


// ------ Installeren van libraries ------
#include "esp_flash.h"
#include "esp_partition.h"
#include <bb_spi_lcd.h>
#include <AnimatedGIF.h>
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
#include "gif_files/Hint_1_Opruimen.h"
#include "gif_files/Hint_2_Tandenpoetsen.h"
#include "gif_files/Hint_3_Jasje.h"
#define GifData Hint_1_Opruimen
#define GifData Hint_2_Tandenpoetsen
#define GifData Hint_3_Jasje


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
#define DF_RX 18 // verbind met TX van de DFPlayer mini
#define DF_TX 17 // verbind met RX van de DFPlayer mini + 1k weerstand

// --- Drukknoppen ---
#define Blauwe_knop 3
#define Orange_knop 7

// --- LED's ---
#define Blauwe_led 4 
#define Orange_led 6
#define Groene_led 5


// ------ Definieer variabelen -------
// --- Objecten ---
BB_SPI_LCD tft;
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
const uint8_t* hintgifData[GIF_COUNT] = {Hint_1_Opruimen, Hint_2_Tandenpoetsen, Hint_3_Jasje};
const size_t hintgifSizes[GIF_COUNT] = {sizeof(Hint_1_Opruimen), sizeof(Hint_2_Tandenpoetsen), sizeof(Hint_3_Jasje)};

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
  Serial2.begin(9600, SERIAL_8N1, 18, 17);
  
  if (!myDFPlayer.begin(Serial2)) {
    Serial.println("DFPlayer error: Check verbinding en SD-kaart!");
  } else {
    Serial.println("DFPlayer online!");
  }
  myDFPlayer.volume(20);

  if (psramInit()) {
    Serial.println("PSRAM succesvol geïnitialiseerd!");
  } else {
    Serial.println("PSRAM niet gevonden!");
  }
  
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

  loadtaakGif(Huidige_taak); // Dit toont de eerste taak op het scherm
}

void loop() {
  if (Taak_gedaan) return;
  unsigned long nu = millis();

  // --- Optie 1: Ga door naar de volgende taak (blauwe knop + blauwe LED knipperen + groene LED) ---
  if (isButtonPressed(Blauwe_knop, nextState, lastNextState, lastNextDebounce)) { // Deze code werd gegenereerd door AI (gemini)
    Huidige_taak++;                                                                 // |||||||||||||||||||||||||||||||||||||||||||
    Wordt_actie_getoond = false;                                                    // |||||||||||||||||||||||||||||||||||||||||||
    
    myDFPlayer.play(1);

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
    return;
  }

  // --- Optie 2: Geef extra instructies (oranje knop + oranje LED) ---
  if (!Taak_gedaan && isButtonPressed(Orange_knop, actionState, lastActionState, lastActionDebounce)) { // Deze code werd gegenereerd door AI (gemini)
    if (!Wordt_actie_getoond) {                                                                        // |||||||||||||||||||||||||||||||||||||||||||
      Serial.println("Kindje wil extra hulp");                                                         // |||||||||||||||||||||||||||||||||||||||||||
      Wordt_actie_getoond = true;                                                                      // |||||||||||||||||||||||||||||||||||||||||||
      
      Knipper_Orange_On = true;
      Knipper_Orange_Start = nu;

      myDFPlayer.play(Huidige_taak + 1);
      delay(100); 
      yield();
      
      tft.fillScreen(TFT_BLACK);
      loadhintGif(Huidige_taak);
      return;
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
    if (gif.getCanvasWidth() > 0) {
      gif.playFrame(false, NULL);
    }
  }
}


// ------ Functie om een nieuwe GIF klaar te zetten ------
void loadtaakGif(int index) {
  Serial.println("Vrijmaken geheugen...");
  gif.close();
  gif.freeFrameBuf(GIFFree);
  delay(100); 
  yield();
  Serial.printf("PSRAM vrij voor start: %d bytes\n", ESP.getFreePsram());
  gif.begin(GIF_PALETTE_RGB565_BE);
  if (gif.open((uint8_t *)taakgifData[index], taakgifSizes[index], GIFDraw)) {
    gif.setDrawType(GIF_DRAW_RAW);
    if (gif.allocFrameBuf(GIFAlloc) != GIF_SUCCESS) {
      Serial.println("PSRAM Allocatie mislukt!");
    } else {
      Serial.println("Taak GIF geladen!");
    }
  }
}

void loadhintGif(int index) {
  Serial.println("Vrijmaken geheugen...");
  gif.close();
  gif.freeFrameBuf(GIFFree);
  delay(100); 
  yield();
  Serial.printf("PSRAM vrij voor start: %d bytes\n", ESP.getFreePsram());
  gif.begin(GIF_PALETTE_RGB565_BE);
  if (gif.open((uint8_t *)hintgifData[index], hintgifSizes[index], GIFDraw)) {
    gif.setDrawType(GIF_DRAW_COOKED);
    if (gif.allocFrameBuf(GIFAlloc) != GIF_SUCCESS) {
      Serial.println("PSRAM Allocatie mislukt!");
    } else {
      Serial.println("Hint GIF geladen!");
    }
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

void *GIFAlloc(uint32_t u32Size) { return heap_caps_malloc(u32Size, MALLOC_CAP_SPIRAM); }
void GIFFree(void *p) { heap_caps_free(p); }
void GIFDraw(GIFDRAW *pDraw) {
  uint8_t *s;
  uint16_t *d, *pPal;
  static uint16_t lineBuffer[320];

  if (pDraw->y >= tft.height() || pDraw->iX >= tft.width()) return;

  s = pDraw->pPixels;
  pPal = pDraw->pPalette;
  
  for (int x = 0; x < pDraw->iWidth; x++) {
    uint16_t c = pPal[s[x]];
    lineBuffer[x] = (c >> 8) | (c << 8); 
  }

  tft.pushImage(pDraw->iX, pDraw->iY + pDraw->y, pDraw->iWidth, 1, lineBuffer);
}