#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
 
/*
   This sample sketch demonstrates the normal use of a TinyGPS++ (TinyGPSPlus) object.
   It requires the use of SoftwareSerial, and assumes that you have a
   4800-baud serial GPS device hooked up on pins 4(rx) and 3(tx).
*/
static const int RXPin = 12, TXPin = 13;
static const uint32_t GPSBaud = 9600;
 
// repace your wifi username and password
const char* ssid     = "your_wifi_name";
const char* password = "your_wifi_password";


#define FIREBASE_HOST "your_firebase_url.firebase.io"
#define FIREBASE_AUTH "your_firebase_secret_key"
 
 
// The TinyGPS++ object
TinyGPSPlus gps;
WiFiClient  client;
 
// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);
 
void setup()
{
  Serial.begin(115200);
  ss.begin(GPSBaud);
  Serial.println(F(":App Started"));
  Serial.println(F("Realtime GPS data IoT"));
  Serial.print(F("GPS Started")); 
  Serial.println(TinyGPSPlus::libraryVersion());
  Serial.println();
 
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
   Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Netmask: ");
  Serial.println(WiFi.subnetMask());
  Serial.print("Gateway: ");
  Serial.println(WiFi.gatewayIP());
  Firebase.begin(FIREBASE_HOST,FIREBASE_AUTH);
  
  
}
 
void loop()
{
  // This sketch displays information every time a new sentence is correctly encoded.
  while (ss.available() > 0)
    if (gps.encode(ss.read()))
      displayInfo();
 
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
  }
}

void displayInfo()
{
// Serial.print(F("Location: "));
  if (gps.location.isValid())
  {

    //set default lat lng
    const double TOWER_LAT = -7.441299;
    const double TOWER_LNG = 109.211704;
 
    double latitude = (gps.location.lat());
    double longitude = (gps.location.lng());

    //get distance 
    double distanceM = gps.distanceBetween(TOWER_LAT,TOWER_LNG,latitude,longitude)/10;
 
    Serial.print("Jarak titik: ");
    Serial.println(distanceM);
    Serial.print(" meter");
    Serial.println("");
    
    String latbuf;
    latbuf += (String(latitude, 6));
    Serial.print("lat : ");
    Serial.print(latbuf);

    Serial.println();
 
    String lonbuf;
    lonbuf += (String(longitude, 6));
    Serial.print("lon : ");
    Serial.print(lonbuf);Serial.println();

    if (Firebase.failed()) {
      Serial.println("Firebase get failed");
      Serial.println(Firebase.error());
      return;
    }else {
      Firebase.setFloat("Lat",latitude);
    Serial.println("mengirim data latitude : ");
    Serial.println();
    Firebase.setFloat("Lon",longitude);
    Serial.println("mengirim data longitude : ");
    Firebase.setFloat("Distance",distanceM);
    Serial.println("mengirim data distance : ");

    if(distanceM >= 0.3){
      Serial.println("status bahaya : ");
      Firebase.set("Status","Bahaya");
    }else if(distanceM >= 0.2){
      Serial.println("status waspada : ");
      Firebase.set("Status","Waspada");
    }
    else if(distanceM >= 0.1){
      Serial.println("status hati-hati : ");
      Firebase.set("Status","Hati-hati");
    }else {
      Serial.println("status aman : ");
      Firebase.set("Status","Aman");
    }
    }

    delay(200);
    
  }
  else
  {
    Serial.print(F("INVALID"));
  }
 
  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }
 
  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
  }
 
  Serial.println();
}
