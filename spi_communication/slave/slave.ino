volatile byte command = 0;

const int bufferSize = 32;
char stringBuffer[bufferSize];
bool userInputAvailable = false;
int currentByte = 0;
bool openGate = false;
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
  byte mosi_value = SPDR;
  if (mosi_value == 'x') {
    Serial.println("Welcome");
    openGate = true;
  } else{
    // Gate should not open
  }

  switch (command) {
    case 0:
      if (userInputAvailable) {
        currentByte = 0;
        command = mosi_value;
        SPDR = stringBuffer[currentByte];
        currentByte++;
      }
      break;

    case 'a':
      if (currentByte <= bufferSize) {
        SPDR = stringBuffer[currentByte];
        currentByte++;
      } else {
        currentByte = 0;
        userInputAvailable = false;
      }
      break;
  }
}

void loop(void) {
  delay(100);
  if (digitalRead(SS) == HIGH) {
    command = 0;
  }

  if(openGate){
    // Turn the LED on (HIGH)
    digitalWrite(7, HIGH);
    delay(1000);

    // Turn the LED off (LOW)
    digitalWrite(7, LOW);
    delay(1000);

    openGate = false;
  }

  if (Serial.available() > 0) {

    String receivedString = Serial.readString();

    if (receivedString.length() == bufferSize) {

      receivedString.trim();

      // Copy the contents of the received string to the buffer
      receivedString.toCharArray(stringBuffer, bufferSize);
      userInputAvailable = true;
      SPDR = 1;

    } else {
      Serial.println("Input is not valid");
    }
  }
}