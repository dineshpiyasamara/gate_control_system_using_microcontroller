#include "UUID.h"

volatile byte command = 0;
UUID uuid;

const int bufferSize = 32;
char stringBuffer[bufferSize];
bool user_input_available = false;
int current_byte = 0;
bool open_gate = false;
void setup(void) {

  // have to send on master in, slave out
  pinMode(MISO, OUTPUT);

  // turn on SPI in slave mode
  SPCR |= _BV(SPE);

  // blink LED
  pinMode(7, OUTPUT);

  // turn on interrupts
  SPCR |= _BV(SPIE);
  Serial.begin(9600);
}

// SPI interrupt routine
ISR(SPI_STC_vect) {
  byte c = SPDR;
  if (c == 'g') {
    Serial.println("Welcome");
    open_gate = true;
  } else{
    // Gate should not open
  }

  switch (command) {
    case 0:
      if (user_input_available) {
        current_byte = 0;
        command = c;
        SPDR = stringBuffer[current_byte];
        current_byte++;
      }
      break;

    case 'a':
      if (current_byte <= bufferSize) {
        SPDR = stringBuffer[current_byte];
        current_byte++;
      } else {
        current_byte = 0;
        user_input_available = false;
      }
      break;
  }
}

void loop(void) {
  delay(100);
  if (digitalRead(SS) == HIGH) {
    command = 0;
  }

  if(open_gate){
    // Turn the LED on (HIGH)
    digitalWrite(7, HIGH);
    delay(1000);

    // Turn the LED off (LOW)
    digitalWrite(7, LOW);
    delay(1000);

    open_gate = false;
  }

  if (Serial.available() > 0) {

    String receivedString = Serial.readString();

    if (receivedString.length() == bufferSize) {

      receivedString.trim();

      // Copy the contents of the received string to the buffer
      receivedString.toCharArray(stringBuffer, bufferSize);
      user_input_available = true;
      SPDR = 1;

    } else {
      Serial.println("Input is not valid");
    }
  }
}

String removeCharFromString(String inputString, char charToRemove) {
  String outputString = "";

  for (int i = 0; i < inputString.length(); i++) {
    if (inputString.charAt(i) != charToRemove) {
      outputString += inputString.charAt(i);
    }
  }

  return outputString;
}