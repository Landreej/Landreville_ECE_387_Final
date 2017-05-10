#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware-specific library
#include <SPI.h>
#include "Adafruit_BLE_UART.h"

#define ADAFRUITBLE_REQ 10
#define ADAFRUITBLE_RST 7
#define ADAFRUITBLE_RDY 2

Adafruit_BLE_UART uart = Adafruit_BLE_UART(ADAFRUITBLE_REQ, ADAFRUITBLE_RDY, ADAFRUITBLE_RST);

unsigned long time = 0l;
boolean connection = false;
uint8_t btm = 65;
uint8_t out = btm;
uint8_t cap = 90;

//LED matrix set up
#define CLK 11  // MUST be on PORTB! (Use pin 11 on Mega)
#define OE  9
#define LAT 10
#define A   A0
#define B   A1
#define C   A2
#define D   A3
RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, false);
uint16_t black = matrix.Color444(0, 0, 0);
uint16_t white = matrix.Color444(15, 15, 15);
uint16_t yellow = matrix.Color444(15, 15, 0);

//Define Output Pins
const int pin1 = 40;
const int pin2 = 41;
const int pin3 = 42;
const int pin4 = 43;
const int pin5 = 44;
const int pin6 = 45;
const int pin7 = 46;
const int pin8 = 47;

int songSelect = 0;
int volume = 100;

#define persec 30
#define sendat (1000/persec)

/**************************************************************************/
/*!
 This function is called whenever select ACI events happen
 */
/**************************************************************************/
void aciCallback(aci_evt_opcode_t event)
{
  switch(event)
  {
  case ACI_EVT_DEVICE_STARTED:
    Serial.println(F("Advertising started"));
    break;
  case ACI_EVT_CONNECTED:
    Serial.println(F("Connected!"));
    connection = true;
    break;
  case ACI_EVT_DISCONNECTED:
    Serial.println(F("Disconnected or advertising timed out"));
    connection = false;
    break;
  default:
    break;
  }
}

/**************************************************************************/
/*!
 This function is called whenever data arrives on the RX channel
 */
/**************************************************************************/
void rxCallback(uint8_t *buffer, uint8_t len)
{
  //  char pfx[2] = { (char)buffer[0], (char)buffer[1] };
  if ((char)buffer[0] == '!') {  //Sensor data flag
    switch ((char)buffer[1]) {
    case 'Q':
      if (checkCRC(buffer) == false) { break; }
      Serial.println("Quaternion Data:");
      printQuaternionData(buffer);
      break;
    case 'A':
      if (checkCRC(buffer) == false) { break; }
      Serial.println("Accelerometer Data:");
      printSensorDataXYZ(buffer);
      break; 
    case 'G':
      if (checkCRC(buffer) == false) { break; }
      Serial.println("Gyro Data:");
      printSensorDataXYZ(buffer);
      break;
    case 'M':
      if (checkCRC(buffer) == false) { break; }
      Serial.println("Magnetometer Data:");
      printSensorDataXYZ(buffer);
      break;
    case 'L':
      if (checkCRC(buffer) == false) { break; }
      Serial.println("Location Data:");
      printLocationData(buffer);
      break;
    case 'B':
      if (checkCRC(buffer) == false) { break; }
      
      Serial.print("Button ");
      Serial.print((char)buffer[2]);
      Serial.print(" ");
      printButtonState((char)buffer[3]);
      Serial.println(" ");
      if((char)buffer[2] == '1' && (char)buffer[3] == '0'){
        Serial.println("Playing Song #1");
        //set a pin high to other arduino
        digitalWrite(pin1, HIGH);
        delay(50);
        digitalWrite(pin1, LOW);
        songSelect = 1;
        displayInfo();
      }
      else if((char)buffer[2] == '2' && (char)buffer[3] == '0'){
        Serial.println("Playing Song #2");
        //set a pin high to other arduino
        digitalWrite(pin2, HIGH);
        delay(50);
        digitalWrite(pin2, LOW);
        songSelect = 2;
        displayInfo();
      }
      else if((char)buffer[2] == '3' && (char)buffer[3] == '0'){
        Serial.println("Playing Song #3");
        //set a pin high to other arduino
        digitalWrite(pin3, HIGH);
        delay(50);
        digitalWrite(pin3, LOW);
        songSelect = 3;
        displayInfo();
      }
      else if((char)buffer[2] == '4' && (char)buffer[3] == '0'){
        Serial.println("Playing Song #4");
        //set a pin high to other arduino
        digitalWrite(pin4, HIGH);
        delay(50);
        digitalWrite(pin4, LOW);
        songSelect = 4;
        displayInfo();
      }
      else if((char)buffer[2] == '5' && (char)buffer[3] == '0'){
        Serial.println("Turning Volume Up");
        //set turn up pin high
        digitalWrite(pin5, HIGH);
        delay(50);
        digitalWrite(pin5, LOW);
        volume++;
        displayInfo();
      }
      else if((char)buffer[2] == '6' && (char)buffer[3] == '0'){
        Serial.println("Turning Volume Down");
        //set turn down pin high
        digitalWrite(pin6, HIGH);
        delay(50);
        digitalWrite(pin6, LOW);
        volume--;
        displayInfo();
      }
      else if((char)buffer[2] == '7' && (char)buffer[3] == '0'){
        Serial.println("Switching to Previous Song");
        //set back pin high
        digitalWrite(pin7, HIGH);
        delay(50);
        digitalWrite(pin7, LOW);
        if(songSelect == 1){
          songSelect == 4;
        }
        else{
        songSelect--;
        }
        displayInfo();
      }
      else if((char)buffer[2] == '8' && (char)buffer[3] == '0'){
        Serial.println("Switching to Next Song");
        //set next pin high
        digitalWrite(pin8, HIGH);
        delay(50);
        digitalWrite(pin8, LOW);
        if(songSelect == 4){
          songSelect == 1;
        }
        else{
        songSelect++;
        }
        displayInfo();
      }
      break;
      
    case 'C':
      if (checkCRC(buffer) == false) { break; }
      Serial.println("Color ");
      printColorData(buffer);
      break;
    default:
      Serial.println("Unknown Data Type");
      break;
    }
    Serial.println("");
  }

  /* Echo the same data back! */
  //  uart.write(buffer, len);
}

