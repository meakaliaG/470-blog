import processing.sound.*;
import processing.serial.*;

SinOsc osc;

Serial myPort;

void setup() {
  size(600, 400);

  println("Available ports:");
  for (int i = 0; i < Serial.list().length; i++) {
    println(i + ": " + Serial.list()[i]);
  }
  myPort = new Serial(this, "/dev/cu.usbmodem11201", 9600);
  delay(1000);
  println(myPort);

  osc = new SinOsc(this);
  osc.play();
}

void draw() {
  if (myPort.available() > 0) {
  String data = myPort.readStringUntil('\n');

  if (data != null) {
    data = trim(data);
    println("data: " + data);

    // skip empty lines
    if (data.length() == 0) return;

    String[] values = split(data, ',');

    // make sure we have exactly 2 values
    if (values.length != 2) {
      println("⚠️ Bad data: " + data);
      return;
    }

    // extra safety: avoid empty strings
    if (values[0].equals("") || values[1].equals("")) {
      println("⚠️ Incomplete data: " + data);
      return;
    }

    float distance = float(values[0]);
    float light = float(values[1]);

    // prevent NaN propagation
    if (Float.isNaN(distance) || Float.isNaN(light)) {
      println("⚠️ NaN detected");
      return;
    }

    float freq = map(distance, 5, 50, 2000, 100);
    float amp  = map(light, 0, 1023, 0.05, 1);

    osc.freq(freq);
    osc.amp(amp);

    background(freq/3, 0, amp*255);
  }
}
}
