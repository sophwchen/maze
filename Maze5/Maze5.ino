#include "main.h"
#include "MeMegaPi.h"
#include <Wire.h>

#include <VL53L0X.h> //TOF

VL53L0X sensor;
#define TCAADDR 0x70
#define TCATOF 0x72
int tof_channels[] = {1, 2, 3, 4, 5, 6};

#define LED 11

int tcaselect(uint8_t i) {
  if (i > 7) return;

  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  return Wire.endTransmission();
}

int tcatof(uint8_t i) {
  tcaselect(5);
  if (i > 7) return;

  Wire.beginTransmission(TCATOF);
  Wire.write(1 << i);
  return Wire.endTransmission();
}

#include <Adafruit_BNO055.h> //IMU
#include <utility/imumaths.h>
#define BNO055_SAMPLERATE_DELAY_MS (100)
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);

//#include <Adafruit_MLX90614.h>
//Adafruit_MLX90614 mlx = Adafruit_MLX90614();

//Motors/ Encoder stuff
const byte interruptPin = 19;
const byte NE1 = 38;
uint8_t motorSpeed = 150;
volatile long count = 0;
unsigned long time;
unsigned long last_time;
long temp = 200;
String prevVictim = "v";

//RIGHT
MeMegaPiDCMotor motor1(PORT2A);
MeMegaPiDCMotor motor2(PORT2B);

//LEFT
MeMegaPiDCMotor motor3(PORT3A);
MeMegaPiDCMotor motor4(PORT3B);

#include <Adafruit_Sensor.h>

//TEMPERATURE SENSORS
#include <Adafruit_MLX90614.h>
int mlxports[] = {6, 7};
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

//COLOR SENSOR
#include "Adafruit_TCS34725.h"
#define redpin 3
#define greenpin 5
#define bluepin 6
#define commonAnode true
byte gammatable[256];
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);


//STEPPER
#include <Stepper.h>
#define STEPS 200
#define NUM_DROPPERS  13
#define STEPS_PER_DROP int(STEPS/NUM_DROPPERS)
#define AIN1 4
#define AIN2 5
#define BIN2 2
#define BIN1 A1
#define STEPPER_SPEED_RPM  15
Stepper stepper(STEPS, AIN1, AIN2, BIN2, BIN1);


void StopMotors() {
  motor1.stop();
  motor2.stop();
  motor3.stop();
  motor4.stop();
}
void Forward() {
  motor1.run(motorSpeed);
  motor2.run(motorSpeed);
  motor3.run(-motorSpeed);
  motor4.run(-motorSpeed);
}
void Back() {
  motor1.run(-motorSpeed);
  motor2.run(-motorSpeed);
  motor3.run(motorSpeed);
  motor4.run(motorSpeed);
}
void Right() {
  motor1.run(-motorSpeed);
  motor2.run(-motorSpeed);
  motor3.run(-motorSpeed);
  motor4.run(-motorSpeed);
}
void Left() {
  motor1.run(motorSpeed);
  motor2.run(motorSpeed);
  motor3.run(motorSpeed);
  motor4.run(motorSpeed);
}
void readTile() {
  Serial.println("read Tile");
  Serial.print("Dir = ");
  Serial.println(curDir);
  Serial.println("Distances: ");

  tcatof(1);
  int a = sensor.readRangeContinuousMillimeters();
  tcatof(4);
  int b = sensor.readRangeContinuousMillimeters();
  if (a < 250 && b < 250) {
    Serial.println("Wall ahead");
    if (curDir == N) field[curR][curC].N = 1;
    if (curDir == E) field[curR][curC].E = 1;
    if (curDir == S) field[curR][curC].S = 1;
    if (curDir == W) field[curR][curC].W = 1;
  }

  tcatof(2);
  a = sensor.readRangeContinuousMillimeters();
  tcatof(5);
  b = sensor.readRangeContinuousMillimeters();
  if (a < 250 && b < 250) {
    Serial.println("Wall right");
    if ((curDir + 1) % 4 == N) field[curR][curC].N = 1;
    if ((curDir + 1) % 4 == E) field[curR][curC].E = 1;
    if ((curDir + 1) % 4 == S) field[curR][curC].S = 1;
    if ((curDir + 1) % 4 == W) field[curR][curC].W = 1;
  }

  tcatof(3);
  a = sensor.readRangeContinuousMillimeters();
  tcatof(6);
  b = sensor.readRangeContinuousMillimeters();
  if (a < 250 && b < 250) {
    Serial.println("Wall left");
    if ((curDir + 3) % 4 == N) field[curR][curC].N = 1;
    if ((curDir + 3) % 4 == E) field[curR][curC].E = 1;
    if ((curDir + 3) % 4 == S) field[curR][curC].S = 1;
    if ((curDir + 3) % 4 == W) field[curR][curC].W = 1;
  }
  field[curR][curC].visited = 1;

  Serial.println();
  Serial.println("walls: ");
  Serial.println(field[curR][curC].N);
  Serial.println(field[curR][curC].E);
  Serial.println(field[curR][curC].S);
  Serial.println(field[curR][curC].W);
  Serial.println();
}

