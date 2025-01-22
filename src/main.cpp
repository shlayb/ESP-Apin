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

#define WIFI_SSID_1 "aku siapaaa kamu siapaaa"
#define WIFI_PASSWORD_1 "punyayubiseorang"
#define WIFI_SSID_2 "yubi"
#define WIFI_PASSWORD_2 "udinganteng123"
#define WIFI_SSID_3 "aulia izza"
#define WIFI_PASSWORD_3 "123456798"
#define WIFI_SSID_4 "Damie"
#define WIFI_PASSWORD_4 "diandamieeee"
// make array of wifi ssid and password
const char* WIFI_SSID[] = {WIFI_SSID_1, WIFI_SSID_2, WIFI_SSID_3, WIFI_SSID_4};
const char* WIFI_PASSWORD[] = {WIFI_PASSWORD_1, WIFI_PASSWORD_2 , WIFI_PASSWORD_3, WIFI_PASSWORD_4};

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
int modeValue = 0;
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
  servo.write(90);
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

void setmodeAuto(){
  if (Firebase.ready() && (millis() - sendDataPrevMillis > 1000 || sendDataPrevMillis == 0)){
    // get data from firebase database
    if (Firebase.RTDB.getInt(&fbdo, "/isAuto")){
      if (fbdo.dataType() == "int"){
        modeValue = fbdo.intData();
      }
    }
  }
}

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

int buttonSelectPin = 14;
int buttonEnterPin = 12;
int wifiselected = 0;
bool isWifiSelected = false;
int selectedValue = 0;
void selectWifi(){
  while(WiFi.status() != WL_CONNECTED && !isWifiSelected){
    lcd.setCursor(0,0);
    lcd.print("Select Wi-Fi");
    if(digitalRead(buttonSelectPin) == LOW){
      for(int x = 0; x < 6; x++){
        if(selectedValue < 5){
          if(digitalRead(buttonSelectPin) == LOW){
            selectedValue++;
            lcd.setCursor(0,1);
            lcd.print(WIFI_SSID[wifiselected]);
            delay(200);
          }else{
            selectedValue = 0;
          }
        }else{
          isWifiSelected = true;
          continue;
        }
      }
      if(isWifiSelected){
        break;
      }
      // why it change the value of wifiselected to 0 and 1?
      
      wifiselected++;
      if (wifiselected > 3){
        wifiselected = 0;
      }
    }
    if(!isWifiSelected){
      lcd.setCursor(0,1);
      lcd.print(WIFI_SSID[wifiselected]);
      delay(100);
      lcd.clear();
    }
    
    Serial.print("is wifi selected: ");
    Serial.println(isWifiSelected);
    Serial.print("Wifi selected: ");
    Serial.println(wifiselected);
    Serial.print("Selected value: ");
    Serial.println(selectedValue);
  }
  if (isWifiSelected){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Connecting..");
    WiFi.begin(WIFI_SSID[wifiselected], WIFI_PASSWORD[wifiselected]);
    int i = 1;
    int len = 14;
    while (WiFi.status() != WL_CONNECTED){
      lcd.setCursor(0,1);
      lcd.print("|");
      if(i <= 14){
        for(int j = 0; j < i; j++){
          lcd.print("=");
        }
        for (int j = 0; j < len - i; j++){
          lcd.print(" ");
        }
        lcd.print("|");
        i++;
      } else{
        i = 0;
        len = 14;
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Connecting.."); 
        lcd.setCursor(0,1);
        lcd.print("|");
      }
      delay(300);
    }
    lcd.print("|");
    delay(1000);
    if (WiFi.status() == WL_CONNECTED){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("WiFi connect to");
      lcd.setCursor(0,1);
      lcd.print(WiFi.SSID());
      delay(1000);
      lcd.clear();
    } else{
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Failed to connect to Wi-Fi");
      delay(1000);
      lcd.clear();
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
  pinMode(buttonSelectPin, INPUT_PULLUP);
  pinMode(buttonEnterPin, INPUT_PULLUP);
  // Connect to Wi-Fi
  selectWifi();
  // initialize Firebase
  /* Assign the api key (required) */
  config.api_key = API_KEY;

  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  Firebase.signUp(&config, &auth, "", "");

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
  servo.write(0);
}

void loop() { 
  setmodeAuto();

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