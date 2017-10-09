//Bluetooth Setup
#include "SoftwareSerial.h"
SoftwareSerial serial_connection(9, 10); // TX on 10, RX on 11
#define BUFFER_SIZE 64
char inData[BUFFER_SIZE]; // Stores data from Python code
char inChar = -1;
int count = 0; // # of lines from Python code
int i = 0;

// Right motor
int InA1 = 8;
int InB1 = 7;
int PWM2 = 6;  //PWM2 connects to pin 6
int PWM2_val = 49; //(25% = 64; 50% = 127; 75% = 191; 100% = 255) was 30

// Left motor
int InA2 = 4;
int InB2 = 2;
int PWM1 = 3;  //PWM1 connects to pin 3
int PWM1_val = 45; //(25% = 64; 50% = 127; 75% = 191; 100% = 255) was 63

const int pingRightPin = 12;
const int pingLeftPin = 13;

void setup() {
  Serial.begin(9600);
  pinMode(InA2, OUTPUT);
  pinMode(InB2, OUTPUT);
  pinMode(PWM2, OUTPUT);
  pinMode(InA1, OUTPUT);
  pinMode(InB1, OUTPUT);
  pinMode(PWM1, OUTPUT);
  serial_connection.begin(9600);
  serial_connection.println("Ready");
  Serial.println("Started");
}
void loop() {
  //Follow a yellow color
  byte byte_count = serial_connection.available();
  long rightDuration, rightInches, rightCm, leftDuration, leftInches, leftCm;
  //Right sensor setup
  pinMode(pingRightPin, OUTPUT);
  digitalWrite(pingRightPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingRightPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingRightPin, LOW);
  pinMode(pingRightPin, INPUT);
  rightDuration = pulseIn(pingRightPin, HIGH);
  rightInches = microsecondsToInches(rightDuration);
  rightCm = microsecondsToCentimeters(rightDuration);
  //Left sensor setup
  pinMode(pingLeftPin, OUTPUT);
  digitalWrite(pingLeftPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingLeftPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingLeftPin, LOW);
  pinMode(pingLeftPin, INPUT);
  leftDuration = pulseIn(pingLeftPin, HIGH);
  leftInches = microsecondsToInches(leftDuration);
  leftCm = microsecondsToCentimeters(leftDuration);
  if (byte_count) {
    Serial.println("Incoming data");
    int first_bytes = byte_count;
    int remaining_bytes = 0;
    if (first_bytes >= BUFFER_SIZE - 1) {
      remaining_bytes = byte_count - (BUFFER_SIZE - 1);
    }
    for (i = 0; i < first_bytes; i++) {
      inChar = serial_connection.read();
      inData[i] = inChar;
    }
    inData[i] = '\0';
    // 1-STOP 2-LEFT 3-RIGHT 4-FORWARD
    if (String(inData) == "2" && leftInches > 8 && rightInches > 8) {
      rightForward();
      leftBackward();
    } else if (String(inData) == "3" && leftInches >8 && rightInches > 8) {
      rightBackward();
      leftForward();
    } else if (String(inData) == "4" && leftInches >8 && rightInches > 8) {
      rightForward();
      leftForward();
    } else if (String(inData) == "1" || String(inData) == "0" || leftInches <= 8 || rightInches <= 8) {
      //rightStop();
      //leftStop();
      rightForward();
      leftBackward();
    }
    for (i = 0; i < remaining_bytes; i++) {
      inChar = serial_connection.read();
    }
    Serial.println(inData);
    serial_connection.println("Hello from Blue " + String(count));
    count++;
  }


  //Avoid Obstacles
  //    long rightDuration, rightInches, rightCm, leftDuration, leftInches, leftCm;
  //    //Right sensor setup
  //    pinMode(pingRightPin, OUTPUT);
  //    digitalWrite(pingRightPin, LOW);
  //    delayMicroseconds(2);
  //    digitalWrite(pingRightPin, HIGH);
  //    delayMicroseconds(5);
  //    digitalWrite(pingRightPin, LOW);
  //    pinMode(pingRightPin, INPUT);
  //    rightDuration = pulseIn(pingRightPin, HIGH);
  //    rightInches = microsecondsToInches(rightDuration);
  //    rightCm = microsecondsToCentimeters(rightDuration);
  //    //Left sensor setup
  //    pinMode(pingLeftPin, OUTPUT);
  //    digitalWrite(pingLeftPin, LOW);
  //    delayMicroseconds(2);
  //    digitalWrite(pingLeftPin, HIGH);
  //    delayMicroseconds(5);
  //    digitalWrite(pingLeftPin, LOW);
  //    pinMode(pingLeftPin, INPUT);
  //    leftDuration = pulseIn(pingLeftPin, HIGH);
  //    leftInches = microsecondsToInches(leftDuration);
  //    leftCm = microsecondsToCentimeters(leftDuration);
  //    //If the sensors see a wall, turn accordingly
  //    if (leftInches < 24){
  //      //rightBackward();
  //      //leftForward();
  //      rightStop();
  //      leftStop();
  //      //delay(1);
  //    } else if (rightInches < 24) {
  //      //rightForward();
  //      //leftBackward();
  //      rightStop();
  //      leftStop();
  //      //delay(1);
  //    }
  //    else {
  //      //rightForward();
  //      //leftForward();
  //      rightStop();
  //      leftStop();
  //    }
  //delay(1);
}

long microsecondsToInches(long microseconds) {
  // According to Parallax's datasheet for the PING))), there are
  // 73.746 microseconds per inch (i.e. sound travels at 1130 feet per
  // second).  This gives the distance travelled by the ping, outbound
  // and return, so we divide by 2 to get the distance of the obstacle.
  // See: http://www.parallax.com/dl/docs/prod/acc/28015-PING-v1.3.pdf
  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds) {
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
}

void rightForward() {
  digitalWrite(InA2, HIGH);
  digitalWrite(InB2, LOW);
  analogWrite(PWM2, PWM2_val);
}

void rightBackward() {
  digitalWrite(InA2, LOW);
  digitalWrite(InB2, HIGH);
  analogWrite(PWM2, PWM2_val);
}

void rightStop() {
  digitalWrite(InA2, LOW);
  digitalWrite(InB2, LOW);
  analogWrite(PWM2, 0);
}

void leftForward() {
  digitalWrite(InA1, HIGH);
  digitalWrite(InB1, LOW);
  analogWrite(PWM1, PWM1_val);
}

void leftBackward() {
  digitalWrite(InA1, LOW);
  digitalWrite(InB1, HIGH);
  analogWrite(PWM1, PWM1_val);
}

void leftStop() {
  digitalWrite(InA1, LOW);
  digitalWrite(InB1, LOW);
  analogWrite(PWM1, 0l);
}

