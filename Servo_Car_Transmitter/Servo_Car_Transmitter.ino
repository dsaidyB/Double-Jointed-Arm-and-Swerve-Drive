#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define X1 A0
#define Y1 A1

#define X2 A2
#define Y2 A3
int joyPwr = 2;

int x1Pos;
int y1Pos;
int x2Pos;
int y2Pos;

char data[32];

int CE = 7;
int CSN = 8;
RF24 radio(CE, CSN);
// CSN --> standby mode or not --> pin 8, CE --> transmit or command mode --> pin 7

const byte address[6] = "00001";
//can choose what 5 letter string we want to act as the address for the communication channel between the two modules

void setup() {
  pinMode(X1, INPUT);
  pinMode(Y1, INPUT);

  pinMode(joyPwr, OUTPUT);
  digitalWrite(joyPwr, HIGH);

  pinMode(X2, INPUT);
  pinMode(Y2, INPUT);
  
//  Serial.begin(9600);
  
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);     // dont rlly need amplification
  radio.setDataRate(RF24_250KBPS);   // dont need to send that much info over and if u have lower bit rate it says higher range
  radio.stopListening();             //sets as transmitter
}

void loop() {
  x1Pos = map(analogRead(X1), 0, 1023, -1000, 1000);
  y1Pos = map(analogRead(Y1), 0, 1023, 1000, -1000);
  x2Pos = map(analogRead(X2), 0, 1023, -1000, 1000);
  y2Pos = map(analogRead(Y2), 0, 1023, 1000, -1000);
  
//  Serial.println(xPos);
//  Serial.println();
//  Serial.println(yPos);
//  Serial.println(analogRead(X2));
//  Serial.println();
//  Serial.println(analogRead(Y2));

  String combineData = String(x1Pos) + "," + String(y1Pos) + "," + String(x2Pos) + "," + String(y2Pos) + ",";
  combineData.toCharArray(data, sizeof(data));

  radio.write(&data, sizeof(data));  // & gets address of variable; * points to value at address
  delay(200);

//  Serial.println(data);
}
