#include <Servo.h>

Servo bottomArm;
Servo topArm;

void calibrate(Servo arm){
  arm.write(0);
  delay(1000);
  arm.write(45);
  delay(1000);
  arm.write(90);
  delay(1000);
  arm.write(135);
  delay(1000);
  arm.write(180);
  delay(1000);
}

void setup() {
  // put your setup code here, to run once:
  bottomArm.attach(3);
  topArm.attach(5);
  
  bottomArm.write(0);
  topArm.write(0);
}

void loop() {
  // put your main code here, to run repeatedly
  //calibrate(bottomArm);
}



// code for manually doing the PWM lol
//#define servoPin 9
//
//void setup() {
//  pinMode(servoPin, OUTPUT);
//}
//
//void loop() {
//   // A pulse each 20ms
//    digitalWrite(servoPin, HIGH);
//    delayMicroseconds(1450); // Duration of the pusle in microseconds
//    digitalWrite(servoPin, LOW);
//    delayMicroseconds(18550); // 20ms - duration of the pusle
//    // Pulses duration: 600 - 0deg; 1450 - 90deg; 2300 - 180deg
//}
