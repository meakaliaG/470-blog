// Pin definitions
const int photocellPin = A0;
const int buttonPin = 2;
const int greenLED = 11;
const int yellowLED = 10;
const int redLED = 9;
const int blueLED = 8;
const int buzzer = 12;

// State definitions
enum State {
  HEALTHY,
  NEEDS_LIGHT,
  WILTING,
  CRITICAL,
  RECOVERY
};

State currentState = HEALTHY;

// Variables
int lightLevel = 0;
int buttonState = 0;
int lastButtonState = 0;
unsigned long stateTimer = 0;
const int lightThreshold = 100;  // Adjust based on light level of room
const unsigned long wiltingDelay = 5000;  // 5 seconds before wilting
const unsigned long criticalDelay = 10000;  // 10 seconds in wilting = critical
const unsigned long recoveryDuration = 5000;  // 5 seconds recovery

// Watering variables
bool isWatering = false;
unsigned long wateringStartTime = 0;
const unsigned long wateringDuration = 3000;

void setup() {
  pinMode(photocellPin, INPUT);
  pinMode(buttonPin, INPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(blueLED, OUTPUT);
  pinMode(buzzer, OUTPUT);
  
  Serial.begin(9600);
  stateTimer = millis();
  
  Serial.println("=== Plant Care Monitor Started ===");
  Serial.println("5 States: HEALTHY, NEEDS_LIGHT, WILTING, CRITICAL, RECOVERY");
  Serial.println("Press button anytime to water!");
  Serial.println();
}

void loop() {
  // Read sensors
  lightLevel = analogRead(photocellPin);
  buttonState = digitalRead(buttonPin);
  
  // Debug output
  Serial.print("State: ");
  printStateName(currentState);
  Serial.print(" | Light: ");
  Serial.print(lightLevel);
  Serial.print(" | Time in state: ");
  Serial.print((millis() - stateTimer) / 1000);
  Serial.print("s | Watering: ");
  Serial.println(isWatering ? "YES" : "NO");
  
  // Button press - trigger in any state
  if (buttonState == HIGH && lastButtonState == LOW) {
    isWatering = true;
    wateringStartTime = millis();
    Serial.println(">>> Button pressed! Watering started.");
  }
  lastButtonState = buttonState;
  
  // Watering output
  if (isWatering) {
    handleWateringAnimation();
    if (millis() - wateringStartTime > wateringDuration) {
      isWatering = false;
      digitalWrite(blueLED, LOW);
      Serial.println(">>> Watering complete.");
    }
  } else {
    digitalWrite(blueLED, LOW);
  }
  
  // Main state machine logic
  switch(currentState) {
    case HEALTHY:
      handleHealthyState();
      break;
    case NEEDS_LIGHT:
      handleNeedsLightState();
      break;
    case WILTING:
      handleWiltingState();
      break;
    case CRITICAL:
      handleCriticalState();
      break;
    case RECOVERY:
      handleRecoveryState();
      break;
  }
  
  delay(100);
}

void printStateName(State state) {
  switch(state) {
    case HEALTHY: Serial.print("HEALTHY    "); break;
    case NEEDS_LIGHT: Serial.print("NEEDS_LIGHT"); break;
    case WILTING: Serial.print("WILTING    "); break;
    case CRITICAL: Serial.print("CRITICAL   "); break;
    case RECOVERY: Serial.print("RECOVERY   "); break;
  }
}

void handleWateringAnimation() {
  // Blue LED pulses while watering
  int pulseSpeed = 200;
  if ((millis() / pulseSpeed) % 2 == 0) {
    digitalWrite(blueLED, HIGH);
  } else {
    digitalWrite(blueLED, LOW);
  }
}

void handleHealthyState() {
  // Green LED glowing
  digitalWrite(greenLED, HIGH);
  digitalWrite(yellowLED, LOW);
  digitalWrite(redLED, LOW);
  noTone(buzzer);
  
  // Check if light drops
  if (lightLevel < lightThreshold) {
    currentState = NEEDS_LIGHT;
    stateTimer = millis();
    Serial.println();
    Serial.println(">>> State changed to NEEDS_LIGHT - light level too low!");
  }
}

void handleNeedsLightState() {
  // Yellow LED blinks slowly
  digitalWrite(greenLED, LOW);
  digitalWrite(redLED, LOW);
  noTone(buzzer);
  
  int blinkSpeed = 500;
  if ((millis() / blinkSpeed) % 2 == 0) {
    digitalWrite(yellowLED, HIGH);
  } else {
    digitalWrite(yellowLED, LOW);
  }
  
  // Check if light improves
  if (lightLevel >= lightThreshold) {
    currentState = HEALTHY;
    stateTimer = millis();
    Serial.println();
    Serial.println(">>> State changed to HEALTHY - light restored!");
  }
  
  // Check if plant starts wilting (5+ seconds without light)
  if (millis() - stateTimer > wiltingDelay) {
    currentState = WILTING;
    stateTimer = millis();
    Serial.println();
    Serial.println(">>> State changed to WILTING - needs light urgently!");
  }
}

void handleWiltingState() {
  // Alternating yellow/red blink - warning!
  digitalWrite(greenLED, LOW);
  
  int blinkSpeed = 300;
  if ((millis() / blinkSpeed) % 2 == 0) {
    digitalWrite(yellowLED, HIGH);
    digitalWrite(redLED, LOW);
  } else {
    digitalWrite(yellowLED, LOW);
    digitalWrite(redLED, HIGH);
  }
  
  // Soft beep occasionally
  if ((millis() / 2000) % 2 == 0 && (millis() % 2000) < 100) {
    tone(buzzer, 600, 50);  // warning beep
  }
  
  // Check if light improves
  if (lightLevel >= lightThreshold) {
    currentState = RECOVERY;
    stateTimer = millis();
    Serial.println();
    Serial.println(">>> State changed to RECOVERY - light restored, plant recovering!");
  }
  
  // Check if it gets worse (becomes critical after 10+ seconds)
  if (millis() - stateTimer > criticalDelay) {
    currentState = CRITICAL;
    stateTimer = millis();
    Serial.println();
    Serial.println(">>> State changed to CRITICAL - plant in danger!");
  }
}

void handleCriticalState() {
  // Red LED blinks fast, buzzer chirps urgently
  digitalWrite(greenLED, LOW);
  digitalWrite(yellowLED, LOW);
  
  int blinkSpeed = 250;
  if ((millis() / blinkSpeed) % 2 == 0) {
    digitalWrite(redLED, HIGH);
    tone(buzzer, 800, 50);  // annoying beep
  } else {
    digitalWrite(redLED, LOW);
  }
  
  // Recovery requires light
  if (lightLevel >= lightThreshold) {
    currentState = RECOVERY;
    stateTimer = millis();
    Serial.println();
    Serial.println(">>> State changed to RECOVERY - critical plant being saved!");
  }
}

void handleRecoveryState() {
  // Green LED fades in slowly
  digitalWrite(yellowLED, LOW);
  digitalWrite(redLED, LOW);
  noTone(buzzer);
  
  // Faster blinking over time = fade-in effect
  unsigned long elapsed = millis() - stateTimer;
  int fadeSpeed = map(elapsed, 0, recoveryDuration, 200, 50);
  fadeSpeed = constrain(fadeSpeed, 50, 200);  // Keep in range
  
  if ((millis() / fadeSpeed) % 2 == 0) {
    digitalWrite(greenLED, HIGH);
  } else {
    digitalWrite(greenLED, LOW);
  }
  
  // Check conditions for full recovery
  if (millis() - stateTimer > recoveryDuration && lightLevel >= lightThreshold) {
    currentState = HEALTHY;
    digitalWrite(greenLED, HIGH);
    Serial.println();
    Serial.println(">>> State changed to HEALTHY - recovery complete!");
  }
  
  // If light drops again during recovery, relapse
  if (lightLevel < lightThreshold) {
    currentState = NEEDS_LIGHT;
    stateTimer = millis();
    Serial.println();
    Serial.println(">>> State changed to NEEDS_LIGHT - relapse during recovery!");
  }
}
