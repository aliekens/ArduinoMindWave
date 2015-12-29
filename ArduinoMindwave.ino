#define NEOPIXELRING

#include "ThinkGearStreamParser.h"
ThinkGearStreamParser parser;

#ifdef NEOPIXELRING
#include <Adafruit_NeoPixel.h>

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, 3, NEO_GRB + NEO_KHZ800);
#endif

#include <SoftwareSerial.h>     // library for software serial
SoftwareSerial bluetoothSerial(5, 6);  // RX, TX pins from the bluetooth module (here, we used a bluesmirf)

void setup() {
  Serial.begin( 57600 ); // USB serial monitor to Arduino
  bluetoothSerial.begin( 57600 ); // bluetooth serial to NeuroSky MindWave
  THINKGEAR_initParser(&parser, handleDataValueFunc, NULL); // assign the handlers for the parser
  Serial.flush();

#ifdef NEOPIXELRING
  strip.begin();
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color( 0, 100, 0 ) );
  }
  strip.show(); // Initialize all pixels to 'off'
#endif
}

int timeOfLastSignal = millis();
int redSet = false;
int loopCounter = 0;

void loop() {
  while (!bluetoothSerial.available()) {
    loopCounter++;
    if( loopCounter % 1000 == 0 ) {
      int currentTime = millis();
      if( !redSet && ( currentTime > timeOfLastSignal + 10000 ) ) {
        for( int i = 0; i < strip.numPixels(); i++ )
          strip.setPixelColor(i, strip.Color( 255, 0, 0 ) );
        redSet = true;
        strip.show(); // Initialize all pixels to 'off'
      }
    }
  }
  THINKGEAR_parseByte(&parser, bluetoothSerial.read()); // forward the byte to the stream parser
}

int attention = 0;
int attentioncurrent = 0;
int meditation = 0;
int meditationcurrent = 0;

void updateRing1() {
  for ( int j = 0; j < 20; j++ ) {
    attentioncurrent = 0.9 * attentioncurrent + 0.1 * ( attention * 16 );
    for (uint16_t i = 0; i < strip.numPixels(); i++) {
      int red = 0;
      int green = 0;
      int blue = 0;
      if ( attentioncurrent / 100 > i )
        blue = 100;
      if ( attentioncurrent / 100 == i )
        blue = attentioncurrent % 100;
      if ( attentioncurrent / 200 == 7 ) {
        green = ( attentioncurrent % 200 ) / 2;
        red = ( attentioncurrent % 200 ) / 2;
      }
      strip.setPixelColor(i, strip.Color( red, green, blue ) );
    }
    strip.show();
    delay( 20 );
  }
}

uint32_t Wheel(byte WheelPos, float brightness) {
  if(WheelPos < 85) {
   return strip.Color(brightness * WheelPos * 3, brightness * ( 255 - WheelPos * 3 ), 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(brightness * ( 255 - WheelPos * 3 ), 0, brightness * WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, brightness * WheelPos * 3, brightness * ( 255 - WheelPos * 3 ) );
  }
}

void updateRing3() {
  for ( int j = 0; j < 20; j++ ) {
    attentioncurrent = 0.9 * attentioncurrent + 0.1 * ( attention * 16 );
    for (uint16_t i = 0; i < strip.numPixels(); i++) {
      float brightness = 0.0;
      if ( attentioncurrent / 100 > i )
        brightness = 1.0;
      if ( attentioncurrent / 100 == i )
        brightness = ( attentioncurrent % 100 ) / 100.0;
      strip.setPixelColor(i, Wheel( i * 16, brightness ) );
    }
    strip.show();
    delay( 20 );
  }
}

void updateLED() {
  attentioncurrent = 0.9 * attentioncurrent + 0.1 * ( attention );
  analogWrite( 10, applyGammaCorrection( attentioncurrent * 2.55 ) );
}

uint8_t gammaCorrection[ 256 ] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10, 10, 11, 11, 11, 12, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 23, 23, 24, 24, 25, 26, 26, 27, 28, 28, 29, 30, 30, 31, 32, 33, 33, 34, 35, 36, 37, 37, 38, 39, 40, 41, 42, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 61, 62, 63, 64, 65, 66, 67, 69, 70, 71, 72, 74, 75, 76, 77, 79, 80, 81, 83, 84, 86, 87, 88, 90, 91, 93, 94, 96, 97, 99, 100, 102, 103, 105, 107, 108, 110, 111, 113, 115, 116, 118, 120, 122, 123, 125, 127, 129, 130, 132, 134, 136, 138, 140, 142, 144, 146, 148, 150, 152, 154, 156, 158, 160, 162, 164, 166, 168, 170, 172, 175, 177, 179, 181, 184, 186, 188, 191, 193, 195, 198, 200, 202, 205, 207, 210, 212, 215, 217, 220, 222, 225, 227, 230, 233, 235, 238, 241, 243, 246, 249, 252, 255 };

uint8_t applyGammaCorrection( uint8_t input ) {
  return gammaCorrection[ input ];
}

int counter = 0;

void updateRing2() {
  counter += 1;
  attentioncurrent = 0.9 * attentioncurrent + 0.1 * attention;
  for (int i = 0; i < strip.numPixels(); i++) {
    int brightness = attentioncurrent * 2.55;
    brightness = applyGammaCorrection( brightness );
    strip.setPixelColor(i, strip.Color(
                          brightness * ( 0.5 + sin( i + counter / 3.0 ) / 2 ),
                          brightness * ( 0.5 + cos( i + counter / 4.0 ) / 2 ),
                          brightness * ( 0.5 + sin ( i - counter / 5.0 ) / 2 )
                        ) );
  }
  strip.show();
}

void updateAttention( int value ) {
  attention = value;
  updateRing3();
}

void updateMeditation( int value ) {
  meditation = value;
}

void handleDataValueFunc(unsigned char extendedCodeLevel, unsigned char code, unsigned char valueLength, const unsigned char *value) {

  if ( extendedCodeLevel == 0 ) {

    switch ( code ) {

      /* [CODE]: ATTENTION eSense */
      case ( 0x04 ):
        Serial.println( value[0] & 0xFF );
        if ( ( value[ 0 ] > 5 ) && ( value[ 0 ] < 100 ) ) {
          //Serial.print( "Attention Level: " );
          timeOfLastSignal = millis();
          redSet = false;
          updateAttention( value[ 0 ] );
        }
        if( value[ 0 ] == 0 ) {
          timeOfLastSignal = millis();
          for( int i = 0; i < strip.numPixels(); i++ )
            strip.setPixelColor(i, strip.Color( 0, 0, 255 ) );
          strip.show();
        }
        break;

      /* [CODE]: MEDITATION eSense */
      case ( 0x05 ):
        //Serial.print( "Meditation Level: " );
        //Serial.println( value[0] & 0xFF );
        updateMeditation( value[ 0 ] );
        break;

        /* Other [CODE]s */
        /*default:
            Serial.print( "EXCODE level: ");
            Serial.print( extendedCodeLevel );
            Serial.print( " code: " );
            Serial.print( code );
            Serial.print( " vLength: " );
            Serial.print( valueLength );
            Serial.print( " value(s):" );
            for( int i=0; i<valueLength; i++ ) {
              Serial.print( value[i] & 0xFF );
              Serial.print( " " );
            }
            Serial.println();*/
    }

  }
}
