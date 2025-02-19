#include <Arduino.h>
#include <ESP32Servo.h>
#include <ArduinoJson.h>

//  Include the required libraries I2C and LiquidCrystal.

// Include the wifi setting
// #include <wifiSetting.h>
#include <initwifi.h>
#include <FirebaseControl.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);

// Setup Variable control
bool isOn = false;
bool isLDR = false; // true ketika gelap dan false ketika terang
bool isRain = false; // true ketika hujan dan false ketika tidak hujan
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


// Function declaration
void servoOn(){
  servo.write(90);
};
void servoOff(){
  servo.write(0);
};

void ldrRead(){
  ldrValue = analogRead(ldrPin);
  if (ldrValue > 1000){
    isLDR = true;
  } else{
    isLDR = false;
  }
};
   
void rainRead(){
  rainValue = analogRead(rainPin);
  if (rainValue < 2500){
    isRain = true;
  } else{ 
    isRain = false;
  }
};



void controlFirebase(){
  if (Firebase.ready()){
    // get data from firebase database
    if (Firebase.RTDB.getInt(&fbdo, "/servo")){
      if (fbdo.dataType() == "int"){
        servoValue = fbdo.intData();
      }
    }
  }
};

void setmodeAuto(){
  if (Firebase.ready()){
    // get data from firebase database
    if (Firebase.RTDB.getInt(&fbdo, "/isAuto")){
      if (fbdo.dataType() == "int"){
        modeValue = fbdo.intData();
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  // initialize LCD
  lcd.init();                      // initialize the lcd 
  // Print a message to the LCD.
  lcd.backlight();
  lcd.clear();
  // Welcoming 
  lcd.setCursor(0,0);
  const char* welcome = "~~SLMT  DATANG~~";
  for (int i = 0; i < strlen(welcome); i++){
    lcd.print(welcome[i]);
    delay(100);
  }
  lcd.setCursor(0,1);
  const char* project = "~~Project ETA~~";
  for (int i = 0; i < strlen(project); i++){
    lcd.print(project[i]);
    delay(100);
  }
  delay(1000);
  lcd.clear();
  // pinMode(buttonSelectPin, INPUT_PULLUP);
  // pinMode(buttonEnterPin, INPUT_PULLUP);
  // Connect to Wi-Fi
  InitWifi();
  FirebaseSetup();

  // Attach the servo object to the servo pin
  pinMode(ldrPin, INPUT);
  servo.attach(servoPin);
  servo.write(0);
}

void loop() { 
  setmodeAuto();
  Serial.print("MODE VALUE: ");
  Serial.println(modeValue);

  if (modeValue == 1){
    ldrRead();
    rainRead();
    Serial.print("LDR VALUE: "); 
    Serial.println(ldrValue);
    Serial.print("RAIN VALUE: ");
    Serial.println(rainValue);

    if (isLDR || isRain){
      // write in database
      Firebase.RTDB.setInt(&fbdo, "/servo", 1);
      servoOn();
    } else{
      Firebase.RTDB.setInt(&fbdo, "/servo", 0);
      servoOff();
    }
  } else{
    controlFirebase();
    if (servoValue == 1){
      servoOn();
    } else{
      servoOff();
    }
  }

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Mode: ");
  if (modeValue == 1){
    lcd.print("Auto");
  } else{
    lcd.print("Manual");
  }
  if(modeValue == 1){
    lcd.setCursor(0,1); 
    lcd.print("LDR:");
    lcd.print(isLDR);
    lcd.setCursor(8,1);
    lcd.print("RDS:");
    lcd.print(isRain);
  } else{
    lcd.setCursor(0,1);
    lcd.print("Servo: ");
    lcd.print(servoValue);
  }
  delay(1000);
}