import processing.serial.*;

Serial myPort;
float xPos = 1;
float yPos = 0;

void setup() {
  size(800, 400);
  background(0);

  // Replace with your actual serial port name:
  String portName = Serial.list()[0];
  myPort = new Serial(this, portName, 9600);
  myPort.bufferUntil('\n'); // buffer until newline
}

void draw() {
  // drawing happens in serialEvent()
}

void serialEvent(Serial myPort) {
  String inString = myPort.readStringUntil('\n');
  if (inString != null) {
    inString = inString.trim();
    float inByte = float(inString);
    // map 0-1023 to screen height
    inByte = map(inByte, 0, 1023, 0, height);

    stroke(0, 200, 100);
    line(xPos - 1, yPos, xPos, height - inByte);
    yPos = height - inByte;

    if (xPos >= width) {
      xPos = 1;
      background(0);
    } else {
      xPos++;
    }
  }
}

