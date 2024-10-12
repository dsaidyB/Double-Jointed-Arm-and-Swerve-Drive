#include <Servo.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8); // CE, CSN
const byte address[6] = "00001";
char data[32];

int resetPin = 26;
unsigned long myTime;
unsigned long prevTime = 0;

String x1Holder;  // string arrays was what was making arduino freeze so made individual variables
String y1Holder;
String x2Holder;
String y2Holder;
String temp;

int x1Pos;
int y1Pos;
int x2Pos;
int y2Pos;

double moveAngle;
double moveAngleDeg;
double finalAngle;
double finalAngleDeg;

Servo front_left;
int front_left_servo = 2;

Servo front_right;
int front_right_servo = 3;

Servo back_left;
int back_left_servo = 4;

Servo back_right;
int back_right_servo = 5;

int front_left_m_in1 = 42;
int front_left_m_in2 = 43;
int front_left_m_pwm = 10;

int front_right_m_in1 = 40;
int front_right_m_in2 = 41;
int front_right_m_pwm = 11;

int back_left_m_in1 = 39;
int back_left_m_in2 = 38;
int back_left_m_pwm = 12;

int back_right_m_in1 = 36;
int back_right_m_in2 = 37;
int back_right_m_pwm = 13;

double posVec[4][2] = {{1, 1}, {-1, 1}, {-1, -1}, {1, -1}};  // VECTOR POSITION of each wheel, [x, y]
double spinVec[4][2];     // perpendicular vector to each original wheel position COUNTER Clockwise  [[0, 1][-1, 0]]
double finalPos[4][2]; // sum of vectors
double finalPosAngles[4];  // in degrees
double finalPosMags[4];  // normalized power

int moveV = 1;
int spinV = 1;
int power = 135;

double arctan(double x, double y){
  if (x == 0 && y > 0){
    return 1.57; // 90 degrees, pi/2
  }
  else if (x == 0 && y < 0){
    return 4.71; // 90 degrees, pi/2
  }
  else if (x < 0){
   return atan(y / x) + 3.14;
  }
  else if (x > 0 && y < 0){
    return 6.28 + atan(y / x);
  }
  else{
    return atan(y / x); // quite a few edge cases, needa work with 180 degree range, pi/2 < x < -pi/2
  }
}

void setup() {
  Serial.begin(9600);

  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.setDataRate(RF24_250KBPS);
  radio.startListening();
  
  front_left.attach(front_left_servo);
  front_right.attach(front_right_servo);
  back_left.attach(back_left_servo);
  back_right.attach(back_right_servo);

  pinMode(front_left_m_in1, OUTPUT);
  pinMode(front_left_m_in2, OUTPUT);
  pinMode(front_left_m_pwm, OUTPUT);
  
  pinMode(back_left_m_in1, OUTPUT);
  pinMode(back_left_m_in2, OUTPUT);
  pinMode(back_left_m_pwm, OUTPUT);

  pinMode(front_right_m_in1, OUTPUT);
  pinMode(front_right_m_in2, OUTPUT);
  pinMode(front_right_m_pwm, OUTPUT);

  pinMode(back_right_m_in1, OUTPUT);
  pinMode(back_right_m_in2, OUTPUT);
  pinMode(back_right_m_pwm, OUTPUT);

  int angle = 90;
  front_left.write(180-angle);
  front_right.write(180-angle);  // this goes from 0 at left to 180 on right, clockwise
  back_left.write(180-angle); // opposite side rotation so in order to match angle, start the opposite side and put motor in reverse
  back_right.write(180-angle);

  delay(2000);

}

