/*************************************************** 
  This is an example for the Adafruit VS1053 Codec Breakout

  Designed specifically to work with the Adafruit VS1053 Codec Breakout 
  ----> https://www.adafruit.com/products/1381

  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

// include SPI, MP3 and SD libraries
#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>

// These are the pins used for the music maker shield
#define SHIELD_RESET  -1      // VS1053 reset pin (unused!)
#define SHIELD_CS     7      // VS1053 chip select pin (output)
#define SHIELD_DCS    6      // VS1053 Data/command select pin (output)

// These are common pins between breakout and shield
#define CARDCS 4     // Card chip select pin
// DREQ should be an Int pin, see http://arduino.cc/en/Reference/attachInterrupt
#define DREQ 3       // VS1053 Data request, ideally an Interrupt pin

Adafruit_VS1053_FilePlayer musicPlayer = 
  // create shield-example object!
  Adafruit_VS1053_FilePlayer(SHIELD_RESET, SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);


////

//define variables 
const uint8_t pin1 = A0;
const uint8_t pin2 = A1;
const uint8_t pin3 = A2;
const uint8_t pin4 = A3;
const uint8_t pin5 = A4;
const uint8_t pin6 = A5;
const uint8_t pin7 = 8;
const uint8_t pin8 = 9;
byte pins = 0000;
int songSelect = 0;
int volume = 40;

/// File listing helper
void printDirectory(File dir, int numTabs) {
   while(true) {
     
     File entry =  dir.openNextFile();
     if (! entry) {
       // no more files
       //Serial.println("**nomorefiles**");
       break;
     }
     for (uint8_t i=0; i<numTabs; i++) {
       Serial.print('\t');
     }
     Serial.print(entry.name());
     if (entry.isDirectory()) {
       Serial.println("/");
       printDirectory(entry, numTabs+1);
     } else {
       // files have sizes, directories do not
       Serial.print("\t\t");
       Serial.println(entry.size(), DEC);
     }
     entry.close();
   }
}


void VolumeControl(){
    musicPlayer.setVolume(volume,volume);
}


void setup() {
  pinMode(pin1, INPUT);
  pinMode(pin2, INPUT);
  pinMode(pin3, INPUT);
  pinMode(pin4, INPUT);
  pinMode(pin5, INPUT);
  pinMode(pin6, INPUT);
  pinMode(pin7, INPUT);
  pinMode(pin8, INPUT);

  Serial.begin(9600);
  Serial.println("Adafruit VS1053 Library Test");

  // initialise the music player
  if (! musicPlayer.begin()) { // initialise the music player
     Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
     while (1);
  }
  Serial.println(F("VS1053 found"));

  musicPlayer.sineTest(0x44, 500);    // Make a tone to indicate VS1053 is working
 
  if (!SD.begin(CARDCS)) {
    Serial.println(F("SD failed, or not present"));
    while (1);  // don't do anything more
  }
  Serial.println("SD OK!");
  
  // list files
  printDirectory(SD.open("/"), 0);
  
  // Set volume for left, right channels. lower numbers == louder volume!
  musicPlayer.setVolume(volume,volume);

  /***** Two interrupt options! *******/ 
  // This option uses timer0, this means timer1 & t2 are not required
  // (so you can use 'em for Servos, etc) BUT millis() can lose time
  // since we're hitchhiking on top of the millis() tracker
  //musicPlayer.useInterrupt(VS1053_FILEPLAYER_TIMER0_INT);
  
  // This option uses a pin interrupt. No timers required! But DREQ
  // must be on an interrupt pin. For Uno/Duemilanove/Diecimilla
  // that's Digital #2 or #3
  // See http://arduino.cc/en/Reference/attachInterrupt for other pins
  // *** This method is preferred
  if (! musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT))
    Serial.println(F("DREQ pin is not an interrupt pin"));
}

void loop() {  
  int bit1 = digitalRead(pin1);
  int bit2 = digitalRead(pin2);
  int bit3 = digitalRead(pin3);
  int bit4 = digitalRead(pin4);
  int bit5 = digitalRead(pin5);
  int bit6 = digitalRead(pin6);
  int bit7 = digitalRead(pin7);
  int bit8 = digitalRead(pin8);

    if(bit1 == 1){
      //play track001
      delay(50);
      songSelect == 1;
      Serial.println("Playing Song #1 ");
      songSelect = 1;
      pickSong();
      //musicPlayer.startPlayingFile("track001.mp3");
    }
    if(bit2 == 1){
      delay(50);
      //play track002
      Serial.println("Playing Song #2 ");
      songSelect = 2;
      pickSong();
    }
    if(bit3 == 1){
      delay(50);
      //play track003
      Serial.println("Playing Song #3 ");
      songSelect = 3;
      pickSong();
    }
    if(bit4 == 1){
      delay(50);
      //play track004
      Serial.println("Playing Song #4 ");
      songSelect = 4;
      pickSong();
    }
    if(bit5 == 1){
      delay(50);
      //increase volume
      volume--;
      delay(100);
      VolumeControl();
      Serial.println("Volume Up!");
    }
    if(bit6 == 1){
      delay(50);
      //decrease volume
      volume++;
      delay(100);
      VolumeControl();
      Serial.println("Volume Down!");
    }
    if(bit7 == 1){
      delay(50);
      //Skip to Previous Song
      Serial.println("Skipping to Previous Song!");
      songSelect--;
      delay(100);
      pickSong();
      delay(100);
    }
    if(bit8 == 1){
      delay(50);
      //Skip to Next Song
      Serial.println("Skipping to Next Song!");
      songSelect++;
      delay(100);
      pickSong();
      delay(100);
    }
  
} 

void pickSong(){
  if(songSelect == 1){
    musicPlayer.startPlayingFile("track001.mp3");
  }
  else if (songSelect == 2){
    musicPlayer.startPlayingFile("track002.mp3");
  }
  else if (songSelect == 3){
    musicPlayer.startPlayingFile("track003.mp3");
  }
  else if (songSelect == 4){
    musicPlayer.startPlayingFile("track004.mp3");
  }
}

