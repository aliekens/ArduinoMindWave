#include "ThinkGearStreamParser.h"
#include <SoftwareSerial.h>   //Software Serial Port

ThinkGearStreamParser parser;
 
#include <SoftwareSerial.h>     // library for software serial

SoftwareSerial bluetoothSerial(5, 6);  // RX, TX pins from the bluetooth module (here, we used a bluesmirf)

void setup(){
  Serial.begin( 9600 ); // USB serial monitor to Arduino
  bluetoothSerial.begin( 57600 ); // bluetooth serial to NeuroSky MindWave
  THINKGEAR_initParser(&parser, handleDataValueFunc, NULL); // assign the handlers for the parser
  Serial.flush();
} 

void loop(){
  while(!bluetoothSerial.available()) delay(4); // wait for a byte from the bluetooth connection
  THINKGEAR_parseByte(&parser, bluetoothSerial.read()); // forward the byte to the stream parser
} 

void handleDataValueFunc(unsigned char extendedCodeLevel, unsigned char code, unsigned char valueLength, const unsigned char *value) {
 
    if( extendedCodeLevel == 0 ) {
 
        switch( code ) {
 
            /* [CODE]: ATTENTION eSense */
            case( 0x04 ):
                Serial.print( "Attention Level: " );
                Serial.println( value[0] & 0xFF );
                break;
 
            /* [CODE]: MEDITATION eSense */
            case( 0x05 ):
                Serial.print( "Meditation Level: " );
                Serial.println( value[0] & 0xFF );
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
