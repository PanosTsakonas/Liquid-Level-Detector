#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>
#include <Wire.h>
#include "Adafruit_VL53L0X.h"

// address we will assign if dual sensor is present
#define LOX1_ADDRESS 0x30
#define LOX2_ADDRESS 0x31

// set the pins to shutdown
#define SHT_LOX1 5
#define SHT_LOX2 4

// objects for the vl53l0x
Adafruit_VL53L0X lox1 = Adafruit_VL53L0X();
Adafruit_VL53L0X lox2 = Adafruit_VL53L0X();

// this holds the measurement
VL53L0X_RangingMeasurementData_t measure1;
VL53L0X_RangingMeasurementData_t measure2;
uint8_t pin = 12;
uint8_t pin1=13;
bool pinstatus = LOW;
#define dhtpin 23
DHT dht(dhtpin,DHT11);
/* Put your SSID & Password */
const char* ssid = "Liquid Level Meter";  // Enter SSID here
const char* password = "Set your own password here";  //Enter Password here

/* Put IP Address details */
IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);
String petrol="";
String water ="";
String temp="";
WebServer server(80);
int i;
float pet_width=50;
float pet_depth=50;
float wat_width=100;
float wat_depth=50;
float te=0;
float d1=0;
float d2=0;
void setup() {
 Wire.begin();
 Serial.begin(115200);
 pinMode(pin,OUTPUT);
 pinMode(pin1,OUTPUT);
 digitalWrite(pin,LOW);
 digitalWrite(pin1,LOW);
 delay(200);
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);
  
  server.on("/", handle_OnConnect);
  server.on("/systemon", handle_systemon);
  server.on("/systemoff", handle_systemoff);
  server.onNotFound(handle_NotFound);
  
  server.begin();
  Serial.println("HTTP server started");
  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN,LOW);
   pinMode(SHT_LOX1, OUTPUT);
  pinMode(SHT_LOX2, OUTPUT);

  Serial.println("Shutdown pins inited...");

  digitalWrite(SHT_LOX1, LOW);
  digitalWrite(SHT_LOX2, LOW);

  Serial.println("Both in reset mode...(pins are low)");

}

void loop() {
  
 server.handleClient();
  if(pinstatus)
  {
    digitalWrite(pin, HIGH);
      digitalWrite(pin1, HIGH);
  }
  else
  {
    digitalWrite(pin, LOW);
  digitalWrite(pin1, LOW);
  }

}

void handle_OnConnect() {
  pinstatus = LOW;
  Serial.println("GPIO13 Status: OFF");
  server.send(200, "text/html", SendHTML(pinstatus)); 
}

void handle_systemon() {
  pinstatus = HIGH;
  digitalWrite(pin,HIGH);
  digitalWrite(pin1,HIGH);
   digitalWrite(LED_BUILTIN,HIGH);
   delay(200);
    pinMode(SHT_LOX1, OUTPUT);
  pinMode(SHT_LOX2, OUTPUT);

  Serial.println("Shutdown pins inited...");

  digitalWrite(SHT_LOX1, LOW);
  digitalWrite(SHT_LOX2, LOW);

  Serial.println("Both in reset mode...(pins are low)");
  
  
  Serial.println("Starting...");
  delay(30);
  setID();
  lox1.rangingTest(&measure1, false); // pass in 'true' to get debug data printout!
  lox2.rangingTest(&measure2, false); // pass in 'true' to get debug data printout!
     dht.begin();
     delay(20);
  for (i=1;i<=50;i++)
  {
    te=te+dht.readTemperature();
  if(measure1.RangeStatus != 4) {     // if not out of range
   d1=d1+(measure1.RangeMilliMeter);
  } else {
    Serial.print("Out of range");
  }
  if(measure2.RangeStatus != 4) {
    d2=d2+(measure2.RangeMilliMeter);
  } else {
    Serial.print("Out of range");
  }
  }
  delay(100);
  te=te/50;
  temp=+te;
  d1=d1/50;
  d2=d2/50;
  Serial.println("Distance1");
  Serial.println(d1);
  Serial.println("Distance2");
  Serial.println(d2);
  d1=pet_width*pet_depth*d1/10000;
  d2=wat_width*wat_depth*d2/10000;
  petrol=+d1;
  water=+d2;
  Serial.println("GPIO13 Status: ON");
  
  server.send(200, "text/html", SendHTML(true)); 
}

void handle_systemoff() {
  pinstatus = LOW;
  digitalWrite(LED_BUILTIN,LOW);
  Serial.println("GPIO13 Status: OFF");
  server.send(200, "text/html", SendHTML(false)); 
}
void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

String SendHTML(uint8_t pinst)
{
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>Liquid Level</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr +=".button {display: block;width: 80px;background-color: #3498db;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr +=".button-on {background-color: #3498db;}\n";
  ptr +=".button-on:active {background-color: #2980b9;}\n";
  ptr +=".button-off {background-color: #34495e;}\n";
  ptr +=".button-off:active {background-color: #2c3e50;}\n";
  ptr +="p {font-size: 09px;color: #888;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<h1>Liquid Level and Temperature</h1>\n";
  
  if(pinst)
  {ptr +="<p>System Status: ON</p><a class=\"button button-off\" href=\"/systemoff\">OFF</a>\n";
   ptr +="<h3>Petrol Tank Level: "+petrol+" Litres"+"</h3>\n";
   ptr +="<h3>Water Tank Level: "+water+" Litres"+"</h3>\n";
   ptr+="<h3>Temperature: "+temp+" C"+"</h3>\n";  
  }
  else
  {ptr +="<p>System Status: OFF</p><a class=\"button button-on\" href=\"/systemon\">ON</a>\n";}

   ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}

void setID() {
  // all reset
  digitalWrite(SHT_LOX1, LOW);    
  digitalWrite(SHT_LOX2, LOW);
  delay(10);
  // all unreset
  digitalWrite(SHT_LOX1, HIGH);
  digitalWrite(SHT_LOX2, HIGH);
  delay(10);

  // activating LOX1 and reseting LOX2
  digitalWrite(SHT_LOX1, HIGH);
  digitalWrite(SHT_LOX2, LOW);

  // initing LOX1
  if(!lox1.begin(LOX1_ADDRESS)) {
    Serial.println(F("Failed to boot first VL53L0X"));
    while(1);
  }
  Serial.println("First Sensor is OK!");
  delay(10);

  // activating LOX2
  digitalWrite(SHT_LOX2, HIGH);
  delay(10);

  //initing LOX2
  if(!lox2.begin(LOX2_ADDRESS)) {
    Serial.println(F("Failed to boot second VL53L0X"));
    while(1);
  }
  Serial.println("Second sensor is OK!");
}
