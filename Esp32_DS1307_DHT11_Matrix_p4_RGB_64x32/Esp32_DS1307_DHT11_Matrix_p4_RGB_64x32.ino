// REQUIRES the following Arduino libraries:
// - Lien vidéo: https://youtu.be/0gGnr2HYCnQ
// - Adafruit RGB matrix Panel Library: https://github.com/adafruit/RGB-matrix-Panel
// - Adafruit_GFX Library: https://github.com/adafruit/Adafruit-GFX-Library
// - Adafruit_BusIO Library: https://github.com/adafruit/Adafruit_BusIO
// - ESP32Time Library: https://github.com/fbiego/ESP32Time
// - RTC Library: https://github.com/adafruit/RTClib
// - DHT Sensor Library: https://github.com/beegee-tokyo/DHTesp
// - Getting Started ESP32 / ESP32S : https://www.youtube.com/watch?v=9b0Txt-yF7E
// Find All "Great Projects" Videos : https://www.youtube.com/c/GreatProjects

#include <EEPROM.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_GFX.h>
#include <RGBmatrixPanel.h>
#include <ESP32Time.h>
#include "RTClib.h"
#include "DHTesp.h"
#include "index.h" 
#include <Fonts/atawi8b.h>
#include <Fonts/atawi10b.h>
#include <Fonts/atawi12b.h>

#define A    15
#define B    16
#define C    17
#define D    18
#define CLK  19 
#define LAT  32 
#define OE   33
RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, false, 64);

#define EEPROM_SIZE 2
ESP32Time rtc;
RTC_DS1307 myRTC;
DHTesp dht;
WebServer server(80);
int dhtPin = 4;

char ssid[30] = "";
char pass[30] = "";
const char* Apssid = "ESP32-P5";
const char* Appassword = "internet";

//String Message = "Great Projects wish you a very nice day. Hope you have a great time ahead.";
String Message = "Welcome to my instagram page";
int Tz = 1; // Time Zone
//int Tz = 7; // Time Zone

int NewRTCh = 24;
int NewRTCm = 60;
int NewRTCs = 60;
bool flasher = false;
String text, textmsg, textip;
float Temp, Humi;
uint32_t prevTime, mesagTime; 
int sp0 = 15, msg = 4, xpos = 64;
// standard colors
uint16_t myRED = matrix.Color333(7,0,0);
uint16_t myGREEN = matrix.Color333(0,7,0);
uint16_t myBLUE = matrix.Color333(0,0,7);
uint16_t myWHITE = matrix.Color333(7, 7,7);
uint16_t myYELLOW = matrix.Color333(7,7,0);
uint16_t myCYAN = matrix.Color333(0,7,7);
uint16_t myMAGENTA = matrix.Color333(7,0,7);
uint16_t myShadow = matrix.Color333(4,0,7);
uint16_t myROSE = matrix.Color333(7,0,4);
uint16_t myBLACK = matrix.Color333(0,0,0);
uint16_t myCOLORS[7] = {myRED, myYELLOW, myCYAN, myBLUE, myWHITE, myMAGENTA, myGREEN};
uint8_t r = 0, g = 0, b = 0;

void(* resetFunc) (void) = 0;//declare reset function at address 0

void getWifi()
{  
  if (WiFi.status() == WL_CONNECTED) { 
    WiFi.disconnect();
    }
  WiFi.begin(ssid, pass);
  int xc = 0;
  while (WiFi.status() != WL_CONNECTED && xc < 50) {
    delay(100);
    xc++;
  }
  if (WiFi.status() == WL_CONNECTED) { 
    WiFi.softAPdisconnect(true);
    textip = "CONNECTED To: ";
    textip += ssid;
    textip += " IP Address: http://";
    textip += WiFi.localIP().toString().c_str();
    } else { 
    WiFi.softAP(Apssid, Appassword);
    textip = "Ssid or password is incorrect";
    }
}
void getTemhumi()
{
  TempAndHumidity newValues = dht.getTempAndHumidity();
       Humi = newValues.humidity;
       Temp = newValues.temperature;
  }
