// --- Pin Definitions ---
const int xPin = A0;
const int yPin = A1;
const int zPin = A2;

const int ledPin = 3;
const int buzzerPin = 4;

// --- Calibration Variables ---
int xBase, yBase, zBase;

// --- Settings ---
const int threshold = 300;              // Sensitivity threshold
const int sampleCount = 10;            // Number of samples to average
const unsigned long cooldown = 3000;   // Time between detections (ms)

unsigned long lastTriggerTime = 0;

void setup() {
  Serial.begin(9600);

  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  // --- Calibration ---
  Serial.println("Calibrating... Keep the device still.");
  delay(2000); // Wait for stability

  long xSum = 0, ySum = 0, zSum = 0;
  for (int i = 0; i < sampleCount; i++) {
    xSum += analogRead(xPin);
    ySum += analogRead(yPin);
    zSum += analogRead(zPin);
    delay(10);
  }

  xBase = xSum / sampleCount;
  yBase = ySum / sampleCount;
  zBase = zSum / sampleCount;

  Serial.println("Calibration complete.");
  Serial.print("X Base: "); Serial.println(xBase);
  Serial.print("Y Base: "); Serial.println(yBase);
  Serial.print("Z Base: "); Serial.println(zBase);
}

void loop() {
  long xTotal = 0, yTotal = 0, zTotal = 0;

  // --- Read and average multiple samples ---
  for (int i = 0; i < sampleCount; i++) {
    xTotal += analogRead(xPin);
    yTotal += analogRead(yPin);
    zTotal += analogRead(zPin);
    delay(5);
  }

  int xVal = xTotal / sampleCount;
  int yVal = yTotal / sampleCount;
  int zVal = zTotal / sampleCount;

  // --- Calculate deviation from baseline ---
  int dx = abs(xVal - xBase);
  int dy = abs(yVal - yBase);
  int dz = abs(zVal - zBase);

  int totalChange = dx + dy + dz;

  Serial.print("ΔX: "); Serial.print(dx);
  Serial.print(" ΔY: "); Serial.print(dy);
  Serial.print(" ΔZ: "); Serial.print(dz);
  Serial.print(" Total: "); Serial.println(totalChange);

  // --- Earthquake detection logic ---
  if (totalChange > threshold && millis() - lastTriggerTime > cooldown) {
    Serial.println("⚠️ EARTHQUAKE DETECTED!");
    digitalWrite(ledPin, HIGH);
    digitalWrite(buzzerPin, HIGH);
    lastTriggerTime = millis();

    delay(5000); // Keep buzzer and LED on for 5 seconds

    digitalWrite(ledPin, LOW);
    digitalWrite(buzzerPin, LOW);
  }

  delay(100); // Loop delay
}
