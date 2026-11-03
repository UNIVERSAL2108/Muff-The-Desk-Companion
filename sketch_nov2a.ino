
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define TRIG_PIN 5
#define ECHO_PIN 18

#define APPROACH_DISTANCE 30.0
#define LEAVE_DISTANCE 35.0
#define SLEEP_TIMEOUT 10000

Adafruit_SSD1306 display(
  SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1
);

enum DisplayMode {
  COMPANION_MODE,
  DISTANCE_MODE
};

enum BuddyState {
  IDLE,
  HAPPY,
  SLEEPING
};

DisplayMode currentMode = COMPANION_MODE;
BuddyState buddyState = IDLE;

float distanceCM = -1;

bool isNear = false;
bool hasValidReading = false;

unsigned long lastSensorRead = 0;
unsigned long lastDisplayUpdate = 0;
unsigned long lastPresence = 0;
unsigned long lastBlink = 0;
unsigned long blinkStart = 0;

bool blinking = false;

void readDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  unsigned long duration =
    pulseIn(ECHO_PIN, HIGH, 25000);

  if (duration == 0) {
    hasValidReading = false;
    isNear = false;
    distanceCM = -1;
    return;
  }

  hasValidReading = true;
  distanceCM = duration * 0.0343 / 2.0;

  if (distanceCM <= APPROACH_DISTANCE) {
    isNear = true;
    lastPresence = millis();
  }
  else if (distanceCM > LEAVE_DISTANCE) {
    isNear = false;
  }

  Serial.print("Distance: ");
  Serial.print(distanceCM);
  Serial.println(" cm");
}

void drawEyes(bool happy) {
  int pupilOffset = 0;

  if (hasValidReading && distanceCM > 0) {
    pupilOffset = map(
      constrain((int)distanceCM, 0, 100),
      0, 100, -3, 3
    );
  }

  bool closeEyes =
    blinking && millis() - blinkStart < 150;

  if (closeEyes) {
    display.drawLine(25, 27, 49, 27, SSD1306_WHITE);
    display.drawLine(79, 27, 103, 27, SSD1306_WHITE);
  }
  else if (happy) {
    display.fillCircle(37, 27, 9, SSD1306_WHITE);
    display.fillCircle(91, 27, 9, SSD1306_WHITE);

    display.fillCircle(37 + pupilOffset, 27, 3, SSD1306_BLACK);
    display.fillCircle(91 + pupilOffset, 27, 3, SSD1306_BLACK);
  }
  else {
    display.fillCircle(37, 27, 8, SSD1306_WHITE);
    display.fillCircle(91, 27, 8, SSD1306_WHITE);

    display.fillCircle(37 + pupilOffset, 27, 3, SSD1306_BLACK);
    display.fillCircle(91 + pupilOffset, 27, 3, SSD1306_BLACK);
  }
}


void drawSmile() {
  display.drawLine(53, 40, 56, 43, SSD1306_WHITE);
  display.drawLine(56, 43, 60, 45, SSD1306_WHITE);
  display.drawLine(60, 45, 64, 46, SSD1306_WHITE);
  display.drawLine(64, 46, 68, 45, SSD1306_WHITE);
  display.drawLine(68, 45, 72, 43, SSD1306_WHITE);
  display.drawLine(72, 43, 75, 40, SSD1306_WHITE);
}

void drawCompanion() {
  display.clearDisplay();

  display.setTextSize(1);
  display.setCursor(2, 2);
  display.print("DESKMATE");

  display.setCursor(88, 2);
  display.print("MODE 1");

  if (buddyState == SLEEPING) {
    display.drawLine(27, 29, 49, 29, SSD1306_WHITE);
    display.drawLine(79, 29, 101, 29, SSD1306_WHITE);

    display.setCursor(59, 43);
    display.print("zZ");

    display.setCursor(2, 55);
    display.print("Sleeping...");

  } else {
    drawEyes(buddyState == HAPPY);
    drawSmile();

    if (buddyState == HAPPY) {
      display.setCursor(2, 55);
      display.print("Hello there!");
    } else {
      display.setCursor(2, 55);
      display.print("Just chilling");
    }
  }

  // Distance shown in every companion state
  display.setCursor(82, 55);

  if (hasValidReading) {
    display.print(distanceCM, 0);
    display.print("cm");
  } else {
    display.print("--cm");
  }

  display.display();
}

void drawDistanceMode() {
  display.clearDisplay();

  display.setTextSize(1);
  display.setCursor(2, 2);
  display.print("DESKMATE DISTANCE");

  display.drawLine(0, 13, 127, 13, SSD1306_WHITE);

  if (hasValidReading) {
    display.setTextSize(3);
    display.setCursor(8, 23);
    display.print(distanceCM, 1);

    display.setTextSize(1);
    display.print(" cm");

    display.setCursor(2, 55);

    if (isNear) {
      display.print("Someone is near!");
    } else {
      display.print("Area is clear");
    }

  } else {
    display.setTextSize(2);
    display.setCursor(18, 27);
    display.print("NO ECHO");

    display.setTextSize(1);
    display.setCursor(2, 55);
    display.print("Check sensor");
  }

  display.display();
}

void setup() {
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  digitalWrite(TRIG_PIN, LOW);

  Wire.begin(21, 22);

  if (!display.begin(
    SSD1306_SWITCHCAPVCC, 0x3C
  )) {
    Serial.println("OLED ERROR!");
    while (true);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  lastPresence = millis();

  Serial.println();
  Serial.println("DESKMATE V3 READY");
  Serial.println("1 = Companion Mode");
  Serial.println("2 = Distance Mode");
}

void loop() {
  unsigned long now = millis();

  // Read sensor
  if (now - lastSensorRead >= 100) {
    lastSensorRead = now;
    readDistance();
  }

  // Serial mode selection
  while (Serial.available()) {
    char command = Serial.read();

    if (command == '1') {
      currentMode = COMPANION_MODE;
      Serial.println("Companion Mode");
    }

    if (command == '2') {
      currentMode = DISTANCE_MODE;
      Serial.println("Distance Mode");
    }
  }

  // Determine expression
  if (isNear) {
    buddyState = HAPPY;
  }
  else if (now - lastPresence >= SLEEP_TIMEOUT) {
    buddyState = SLEEPING;
  }
  else {
    buddyState = IDLE;
  }

  // Blink animation
  if (buddyState != SLEEPING &&
      now - lastBlink >= 3000) {
    blinking = true;
    blinkStart = now;
    lastBlink = now;
  }

  if (blinking && now - blinkStart >= 150) {
    blinking = false;
  }

  // Display refresh
  if (now - lastDisplayUpdate >= 100) {
    lastDisplayUpdate = now;1

    if (currentMode == COMPANION_MODE) {
      drawCompanion();
    } else {
      drawDistanceMode();
    }
  }
}