/**************************************************************************/
/*!
 Configure the Arduino and start advertising with the radio
 */
/**************************************************************************/
void setup(void)
{ 
  Serial.begin(9600);
  Serial.println(F("Arduino setup"));

  pinMode(pin1, OUTPUT);
  pinMode(pin2, OUTPUT);
  pinMode(pin3, OUTPUT);
  pinMode(pin4, OUTPUT);
  pinMode(pin5, OUTPUT);
  pinMode(pin6, OUTPUT);
  pinMode(pin7, OUTPUT);
  pinMode(pin8, OUTPUT);

  uart.setRXcallback(rxCallback);
  uart.setACIcallback(aciCallback);
  uart.begin();
}

/**************************************************************************/
/*!
 Constantly checks for new events on the nRF8001
 */
/**************************************************************************/
void loop()
{

  uart.pollACI();

}


void printSensorDataXYZ(uint8_t *buffer) {

  float x = *( (float*)(buffer + 2) );
  Serial.print("x = ");
  Serial.println(x, 7);

  float y = *( (float*)(buffer + 6) );
  Serial.print("y = ");
  Serial.println(y, 7);

  float z = *( (float*)(buffer + 10) );
  Serial.print("z = ");
  Serial.println(z, 7); 

}


void printLocationData(uint8_t *buffer) {

  float x = *( (float*)(buffer + 2) );
  Serial.print("lat = ");
  Serial.println(x, 7);

  float y = *( (float*)(buffer + 6) );
  Serial.print("lng = ");
  Serial.println(y, 7);

  float z = *( (float*)(buffer + 10) );
  Serial.print("alt = ");
  Serial.println(z, 7); 

}


void printColorData(uint8_t *buffer) {

  byte r = *(buffer + 2);
  Serial.print("r = ");
  Serial.println(r, DEC);

  byte g = *(buffer + 3);
  Serial.print("g = ");
  Serial.println(g, DEC);

  byte b = *(buffer + 4);
  Serial.print("b = ");
  Serial.println(b, DEC); 

}


void printQuaternionData(uint8_t *buffer) {

  float x = *( (float*)(buffer + 2) );
  Serial.print("x = ");
  Serial.println(x, 7);

  float y = *( (float*)(buffer + 6) );
  Serial.print("y = ");
  Serial.println(y, 7);

  float z = *( (float*)(buffer + 10) );
  Serial.print("z = ");
  Serial.println(z, 7); 

  float w = *( (float*)(buffer + 14) );
  Serial.print("w = ");
  Serial.println(w, 7); 

}


boolean checkCRC(uint8_t *buffer) {

  uint8_t len = sizeof(buffer);
  uint8_t crc = buffer[len-2];
  uint8_t sum = 0;

  for (int i = 0; i < (len-1); i++) {

    sum += buffer[i];

  }

  Serial.print("CRC ");

  if ((crc & ~sum) == 0) {
    Serial.println("PASS");
    return true;
  }

  else {
    Serial.println("FAIL");
    return false;
  }

}


void printButtonState(char buttonState) {

  if (buttonState == '0') {
    Serial.println("released");
  }
  else if (buttonState == '1') {
    Serial.println("pressed"); 
  }
  else {
    Serial.println("unknown state");
  }
}

void displayInfo(){

        matrix.begin();
        matrix.fillScreen(matrix.Color333(0,0,0));
        matrix.setCursor(0, 0);    // start at top left, with one pixel of spacing
        matrix.setTextSize(1);
        matrix.println("Track");
        matrix.print("  ");
        matrix.print(songSelect);
        matrix.println("     ");
        matrix.print("V:");
        matrix.print(volume);
  
}

