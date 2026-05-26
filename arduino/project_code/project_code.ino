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
#define TFT_LED   21
#define TFT_MISO  13
#define TFT_MOSI  11
#define TFT_CLK   12

// --- DFPlayer Mini ---
#define DF_RX 16 // verbind met TX van de DFPlayer mini
#define DF_TX 17 // verbind met RX van de DFPlayer mini (+ 1k weerstand als VCC aangesloten is op 5V)

// --- Drukknoppen ---
#define Blauwe_knop 15
#define Orange_knop 5

// --- LED's ---
#define Blauwe_led 4
#define Orange_led 7
#define Groene_led 6


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
  Serial2.begin(9600, SERIAL_8N1, DF_RX, DF_TX);
  
  if (!myDFPlayer.begin(Serial2)) { // kleine status controle. Ik merk op dat de dfplayer op 3.3V niet altijd werkt. Kan opgelost worden door serial monitor eens aan/uit te doen.
    Serial.println("DFPlayer error: Check verbinding en SD-kaart!");
  } else {
    Serial.println("DFPlayer online!");
  }
  myDFPlayer.volume(30); // volume gaat van 0 t.e.m. 30

  if (psramInit()) { // we werken niet met de SD kaartlezer op het TFT scherm. We werken met het PSRAM (8Mb) van de ESP32. Dit is een kleine statuscontrole.
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

  pinMode(TFT_LED, OUTPUT);
  digitalWrite(TFT_LED, HIGH);
  
  tft.begin(LCD_ILI9341, FLAGS_NONE, 40000000, TFT_CS, TFT_DC, TFT_RST, TFT_LED, TFT_MISO, TFT_MOSI, TFT_CLK);
  tft.setRotation(LCD_ORIENTATION_90);
  tft.fillScreen(TFT_BLACK);

  loadtaakGif(Huidige_taak); // Dit toont de eerste taak op het scherm
}