bool goForward(int x) {
  Serial.println("GO Forward");
  bool tflag = false;
  float red, green, blue;
  int tside = -1;
  int start = count;
  bool r = true;
  tcaselect(1);
  Forward();

  while (abs(count - start) < x) {
    int kp = 1;
    int targetvalue = 85;
    char side;
    int curvalue;
    curvalue = 85;
    tcatof(2);
    int a = sensor.readRangeContinuousMillimeters();
    tcatof(5);
    int b = sensor.readRangeContinuousMillimeters();
    tcatof(3);
    int c = sensor.readRangeContinuousMillimeters();
    tcatof(6);
    int d = sensor.readRangeContinuousMillimeters();
    if (a + b < c + d) {
      side = 'r';
      if (abs(a - b) < 70 && a < 200 && b < 200) {
        curvalue = (a + b) / 2;
      }
    }
    else {
      side = 'l';
      if (abs(c - d) < 70 && c < 200 && d < 200) {
        curvalue = (c + d) / 2;
      }
    }
    Serial.print("a = ");
    Serial.println(a);
    Serial.print("b = ");
    Serial.println(b);
    Serial.print("c = ");
    Serial.println(c);
    Serial.print("d = ");
    Serial.println(d);
    Serial.print("Using side ");
    Serial.println(side);
    int error = (targetvalue - curvalue) * kp;
    Serial.print("Current value = ");
    Serial.println(curvalue);
    Serial.print("Error: ");
    Serial.println(error);

    if (side == 'r') {
      Serial.println("Using rightside");
      Serial.print("right speed = ");
      Serial.println(motorSpeed + error);
      Serial.print("left speed = ");
      Serial.println(-motorSpeed + error);
      motor1.run(motorSpeed + error);
      motor2.run(motorSpeed + error);
      motor3.run(-motorSpeed + error);
      motor4.run(-motorSpeed + error);
    }
    else {
      Serial.println("Using leftside");
      Serial.print("right speed = ");
      Serial.println(motorSpeed - error);
      Serial.print("left speed = ");
      Serial.println(-motorSpeed - error);
      motor1.run(motorSpeed - error);
      motor2.run(motorSpeed - error);
      motor3.run(-motorSpeed - error);
      motor4.run(-motorSpeed - error);
    }
    //Serial.println(abs(count - start));
    tcatof(1);
    int rtof = sensor.readRangeContinuousMillimeters();
    tcatof(4);
    int ltof = sensor.readRangeContinuousMillimeters();
    if (rtof < 160 && ltof < 160) {
      break;
    }
    //    if(rtof < 160 || ltof < 160) {
    //      if (rtof < 160 && ltof > 180) {
    //        StopMotors();
    //        delay(500);
    //        tcatof(1);
    //        motor1.run(motorSpeed);
    //        motor2.run(motorSpeed);
    //        motor3.run(motorSpeed);
    //        motor4.run(motorSpeed);
    //        while (sensor.readRangeContinuousMillimeters() < 160) {}
    //        StopMotors();
    //        delay(500);
    //        Forward();
    //        break;
    //      }
    //      if(ltof < 160 && rtof > 180){
    //        StopMotors();
    //        delay(500);
    //        tcatof(4);
    //        motor1.run(-motorSpeed);
    //        motor2.run(-motorSpeed);
    //        motor3.run(-motorSpeed);
    //        motor4.run(-motorSpeed);
    //        while (sensor.readRangeContinuousMillimeters() < 160) {}
    //        StopMotors();
    //        delay(500);
    //        Forward();
    //      }
    //    }

    tcatof(7);
    tcs.getRGB(&red, &green, &blue); //red green blue are values
    //Serial.println(red);
    if (red > 112 && red < 119 ) {
      r = false;
      break;
    }

    for (int j : mlxports) {
      tcaselect(j);
      if (prevVictim != "H") {
        if (mlx.readObjectTempF() > temp  && mlx.readObjectTempF() < 100) {
          Serial.println("Heat Victim Detected");
          Serial.println(mlx.readObjectTempF());
          tflag = true;
          if (j == 7) {
            tside = 0;
          }
          else {
            tside = 1;
          }
        }
      }
    }
  }

  if (!r) {
    int y = abs(count - start);
    start = count;
    while (abs(start - count) < y) {
      Back();
    }
  }


  Serial.println("Finish forward");
  StopMotors();
  align();

  if (tflag) {
    prevVictim = "H";
    RescueKit(1, tside);
    lcvictim();
  }
  else {
    victim();
  }

  //delay(5000);
  return r;
}