void loop() {
  if (radio.available()){    
    radio.read(&data, sizeof(data));
    
    temp = "";
    int currentVal = 0;
    for (int x = 0; x < sizeof(data); x++){
      if (data[x] == ','){
        if (currentVal == 0){
          x1Holder = temp;
        }
        if (currentVal == 1){
          y1Holder = temp;
        }
        if (currentVal == 2){
          x2Holder = temp;
        }
        if (currentVal == 3){
          y2Holder = temp;
        }
        if (currentVal > 3){
          break;
        }
        currentVal += 1;
        temp = "";
      }
      
      else{
        temp += data[x];
      }
    }
    
    Serial.print("Recieved Data: ");
    Serial.println(data);
    
    x1Pos = x1Holder.toInt();
    y1Pos = y1Holder.toInt();
    x2Pos = x2Holder.toInt();
    y2Pos = y2Holder.toInt();    
  
    if (x1Pos < 50 && x1Pos > -50 && y1Pos > -50 && y1Pos < 50){
      moveV = 0;
      
      power = 0;
      
    }
    else{
      moveV = 1;
      moveAngle = arctan(x1Pos, y1Pos);
      moveAngleDeg = moveAngle*180/3.14;

      power = 135;
    }
    
    Serial.print("Angle (Deg): ");
    Serial.println(moveAngleDeg);
    Serial.println();
  }

  double turnVec[4][2] = {{cos(moveAngle), sin(moveAngle)}, {cos(moveAngle), sin(moveAngle)}, {cos(moveAngle), sin(moveAngle)}, {cos(moveAngle), sin(moveAngle)}};  // rotation matrix on each wheel  // vector after turn of each wheel

  if (x2Pos > 100){
    spinVec[0][0] = 1;
    spinVec[0][1] = -1;
    
    spinVec[1][0] = 1;
    spinVec[1][1] = 1;
    
    spinVec[2][0] = -1;
    spinVec[2][1] = 1;
    
    spinVec[3][0] = -1;
    spinVec[3][1] = -1;
    // spinVec[4][2] = {{1, -1}, {1, 1}, {-1, 1}, {-1, -1}};     // perpendicular vector to each original wheel position Clockwise [[0, -1][1, 0]]
  }
  
  else if (x2Pos < -100){
    spinVec[0][0] = -1;
    spinVec[0][1] = 1;
    
    spinVec[1][0] = -1;
    spinVec[1][1] = -1;
    
    spinVec[2][0] = 1;
    spinVec[2][1] = -1;
    
    spinVec[3][0] = 1;
    spinVec[3][1] = 1;
    // spinVec[4][2] = {{-1, 1}, {-1, -1}, {1, -1}, {1, 1}};     // perpendicular vector to each original wheel position COUNTER Clockwise [[0, 1][-1, 0]]
  }
  
  else{
    spinVec[0][0] = 0;
    spinVec[0][1] = 0;
    
    spinVec[1][0] = 0;
    spinVec[1][1] = 0;
    
    spinVec[2][0] = 0;
    spinVec[2][1] = 0;
    
    spinVec[3][0] = 0;
    spinVec[3][1] = 0;
    //double spinVec[4][2] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}};     // no spin
  }

  for (int i = 0; i < 4; i++){
    for (int j = 0; j < 2; j++){
      finalPos[i][j] = turnVec[i][j] + spinVec[i][j];
    }
  }

  for (int a = 0; a < 4; a++){
    finalAngle = arctan(finalPos[a][0], finalPos[a][1]);
    finalAngleDeg = finalAngle*180/3.14; 
    finalPosAngles[a] = round(finalAngleDeg);
  }

  double largestMag = 0;
  for (int m = 0; m < 4; m++){
    double currentMag = sqrt((pow(finalPos[m][0], 2) + pow(finalPos[m][1], 2)));
    finalPosMags[m] = currentMag;
    
    if (currentMag > largestMag){
      largestMag = currentMag;
    }
  }

  for (int n = 0; n < 4; n++){
    finalPosMags[n] = finalPosMags[n] / largestMag;
    finalPosMags[n] = map(finalPosMags[n], 0, 1, 140, 255);
  }
// analogWrite(motor, int/round( 255 * min(finalPosMags[n], 1)) ); // in case of some floating point issues
//  magnitude of each vector all normalized relative to largest value  

  for (int d = 0; d < 4; d++){
     finalPosAngles[d] = round(finalPosAngles[d]/2);
  }
// 
//  int angle = 90;
//  front_left.write(180-angle);
//  front_right.write(180-angle);  // this goes from 0 at left to 180 on right, clockwise
//  back_left.write(180-angle); // opposite side rotation so in order to match angle, start the opposite side and put motor in reverse
//  back_right.write(180-angle);
//
//  delay(3000);
//
//  angle = 45;
//  front_left.write(180-angle);
//  front_right.write(180-angle);  // this goes from 0 at left to 180 on right, clockwise
//  back_left.write(180-angle); // opposite side rotation so in order to match angle, start the opposite side and put motor in reverse
//  back_right.write(180-angle);
//    delay(3000);
//
//  angle = 90;
//  front_left.write(180-angle);
//  front_right.write(180-angle);  // this goes from 0 at left to 180 on right, clockwise
//  back_left.write(180-angle); // opposite side rotation so in order to match angle, start the opposite side and put motor in reverse
//  back_right.write(180-angle);
//  delay(3000);
//
//  angle = 135;
//  front_left.write(180-angle);
//  front_right.write(180-angle);  // this goes from 0 at left to 180 on right, clockwise
//  back_left.write(180-angle); // opposite side rotation so in order to match angle, start the opposite side and put motor in reverse
//  back_right.write(180-angle);
//  delay(3000);
//
//  angle = 90;
//  front_left.write(180-angle);
//  front_right.write(180-angle);  // this goes from 0 at left to 180 on right, clockwise
//  back_left.write(180-angle); // opposite side rotation so in order to match angle, start the opposite side and put motor in reverse
//  back_right.write(180-angle);
//  delay(3000);


    front_left.write(180-finalPosAngles[1]);
    front_right.write(180-finalPosAngles[0]);  // this goes from 0 at left to 180 on right, clockwise
    back_left.write(180-finalPosAngles[2]); // opposite side rotation so in order to match angle, start the opposite side and put motor in reverse
    back_right.write(180-finalPosAngles[3]);

    digitalWrite(front_left_m_in1, HIGH); //forwards
    digitalWrite(front_left_m_in2, LOW);
    analogWrite(front_left_m_pwm, moveV * finalPosMags[1]);
        
    digitalWrite(front_right_m_in1, HIGH); //forwards
    digitalWrite(front_right_m_in2, LOW);
    analogWrite(front_right_m_pwm, moveV * finalPosMags[0]);
        
    digitalWrite(back_left_m_in1, HIGH); //forwards
    digitalWrite(back_left_m_in2, LOW);
    analogWrite(back_left_m_pwm, moveV * finalPosMags[2]);
        
    digitalWrite(back_right_m_in1, HIGH); //forwards
    digitalWrite(back_right_m_in2, LOW);
    analogWrite(back_right_m_pwm, moveV * finalPosMags[3]);
  
  delay(100);
}














































