void dofw_text()
{
  text = rtc.getTime("%A");
  uint16_t text_length = text.length() * 7;
  int xpo = (64 - text_length) / 2;
  matrix.setFont(&atawi8b);
  matrix.fillRect(0, 26, 64, 7, myBLACK);
  matrix.setCursor(xpo, 26);
  for (int y = 0; y < 9; y++) {
    matrix.setTextColor(myCOLORS[r]);
    matrix.print(text[y]);
    r++;
    if (r > 6) { r = 0; }
  }
  matrix.setFont();
}
void getdate() {
  matrix.setFont(&atawi8b);
  matrix.fillRect(0, 26, 64, 6, myBLACK);
  matrix.setCursor(1, 26);
  matrix.setTextColor(myRED);
  matrix.printf("%02d", rtc.getDay());
  matrix.setTextColor(myGREEN);
  matrix.print(rtc.getTime("%b"));
  matrix.setTextColor(myRED);
  matrix.printf("%04d", rtc.getYear());
  matrix.setFont();
} 
void getmesg()
{
    switch(msg)
    {
    case 0 :
        textmsg = Message;
        getTemhumi();
        msg++;
        break;
    case 1 :
        textmsg = "Temperature: ";
        textmsg += Temp;
        textmsg += " °C";
        getdate();
        msg++;
        break;
    case 2 :
        textmsg = "Humidity: ";
        textmsg += Humi;
        textmsg += " %";
        msg++;
        break;
    case 3 :
        textmsg = rtc.getDate(true);
        dofw_text();
        msg = 0;
        break;
    case 4 :
        textmsg = textip;
        msg = 0;
        break;
    }
}
void scroll_text(String text) {
  uint16_t text_length = text.length() * 12;
    int y = r;
    matrix.setFont(&atawi12b);
    matrix.fillRect(0, 13, 64, 12, myBLACK);
    matrix.setCursor(xpos, 13);
  for (int x = 0; x < text.length(); x++) {
    matrix.setTextColor(myCOLORS[y]);
    matrix.print(text[x]);
     y++;
  if (y > 6) { y = 0; }
  }
    matrix.setFont();
     xpos--;
     if (xpos < -text_length) {xpos = 64, getmesg();}
  }
void getTim() {
  if (flasher)
  {
    matrix.fillRect(52, 0, 7, 6, myBLACK);
    matrix.setFont(&atawi10b);
    matrix.setCursor(22, 0);
    matrix.setTextColor(myCOLORS[b]);
    matrix.print(":");
    matrix.setFont();
    g = random(0,6);
  } else {
    matrix.fillRect(22, 0, 4, 12, myBLACK);
    matrix.setFont(&atawi10b);
    matrix.setCursor(52, 0);
    matrix.setTextColor(myCOLORS[g]);
    matrix.print("*");
    matrix.setFont();
    b = random(0,6);
  } 
  if (NewRTCs != rtc.getSecond() / 10) {  
    matrix.setFont(&atawi8b);
    matrix.setCursor(49, 6);
    matrix.setTextColor(myCOLORS[r]);
    matrix.fillRect(49, 6, 13, 6, myBLACK);
    matrix.printf("%02d", rtc.getSecond());
    matrix.setFont();
    r = random(0,6); 
    NewRTCs = rtc.getSecond() / 10;
  } else {
  matrix.setFont(&atawi8b);
  matrix.setCursor(49, 6);
  matrix.setTextColor(myCOLORS[b]);
    matrix.fillRect(56, 6, 6, 6, myBLACK);
    matrix.printf("%02d", rtc.getSecond());
    matrix.setFont();
  }
    if (NewRTCm != rtc.getMinute()) {
  matrix.setFont(&atawi10b);
  matrix.setCursor(26, 0);
  matrix.setTextColor(myMAGENTA);
  matrix.fillRect(26, 0, 22, 12, myBLACK);
  matrix.printf("%02d", rtc.getMinute());
  matrix.setFont();
  NewRTCm = rtc.getMinute();
      }
  if (NewRTCh != rtc.getHour(true)) {
  matrix.setFont(&atawi10b);
  matrix.setCursor(1, 0);
  matrix.setTextColor(myMAGENTA);
  matrix.fillRect(0, 0, 22, 12, myBLACK);
  matrix.printf("%02d", rtc.getHour(true) + 1);
  matrix.setFont();
  NewRTCh = rtc.getHour(true);
      }
}
void getzoneTime() { 
    Tz = EEPROM.read(1);
    if (Tz > 12) { Tz = Tz - 256; }
    updateTime();
}
  void handleRoot() {
    String s = MAIN_page; //Read HTML contents
    server.send(200, "text/html", s); //Send web page
}
void handleSpe0() {
    handleRoot();
    sp0 = server.arg("Speed0").toInt();
  }
void handleMesg() { 
    handleRoot();
    Message = server.arg("Message");
    msg = 0, xpos = 64, getmesg();
  }