void align(void) {
  Serial.println("Aligning: ");
  delay(500);
  tcatof(1);
  if (sensor.readRangeContinuousMillimeters() < 100) {
    while (sensor.readRangeContinuousMillimeters() < 150) {
      Back();
    }
  }
  if (sensor.readRangeContinuousMillimeters() < 300) {
    while (sensor.readRangeContinuousMillimeters() > 150) {
      Forward();
    }
  }
  StopMotors();

  int side = 0;

  //left back is around 12 greater than left front
  int lerror = 12;

  tcatof(2);
  int a = sensor.readRangeContinuousMillimeters();
  tcatof(3);
  int b = sensor.readRangeContinuousMillimeters() - lerror;
  tcatof(5);
  int c = sensor.readRangeContinuousMillimeters();
  tcatof(6);
  int d = sensor.readRangeContinuousMillimeters();

  int A = -1, B = -1, x;
  if (abs(a - c) < 60) {
    A = a;
  }
  if (abs(b - d) < 60) {
    B = b;
  }
  if (A != -1 && B == -1) {
    Serial.println("Using rightside for alignment");
    tcatof(2);
    int a = sensor.readRangeContinuousMillimeters();
    tcatof(5);
    int b = sensor.readRangeContinuousMillimeters();
    while (abs(b - a) > 5 and abs(b - a) < 50) {
      while (b - a > 5 and b - a < 50) {
        Serial.println("Front farther than back");
        Right();
        tcatof(2);
        a = sensor.readRangeContinuousMillimeters();
        Serial.print("Back = ");
        Serial.println(a);
        tcatof(5);
        b = sensor.readRangeContinuousMillimeters();
        Serial.print("Front = ");
        Serial.println(b);
      }
      StopMotors();
      while (a - b > 5 and a - b < 50) {
        Serial.println("Back farther than front");
        Left();
        tcatof(2);
        a = sensor.readRangeContinuousMillimeters();
        Serial.print("Back = ");
        Serial.println(a);
        tcatof(5);
        b = sensor.readRangeContinuousMillimeters();
        Serial.print("Front = ");
        Serial.println(b);
      }
      StopMotors();
    }
  }
  if (B != -1 && A == -1) {
    Serial.println("Using leftside for alignment");
    tcatof(3);
    int a = sensor.readRangeContinuousMillimeters() - lerror;
    tcatof(6);
    int b = sensor.readRangeContinuousMillimeters();
    while (abs(b - a) > 5 and abs(b - a) < 50) {
      while (b - a > 5 and b - a < 50) {
        Serial.println("Front farther than back");
        Left();
        tcatof(3);
        a = sensor.readRangeContinuousMillimeters() - lerror;
        Serial.print("Back = ");
        Serial.println(a);
        tcatof(6);
        b = sensor.readRangeContinuousMillimeters();
        Serial.print("Front = ");
        Serial.println(b);
      }
      StopMotors();
      while (a - b > 5 and b - a < 50) {
        Serial.println("back farther than front");
        Right();
        tcatof(3);
        a = sensor.readRangeContinuousMillimeters() - lerror;
        Serial.print("Back = ");
        Serial.println(a);
        tcatof(6);
        b = sensor.readRangeContinuousMillimeters();
        Serial.print("Front = ");
        Serial.println(b);
      }
      StopMotors();
    }
  }
  if (B != -1 && A != -1) {
    if (B > A) {
      Serial.println("Using rightside for alignment");
      tcatof(2);
      int a = sensor.readRangeContinuousMillimeters();
      tcatof(5);
      int b = sensor.readRangeContinuousMillimeters();
      while (abs(b - a) > 5 and abs(b - a) < 50) {
        while (b - a > 5 and b - a < 50) {
          Serial.println("Front farther than back");
          Right();
          tcatof(2);
          a = sensor.readRangeContinuousMillimeters();
          Serial.print("Back = ");
          Serial.println(a);
          tcatof(5);
          b = sensor.readRangeContinuousMillimeters();
          Serial.print("Front = ");
          Serial.println(b);
        }
        StopMotors();
        while (a - b > 5 and a - b < 50) {
          Serial.println("Back farther than front");
          Left();
          tcatof(2);
          a = sensor.readRangeContinuousMillimeters();
          Serial.print("Back = ");
          Serial.println(a);
          tcatof(5);
          b = sensor.readRangeContinuousMillimeters();
          Serial.print("Front = ");
          Serial.println(b);
        }
        StopMotors();
      }
    }
    else {
      Serial.println("Using leftside for alignment");
      tcatof(3);
      int a = sensor.readRangeContinuousMillimeters() - lerror;
      tcatof(6);
      int b = sensor.readRangeContinuousMillimeters();
      while (abs(b - a) > 5 and abs(b - a) < 50) {
        while (b - a > 5 and b - a < 50) {
          Serial.println("Front farther than back");
          Left();
          tcatof(3);
          a = sensor.readRangeContinuousMillimeters() - lerror;
          Serial.print("Back = ");
          Serial.println(a);
          tcatof(6);
          b = sensor.readRangeContinuousMillimeters();
          Serial.print("Front = ");
          Serial.println(b);
        }
        StopMotors();
        while (a - b > 5 and b - a < 50) {
          Serial.println("back farther than front");
          Right();
          tcatof(3);
          a = sensor.readRangeContinuousMillimeters() - lerror;
          Serial.print("Back = ");
          Serial.println(a);
          tcatof(6);
          b = sensor.readRangeContinuousMillimeters();
          Serial.print("Front = ");
          Serial.println(b);
        }
        StopMotors();
      }
    }
    StopMotors();
    delay(500);
  }
}

