#include <ESP32Servo.h>

#define TRIG_PIN 22  // ESP32 pin GPIO23 connected to Ultrasonic Sensor's TRIG pin
#define ECHO_PIN 23  // ESP32 pin GPIO22 connected to Ultrasonic Sensor's ECHO pin

int servoPin = 13;

float duration_us, distance_cm;
int servovalue = 0;

Servo myservo;

void measureDistance() {
  // generate a 10-microsecond pulse to TRIG pin
  digitalWrite(TRIG_PIN, LOW);  // ensure that the TRIG pin is initially low
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);  // start the pulse
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);  // stop the pulse

  // measure the duration of the pulse from the ECHO pin
  duration_us = pulseInLong(ECHO_PIN, HIGH);

  // calculate the distance in centimeters
  distance_cm = duration_us * 0.017;

  // print the value to Serial Monitor
  Serial.print("Distance: ");
  Serial.print(distance_cm);
  Serial.println(" cm");
}

void setup() {
  // initialize serial communication at 115200 bits per second
  Serial.begin(115200);

  // configure the trigger pin to output mode
  pinMode(TRIG_PIN, OUTPUT);

  // configure the echo pin to input mode
  pinMode(ECHO_PIN, INPUT);
  myservo.attach(servoPin);
}

void loop() {
  // measure the distance
  measureDistance();

  if(distance_cm > 10){
    
  }

  // delay between measurements

  delay(500);
}
