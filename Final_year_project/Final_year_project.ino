/////Successful/////********vinojv********////////
//////SENSORS VERIFIED//////
/////////****1.DHT22***2.MQ7****3.DSM501A
//Code//
////DHT22////

#include <DHT.h>
#include <ESP8266WiFi.h>
 /////////////////////****** MQ-7 Carbon Monoxide******/////////
//the AOUT pin of the CO sensor goes into analog pin A0 of the arduino
int sensorValue;
int ledpin = 16;
/////////////////////////////*********DSM501A*******//////////////////////////////
#include<string.h>
byte buff[2];
int pin = 12;//DSM501A input D8
unsigned long duration;
unsigned long starttime;
unsigned long endtime;
unsigned long sampletime_ms = 30000;
unsigned long lowpulseoccupancy = 0;
float ratio = 0;
float concentration = 0;
//////Wi-Fi**Connection////////// 
String apiKey = "CZ0ZRFNO4EYXLZ55";
const char* ssid = "Vino";
const char* password = "vinoth@1996";
 
const char* server = "api.thingspeak.com";
#define DHTPIN 14 // what pin we're connected to
 
DHT dht(DHTPIN, DHT22,15);
WiFiClient client;
   
 
void setup() {                
  Serial.begin(115200);
  delay(10);
  dht.begin();
  pinMode(12,INPUT);
  pinMode(16,OUTPUT);
starttime = millis(); 
  WiFi.begin(ssid, password);
 
  Serial.println();
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
  
}
 
 
void loop() {
   
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
delay(1000);
 Serial.print("Temperature: ");
     Serial.print(t);
     Serial.print(" degrees Celcius Humidity: "); 
     Serial.print(h);
     Serial.println("% send to Thingspeak"); 
 /////////////////////****** MQ-7******///////////////////
sensorValue = analogRead(0);       // read analog input pin 0
  Serial.print(sensorValue/60, DEC);  // prints the value read
  Serial.println("ppm");
  if (sensorValue/60 > 6) {
    // Activate digital output pin 8 - the LED will light up
    digitalWrite(ledpin, HIGH);
  }
  else {
    // Deactivate digital output pin 8 - the LED will not light up
    digitalWrite(ledpin, LOW);
  }

  delay(100);                        // wait 100ms for next reading
/////////////////////////////*********DSM501A*******//////////////////////////////
 duration = pulseIn(pin, LOW);
  lowpulseoccupancy += duration;
  endtime = millis();
  if ((endtime-starttime) > sampletime_ms)
  {
    ratio = (lowpulseoccupancy-endtime+starttime + sampletime_ms)/(sampletime_ms*10.0);  // Integer percentage 0=>100
    concentration = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62; // using spec sheet curve
    Serial.print("lowpulseoccupancy:");
    Serial.print(lowpulseoccupancy);
    Serial.print("    ratio:");
    Serial.print(ratio);
    Serial.print("    DSM501A:");
    Serial.println(concentration);
    lowpulseoccupancy = 0;
    starttime = millis();
  }
  ////////thingspeak ping//////// 
  if (client.connect(server,80)) {  //   "184.106.153.149" or api.thingspeak.com
    String postStr = apiKey;
           postStr +="&field1=";
           postStr += String(t);
           postStr +="&field2=";
           postStr += String(h);
           postStr +="&field3=";
           postStr += String(sensorValue/60);
           postStr +="&field4=";
           postStr += String(concentration);
            
           postStr += "\r\n\r\n";
 
     client.print("POST /update HTTP/1.1\n"); 
     client.print("Host: api.thingspeak.com\n"); 
     client.print("Connection: close\n"); 
     client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n"); 
     client.print("Content-Type: application/x-www-form-urlencoded\n"); 
     client.print("Content-Length: "); 
     client.print(postStr.length()); 
     client.print("\n\n"); 
     client.print(postStr);
           
  }
  client.stop();
   
  Serial.println("Waiting...");    
  // thingspeak needs minimum 15 sec delay between updates
  delay(30000);  
}

