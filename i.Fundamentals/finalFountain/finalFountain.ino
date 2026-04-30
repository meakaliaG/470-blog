#include <Arduino.h>

// ── Pins ─────────────────────────────────────────────────────
const int BUTTON_A_PIN  = 3;
const int BUTTON_B_PIN  = 5;

const int PUMP_CORRECT_PIN = 6;   // pump that fires on a correct answer
const int PUMP_WRONG_PIN   = 9;   // pump that fires on a wrong answer

// ── Answer key ───────────────────────────────────────────────
const char ANSWER_KEY[5]   = {'A', 'B', 'A', 'A', 'B'};
const int  TOTAL_QUESTIONS = 5;

// ── Timing ───────────────────────────────────────────────────
const unsigned long PUMP_MS = 750;   // how long each pump runs (ms)

// ── State ────────────────────────────────────────────────────
int  currentQuestion   = 0;
bool waitingForRelease = false;

// ── Helpers ──────────────────────────────────────────────────

const int PUMP_POWER = 140;  // 0 = off, 255 = full power — tune this value

void pulsePump(int pin) {
  analogWrite(pin, PUMP_POWER);
  delay(PUMP_MS);
  analogWrite(pin, 0);
}
// ── Answer logic ─────────────────────────────────────────────

void processAnswer(char pressed) {
  bool correct = (pressed == ANSWER_KEY[currentQuestion]);
  currentQuestion++;

  Serial.print("Q"); Serial.print(currentQuestion);
  Serial.print(": pressed="); Serial.print(pressed);
  Serial.print("  answer="); Serial.print(ANSWER_KEY[currentQuestion - 1]);
  Serial.println(correct ? "  CORRECT" : "  WRONG");

  pulsePump(correct ? PUMP_CORRECT_PIN : PUMP_WRONG_PIN);

  if (currentQuestion >= TOTAL_QUESTIONS) {
    Serial.println("Quiz complete! Reset to play again.");
  }
}

// ── Setup ────────────────────────────────────────────────────

void setup() {
  Serial.begin(9600);

  pinMode(BUTTON_A_PIN,    INPUT_PULLUP);
  pinMode(BUTTON_B_PIN,    INPUT_PULLUP);
  pinMode(PUMP_CORRECT_PIN, OUTPUT);
  pinMode(PUMP_WRONG_PIN,   OUTPUT);

  digitalWrite(PUMP_CORRECT_PIN, LOW);
  digitalWrite(PUMP_WRONG_PIN,   LOW);

  delay(100);

  Serial.print("D3 (Button A) at boot: ");
  Serial.println(digitalRead(BUTTON_A_PIN) == LOW ? "LOW — check wiring!" : "HIGH (OK)");
  Serial.print("D5 (Button B) at boot: ");
  Serial.println(digitalRead(BUTTON_B_PIN) == LOW ? "LOW — check wiring!" : "HIGH (OK)");

  Serial.println("Quiz ready. Answer Q1 by pressing A or B.");
}

// ── Loop ─────────────────────────────────────────────────────

void loop() {
  if (currentQuestion >= TOTAL_QUESTIONS) return;

  bool aPressed = (digitalRead(BUTTON_A_PIN) == LOW);
  bool bPressed = (digitalRead(BUTTON_B_PIN) == LOW);

  if (waitingForRelease) {
    if (!aPressed && !bPressed) waitingForRelease = false;
    return;
  }

  if (!aPressed && !bPressed) return;

  waitingForRelease = true;
  processAnswer(aPressed ? 'A' : 'B');
}
