#include <TimerOne.h>

#include <SPI.h> // SPI-Bibiothek hinzufügen

#include <MFRC522.h> // RFID-Bibiothek hinzufügen
#include <Servo.h>
#include <TM1637.h>


#define SS_PIN 10
#define RST_PIN 9

#define CLK 2
#define DIO 3

MFRC522 mfrc522(SS_PIN, RST_PIN); // RFID-Empfänger benennen
TM1637 tm1637(CLK, DIO);

const int buzzer = 6;

int counter = 10;
bool acceptRFID = false;

byte white_key[] = {0xF2, 0x41, 0xBF, 0x24};
byte blue_key[] = {0x34, 0x55, 0x5A, 0x89};

void setup() // Beginn des Setups:

{

  Serial.begin(9600); // Serielle Verbindung starten (Monitor)

  SPI.begin(); // SPI-Verbindung aufbauen

  tm1637.init();
  tm1637.set(BRIGHTEST);//BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;

  mfrc522.PCD_Init(); // Initialisierung des RFID-Empfängers

  Serial.print("Setup complete");

  pinMode(buzzer, OUTPUT);
  //    for (int i=100; i<10000; i = i + 100){
  //      Serial.println(i);
  //      tone(buzzer, i);
  //      delay(500);
  //      noTone(buzzer);
  //      delay(100);
  //    }
}

void printKey() {
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i], HEX); 
    Serial.print(" ");
  }
  Serial.println();
}

void countdown() {
  counter--;

  tm1637.display(3, counter / 1 % 10);
  tm1637.display(2, counter / 10 % 10);
  tm1637.display(1, counter / 100 % 10);
  tm1637.display(0, counter / 1000 % 10);

  if (counter <= 0) {
    acceptRFID = false;
    Timer1.detachInterrupt();
  }
}

void startCountdown() {
  counter = 10;
  acceptRFID = true;
  Timer1.initialize();
  Timer1.attachInterrupt(countdown);
}

bool isKey(byte key[]) {
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] != key[i]) {
      return false;
    }
  }
  return true;
}

void displayCode() {
  Timer1.detachInterrupt();
  tm1637.display(3, 4);
  tm1637.display(2, 3);
  tm1637.display(1, 2);
  tm1637.display(0, 1);
}

void beep(int freq) {
  tone(buzzer, freq);
  delay(100);

  noTone(buzzer);
  delay(100);

  tone(buzzer, freq);
  delay(200);

  noTone(buzzer);
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  printKey();

  if (isKey(white_key)) {
    beep(1000);
    startCountdown();
  }

  if (acceptRFID && isKey(blue_key)) {
    beep(4000);
    displayCode();
  }
}
