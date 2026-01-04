#include <SoftwareSerial.h>

// constants (pins and commands)
const byte pinRX = 13;
const byte pinTX = 11;
const byte pin20 = 12;
const byte sit[8] = {0x9b, 0x06, 0x02, 0x00, 0x01, 0xac, 0x60, 0x9d};
const byte stand[8] = {0x9b, 0x06, 0x02, 0x10, 0x00, 0xac, 0xac, 0x9d};
const byte up[8] = {0x9b, 0x06, 0x02, 0x01, 0x00, 0xfc, 0xa0, 0x9d};
const byte down[8] = {0x9b, 0x06, 0x02, 0x02, 0x00, 0x0c, 0xa0, 0x9d};
const byte displaySeq[3] = {0x9b, 0x07, 0x12};

// global variables
byte displayBuf[3] = {0x00, 0x00, 0x00};
byte indexSeq = 0;
byte indexBuf = 0;
String command = "";
String dispHeight = "";
String desdHeight = "";
bool addingBuf = false;
bool moving = false;
float lastHeight = 0;
unsigned long lastTime = 0;

SoftwareSerial DeskSerial = SoftwareSerial(pinRX, pinTX);

void setup() {
  pinMode(pinRX, INPUT);
  pinMode(pinTX, OUTPUT);
  pinMode(pin20, OUTPUT);

  Serial.begin(9600);
  DeskSerial.begin(9600);
}

void loop() {
  readDisplay();
  sendCommands();
}
  
void sendCommands() {
  // reads commands
  if (Serial.available()) {
    command = Serial.readStringUntil('\n');
    command.trim();

    if (command.startsWith("goto")) {
      desdHeight = command.substring(5); // takes "XX.X" from "goto XX.X"

      // condition when outside of desk's height range
      if (desdHeight.toFloat() < 24.5 || desdHeight.toFloat() > 48.5) desdHeight = "";

      // condition when too close to previous height
      if (abs(desdHeight.toFloat() - lastHeight) >= 0.5) {
        moving = true;
      } 
      else {
        desdHeight = "";
      }
    }
    else if (command == "sit") {
      digitalWrite(pin20, HIGH);
      DeskSerial.write(sit, 8);
    }
    else if (command == "stand") {
      digitalWrite(pin20, HIGH);
      DeskSerial.write(stand, 8);
    }
  }

  // manual/desired height logic
  if (desdHeight.length()) {
    float diff = desdHeight.toFloat() - lastHeight;

    if (moving && abs(diff) < 0.5) {
      desdHeight = ""; // escapes manual/desired height mode
      moving = false;
      DeskSerial.write(sit, 8);
    }
    else {
      unsigned long now = millis();

      // repeat sends commands (as if holding up/down button)
      if (now - lastTime > 100) {
        digitalWrite(pin20, HIGH);
        diff > 0 ? DeskSerial.write(up, 8) : DeskSerial.write(down, 8);
        lastTime = now;
      }
    }
  }
}

void readDisplay() {
  while (DeskSerial.available()) {
    byte byteIn = DeskSerial.read();

    // looking for start sequence
    if (!addingBuf) {
      if (indexSeq == 0 && byteIn == displaySeq[0]) {
        indexSeq = 1;
      }
      else if (indexSeq == 1 && byteIn == displaySeq[1]) {
        indexSeq = 2;
      }
      else if (indexSeq == 2 && byteIn == displaySeq[2]) {
        indexSeq = 0;
        addingBuf = true;
        indexBuf = 0;
      }
      else {
        indexSeq = 0;
      }
    }

    // filling buffer
    else {
      displayBuf[indexBuf] = byteIn;
      indexBuf++;

      if (indexBuf >= 3) {
        addingBuf = false;

        // decode each and print
        bool allZero = (displayBuf[0] == 0x00 && displayBuf[1] == 0x00 && displayBuf[2] == 0x00);
        if (!allZero) {
          dispHeight = "";
          for (byte i = 0; i < 3; i++) {
            dispHeight += ssdec(displayBuf[i]);
          }
          Serial.print(dispHeight);
          Serial.print("\n");
          lastHeight = dispHeight.toFloat();
          delay(10);
        }
      }
    }
  }
}

String ssdec(byte digitIn) {
  bool addDot = false;
  String digitOut = "";

  // if the digit has the dot turned on, handles later after decoding
  if (digitIn >= 0x80) {
    digitIn = digitIn - 0x80;
    addDot = true;
  }

  // decoder
  switch (digitIn) {
    case 0x3f:
      digitOut = "0";
      break;
    case 0x06:
      digitOut = "1";
      break;
    case 0x5b:
      digitOut = "2";
      break;
    case 0x4f:
      digitOut = "3";
      break;
    case 0x66:
      digitOut = "4";
      break;
    case 0x6d:
      digitOut = "5";
      break;
    case 0x7d:
      digitOut = "6";
      break;
    case 0x07:
      digitOut = "7";
      break;
    case 0x7f:
      digitOut = "8";
      break;
    case 0x6f:
      digitOut = "9";
      break;
    default:
      digitOut = "";
      break;
  }

  // adds dot as needed
  if (addDot) digitOut += ".";

  // returns the displayed digit as a String
  return digitOut;
}