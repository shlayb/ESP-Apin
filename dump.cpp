#include <ESP32Servo.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <ArduinoJson.h>
#include <Firebase_ESP_Client.h>
// Provide the token generation process info.
#include <addons/TokenHelper.h>

// Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

//  Include the required libraries I2C and LiquidCrystal.
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);

#define WIFI_SSID "Damie"
#define WIFI_PASSWORD "diandamieeee"

// setup firebase
#define DATABASE_URL "https://projectep-abbca-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define FIREBASE_AUTH "ChecdgjtQWSexTEzs3gn7i4WDp9bVvvXZl6wOrjM"
#define API_KEY "AIzaSyAVAgUbn0L5J8LXeQP9C2-CijAqy-51Cp0"

// setup user
#define USER_EMAIL "ayubisal1345@gmail.com"
#define USER_PASSWORD "udinganteng123"

// Setup Variable control
bool isOn = false;
bool isLDR = false; // true ketika gelap dan false ketika terang
bool isRain = false; // true ketika hujan dan false ketika tidak hujan
int ldrValue = 0;
int rainValue = 0;
int servoValue = 0;
bool isSignUp = false;
bool modeAuto = false;

// Firebase objects
FirebaseData fbdo;
FirebaseData firebaseData;
FirebaseAuth auth;          // FirebaseAuth object
FirebaseConfig config;      // FirebaseConfig object
FirebaseJson json;       // or constructor with contents e.g. FirebaseJson json("{\"a\":true}");
FirebaseJsonArray arr;   // or constructor with contents e.g. FirebaseJsonArray arr("[1,2,true,\"test\"]");
FirebaseJsonData result; // object that keeps the deserializing result
unsigned long sendDataPrevMillis = 0;

// Setup pin
int servoPin = 13;
int ldrPin = 32;
int rainPin = 33;

// Create a servo object
Servo servo;


// Function declaration
void servoOn(){
  servo.write(180);
};
void servoOff(){
  servo.write(0);
};

void controlFirebase(){
  if (Firebase.ready() && (millis() - sendDataPrevMillis > 1000 || sendDataPrevMillis == 0)){
    // get data from firebase database
    if (Firebase.RTDB.getInt(&fbdo, "/servo")){
      if (fbdo.dataType() == "int"){
        servoValue = fbdo.intData();
      }
    }
  }
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
  if (rainValue > 1000){
    isRain = true;
  } else{
    isRain = false;
  }
};



void setup() {
  Serial.begin(115200);
  // initialize LCD
  lcd.init();                      // initialize the lcd 
  // Print a message to the LCD.
  lcd.backlight();
  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  Serial.println();
  lcd.print("|");
  while (WiFi.status() != WL_CONNECTED) {
    lcd.print("=");
    delay(300);
  }
  lcd.print("|");
  delay(1000);
  if (WiFi.status() == WL_CONNECTED){
    Serial.println("Connected to Wi-Fi");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Wi-Fi connected");
    lcd.setCursor(0,1);
    lcd.print(WiFi.localIP());
    delay(1000);
    lcd.clear();
  } else{
    Serial.println("Failed to connect to Wi-Fi");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Failed to connect to Wi-Fi");
    delay(1000);
    lcd.clear();
  }

  // initialize Firebase
  /* Assign the api key (required) */
  config.api_key = API_KEY;

  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("Successfully signed up");
    isSignUp = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
    Serial.println("Failed to sign up");
    isSignUp = false;
  }

  config.token_status_callback = tokenStatusCallback;
  
  Firebase.begin(&config, &auth);
  // check if the connection is successful
  if (Firebase.ready()){
    lcd.setCursor(0,0);
    lcd.print("Firebase ready");
    delay(1000);
    lcd.clear();
  } else{
    lcd.setCursor(0,0);
    lcd.print("Firebase not ready");
    delay(1000);
    lcd.clear();
  }
  Firebase.reconnectWiFi(true);
  // Attach the servo object to the servo pin
  pinMode(ldrPin, INPUT);
  servo.attach(servoPin);
}

void loop() { 
  controlFirebase();
  ldrRead();
  rainRead();

  if (isLDR || servoValue == 1 || isRain){
    servoOn();
  } else{
    servoOff();
  }

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("LDR: ");
  lcd.print(ldrValue);
  lcd.setCursor(0,1);
  lcd.print("Servo: ");
  lcd.print(servoValue);
  delay(1000);
}