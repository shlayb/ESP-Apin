#include "WiFi.h"
String SSID = "";
String PASSWORD = "";

#include <WebServer.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C elcidi(0x27,16,2);

WebServer server(80);

void handleRoot() {
  String html = "<html><body>";
  html += "<form action=\"/connect\" method=\"POST\">";
  html += "SSID: <input type=\"text\" name=\"ssid\"><br>";
  html += "Password: <input type=\"password\" name=\"password\"><br>";
  html += "<input type=\"submit\" value=\"Connect\">";
  html += "</form>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleConnect() {
  SSID = server.arg("ssid");
  PASSWORD = server.arg("password");
  server.send(200, "text/html", "Connecting to WiFi...");
  WiFi.begin(SSID.c_str(), PASSWORD.c_str());
  elcidi.clear();
  elcidi.setCursor(0,0);
  elcidi.print("Connecting to Wifi");
  elcidi.setCursor(0,1);
  int i = 0;
  int startMillis = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startMillis < 5000) {
    Serial.print(".");
    if (i == 16) {
      elcidi.clear();
      elcidi.setCursor(0,0);
      i = 0;
    }
    elcidi.print("=");
    i++;
    delay(500);
  }
  if(WiFi.status() != WL_CONNECTED) {
    elcidi.clear();
    elcidi.setCursor(0,0);
    elcidi.print("Failed to connect");
    delay(2000);
    server.sendHeader("Location", "/");
    server.send(301);
    return;
  }
  elcidi.clear();
  elcidi.print("Connected");
  Serial.println("Connected to WiFi");
  Serial.print("SSID: ");
  Serial.println(SSID);
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());

  String html = "<html><body>";
  html += "Connected to WiFi.<br>";
  html += "Local IP: ";
  html += WiFi.localIP().toString();
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void InitWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  WiFi.softAP("ESP32_AP", "12345678");
  elcidi.begin(16,2);
  elcidi.backlight();
  elcidi.clear();
  server.on("/", HTTP_GET, handleRoot);
  server.on("/connect", HTTP_POST, handleConnect);
  server.begin();
  while(WiFi.status() != WL_CONNECTED) {
    elcidi.setCursor(0,0);
    elcidi.print("IP address: ");
    elcidi.setCursor(0,1);
    elcidi.print(WiFi.softAPIP());
    Serial.print("IP address: ");
    Serial.println(WiFi.softAPIP());
    server.handleClient();
    delay(500);
  }
}