void turnRight90(void) {
  delay(500);
  Serial.println("Right");
  tcaselect(0);
  sensors_event_t event;
  bno.getEvent(&event);
  int start = (event.orientation.x);
  int sum = 90;
  Right();
  while (abs((event.orientation.x) - start) < sum) {
    int prev = abs((event.orientation.x) - start);
    bno.getEvent(&event);
    //Serial.println(event.orientation.x);
    if (abs(event.orientation.x - start) > 100) {
      sum -=  prev;
      start = event.orientation.x;
    }

  }
  Serial.println("Turn Done: ");
  Serial.println();

  StopMotors();
  delay(500);
  align();
}

void turnLeft90(void) {
  delay(500);
  Serial.println("Left");
  tcaselect(0);
  sensors_event_t event;
  bno.getEvent(&event);
  int start = (event.orientation.x);
  int sum = 90;
  Left();
  while (abs((event.orientation.x) - start) < sum) {
    int prev = abs((event.orientation.x) - start);
    bno.getEvent(&event);
    //Serial.println(event.orientation.x);
    if (abs(event.orientation.x - start) > 100) {
      sum -=  prev;
      start = event.orientation.x;
    }

  }
  Serial.println("Turn Done: ");
  Serial.println();

  StopMotors();
  delay(500);
  align();
}

void turn180(void) {
  Serial.println("180");
  delay(500);
  tcaselect(0);
  sensors_event_t event;
  bno.getEvent(&event);
  int start = (event.orientation.x);
  int sum = 180;
  Left();
  while (abs((event.orientation.x) - start) < sum) {
    int prev = abs((event.orientation.x) - start);
    bno.getEvent(&event);
    //Serial.println(event.orientation.x);
    if (abs(event.orientation.x - start) > 100) {
      sum -=  prev;
      start = event.orientation.x;
    }
  }

  Serial.println("Turn Done: ");
  Serial.println();

  Serial.println(abs(event.orientation.x - start));
  Serial.println();
  StopMotors();
  delay(500);
  align();
}

