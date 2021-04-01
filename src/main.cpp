#include <Arduino.h>
#include <FastLED.h>
#include <SPI.h>
#include <SD.h>

const int numColumns = 32;
const int numRows = 8;
const int numLeds = numColumns * numRows;
const int dataPin = 6;
const int chipSelect = 4;

File file;
CRGB buffer[numLeds];
CRGB leds[numLeds];

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  FastLED.addLeds<NEOPIXEL, dataPin>(leds, numLeds);

  for (int y = 0; y < numLeds; y++)
  {
    leds[y] = CRGB::Black;
  }

  FastLED.show();

  if (!SD.begin(chipSelect)) {
    digitalWrite(LED_BUILTIN, HIGH);
    return;
  }

  File root = SD.open("/");

  while (true) {
    File entry = root.openNextFile();

    if (!entry) {
      // No more files here.
      digitalWrite(LED_BUILTIN, HIGH);
      break;
    }

    if (!entry.isDirectory()) {
      // Found a file.
      file = entry;
      break;
    }
  }  
}

int calculateSource(int column, int row) {
  return column + row * numColumns;
}

int calculateTarget(int column, int row) {
  if (column % 2 == 0) {
      return row + column * numRows; 
  } else {
      return (numRows - row - 1) + column * numRows;
  }  
}

void loop() {
  if (file.seek(0)) {
    while (file.available()) {
      file.readBytes((char*) buffer, numLeds*3);

      for (int column = 0; column < numColumns; ++column) {
        for (int row = 0; row < numRows; ++row) {
          int source = calculateSource(column, row);
          int target = calculateTarget(column, row);;
          leds[target] = buffer[source].nscale8(16);
        }
      }

      FastLED.show();
      delay(27);
    }    
  } else {
     digitalWrite(LED_BUILTIN, HIGH);
  }
}