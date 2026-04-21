// Deze code werd gebaseerd op de Youtube Tutorial: https://youtu.be/omUWkUqFYrQ
// De code wordt aangepast voor dit project
// AI wordt gebruikt om de code te verbeteren

#include "esp_flash.h"
#include "esp_partition.h"
#include <bb_spi_lcd.h>
#include <AnimatedGIF.h>
#include "gif_files/JasjePixel.h" // Dit is de gif voor deze taak

#define GifData JasjePixel // Dit stuurt de GIF naar het scherm

BB_SPI_LCD tft;

void setup() {
  Serial.begin(115200);
  tft.begin(LCD_ILI9341, FLAGS_NONE, 40000000, 10, 9, 8, -1, 13, 11, 12);
  tft.setRotation(LCD_ORIENTATION_90); // Value=90 omdat de gifs van formaat zijn: 320x240, de gif's worden in landscape mode afgespeeld
  tft.fillScreen(TFT_BLACK);

  AnimatedGIF *gif;
  gif = openGif((uint8_t *)GifData, sizeof(GifData));
  if (gif == NULL) {
    Serial.println("De gif kan niet worden getoond"); // Controle punt: 
    while (true) {
    }
  }

  while (true) {
    gif->playFrame(false, NULL);
  }
}

void loop() {
  delay(1);
}

AnimatedGIF *openGif(uint8_t *gifdata, size_t gifsize) {
  AnimatedGIF *gif;
  gif = (AnimatedGIF *)malloc(sizeof(AnimatedGIF));
  if (gif == NULL) {
    Serial.println("Er is niet genoeg RAM om de gif af te laten spelen! Probeer de gif te comprimeren.");
    return NULL;
  }

  gif->begin(GIF_PALETTE_RGB565_BE);

  if (gif->open(gifdata, gifsize, GIFDraw)) {
    Serial.printf("gif is geopend; Canvas size = %d x %d\n", gif->getCanvasWidth(), gif->getCanvasHeight());
    Serial.printf("geheugen dat in beslag genomen is %ld (%2.2f MB)", gifsize, (float)gifsize / (1024 * 1024));
    gif->setDrawType(GIF_DRAW_COOKED);
    if (gif->allocFrameBuf(GIFAlloc) != GIF_SUCCESS) {
      Serial.println("Niet genoeg RAM geheugen voor de frame buffer!");
      return NULL;
    }
    return gif;
  }
  else 
  {
    // Hier stond in de tutorial een reeks van error messages, heb deze weg gelaten voor ons project
  }
}

void *GIFAlloc(uint32_t u32Size) {
  return malloc(u32Size);
}

void GIFFree(void *p) {
  free(p);
}

void GIFDraw(GIFDRAW *pDraw) {
  if (pDraw->y == 0) {
    tft.setAddrWindow(pDraw->iX, pDraw->iY, pDraw->iWidth, pDraw->iHeight);
  }
  tft.pushPixels((uint16_t *)pDraw->pPixels, pDraw->iWidth);
}