void handlezoneTime() { 
    handleRoot();
    Tz = server.arg("locatz").toInt(); 
    configTime(Tz * 3600, 0, "", "");
    getdate();
}
void handlentpTime() { 
    handleRoot();  
    if (WiFi.status() == WL_CONNECTED) { 
    configTime(Tz * 3600, 0, "pool.ntp.org", "time.nist.gov");
    getdate();
    }
}
void handlelocaltime() { 
    handleRoot();
    rtc.setTime(server.arg("locadt").toInt()); 
    configTime(Tz * 3600, 0, "", "");
    getdate();
}
void handlesaveTime() { 
    handleRoot();
    EEPROM.write(1, Tz);
    EEPROM.commit();
    writeTime(); 
    updateTime();
    getdate();
}
void handleResTime() {  
    String hbuf = server.arg("htmie"); 
    String dbuf = server.arg("bday"); 
    text = hbuf[0];
    text += hbuf[1];
    int h = text.toInt(); 
    text = hbuf[3];
    text += hbuf[4];
    int m = text.toInt(); 
    text = hbuf[6];
    text += hbuf[7];
    int s = text.toInt();
    text = dbuf[0];
    text += dbuf[1];
    text += dbuf[2];
    text += dbuf[3];
    int yr = text.toInt(); 
    text = dbuf[5];
    text += dbuf[6];
    int mo = text.toInt();
    text = dbuf[8];
    text += dbuf[9];
    int d = text.toInt();
    rtc.setTime(s, m, h, d, mo, yr); 
    getdate();
    handleRoot();
}
void handleWifi() { 
  handleRoot();
  String text = server.arg("usname");
  int len = text.length() + 1; 
  text.toCharArray(ssid, len);
  text = server.arg("pssw");
  len = text.length() + 1;
  text.toCharArray(pass, len); 
  getWifi(); 
  msg = 4, xpos = 64, getmesg();
}
void handlestate() {
  getTemhumi();
  String content = "<?xml version = \"1.0\" ?>";
  content += "<inputs><analog>";
  content += Temp;
  content += "</analog><analog>";
  content += Humi;
  content += "</analog><analog>";
  content += Tz;
  content += "</analog></inputs>";
  server.sendHeader("Cache-Control", "no-cache");
  server.send(200, "text/xml", content); //Send web page
}
void handleRestesp() {  
    handleRoot();
    delay(4000);     
    resetFunc();
}
void setup() {
  dht.setup(dhtPin, DHTesp::DHT11);
  matrix.begin();                           // setup the LED matrix
  matrix.setTextWrap(false); // Don't wrap at end of line - will do ourselves
  EEPROM.begin(EEPROM_SIZE);
  myRTC.begin();
  if (! myRTC.isrunning()) { myRTC.adjust(DateTime(F(__DATE__), F(__TIME__))); }
  WiFi.softAP(Apssid, Appassword);
  textip = "Ssid: 'Esp32-P5' Password: 'internet' IP: 'http://192.168.4.1'";
  msg = 4, xpos = 64, getmesg();
  server.on("/", handleRoot); 
  server.on("/MesSpeed0", handleSpe0); 
  server.on("/myMesg", handleMesg);  
  server.on("/mytimezon", handlezoneTime); 
  server.on("/NTPtime", handlentpTime);
  server.on("/localdatime", handlelocaltime);   
  server.on("/savetime", handlesaveTime); 
  server.on("/restime", handleResTime);  
  server.on("/Mywifi", handleWifi); 
  server.on("/readtemhu", handlestate); 
  server.on("/restesp", handleRestesp);
  server.begin();
  getzoneTime();
  matrix.fillScreen(0);
  delay(2000);
  getdate();
}
void loop() {
  server.handleClient();
  if(millis() - prevTime >= 1000) {
      getTim();
      flasher = !flasher; 
      prevTime = millis();
    }
  if(millis() - mesagTime >= sp0) {
      scroll_text(textmsg);
      mesagTime = millis();
    }
}
void updateTime()
{
  DateTime now = myRTC.now();
  configTime(0, 0, "", "");
  rtc.setTime(now.unixtime()); 
  configTime(Tz * 3600, 0, "", "");
  if(rtc.getYear() < 2021) { rtc.setTime(0, 0, 0, 1, 1, 2021); }
} 
void writeTime()
{
    configTime(0, 0, "", "");
    myRTC.adjust(DateTime(rtc.getEpoch())); 
    configTime(Tz * 3600, 0, "", "");
}
