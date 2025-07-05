#include <ArduinoBLE.h>
#include <Crypto.h>
#include <AES.h>

#define BUTTON1_PIN 2
#define BUTTON2_PIN 3
#define BUTTON3_PIN 4

AES128 aes;
byte aes_key[] = {
  0x2B, 0x7E, 0x15, 0x16,
  0x28, 0xAE, 0xD2, 0xA6,
  0xAB, 0xF7, 0x76, 0xCF,
  0x84, 0x4A, 0xDF, 0xBD
};

byte auth_plain[16] = { 'A', 'U', 'T', 'H' };
byte auth_encrypted[16] = { 0 };
byte plain_data[16] = { 0 };
byte encrypted_data[16] = { 0 };

BLEDevice peripheral;
BLECharacteristic commandChar;

bool connectedOnce = false;

void setup() {
  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);
  pinMode(BUTTON3_PIN, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);

  BLE.begin();
  BLE.scan();  // Scan for all devices
}

void loop() {
  // Try to connect only once
  if (!connectedOnce) {
    peripheral = BLE.available();
    if (peripheral && peripheral.localName() == "Nano33_Server") {
      BLE.stopScan();
      if (peripheral.connect()) {
        if (peripheral.discoverAttributes()) {
          commandChar = peripheral.characteristic("2A57");
          if (commandChar) {
            aes.setKey(aes_key, sizeof(aes_key));
            aes.encryptBlock(auth_encrypted, auth_plain);
            commandChar.writeValue(auth_encrypted, 16);
            connectedOnce = true;
            blinkLED(2); // Indicate connection
          }
        }
      }
    }
  }

  if (connectedOnce && peripheral.connected()) {
    byte command = 255;

    if (digitalRead(BUTTON1_PIN) == HIGH) command = 2;
    else if (digitalRead(BUTTON2_PIN) == HIGH) command = 1;
    else if (digitalRead(BUTTON3_PIN) == HIGH) command = 0;

    if (command != 255) {
      memset(plain_data, 0, sizeof(plain_data));
      plain_data[0] = command;

      aes.setKey(aes_key, sizeof(aes_key));
      aes.encryptBlock(encrypted_data, plain_data);
      commandChar.writeValue(encrypted_data, 16);

      blinkLED(1); // Command sent
      delay(500);  // Debounce
    }
  }

  // If disconnected, reset everything
  if (connectedOnce && !peripheral.connected()) {
    connectedOnce = false;
    peripheral = BLEDevice();
    BLE.scan();
  }
}

void blinkLED(int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
  }
}