//  front_left.write(4);    // zero makes it glitch cuz if it overshoots by a bit, then it sees like 180 and rotates all the way back
//  delay(2000);
//  front_left.write(90);
//  delay(2000);

//  digitalWrite(front_left_m_in1, LOW);
//  digitalWrite(front_left_m_in2, HIGH);
//  analogWrite(front_left_m_pwm, 255);
//  delay(1000);
//  digitalWrite(front_left_m_in1, LOW);
//  digitalWrite(front_left_m_in2, HIGH);
//  analogWrite(front_left_m_pwm, 0);
//  delay(1000);


//if (x1Pos == 0 && y1Pos > 0){
//    moveAngle = 1.57; // 90 degrees, pi/2
//  }
//  else if (x1Pos == 0 && y1Pos < 0){
//    moveAngle = -1.57; // 90 degrees, pi/2
//  }
//  else if (x1Pos < 0){
//    moveAngle = atan(y1Pos / x1Pos) + 3.14;
//  }
//  else if (x1Pos > 0 && y1Pos < 0){
//    moveAngle = 6.28 + atan(y1Pos / x1Pos);
//  }
//  else{
//    moveAngle = atan(y1Pos / x1Pos); // quite a few edge cases, needa work with 180 degree range, pi/2 < x < -pi/2
//  }


//  digitalWrite(LED_BUILTIN, HIGH);
//  myTime = millis() - prevTime;
//  if (myTime > 5000){
//    digitalWrite(resetPin, LOW);
//  }
//  
// in void loop under radio.available()
//    digitalWrite(LED_BUILTIN, LOW);
//    prevTime = millis();   
//    char vals[5][6] = {"","","","",""};


//
//Serial.print(finalPos[0][0]);
//Serial.print(" ");
//Serial.println(finalPos[0][1]);
//Serial.print(finalPos[1][0]);
//Serial.print(" ");
//Serial.println(finalPos[1][1]);
//Serial.print(finalPos[2][0]);
//Serial.print(" ");
//Serial.println(finalPos[2][1]);
//Serial.print(finalPos[3][0]);
//Serial.print(" ");
//Serial.println(finalPos[3][1]);
//Serial.println();
//
//Serial.println(finalPosAngles[0]);
//Serial.println(finalPosAngles[1]);
//Serial.println(finalPosAngles[2]);
//Serial.println(finalPosAngles[3]);
//Serial.println();
//

  
//  delay(3000);
//
//  angle = 45;
//  front_left.write(180-angle);
//  front_right.write(180-angle);
//  back_left.write(180-angle);
//  back_right.write(180-angle);
//  delay(1000);
//
//  angle = 90;
//  front_left.write(180-angle);
//  front_right.write(180-angle);
//  back_left.write(180-angle);
//  back_right.write(180-angle);
//  delay(1000);
//
//  angle = 180;
//  front_left.write(180-angle);
//  front_right.write(180-angle);
//  back_left.write(180-angle);
//  back_right.write(180-angle);
//  delay(1000);
//
//  angle = 90;
//  front_left.write(180-angle);
//  front_right.write(180-angle);
//  back_left.write(180-angle);
//  back_right.write(180-angle);
//  delay(1000);
