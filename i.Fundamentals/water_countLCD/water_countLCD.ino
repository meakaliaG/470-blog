#include <LiquidCrystal.h>
#include <EEPROM.h>

// LCD 
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// BUTTON
const int buttonPin = 6;
int buttonState     = HIGH;
int lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

// ULTRASONIC
const int trigPin = 9;
const int echoPin = 10;
const float PRESENCE_THRESHOLD_CM = 60.0;
const unsigned long PRESENCE_CONFIRM_MS = 500;

// STATE
enum State { NO_ONE, SOMEONE_PRESENT };
State currentState = NO_ONE;
unsigned long presenceStart = 0;

// EEPROM
const int ADDR_DRINK = 0; // 4 bytes
const int ADDR_VISIT = 4; // 4 bytes
const int ADDR_MAGIC = 8; // 1 byte
const byte MAGIC_VAL = 0xAB;

// COUNTS
int drinkCount = 0;
int visitCount = 0;

// LCD refresh
unsigned long lastLCDUpdate = 0;
const unsigned long LCD_INTERVAL = 250;

// -------------------------------------------------------

float measureDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000UL); // 30 ms timeout
  if (duration == 0) return -1.0;                  // timeout / no echo
  return (duration * 0.0343) / 2.0;
}

void refreshLCD() {
  lcd.setCursor(0, 0);
  lcd.print("Refilled here: ");
  lcd.print(drinkCount);
  lcd.print("        ");

  lcd.setCursor(0, 1);
  if (currentState == SOMEONE_PRESENT) {
    unsigned long secs = (millis() - presenceStart) / 1000;
    lcd.print("HERE ");
    lcd.print(secs);
    lcd.print("s #");
    lcd.print(visitCount);
    lcd.print("   ");
  } else {
    lcd.print("Visits: ");
    lcd.print(visitCount);
    lcd.print("        ");
  }
}

// -------------------------------------------------------

void setup() {
  Serial.begin(9600);

  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // --- EEPROM: magic number check ---
  byte magic;
  EEPROM.get(ADDR_MAGIC, magic);
  if (magic != MAGIC_VAL) {
    // First boot or blank chip — initialize everything cleanly
    drinkCount = 0;
    visitCount = 0;
    EEPROM.put(ADDR_DRINK, drinkCount);
    EEPROM.put(ADDR_VISIT, visitCount);
    EEPROM.put(ADDR_MAGIC, MAGIC_VAL);
    Serial.println("Fresh EEPROM — counts initialized to 0.");
  } else {
    EEPROM.get(ADDR_DRINK, drinkCount);
    EEPROM.get(ADDR_VISIT, visitCount);
    Serial.println("=== Restored from EEPROM ===");
    Serial.print("drinkCount: "); Serial.println(drinkCount);
    Serial.print("visitCount: "); Serial.println(visitCount);
  }

  lcd.begin(16, 2);
  // hold button to reset counts
  if (digitalRead(buttonPin) == LOW) {
    drinkCount = 0;
    visitCount = 0;
    EEPROM.put(ADDR_DRINK, drinkCount);
    EEPROM.put(ADDR_VISIT, visitCount);
    lcd.clear();
    lcd.print("Counts reset!");
    Serial.println("Counts reset via button hold.");
    delay(2000);
    refreshLCD();
  }
}

void loop() {

  // accurate button read
  int reading = digitalRead(buttonPin);
  if (reading != lastButtonState) lastDebounceTime = millis();

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == LOW) {
        drinkCount++;
        EEPROM.put(ADDR_DRINK, drinkCount);
        Serial.print("Drink logged. Total: ");
        Serial.println(drinkCount);
      }
    }
  }
  lastButtonState = reading;

  // presence detection
  float dist = measureDistance();

  if (dist > 0) { // ignore failed/timed-out readings 
    bool close = (dist < PRESENCE_THRESHOLD_CM);

    if (currentState == NO_ONE) {
      if (close) {
        if (presenceStart == 0) {
          presenceStart = millis();
        } else if (millis() - presenceStart >= PRESENCE_CONFIRM_MS) {
          currentState = SOMEONE_PRESENT;
          visitCount++;
          EEPROM.put(ADDR_VISIT, visitCount);
          Serial.print("SOMEONE_PRESENT. Visit #");
          Serial.println(visitCount);
        }
      } else {
        presenceStart = 0; // valid reading but out of range
      }

    } else { // SOMEONE_PRESENT
      if (!close) {
        Serial.print("Left after ");
        Serial.print((millis() - presenceStart) / 1000.0, 1);
        Serial.println(" s");
        currentState = NO_ONE;
        presenceStart = 0;
      }
    }

    // Serial.println(dist);
  }

  // LCD refresh
  if (millis() - lastLCDUpdate >= LCD_INTERVAL) {
    refreshLCD();
    lastLCDUpdate = millis();
  }
}