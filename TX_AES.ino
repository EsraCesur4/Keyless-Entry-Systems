#include <VirtualWire.h>  
#include <Crypto.h>
#include <AES.h>

const int lockButtonPin = 2;    
const int unlockButtonPin = 3;  
const int trunkButtonPin = 4;   
const int transmitPin = 5;      

int prevLockState = LOW;
int prevUnlockState = LOW;
int prevTrunkState = LOW;

const byte key[16] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 
                        0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10 };

AES128 aes;

void setup() {
  pinMode(lockButtonPin, INPUT);
  pinMode(unlockButtonPin, INPUT);
  pinMode(trunkButtonPin, INPUT);

  vw_set_tx_pin(transmitPin);  
  vw_setup(2000);              
  Serial.begin(9600);          

  aes.setKey(key, sizeof(key));
}

void encryptAndSend(const char* message) {
    byte encryptedMessage[16] = {0};
    char buffer[16] = {0};
    strncpy(buffer, message, 15); // 15 karaktere kadar mesaj al

    aes.encryptBlock(encryptedMessage, (byte*)buffer);
    
    vw_send((uint8_t*)encryptedMessage, 16);
    vw_wait_tx();
    Serial.print("Şifreli mesaj gönderildi: ");
    Serial.println(message);
}

void loop() {
  int lockState = digitalRead(lockButtonPin);
  int unlockState = digitalRead(unlockButtonPin);
  int trunkState = digitalRead(trunkButtonPin);

  if (lockState == HIGH && prevLockState == LOW) {
    encryptAndSend("LOCK");
  }

  if (unlockState == HIGH && prevUnlockState == LOW) {
    encryptAndSend("UNLOCK");
  }

  if (trunkState == HIGH && prevTrunkState == LOW) {
    encryptAndSend("TRUNK");
  }

  prevLockState = lockState;
  prevUnlockState = unlockState;
  prevTrunkState = trunkState;

  delay(50); 
}
