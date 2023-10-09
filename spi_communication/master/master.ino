#include <SPI.h>
#include "UUID.h"

#define DB_CAP 50
#define SS_2 10

UUID uuid;
String gate_1_passes[DB_CAP];
String gate_2_passes[DB_CAP];
const int tokenSize = 32;
int gate_1_pos = 0;
int gate_2_pos = 0;

void setup(void) {
  Serial.begin(115200);

  pinMode(SS_2, OUTPUT);
  digitalWrite(SS, HIGH);
  digitalWrite(SS_2, HIGH);
  // Put SCK, MOSI, SS pins into output mode
  // also put SCK, MOSI into LOW state, and SS into HIGH state.
  // Then put SPI hardware into Master mode and turn SPI on
  SPI.begin();

  // Slow down the master a bit
  SPI.setClockDivider(SPI_CLOCK_DIV8);
}

byte transferAndWait(const byte inp) {
  byte a = SPI.transfer(inp);
  delayMicroseconds(20);
  return a;
}

void loop(void) {
  try_slave1();
  try_slave2();

  // check incomming string
  if (Serial.available() > 0) {
    String receivedString = Serial.readStringUntil('\n');

    // Check if the received string has exactly 32 characters
    if (receivedString.length() == 1) {
      // Remove the trailing newline character
      receivedString.trim();
      if (receivedString == "1") {
        register_user(1);
      } else if (receivedString == "2") {
        register_user(2);
      } else{
        Serial.println("Something went wrong");
      }
    }
    else{
      Serial.println("length issue");
    }
  }
}

void try_slave1() {
  byte user_input[tokenSize];
  bool user_verified = false;

  // enable Slave Select
  digitalWrite(SS, LOW);

  byte has_input = transferAndWait('a');  // add command
  if (has_input == 1) {
    for (int i = 0; i < tokenSize; i++) {
      user_input[i] = transferAndWait(i);
    }
    user_input[tokenSize-1] = 'a';

    String user_input_str = byteArrayToString(user_input, tokenSize);
    // verify whether uuid is present in the slave_1_passes
    for (int i = 0; i < DB_CAP; i++) {
      if (user_input_str == gate_1_passes[i]) {
        user_verified = true;
      }
    }
  }
  digitalWrite(SS, HIGH);//close the connection with SLAVE_2
  delay(500);
  if (user_verified) {
    send_verification_to_slave_1();
  }
}

void try_slave2() {
  byte user_input[tokenSize];
  bool user_verified = false;

  // enable Slave Select
  digitalWrite(SS_2, LOW);

  byte has_input = transferAndWait('a');  // add command
  if (has_input == 1) {
    for (int i = 0; i < tokenSize; i++) {
      user_input[i] = transferAndWait(i);
    }
    user_input[tokenSize-1] = 'a';

    String user_input_str = byteArrayToString(user_input, tokenSize);
    // verify whether uuid is present in the slave_2_passes
    for (int i = 0; i < DB_CAP; i++) {
      if (user_input_str == gate_2_passes[i]) {
        user_verified = true;
      }
    }
  }
  // disable Slave Select
  digitalWrite(SS_2, HIGH);
  delay(500);
  if (user_verified) {
    send_verification_to_slave_2();
  }
}

void send_verification_to_slave_1() {
  digitalWrite(SS, LOW);
  transferAndWait('g');
  digitalWrite(SS, HIGH);
  delay(500);
}

void send_verification_to_slave_2() {
  digitalWrite(SS_2, LOW);
  transferAndWait('g');
  digitalWrite(SS_2, HIGH);
  delay(500);
}

String byteArrayToString(byte byteArray[], int length) {
  String result = "";
  for (int i = 0; i < length; i++) {
    result += char(byteArray[i]);
  }
  return result;
}

String generateUUID() {
  uuid.generate();
  String uuid_str = String(uuid.toCharArray());
  return removeCharFromString(uuid_str, '-');
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

// function to receive the gate number and generate UUID and put it into relevant gate
void register_user(int gate_num) {
  if (gate_num == 2) {
    String temp_uuid = generateUUID();
    temp_uuid[tokenSize-1] = 'a';
    Serial.println(temp_uuid);
    gate_1_passes[gate_1_pos] = temp_uuid;
    if (gate_1_pos == (DB_CAP - 1)) {
      gate_1_pos = 0;
    } else {
      gate_1_pos++;
    }
  } else{
    String temp_uuid = generateUUID();
    temp_uuid[tokenSize-1] = 'a';
    Serial.println(temp_uuid);
    gate_2_passes[gate_2_pos] = temp_uuid;

    //change the index related to slave 2 token array
    if (gate_2_pos == (DB_CAP - 1)) {
      gate_2_pos = 0;
    } else {
      gate_2_pos++;
    }
  }
}