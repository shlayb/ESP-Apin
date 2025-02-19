#include <Firebase_ESP_Client.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C licidi(0x27,16,2);
// Provide the token generation process info.
#include <addons/TokenHelper.h>

// Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

// setup firebase
#define DATABASE_URL "https://projectep-abbca-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define FIREBASE_AUTH "ChecdgjtQWSexTEzs3gn7i4WDp9bVvvXZl6wOrjM"
#define API_KEY "AIzaSyAVAgUbn0L5J8LXeQP9C2-CijAqy-51Cp0"

// setup user
#define USER_EMAIL "ayubisal1345@gmail.com"
#define USER_PASSWORD "udinganteng123"


// Firebase objects
FirebaseData fbdo;
FirebaseData firebaseData;
FirebaseAuth auth;          // FirebaseAuth object
FirebaseConfig config;      // FirebaseConfig object
FirebaseJson json;       // or constructor with contents e.g. FirebaseJson json("{\"a\":true}");
FirebaseJsonArray arr;   // or constructor with contents e.g. FirebaseJsonArray arr("[1,2,true,\"test\"]");
FirebaseJsonData result; // object that keeps the deserializing result
unsigned long sendDataPrevMillis = 0;



void FirebaseSetup(){
  licidi.begin(16,2);
  licidi.backlight();
  licidi.clear();

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
    licidi.setCursor(0,0);
    licidi.print("Firebase ready");
    delay(1000);
    licidi.clear();
  } else{
    licidi.setCursor(0,0);
    licidi.print("Firebase not ready");
    delay(1000);
    licidi.clear();
  }
  Firebase.reconnectWiFi(true);
}