void loop() {
  if (Taak_gedaan) return; // Soms heeft de esp32 moeilijk met switchen van gifs. Daarom heb ik hier een korte return funcie gezet.
  unsigned long nu = millis();

  // --- Optie 1: Ga door naar de volgende taak (blauwe knop + blauwe LED knipperen + groene LED) ---
  if (isButtonPressed(Blauwe_knop, nextState, lastNextState, lastNextDebounce)) { // [!] Deze code werd gegenereerd door AI (gemini)
    Huidige_taak++;                                                               // |||||||||||||||||||||||||||||||||||||||||||||||
    Wordt_actie_getoond = false;                                                  // |||||||||||||||||||||||||||||||||||||||||||||||
    
    myDFPlayer.play(1); // speelt geluidje dat taak gedaan is

    Knipper_Blauw_On = true;
    Knipper_Blauw_Start = nu;
    Groen_On = true;
    Groen_Start = nu;
    digitalWrite(Groene_led, HIGH);

    if (Huidige_taak < GIF_COUNT) { // zorgt voor opvolging van taak 1 naar 3. Zal na de derde taak een zwart scherm geven.
      Serial.printf("Naar taak %d\n", Huidige_taak + 1); // [!] Deze code werd gegenereerd door AI (gemini)
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
  if (!Taak_gedaan && isButtonPressed(Orange_knop, actionState, lastActionState, lastActionDebounce)) { // [!] Deze code werd gegenereerd door AI (gemini)
    if (!Wordt_actie_getoond) {                                                                         // |||||||||||||||||||||||||||||||||||||||||||||||
      Serial.println("Kindje wil extra hulp");                                                          // |||||||||||||||||||||||||||||||||||||||||||||||
      Wordt_actie_getoond = true;                                                                       // |||||||||||||||||||||||||||||||||||||||||||||||
      
      Knipper_Orange_On = true;
      Knipper_Orange_Start = nu;

      myDFPlayer.play(Huidige_taak + 2); // speelt het geluidsbestand dat overeenkomt met de taak
      delay(100); // kleine pauze toegevoegd omdat signaal van gif en audio tegelijk verzonden wordt. Volgens AI heeft dat te maken met te veel informatie in de SPI-bus?
      yield();
      
      tft.fillScreen(TFT_BLACK); // altijd bij het switchen van gifs eerst kort een zwart scherm toevoegen, dat helpt bij het renderen.
      loadhintGif(Huidige_taak);
      return;
    }
  }

// --- LED timers en status updates ---
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


// ------ Functie om een nieuwe taak GIF klaar te zetten ------
void loadtaakGif(int index) {                                                   // [!] Deze code werd gegenereerd door AI (gemini)
  Serial.println("Vrijmaken geheugen...");
  gif.close();
  gif.freeFrameBuf(GIFFree);
  delay(100); 
  yield();
  Serial.printf("PSRAM vrij voor start: %d bytes\n", ESP.getFreePsram());       // [!] Deze code werd gegenereerd door AI (gemini)
  gif.begin(GIF_PALETTE_RGB565_BE);                                             // |||||||||||||||||||||||||||||||||||||||||||||||
  if (gif.open((uint8_t *)taakgifData[index], taakgifSizes[index], GIFDraw)) {  // |||||||||||||||||||||||||||||||||||||||||||||||
    gif.setDrawType(GIF_DRAW_COOKED);                                           // |||||||||||||||||||||||||||||||||||||||||||||||
    if (gif.allocFrameBuf(GIFAlloc) != GIF_SUCCESS) {                           // |||||||||||||||||||||||||||||||||||||||||||||||
      Serial.println("PSRAM Allocatie mislukt!");                               // |||||||||||||||||||||||||||||||||||||||||||||||
    } else {                                                                    // |||||||||||||||||||||||||||||||||||||||||||||||
      Serial.println("Taak GIF geladen!");                                      // |||||||||||||||||||||||||||||||||||||||||||||||
    }
  }
}

// ------ Functie om een nieuwe hint GIF klaar te zetten ------
void loadhintGif(int index) {                                                   // [!] Deze code werd gegenereerd door AI (gemini)
  Serial.println("Vrijmaken geheugen...");
  gif.close();
  gif.freeFrameBuf(GIFFree);
  delay(100); 
  yield();
  Serial.printf("PSRAM vrij voor start: %d bytes\n", ESP.getFreePsram());       // [!] Deze code werd gegenereerd door AI (gemini)
  gif.begin(GIF_PALETTE_RGB565_BE);                                             // |||||||||||||||||||||||||||||||||||||||||||||||
  if (gif.open((uint8_t *)hintgifData[index], hintgifSizes[index], GIFDraw)) {  // |||||||||||||||||||||||||||||||||||||||||||||||
    gif.setDrawType(GIF_DRAW_COOKED);                                           // |||||||||||||||||||||||||||||||||||||||||||||||
    if (gif.allocFrameBuf(GIFAlloc) != GIF_SUCCESS) {                           // |||||||||||||||||||||||||||||||||||||||||||||||
      Serial.println("PSRAM Allocatie mislukt!");                               // |||||||||||||||||||||||||||||||||||||||||||||||
    } else {                                                                    // |||||||||||||||||||||||||||||||||||||||||||||||
      Serial.println("Hint GIF geladen!");                                      // |||||||||||||||||||||||||||||||||||||||||||||||
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
void *GIFAlloc(uint32_t u32Size) { return heap_caps_malloc(u32Size, MALLOC_CAP_SPIRAM); } // [!] Deze code werd gegenereerd door AI (gemini)
void GIFFree(void *p) { heap_caps_free(p); }                                              // |||||||||||||||||||||||||||||||||||||||||||||||                

// Dit is de manier van de gif tekenen. Deels gevolgd uit de tutorial, ook AI.
void GIFDraw(GIFDRAW *pDraw) {
  if (pDraw->iY + pDraw->y >= tft.height() || pDraw->iX >= tft.width()) return;
  tft.pushImage(
    pDraw->iX,
    pDraw->iY + pDraw->y,
    pDraw->iWidth,
    1,
    (uint16_t *)pDraw->pPixels
  );
}