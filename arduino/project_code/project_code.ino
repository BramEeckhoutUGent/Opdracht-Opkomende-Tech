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
#define Blauwe_knop 16
#define Orange_knop 15

// --- LED's ---
#define led1 2 
#define led2 3
#define ledGreen 4


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

// --- GIF data voor de taakjes ---
#define GIF_COUNT 3
const uint8_t* taakgifData[GIF_COUNT] = {gif1, gif2, gif3};
const size_t taakgifSizes[GIF_COUNT] = {sizeof(gif1), sizeof(gif2), sizeof(gif3)};

// --- GIF data voor de hints ---
const uint8_t* hintgifData[GIF_COUNT] = {gif1, gif2, gif3};
const size_t hintgifSizes[GIF_COUNT] = {sizeof(gif1), sizeof(gif2), sizeof(gif3)};


// --- Debounce variabelen ---
// Deze code werd gegenereerd door AI (gemini)
int lastNextState = -1, nextState = -1;
unsigned long lastNextDebounce = 0;
int lastActionState = -1, actionState = -1;
unsigned long lastActionDebounce = 0;


// --- LED timers en status ---
unsigned long blink1Start = 0;
unsigned long blink2Start = 0;
unsigned long greenStart = 0;
bool blinking1 = false;
bool blinking2 = false;
bool greenOn = false;


// ----- code

void setup() {
  Serial.begin(115200);
  dfSS.begin(9600);
  
  pinMode(Blauwe_knop, INPUT_PULLUP);
  pinMode(Orange_knop, INPUT_PULLUP);
  
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  digitalWrite(led1, HIGH);
  digitalWrite(led2, HIGH);
  digitalWrite(ledGreen, LOW);
  
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
  unsigned long now = millis();

  // --- Optie 1: Ga door naar de volgende taak (blauwe knop + blauwe LED knipperen + groene LED) ---
  if (isButtonPressed(Volgende_knop, nextState, lastNextState, lastNextDebounce)) { // Deze code werd gegenereerd door AI (gemini)
    Huidige_taak++;                                                                 // |||||||||||||||||||||||||||||||||||||||||||
    Wordt_actie_getoond = false;                                                    // |||||||||||||||||||||||||||||||||||||||||||
    
    blinking1 = true;
    blink1Start = now;
    greenOn = true;
    greenStart = now;
    digitalWrite(ledGreen, HIGH);

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
      
      blinking2 = true;
      blink2Start = now;

      myDFPlayer.play(1); // Speel geluidje
      
      tft.fillScreen(TFT_BLACK);
      loadHintGif(Huidige_taak); // Hint wordt getoond 
    }
  }

if (blinking1) {
    if (now - blink1Start < 3000) {
      if ((now / 200) % 2 == 0) digitalWrite(led1, HIGH);
      else digitalWrite(led1, LOW);
    } else {
      blinking1 = false;
      digitalWrite(led1, HIGH);
    }
  }

