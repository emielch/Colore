#include <Colore.h>
#include <Adafruit_NeoPixel.h>

#define PIN 6
#define LED_AM 60
#define BEAM_AM 10

Beam beams[BEAM_AM];


Adafruit_NeoPixel leds = Adafruit_NeoPixel(LED_AM, PIN, NEO_GRB + NEO_KHZ800);

uint16_t leds1[] = {0,1,2,3,4};
uint16_t leds2[] = {5,6,7,8,9,10,11,12,13,14,15,16,17};

Segment seg1( sizeof(leds1)/2, leds1 );
Segment seg2( sizeof(leds2)/2, leds2 );

Colore colore;

Segment *segArray[] = {
  &seg1,
  &seg2,
};

void setup(){
  leds.begin();
  
  byte segArrayLen = sizeof(segArray)/2; // for arduino: /2, for teensy: /4
  colore.begin(LED_AM, segArray, segArrayLen, beams, BEAM_AM, &set_ledLib, &get_ledLib, &show_ledLib );
}


void loop(){
  colore.update();
}


void set_ledLib(int pixel, byte r, byte g, byte b){
  leds.setPixelColor(pixel,r,g,b);
}

void show_ledLib(){
  leds.show();
}

Color get_ledLib(int pixel){
  uint32_t conn = leds.getPixelColor(pixel);  // retrieve the color that has already been saved
  byte b = conn & 255;       // unpack the color
  byte g = conn >> 8 & 255;
  byte r = conn >> 16 & 255;
  Color pixelCol(r,g,b,RGB_MODE);
  return pixelCol;
}
