#include <TFT_eSPI.h>       // Hardware-specific library

TFT_eSPI tft = TFT_eSPI();  // Invoke custom library

// Include the header files that contain the icons
#include "equal.h"


void setup()
{
  Serial.begin(115200);
  tft.begin();
  tft.setRotation(1);	// landscape

  tft.fillScreen(TFT_BLACK);

  // Swap the colour byte order when rendering
  tft.setSwapBytes(true);

  // Draw the icons
  tft.pushImage(140, 100, 60, 60, stop);

}

void loop()
{
  // do nothing
}