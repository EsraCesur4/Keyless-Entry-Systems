#include <VirtualWire.h>
#include <ServoTimer2.h>
#include <Crypto.h>
#include <AES.h>
#include <SoftwareSerial.h>

// -------------------- RF/Servo Setup --------------------
const int mainServoPin = 10;
const int trunkServoPin = 9; // Avoid LED conflicts
const int receivePin = 11;

ServoTimer2 mainServo;
ServoTimer2 trunkServo;

int mainServoPosition = 544;
int trunkServoPosition = 544;

const byte key[16] = {
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
  0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10
};

AES128 aes;

// -------------------- RFID Setup --------------------
const int BUFFER_SIZE = 14;
const int DATA_TAG_SIZE = 8;
const int RX_PIN = 4;
const int TX_PIN = 2;
const int GREEN_LED = 5; // Changed from 9
const int RED_LED = 7;

SoftwareSerial ssrfid(RX_PIN, TX_PIN);
uint8_t buffer[BUFFER_SIZE];
int bufferIndex = 0;

unsigned long lastTag = 0;
const long VALID_TAG = 0xDCDB7B; // Your tag

// -------------------- Setup --------------------
void setup() {
  Serial.begin(9600);

  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  ssrfid.begin(9600);
  ssrfid.listen();

  mainServo.attach(mainServoPin);
  mainServo.write(mainServoPosition);

  trunkServo.attach(trunkServoPin);
  trunkServo.write(trunkServoPosition);

  vw_set_rx_pin(receivePin);
  vw_setup(2000);
  vw_rx_start();

  aes.setKey(key, sizeof(key));
}

// -------------------- Loop --------------------
void loop() {
  handleRFID(); // RFID tag read (independent)
  handleRFMessage(); // RF encrypted control (independent)
}

// -------------------- RFID Functions --------------------
void handleRFID() {
  if (ssrfid.available() > 0) {
    int ssValue = ssrfid.read();
    if (ssValue == -1) return;

    bool callExtractTag = false;

    if (ssValue == 2) bufferIndex = 0;
    else if (ssValue == 3) callExtractTag = true;

    if (bufferIndex >= BUFFER_SIZE) {
      Serial.println("Error: Buffer overflow detected!");
      return;
    }

    buffer[bufferIndex++] = ssValue;

    if (callExtractTag && bufferIndex == BUFFER_SIZE) {
      unsigned long tag = extractTag();
      validateTag(tag);
    }
  }
}

unsigned long extractTag() {
  uint8_t *msgDataTag = buffer + 3;
  long tag = hexstrToValue(msgDataTag, DATA_TAG_SIZE);

  if (tag != lastTag) {
    Serial.print("Extracted Tag: ");
    Serial.println(tag, HEX);
    lastTag = tag;
  }

  return tag;
}

void validateTag(unsigned long tag) {
  if (tag == VALID_TAG) {
    lightLed(GREEN_LED);
    Serial.println("Valid RFID tag.");
  } else {
    lightLed(RED_LED);
    Serial.println("Invalid RFID tag.");
  }
}

void lightLed(int ledPin) {
  digitalWrite(ledPin, HIGH);
  delay(500);
  digitalWrite(ledPin, LOW);
}

long hexstrToValue(char *str, unsigned int length) {
  char copy[length + 1];
  memcpy(copy, str, length);
  copy[length] = '\0';
  return strtol(copy, NULL, 16);
}

// -------------------- RF AES Message Decryption --------------------
void handleRFMessage() {
  uint8_t receivedData[16];
  uint8_t receivedLength = sizeof(receivedData);

  if (vw_get_message(receivedData, &receivedLength)) {
    decryptAndProcess(receivedData);
  }
}

void decryptAndProcess(const byte* encryptedMessage) {
  byte decryptedMessage[16] = {0};
  aes.decryptBlock(decryptedMessage, encryptedMessage);

  String incomingMsg = String((char*)decryptedMessage);
  Serial.print("Çözülen mesaj: ");
  Serial.println(incomingMsg);

  if (incomingMsg.startsWith("LOCK") && mainServoPosition == 544) {
    mainServoPosition = 2400;
    mainServo.write(mainServoPosition);
    Serial.println("Araba kilitlendi.");
  } 
  else if (incomingMsg.startsWith("UNLOCK") && mainServoPosition == 2400) {
    mainServoPosition = 544;
    mainServo.write(mainServoPosition);
    Serial.println("Araba kilidi açıldı.");
  } 
  else if (incomingMsg.startsWith("TRUNK")) {
    if (trunkServoPosition == 544) {
      trunkServoPosition = 2400;
      trunkServo.write(trunkServoPosition);
      Serial.println("Bagaj açıldı.");
    } else {
      trunkServoPosition = 544;
      trunkServo.write(trunkServoPosition);
      Serial.println("Bagaj kapandı.");
    }
  }
}