if (blinking2) {
    if (now - blink2Start < 3000) {
      if ((now / 200) % 2 == 0) digitalWrite(led2, HIGH);
      else digitalWrite(led2, LOW);
    } else {
      blinking2 = false;
      digitalWrite(led2, HIGH);
    }
  }

  if (greenOn) {
    if (now - greenStart >= 2000) {
      greenOn = false;
      digitalWrite(ledGreen, LOW);
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

het groene lichtje op de neus moet oplichten wanneer er hij op het blauwe licht heeft gedrukt en dus de actie voltooid heeft

dus na actie, kiest hij voor hint (oranje lichtje knippert totdat blauw lichtje ingedrukt wordt) of voor volgende stap (groen lichtje brand kort)

// TEST: Interactie knoppen + LED feedback

// ------ INPUTS (knoppen) ------
const int buttonBlue = 16;     // blauwe knop (rechter oor)
const int buttonOrange = 15;   // gele/oranje knop (linker oor)

// ------ OUTPUTS (LEDs) ------
const int ledBlue = 4;         // blauwe LED
const int ledGreen = 5;        // groene LED (neus)
const int ledOrange = 6;       // gele/oranje LED
void setup() {
  Serial.begin(115200);

  pinMode(buttonBlue, INPUT_PULLUP);
  pinMode(buttonOrange, INPUT_PULLUP);

  pinMode(ledBlue, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(ledOrange, OUTPUT);

  // standaard toestand
  digitalWrite(ledBlue, HIGH);
  digitalWrite(ledOrange, HIGH);
  digitalWrite(ledGreen, LOW);
}

void loop() {

  // blauwe knop = bevestigen
  if (digitalRead(buttonBlue) == LOW) {
    Serial.println("Stap bevestigd");

    // groene LED = feedback
    digitalWrite(ledGreen, HIGH);
    delay(500);
    digitalWrite(ledGreen, LOW);

    Serial.println("Ga naar volgende stap");
    delay(500);
  }

  // oranje knop = hint
  if (digitalRead(buttonOrange) == LOW) {
    Serial.println("Hint gevraagd");

    // knipperende LED
    for (int i = 0; i < 6; i++) {
      digitalWrite(ledOrange, HIGH);
      delay(300);
      digitalWrite(ledOrange, LOW);
      delay(300);
    }

    Serial.println("Herhaal instructie");
  }
}
if (digitalRead(buttonBlue) == LOW) {
  Serial.println("Stap bevestigd");

  // Bevestigingsgeluid afspelen
  myDFPlayer.play(2);   // bv. 002.mp3 = confirm sound => juiste sound moet nog opgeslagen worden op sd kaartje

  // Groene LED (neus)
  digitalWrite(ledGreen, HIGH);
  delay(500);
  digitalWrite(ledGreen, LOW);

  // Volgende stap laden (GIF)
  tft.fillScreen(TFT_BLACK);
  showNextStep();  // moet nog gekoppeld worden aan juiste functie

  delay(300); // kleine buffer
}

testje:
void setup() {
  pinMode(2, OUTPUT);
}

void loop() {
  digitalWrite(2, HIGH);
  delay(100);
  digitalWrite(2, LOW);
  delay(100);
}

FINAL:
werkende code met groen bevestigingslicht bij indrukken blauw en flikkerende blauwe en gele lichtjes:
const int led1 = 17;
const int led2 = 18;
const int ledGreen = 8;

const int button1 = 4;  // blauwe knop
const int button2 = 5;

// Timers
unsigned long blink1Start = 0;
unsigned long blink2Start = 0;
unsigned long greenStart = 0;

// Status flags
bool blinking1 = false;
bool blinking2 = false;
bool greenOn = false;

void setup() {
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(ledGreen, OUTPUT);

  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);

  digitalWrite(led1, HIGH);
  digitalWrite(led2, HIGH);
  digitalWrite(ledGreen, LOW);
}

void loop() {
  unsigned long now = millis();

  // --- Knop 1 (blauw) ---
  if (digitalRead(button1) == LOW) {
    blinking1 = true;
    blink1Start = now;

    greenOn = true;
    greenStart = now;
    digitalWrite(ledGreen, HIGH);
  }

  // --- Knop 2 ---
  if (digitalRead(button2) == LOW) {
    blinking2 = true;
    blink2Start = now;
  }

  // --- LED1 knipperen 3 sec ---
  if (blinking1) {
    if (now - blink1Start < 3000) {
      if ((now / 200) % 2 == 0) digitalWrite(led1, HIGH);
      else digitalWrite(led1, LOW);
    } else {
      blinking1 = false;
      digitalWrite(led1, HIGH);
    }
  }

  // --- LED2 knipperen 3 sec ---
  if (blinking2) {
    if (now - blink2Start < 3000) {
      if ((now / 200) % 2 == 0) digitalWrite(led2, HIGH);
      else digitalWrite(led2, LOW);
    } else {
      blinking2 = false;
      digitalWrite(led2, HIGH);
    }
  }

  // --- Groen LED 2 sec aan ---
  if (greenOn) {
    if (now - greenStart >= 2000) {
      greenOn = false;
      digitalWrite(ledGreen, LOW);
    }
  }
}
