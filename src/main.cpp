#include <Arduino.h>
#include <ESP32Servo.h>
#include <ArduinoJson.h>
#include <initwifi.h>
#include <FirebaseControl.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Setup Variable control
bool isOn = false;
bool isLDR = false; // true when dark, false when light
bool isRain = false; // true when rain, false when no rain
int ldrValue = 0;
int rainValue = 0;
int servoValue = 0;
bool isSignUp = false;
int modeValue = 0;
bool modeAuto = false;

// Setup pin
int servoPin = 13;
int ldrPin = 32;
int rainPin = 33;

// Create a servo object
Servo servo;

// Task handles
TaskHandle_t task1 = NULL;
TaskHandle_t task2 = NULL;

// Function declaration
void servoOn() {
  servo.write(90);
}
void servoOff() {
  servo.write(0);
}

void ldrRead() {
  ldrValue = analogRead(ldrPin);
  if (ldrValue > 1000) {
    isLDR = true;
  } else {
    isLDR = false;
  }
}

void rainRead() {
  rainValue = analogRead(rainPin);
  if (rainValue < 2500) {
    isRain = true;
  } else {
    isRain = false;
  }
}

void controlFirebase(void *pvParameter) {
  while (1) {
    if (Firebase.ready()) {
      // Get data from Firebase database
      if (Firebase.RTDB.getInt(&fbdo, "/servo")) {
        if (fbdo.dataType() == "int") {
          servoValue = fbdo.intData();
        }
      }
    }
    vTaskDelay(100 / portTICK_PERIOD_MS); // Use vTaskDelay instead of delay()
  }
}


void setModeAuto(void *pvParameter) {
  while (1) {
    if (Firebase.ready()) {
      // Get data from firebase database
      if (Firebase.RTDB.getInt(&fbdo, "/isAuto")) {
        if (fbdo.dataType() == "int") {
          modeValue = fbdo.intData();
        }
      }
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);; // Small delay to prevent overloading the Firebase task
  }
}

void setup() {
  Serial.begin(115200);
  // Initialize LCD
  lcd.init();  // Initialize the lcd
  lcd.backlight();
  lcd.clear();

  // Welcoming
  lcd.setCursor(0, 0);
  const char *welcome = "~~SLMT  DATANG~~";
  for (int i = 0; i < strlen(welcome); i++) {
    lcd.print(welcome[i]);
    delay(100);
  }
  lcd.setCursor(0, 1);
  const char *project = "~~Project ETA~~";
  for (int i = 0; i < strlen(project); i++) {
    lcd.print(project[i]);
    delay(100);
  }
  delay(1000);
  lcd.clear();

  // Connect to Wi-Fi
  // InitWifi();
  WiFi.begin("yubi", "udinganteng123");
  FirebaseSetup();

  // Attach the servo object to the servo pin
  pinMode(ldrPin, INPUT);
  servo.attach(servoPin);
  servo.write(0);

  // Create tasks for Firebase and mode handling
  if(Firebase.ready()){
    xTaskCreatePinnedToCore(
      controlFirebase,
      "Control Firebase",
      16384,  // Increase stack size
      NULL,
      1,
      &task1,
      0
    );
  
    xTaskCreatePinnedToCore(
      setModeAuto,
      "Set Mode Auto",
      16384,  // Increase stack size
      NULL,
      1,
      &task2,
      1
    );
  }

  lcd.print("Setup Done");
}

void loop() {
  static int lastModeValue = -1;
  static bool lastIsLDR = false;
  static bool lastIsRain = false;
  static int lastServoValue = -1;

  // Regular loop operations based on mode
  Serial.print("MODE VALUE: ");
  Serial.println(modeValue);

  if (modeValue == 1) {  // Auto mode
    ldrRead();
    rainRead();
    Serial.print("LDR VALUE: ");
    Serial.println(ldrValue);
    Serial.print("RAIN VALUE: ");
    Serial.println(rainValue);

    if (isLDR || isRain) {
      // Write to database
      Firebase.RTDB.setInt(&fbdo, "/servo", 1);
      servoOn();
    } else {
      Firebase.RTDB.setInt(&fbdo, "/servo", 0);
      servoOff();
    }
  } else {  // Manual mode
    if (servoValue == 1) {
      servoOn();
    } else {
      servoOff();
    }
  }

  // Update the LCD display only if there is a change
  if (modeValue != lastModeValue || isLDR != lastIsLDR || isRain != lastIsRain || servoValue != lastServoValue) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Mode: ");
    if (modeValue == 1) {
      lcd.print("Auto");
    } else {
      lcd.print("Manual");
    }
    if (modeValue == 1) {
      lcd.setCursor(0, 1);
      lcd.print("LDR:");
      lcd.print(isLDR);
      lcd.setCursor(8, 1);
      lcd.print("RDS:");
      lcd.print(isRain);
    } else {
      lcd.setCursor(0, 1);
      lcd.print("Servo: ");
      lcd.print(servoValue);
    }
    lastModeValue = modeValue;
    lastIsLDR = isLDR;
    lastIsRain = isRain;
    lastServoValue = servoValue;
  }
  delay(1000);  // Update interval
}
