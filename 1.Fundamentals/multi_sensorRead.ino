int pmPin = A5;
int pcPin = A3;
int buttonPin = A0;


void setup() {
  Serial.begin(9600);
}

void loop() {
  int pmVal = analogRead(pmPin);
  int pcVal = analogRead(pcPin);
  int buttonBool = analogRead(buttonPin);

  int pmOutput = map(pmVal, 0, 1023, 0, 255);

  Serial.print("sensorPinA0: ");
  Serial.print(buttonBool);

  Serial.print("\t sensorPinA3: ");
  Serial.println(pcVal);

  Serial.print("\t sensorPinA5: ");
  Serial.println(pmOutput);

  delay(1);
}
