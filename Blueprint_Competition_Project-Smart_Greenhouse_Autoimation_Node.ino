// ============================================================
// PIN DEFINITIONS
// ============================================================

// Sensors (top row of Arduino)
#define SOUND_PIN 3 // Sound sensor DO
#define OBSTACLE_PIN 4 // Obstacle sensor OUT
#define TILT_PIN 5 // Tilt switch

// Outputs - Buzzers (top row)
#define BUZZER_PASSIVE 6 // Passive buzzer
#define BUZZER_ACTIVE 7 // Active buzzer

// Outputs - Individual indicator LEDs (top row)
#define LED_GREEN_IND 8 // Green indicator LED
#define LED_YELLOW_IND 12 // Yellow indicator LED
#define LED_RED_IND 13 // Red indicator LED

// Outputs - RGB LED (MUST be on PWM pins 9, 10, 11)
#define LED_R 9 // RGB red leg
#define LED_G 10 // RGB green leg
#define LED_B 11 // RGB blue leg

// Sensors (bottom row of Arduino - analog pins)
#define SOIL_PIN A0 // Soil humidity sensor
#define WATER_PIN A1 // Water level sensor
#define LIGHT_PIN A2 // Photoresistor
#define THERMO_PIN A3 // Thermistor
#define POT_PIN A4 // Potentiometer

// Button
#define BUTTON_1 A5 // Mute toggle button

// ============================================================
// THRESHOLDS
// ============================================================

#define SOIL_DRY 400 // Below this = soil too dry
#define WATER_LOW 300 // Below this = reservoir nearly empty
#define LIGHT_LOW 150 // Below this = not enough light

// ============================================================
// GLOBALS
// ============================================================

bool systemMuted = false;

// ============================================================
// SETUP
// ============================================================

void setup() {
Serial.begin(9600);

// Input pins
pinMode(SOUND_PIN, INPUT);
pinMode(OBSTACLE_PIN, INPUT);
pinMode(TILT_PIN, INPUT_PULLUP);
pinMode(BUTTON_1, INPUT_PULLUP);

// Output pins
pinMode(BUZZER_PASSIVE, OUTPUT);
pinMode(BUZZER_ACTIVE, OUTPUT);
pinMode(LED_R, OUTPUT);
pinMode(LED_G, OUTPUT);
pinMode(LED_B, OUTPUT);
pinMode(LED_GREEN_IND, OUTPUT);
pinMode(LED_YELLOW_IND, OUTPUT);
pinMode(LED_RED_IND, OUTPUT);

bootSequence();
}

// ============================================================
// MAIN LOOP
// ============================================================

void loop() {

// Read potentiometer - adjusts all thresholds up or down
int potVal = analogRead(POT_PIN);
int offset = map(potVal, 0, 1023, -100, 100);

// Read all sensors
int soilVal = analogRead(SOIL_PIN);
int waterVal = analogRead(WATER_PIN);
int lightVal = analogRead(LIGHT_PIN);

bool soundDetected = digitalRead(SOUND_PIN) == HIGH;
bool obstacleDetected = digitalRead(OBSTACLE_PIN) == LOW;
bool tiltDetected = digitalRead(TILT_PIN) == HIGH;

// Mute button toggle
if (digitalRead(BUTTON_1) == LOW) {
systemMuted = !systemMuted;
delay(300);
}

// Determine alert level

// CRITICAL: immediate danger
bool isCritical = (waterVal < WATER_LOW + offset)
|| tiltDetected;

// WARNING: attention needed
bool isWarning = (soilVal < SOIL_DRY + offset)
|| (lightVal < LIGHT_LOW)
|| soundDetected
|| obstacleDetected;

// Trigger outputs based on alert level
if (isCritical) {
setRGB(255, 0, 0);
digitalWrite(LED_RED_IND, HIGH);
digitalWrite(LED_YELLOW_IND, LOW);
digitalWrite(LED_GREEN_IND, LOW);
if (!systemMuted) {
digitalWrite(BUZZER_ACTIVE, HIGH);
delay(300);
digitalWrite(BUZZER_ACTIVE, LOW);
}
}
else if (isWarning) {
setRGB(255, 100, 0);
digitalWrite(LED_RED_IND, LOW);
digitalWrite(LED_YELLOW_IND, HIGH);
digitalWrite(LED_GREEN_IND, LOW);
if (!systemMuted) {
tone(BUZZER_PASSIVE, 1200, 150);
delay(3000);
}
}
else {
setRGB(0, 255, 0);
digitalWrite(LED_RED_IND, LOW);
digitalWrite(LED_YELLOW_IND, LOW);
digitalWrite(LED_GREEN_IND, HIGH);
noTone(BUZZER_PASSIVE);
digitalWrite(BUZZER_ACTIVE, LOW);
}

// Serial monitor output
Serial.println("====== GREENHOUSE STATUS ======");
Serial.print("Soil : "); Serial.println(soilVal);
Serial.print("Water Level : "); Serial.println(waterVal);
Serial.print("Light : "); Serial.println(lightVal);
Serial.print("Sound Alert : "); Serial.println(soundDetected ? "YES" : "no");
Serial.print("Obstacle : "); Serial.println(obstacleDetected ? "YES" : "no");
Serial.print("Tilt Alert : "); Serial.println(tiltDetected ? "YES" : "no");
Serial.print("Muted : "); Serial.println(systemMuted ? "YES" : "no");
Serial.print("Sensitivity : "); Serial.println(offset);

if (isCritical) Serial.println(">>> STATUS: CRITICAL <<<");
else if (isWarning) Serial.println(">>> STATUS: WARNING <<<");
else Serial.println(">>> STATUS: ALL CLEAR <<<");

Serial.println();
delay(1000);
}

// ============================================================
// HELPER FUNCTIONS
// ============================================================

void setRGB(int r, int g, int b) {
analogWrite(LED_R, r);
analogWrite(LED_G, g);
analogWrite(LED_B, b);
}

void bootSequence() {
Serial.println("Smart Greenhouse System - Booting...");

setRGB(255, 0, 0); delay(400);
setRGB(255, 165, 0); delay(400);
setRGB(255, 255, 0); delay(400);
setRGB(0, 255, 0); delay(400);
setRGB(0, 0, 255); delay(400);
setRGB(255, 255, 255); delay(200);

tone(BUZZER_PASSIVE, 1000, 100); delay(150);
tone(BUZZER_PASSIVE, 1500, 100); delay(150);
tone(BUZZER_PASSIVE, 2000, 200); delay(300);
noTone(BUZZER_PASSIVE);

setRGB(0, 255, 0);
Serial.println("System Ready.");
}




