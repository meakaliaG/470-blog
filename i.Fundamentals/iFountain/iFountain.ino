#include <Stepper.h>

// ── Pins ─────────────────────────────────────────────────────
const int BUTTON_A_PIN = 3;
const int BUTTON_B_PIN = 5;

const int IN1 = 8, IN2 = 9, IN3 = 10, IN4 = 11;

// ── Stepper config ───────────────────────────────────────────
const int STEPS_PER_REV = 2048;
const int RPM           = 10;

Stepper stepper(STEPS_PER_REV, IN1, IN3, IN2, IN4);

// ── Answer key ───────────────────────────────────────────────
const char ANSWER_KEY[5]   = {'A', 'B', 'A', 'A', 'B'};
const int  TOTAL_QUESTIONS = 5;

// ── Timing ───────────────────────────────────────────────────
const unsigned long HOLD_MS = 1500; // pause at rotated position before returning

// ── State ────────────────────────────────────────────────────
int  currentQuestion   = 0;
bool waitingForRelease = false;

// ── Helpers ──────────────────────────────────────────────────

void coilsOff() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void rotateAndReturn(int steps) {
  // 1. Rotate to position
  stepper.step(steps);

  // 2. Hold so the result is visible
  // coilsOff();
  delay(HOLD_MS);

  // 3. Return to home
  stepper.step(-steps);

  // 4. Power down coils
  coilsOff();
}

// ── Answer logic ─────────────────────────────────────────────

void processAnswer(char pressed) {
  bool correct = (pressed == ANSWER_KEY[currentQuestion]);
  currentQuestion++;

  Serial.print("Q"); Serial.print(currentQuestion);
  Serial.print(": pressed="); Serial.print(pressed);
  Serial.print("  answer="); Serial.print(ANSWER_KEY[currentQuestion - 1]);
  Serial.println(correct ? "  CORRECT" : "  WRONG");

  // Wrong → CW one full rotation then home
  // Correct → CCW one full rotation then home
  rotateAndReturn(correct ? -STEPS_PER_REV/2.5 : STEPS_PER_REV/2.5);

  if (currentQuestion >= TOTAL_QUESTIONS) {
    Serial.println("Quiz complete! Reset to play again.");
  }
}

// ── Setup ────────────────────────────────────────────────────

void setup() {
  Serial.begin(9600);

  pinMode(BUTTON_A_PIN, INPUT_PULLUP);
  pinMode(BUTTON_B_PIN, INPUT_PULLUP);
  delay(100);

  Serial.print("D3 (Button A) at boot: ");
  Serial.println(digitalRead(BUTTON_A_PIN) == LOW ? "LOW — check wiring!" : "HIGH (OK)");
  Serial.print("D5 (Button B) at boot: ");
  Serial.println(digitalRead(BUTTON_B_PIN) == LOW ? "LOW — check wiring!" : "HIGH (OK)");

  stepper.setSpeed(RPM);

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