void turn(Directions target) {
  Serial.println(int(target));
  switch ((target - curDir + 4) % 4) {
    case 1: {
        turnRight90();
        victim();
        break;
      }
    case 2: {
        turn180();
        victim();
        break;
      }
    case 3: {
        turnLeft90();
        victim();
        break;
      }
  }
  curDir = target;
  int start = count;
}

void blink() {
  if (digitalRead(NE1) > 0) //NE1 is helpful for directions
    count++;
  else
    count--;
}

void victim() {
  Serial.println("Entered victim function");
  if (field[curR][curC].visited) {
    Serial.println("Visited tile");
    return;
  }
  Serial.println("Looking for victims");
  heatvictim();
  lcvictim();
  while (Serial2.available()) {
    Serial2.read();
  }

}

bool lcvictim() {
  Serial.print("Prev victim = ");
  Serial.println(prevVictim);
  bool flag = false;
  Serial.println("Checking Serial victims");
  String v, ca;
  int cam;
  if (Serial2.available()) {
    flag = true;
    ca = Serial2.readStringUntil('\n');
    v = Serial2.readStringUntil('\n');
    Serial.print("current victim = ");
    Serial.println(v);
    if (v == prevVictim) {
      while (Serial2.available()) {
        Serial2.read();
      }
      delay(200);
      if (!Serial2.available()) {
        return false;
      }
      else {
        ca = Serial2.readStringUntil('\n');
        v = Serial2.readStringUntil('\n');
      }
    }
    prevVictim = v;
    if (ca == "R") {
      cam = 0;
      tcatof(2);
      if (sensor.readRangeContinuousMillimeters() > 350) {
        return;
      }
    }
    else {
      cam = 1;
      tcatof(3);
      if (sensor.readRangeContinuousMillimeters() > 350) {
        return;
      }
    }
    Serial.print("Camera #: ");
    Serial.println(ca);
    Serial.print("Victim type: ");
    Serial.println(v);
    if (v == "h") {
      RescueKit(3, cam);
    }
    if (v == "s") {
      Serial.println("Calling rescue kit function");
      RescueKit(2, cam);
    }
    if (v == "r" || v == "y") {
      RescueKit(1, cam);
    }
    if (v == "g" || v == "u") {
      digitalWrite(LED, HIGH);
      delay(5000);
      digitalWrite(LED, LOW);
    }
  }
  return flag;
}

bool heatvictim() {
  if (prevVictim == "H") {
    return;
  }
  Serial.println("Heat victims");
  bool flag = false;
  for (int j : mlxports) {
    tcaselect(j);
    Serial.print("Temp: ");
    Serial.println(mlx.readObjectTempF());
    if (mlx.readObjectTempF() > temp && mlx.readObjectTempF() < 100) {
      prevVictim = "H";
      Serial.println("Detected Heat Victim");
      flag = true;
      if (j == 7) {
        RescueKit(1, 0);
      }
      else {
        RescueKit(1, 1);
      }
    }
  }
  return flag;
}

