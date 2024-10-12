#include <Servo.h>

Servo bottomArm;
Servo topArm;

float l1 = 3.5;
float l2 = 3.5;
float x_i = 2.05;
float x_f = 6.8;

float bottomAngle = 0;
float topAngle = 0;
float pi = 3.14159;
float e = 2.71828;

float f(float x) {
  return (-1 * pow((x - 4.5), 2) + 5);
  //3.5
  // 1 * pow(e, (x-4.0) - 1.5)
  // -2 * pow((x - 4.1), 2) + 5.5
}

void setup() {
  // put your setup code here, to run once:
  bottomArm.attach(3);
  topArm.attach(5);
//  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
//  Serial.println("IN LOOP");
  for (float x = x_i; x < x_f; x += 0.05) {
    topAngle = acos( (pow(x, 2) + pow(f(x), 2) - pow(l1, 2) - pow(l2, 2)) / (2.0 * l1 * l2) );
    bottomAngle = atan(f(x) / x) + atan(  (l2 * sin(topAngle)) / (l1 + (l2 * cos(topAngle))) );

//    Serial.print("Top Angle: ");
//    Serial.print(topAngle * 180.0 / pi);
//    Serial.print("    Bottom Angle: ");
//    Serial.println(bottomAngle * 180.0 / pi);

    topArm.write(topAngle * 180.0 / pi);
    bottomArm.write(bottomAngle * 180.0 / pi);
    delay(15);
  }

  for (float x = x_f; x > x_i; x -= 0.05) {
    topAngle = acos( (pow(x, 2) + pow(f(x), 2) - pow(l1, 2) - pow(l2, 2)) / (2.0 * l1 * l2) );
    bottomAngle = atan(f(x) / x) + atan(  (l2 * sin(topAngle)) / (l1 + (l2 * cos(topAngle))) );

//    Serial.print("Top Angle: ");
//    Serial.print(topAngle * 180.0 / pi);
//    Serial.print("    Bottom Angle: ");
//    Serial.println(bottomAngle * 180.0 / pi);

    topArm.write(topAngle * 180.0 / pi);
    bottomArm.write(bottomAngle * 180.0 / pi);
    delay(15);
  }

  delay(2000);
}
