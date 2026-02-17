int ledPinRed = 13;
int ledPinBlue = 3;
int pcPin = A3;

int int_delay = 0;

void setup() {
  Serial.begin(9600);
  pinMode(ledPinRed, OUTPUT);
  pinMode(ledPinBlue, OUTPUT);

}

void loop() {
  int pcVal = analogRead(pcPin);
  Serial.print("\t sensorPinA3: ");
  Serial.println(pcVal);

  if(pcVal < 0){
    analogWrite(ledPinRed, 0);
    analogWrite(ledPinBlue, 255);

  }

  if(pcVal > 30){
    analogWrite(ledPinBlue, 0);
    analogWrite(ledPinRed, 255);
  }
}
