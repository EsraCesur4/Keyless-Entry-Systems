#include <ArduinoBLE.h>
#include <Servo.h>
#include <Crypto.h>
#include <AES.h>

#define SERVO1_PIN 5
#define SERVO2_PIN 6

Servo servo1;
Servo servo2;
bool servo1State = false;

AES128 aes;
byte aes_key[] = {
  0x2B, 0x7E, 0x15, 0x16,
  0x28, 0xAE, 0xD2, 0xA6,
  0xAB, 0xF7, 0x76, 0xCF,
  0x84, 0x4A, 0xDF, 0xBD
};

bool useEncryption = false;
bool authenticated = false;

BLEService carService("180A");
BLECharacteristic commandChar("2A57", BLEWrite | BLEWriteWithoutResponse, 20);  // ✅ Düzeltildi

void setup() {
  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);
  servo1.write(0);
  servo2.write(0);

  BLE.begin();
  BLE.setLocalName("Nano33_Server");
  BLE.setAdvertisedService(carService);
  carService.addCharacteristic(commandChar);
  BLE.addService(carService);
  BLE.advertise();
}

void loop() {
  BLEDevice central = BLE.central();

  if (central) {
    useEncryption = false;
    authenticated = false;

    while (central.connected()) {
      if (commandChar.written()) {
        const uint8_t* data = commandChar.value();
        int length = commandChar.valueLength();
        char cmd = '\0';

        // AUTH mesajı kontrolü
        if (!authenticated && length == 16) {
          byte decrypted[16];
          aes.setKey(aes_key, sizeof(aes_key));
          aes.decryptBlock(decrypted, data);

          if (memcmp(decrypted, "AUTH", 4) == 0) {
            useEncryption = true;
            authenticated = true;
            continue; // AUTH mesajıydı, komut değil
          }
        }

        // Şifreli komut çözümü
        if (useEncryption && length == 16) {
          byte decrypted[16];
          aes.setKey(aes_key, sizeof(aes_key));
          aes.decryptBlock(decrypted, data);
          cmd = decrypted[0];
        }
        // Şifresiz komut (MIT App)
        else if (!useEncryption && length >= 1) {
          cmd = (char)data[0];
        }

        // Komutları uygula
        if (cmd == '2' || cmd == 2) {
          servo1State = !servo1State;
          servo1.write(servo1State ? 90 : 0);
        } else if (cmd == '1' || cmd == 1) {
          servo2.write(90);
        } else if (cmd == '0' || cmd == 0) {  // 🔁 0 yerine 3 kullanıldı
          servo2.write(0);
        }
      }
    }

    useEncryption = false;
    authenticated = false;
  }
}
