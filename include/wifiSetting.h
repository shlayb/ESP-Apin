// file wifiSetting.h

#include <WiFi.h>
#include <Arduino.h>

#include <lcdSetting.h>

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