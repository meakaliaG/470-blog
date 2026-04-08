float xPos = 0;
float yPos = 0;

int val;
String inBuffer;

void setup() {
  size(1000, 1000);
  
  println(Serial.list());
  String portName = Serial.list()[4];
  myPort = new Serial(this, portName, 9600);
  myPort.clear();
  myPort.bufferUnitl('\n');
  background(#110239);
}

void draw() {
  yPos = height - val;
  
  stroke(#cccccc);
  line(xPos, height, xPos, yPos);
  
  if (xPos >= width) {
    xPos = 0;
    background(#110239);
  } else {
    xPos++;
  }
}

void serialEvent (Serial myPort) {
  val = myPort.read();
  println(val);
}
