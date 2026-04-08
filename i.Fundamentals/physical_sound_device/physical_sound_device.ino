
#include <Servo.h>

// --- Pins ---
int trigPin = 9;
int echoPin = 10;
int servoPin = 6;

// --- Servo ---
Servo drummer;

// --- Sensor tracking ---
int lastDistance = 0;
int threshold = 10; // sensitivity to change

// --- Timing ---
unsigned long lastHit = 0;
int cooldown = 200; // ms between hits

void setup() {
  Serial.begin(9600);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  drummer.attach(servoPin);
  drummer.write(90); // resting position
}

void loop() {
  int distance = getDistance();
  int change = abs(distance - lastDistance);

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print(" | Change: ");
  Serial.println(change);

  // Detect sudden disturbance
  if (change > threshold && millis() - lastHit > cooldown) {
    triggerDrum(change/threshold);
    lastHit = millis();
  }

  lastDistance = distance;

  delay(50);
}

// --- Distance function ---
int getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  int distance = duration * 0.034 / 2;

  return distance;
}

// --- Drum hit function ---
void triggerDrum(int intensity) {
  // Map intensity to movement range
  int hitStrength = constrain(map(intensity, 5, 30, 20, 60), 20, 60);

  int forward = 90 + hitStrength;
  int back = 90 - hitStrength;

  // quick hit
  drummer.write(forward);
  delay(40);
  drummer.write(back);
  delay(40);

  // return to rest
  drummer.write(90);
}