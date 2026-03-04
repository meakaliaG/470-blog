void setup() {
  // start serial port at 9600 bps:
  Serial.begin(9600);
}

void loop() {
  // read analog input, map it to make the range 0-255:
  int sensorVal = analogRead(A0);
  int mappedVal = map(sensorVal, 0, 1023, 0, 255);
  
  // print different formats:
  Serial.write(mappedVal); // Print the raw binary value
  Serial.print('\t'); // print a tab
  // print ASCII-encoded values:
  Serial.print(mappedVal, BIN); // print ASCII-encoded binary value
  Serial.print('\t'); // print a tab
  Serial.print(mappedVal); // print decimal value
  Serial.print('\t'); // print a tab
  Serial.print(mappedVal, HEX); // print hexadecimal value
  Serial.print('\t'); // print a tab
  Serial.print(mappedVal, OCT); // print octal value
  Serial.println(); // print linefeed and carriage return
  
  delay(500); // 0.5 second delay so we can see the numbers better
}
