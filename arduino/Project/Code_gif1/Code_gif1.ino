#include "esp_flash.h"
#include "esp_partition.h"
#include <bb_spi_lcd.h>
#include <AnimatedGIF.h>
#include "JasjePixel.h"

BB_SPI_LCD tft;
AnimatedGIF gif;

#define GifData JasjePixel 

void *GIFAlloc(uint32_t u32Size) {
    return heap_caps_malloc(u32Size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
}

void GIFFree(void *p) {
    heap_caps_free(p);
}


void GIFDraw(GIFDRAW *pDraw) {
    uint16_t *usPixels = (uint16_t *)pDraw->pPixels;
    int x = pDraw->iX;
    int y = pDraw->iY + pDraw->y; // Huidige regel
    int w = pDraw->iWidth;
    if (pDraw->y == 0) {
        tft.setAddrWindow(x, pDraw->iY, w, pDraw->iHeight);
    }
    tft.pushPixels(usPixels, w);
}

void setup() {
    Serial.begin(115200);
    if (psramFound()) {
        Serial.printf("PSRAM Gevonden! Totaal: %d bytes\n", ESP.getPsramSize());
    } else {
        Serial.println("WAARSCHUWING: PSRAM NIET gevonden!");
    }      
    tft.begin(LCD_ILI9341, FLAGS_NONE, 40000000, 10, 9, 8, -1, 12, 11, 13);
    tft.setRotation(LCD_ORIENTATION_90);
    tft.fillScreen(TFT_BLACK);

    gif.begin(GIF_PALETTE_RGB565_BE);
}

void loop() {
    if (gif.open((uint8_t *)GifData, sizeof(GifData), GIFDraw)) {
        if (gif.allocFrameBuf(GIFAlloc) == GIF_SUCCESS) {
            while (gif.playFrame(true, NULL)) {
                yield(); 
            }
            gif.freeFrameBuf(GIFFree);
        } else {
            while (gif.playFrame(true, NULL)) { yield(); }
        }
        gif.close();
    }
    delay(1000);
}