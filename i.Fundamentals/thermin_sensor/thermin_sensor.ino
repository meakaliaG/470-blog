 int trigPin = 9;
int echoPin = 10;
int ldrPin = A0;
int ledPin = 7;

// define effective ranges
int minDistance = 5;
int maxDistance = 50;

int minLight = 17;
int maxLight = 36;

void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // Ultrasonic distance
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  int distance = duration * 0.034 / 2;

  int lightValue = analogRead(ldrPin);

  // check if either sensor is out of range
  bool distanceOut = (distance < minDistance || distance > maxDistance);
  bool lightOut = (lightValue < minLight || lightValue > maxLight);

  // turn LED on if either condition is true
  if (distanceOut || lightOut) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }

  

  Serial.print(distance);
  Serial.print(",");
  Serial.println(lightValue);

  

  delay(50);
}