void RescueKit(int n, int dir) {
  digitalWrite(LED, HIGH);
  delay(5000);
  digitalWrite(LED, LOW);
  return;
  Serial.println("rescue kit");
  Serial.println(n);
  Serial.println(dir);
  if (dir == 1) {
    delay(500);
    Serial.println("Left");
    tcaselect(0);
    sensors_event_t event;
    bno.getEvent(&event);
    int start = (event.orientation.x);
    int sum = 90;
    Left();
    while (abs((event.orientation.x) - start) < sum) {
      int prev = abs((event.orientation.x) - start);
      bno.getEvent(&event);
      //Serial.println(event.orientation.x);
      if (abs(event.orientation.x - start) > 100) {
        sum -=  prev;
        start = event.orientation.x;
      }

    }
    Serial.println("Turn Done: ");
    Serial.println();

    StopMotors();
    delay(500);
  }
  else {
    delay(500);
    Serial.println("Right");
    tcaselect(0);
    sensors_event_t event;
    bno.getEvent(&event);
    int start = (event.orientation.x);
    int sum = 90;
    Right();
    while (abs((event.orientation.x) - start) < sum) {
      int prev = abs((event.orientation.x) - start);
      bno.getEvent(&event);
      //Serial.println(event.orientation.x);
      if (abs(event.orientation.x - start) > 100) {
        sum -=  prev;
        start = event.orientation.x;
      }

    }
    Serial.println("Turn Done: ");
    Serial.println();

    StopMotors();
    delay(500);
  }
  delay(100);
  for (int i = 0; i < n; i++) {
    stepper.step(STEPS_PER_DROP);
    delay(1000);
  }
  if (dir == 1) {
    delay(500);
    Serial.println("Right");
    tcaselect(0);
    sensors_event_t event;
    bno.getEvent(&event);
    int start = (event.orientation.x);
    int sum = 90;
    Right();
    while (abs((event.orientation.x) - start) < sum) {
      int prev = abs((event.orientation.x) - start);
      bno.getEvent(&event);
      //Serial.println(event.orientation.x);
      if (abs(event.orientation.x - start) > 100) {
        sum -=  prev;
        start = event.orientation.x;
      }

    }
    Serial.println("Turn Done: ");
    Serial.println();

    StopMotors();
    delay(500);
  }
  else {
    delay(500);
    Serial.println("Left");
    tcaselect(0);
    sensors_event_t event;
    bno.getEvent(&event);
    int start = (event.orientation.x);
    int sum = 90;
    Left();
    while (abs((event.orientation.x) - start) < sum) {
      int prev = abs((event.orientation.x) - start);
      bno.getEvent(&event);
      //Serial.println(event.orientation.x);
      if (abs(event.orientation.x - start) > 100) {
        sum -=  prev;
        start = event.orientation.x;
      }

    }
    Serial.println("Turn Done: ");
    Serial.println();

    StopMotors();
    delay(500);
  }
}


//2: 19
//3: 3


void setup(void) {
  Wire.begin();
  Serial2.begin(9600);
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  //stepper.setSpeed(STEPPER_SPEED_RPM);

  //motor encoders
  pinMode(interruptPin, INPUT_PULLUP);
  pinMode(NE1, INPUT);
  attachInterrupt(digitalPinToInterrupt(interruptPin), blink, RISING);

  //imu
  tcaselect(0);
  if (!bno.begin(Adafruit_BNO055::OPERATION_MODE_IMUPLUS))
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while (1);
  }

  //tof sensors
  for (int i : tof_channels) {
    tcatof(i);
    sensor.init();
    sensor.setTimeout(500);
    if (!sensor.init())
    {
      Serial.println("Failed to detect and initialize sensor!");
      while (1) {}
    }
    sensor.startContinuous();
    int dist = sensor.readRangeContinuousMillimeters();
    Serial.println(dist);
  }
  Serial.println("Setup Done");

  //temperature sensors
  for (int i : mlxports) {
    tcaselect(i);
    mlx.begin();
    if (temp > 120) {
      temp = mlx.readObjectTempF();
    }
  }
  temp += 3;
  Serial.print("Heat victim threshold = ");
  Serial.println(temp);

  //STEPPER
  stepper.setSpeed(STEPPER_SPEED_RPM);

  tcatof(7);
  //color sensor
  if (tcs.begin()) {
    //Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
  }

  // use these three pins to drive an LED
#if defined(ARDUINO_ARCH_ESP32)
  ledcAttachPin(redpin, 1);
  ledcSetup(1, 12000, 8);
  ledcAttachPin(greenpin, 2);
  ledcSetup(2, 12000, 8);
  ledcAttachPin(bluepin, 3);
  ledcSetup(3, 12000, 8);
#else
  pinMode(redpin, OUTPUT);
  pinMode(greenpin, OUTPUT);
  pinMode(bluepin, OUTPUT);
#endif
  // it helps convert RGB colors to what humans see
  for (int i = 0; i < 256; i++) {
    float x = i;
    x /= 255;
    x = pow(x, 2.5);
    x *= 255;

    if (commonAnode) {
      gammatable[i] = 255 - x;
    } else {
      gammatable[i] = x;
    }
    //Serial.println(gammatable[i]);
  }
}


void loop() {

  readTile();

  int prevCur[2];

  do {

    prevCur[0] = curR;
    prevCur[1] = curC;
    printf("\nsimple traverse\n");
    simpleTraverse();

    printf("\n\nbfs\n");
  } while (bfs());

  Serial.print(bfs());

  field[9][9].visited = 0;

  bfs();

  //toStart();

  while (1);
}
