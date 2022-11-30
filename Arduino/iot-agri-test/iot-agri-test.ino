/*
This code measures the 
# environmental parameters - temperature, humidty for DHT11 sensor
# Soil moisture sensor data to control the water pump
# Rain sensor to find the % of rain
# Ultrasonic sensor - to measure water level of the rain water storage containers

Buzzer is used to indicate the water storage container is full

All the above sensor details are uploaded to Google sheets in real time, in google sheet we need to use app script

**requirements - ESP32 url in Arduino IDE preferences, Adafruit Unified senor, DHT sensor library

*/
#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
//Things to change
const char * ssid = "<Hotspot name or Wifi name>";
const char * password = "<password>";

#define DHTPIN 4     // Digital pin connected to the DHT sensor 
#define DHTTYPE    DHT11     // DHT 11
#define SOIL_MOISTURE_PIN 36 // ESP32 pin GIOP36 (ADC0) that connects to AOUT pin of moisture sensor
#define RAIN_SENSOR_PIN 34 // ESP32 pin GIOP34 (ADC6) that connects to AOUT pin of rain sensor
#define waterPump 23 // ESP32 pin GIOP23 that connects to RELAY-water-pump
#define alarmBuzzer 21 // ESP32 pin GIOP23 that connects to RELAY-water-pump
const int trigPin = 5;
const int echoPin = 18;

String GOOGLE_SCRIPT_ID = "aaa.bbb.ccc"; //get the google script id from google sheets 
const int sendInterval = 1000; 

//updated 04.12.2019
const char * root_ca=\
"-----BEGIN CERTIFICATE-----\n" \
"MIIDujCCAqKgAwIBAgILBAAAAAABD4Ym5g0wDQYJKoZIhvcNAQEFBQAwTDEgMB4G\n" \
"A1UECxMXR2xvYmFsU2lnbiBSb290IENBIC0gUjIxEzARBgNVBAoTCkdsb2JhbFNp\n" \
"Z24xEzARBgNVBAMTCkdsb2JhbFNpZ24wHhcNMDYxMjE1MDgwMDAwWhcNMjExMjE1\n" \
"MDgwMDAwWjBMMSAwHgYDVQQLExdHbG9iYWxTaWduIFJvb3QgQ0EgLSBSMjETMBEG\n" \
"A1UEChMKR2xvYmFsU2lnbjETMBEGA1UEAxMKR2xvYmFsU2lnbjCCASIwDQYJKoZI\n" \
"hvcNAQEBBQADggEPADCCAQoCggEBAKbPJA6+Lm8omUVCxKs+IVSbC9N/hHD6ErPL\n" \
"v4dfxn+G07IwXNb9rfF73OX4YJYJkhD10FPe+3t+c4isUoh7SqbKSaZeqKeMWhG8\n" \
"eoLrvozps6yWJQeXSpkqBy+0Hne/ig+1AnwblrjFuTosvNYSuetZfeLQBoZfXklq\n" \
"tTleiDTsvHgMCJiEbKjNS7SgfQx5TfC4LcshytVsW33hoCmEofnTlEnLJGKRILzd\n" \
"C9XZzPnqJworc5HGnRusyMvo4KD0L5CLTfuwNhv2GXqF4G3yYROIXJ/gkwpRl4pa\n" \
"zq+r1feqCapgvdzZX99yqWATXgAByUr6P6TqBwMhAo6CygPCm48CAwEAAaOBnDCB\n" \
"mTAOBgNVHQ8BAf8EBAMCAQYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUm+IH\n" \
"V2ccHsBqBt5ZtJot39wZhi4wNgYDVR0fBC8wLTAroCmgJ4YlaHR0cDovL2NybC5n\n" \
"bG9iYWxzaWduLm5ldC9yb290LXIyLmNybDAfBgNVHSMEGDAWgBSb4gdXZxwewGoG\n" \
"3lm0mi3f3BmGLjANBgkqhkiG9w0BAQUFAAOCAQEAmYFThxxol4aR7OBKuEQLq4Gs\n" \
"J0/WwbgcQ3izDJr86iw8bmEbTUsp9Z8FHSbBuOmDAGJFtqkIk7mpM0sYmsL4h4hO\n" \
"291xNBrBVNpGP+DTKqttVCL1OmLNIG+6KYnX3ZHu01yiPqFbQfXf5WRDLenVOavS\n" \
"ot+3i9DAgBkcRcAtjOj4LaR0VknFBbVPFd5uRHg5h6h+u/N5GJG79G+dwfCMNYxd\n" \
"AfvDbbnvRG15RjF+Cv6pgsH/76tuIMRQyV+dTZsXjAzlAcmgQWpzU/qlULRuJQ/7\n" \
"TBj0/VLZjmmx6BEP3ojY+x1J96relc8geMJgEtslQIxq/H5COEBkEveegeGTLg==\n" \
"-----END CERTIFICATE-----\n";


//define sound speed in cm/uS
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

long duration;
float distanceCm;
float distanceInch;

WiFiClientSecure client;
DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;

void setup() {
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Inputl̥
  pinMode(SOIL_MOISTURE_PIN,INPUT);
  pinMode(RAIN_SENSOR_PIN,INPUT);
  pinMode(waterPump,OUTPUT);
  pinMode(alarmBuzzer,OUTPUT);
  Serial.begin(115200);
  delay(10);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

Serial.println("Started");
Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  DHTInitialization();

Serial.println("Ready to go");
}

void DHTInitialization(){
   dht.begin();
  Serial.println(F("Inializing DHT sensor"));
  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println(F("------------------------------------"));
  Serial.println(F("Temperature Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("°C"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("°C"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("°C"));
  Serial.println(F("------------------------------------"));
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  Serial.println(F("Humidity Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("%"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("%"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("%"));
  Serial.println(F("------------------------------------"));
  // Set delay between sensor readings based on sensor details.
  delayMS = sensor.min_delay / 1000;
}

//Temparature Reading and Processing
float getTemperature(void)
{
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
  }
  else {
    Serial.print(F("Temperature: "));
    Serial.print(event.temperature);
    Serial.println(F("°C"));
  }
float temp = event.temperature;
  return temp;
}

//Humidity Reading and Processing
float getHumidity(void)
{
  sensors_event_t event;
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));
  }
  else {
    Serial.print(F("Humidity: "));
    Serial.print(event.relative_humidity);
    Serial.println(F("%"));
  }
float hum = event.relative_humidity;
  return hum;
}

float getSoilMoisturePercentage(void)
{
  float value = analogRead(SOIL_MOISTURE_PIN); // read the analog value from sensor
  Serial.print("soil sesnor actual value: ");
  Serial.println(value);
  float sm_percentage = map(value, 4095, 2500, 0, 80);

  Serial.print("Moisture sesnor converted value: ");
  Serial.println(sm_percentage);
  return sm_percentage;
}

float getRainPercentage(void)
{
  float value = analogRead(RAIN_SENSOR_PIN); // read the analog value from sensor
    Serial.print("Rain sesnor actual value: ");
  Serial.println(value);
  float rain_percentage = map(value, 4095, 1780, 0, 80);

  Serial.print("Rain sesnor converted value: ");
  Serial.println(rain_percentage);
  return rain_percentage;
}
float getWaterLevelPercentage(void)
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance
  distanceCm = duration * SOUND_SPEED/2;
  
  // Convert to inches
  distanceInch = distanceCm * CM_TO_INCH;
  
  // Prints the distance in the Serial Monitor
  
  Serial.print("Distance (cm): ");
  Serial.println(distanceCm);
  if(distanceCm<4){
    digitalWrite(alarmBuzzer,HIGH);
    Serial.println("Water tank is full");
  }else{
    digitalWrite(alarmBuzzer,LOW);
    Serial.println("Water tank is not full");
  }
  Serial.print("Distance (inch): ");
  Serial.println(distanceInch);
  return map(distanceCm,3,10,0,100);
}


void loop() {
   delay(delayMS);
  float humidity = getHumidity();
  float temperature = getTemperature();
  float soilMoisturePercentage = getSoilMoisturePercentage();
  controlWaterPump(soilMoisturePercentage);
  float rainPercentage = getRainPercentage();
  float waterPercentage = getWaterLevelPercentage();
  String temperature_s(temperature);
  String humidity_s(humidity);
  String soilMoisturePercentage_s(soilMoisturePercentage);
  String rainPercentage_s(rainPercentage);
   String waterPercentage_s(100-waterPercentage);

  sendData("temp="+temperature_s+"&hum="+humidity_s+"&soil="+soilMoisturePercentage_s+"&rain="+rainPercentage_s+"&water="+waterPercentage_s);
  delay(sendInterval);
} 
void controlWaterPump(float soilMoisturePercentage){
if(soilMoisturePercentage > 80){
  digitalWrite(waterPump, LOW); //TURN off water pump if water is sufficient 
  Serial.println("Turned off water pump due to sufficient water");
}else if (soilMoisturePercentage < 20){
digitalWrite(waterPump, LOW); //TURN off water pump if water is in-sufficient 
Serial.println("Turned on water pump due to in-sufficient water");
}
}

void sendData(String params) {
   HTTPClient http;
   String url="https://script.google.com/macros/s/"+GOOGLE_SCRIPT_ID+"/exec?"+params;
   Serial.print(url);
    Serial.println("Making a request");
    http.begin(url); //Specify the URL and certificate
    int httpCode = http.GET();  
    http.end();
    Serial.println(": done "+httpCode);
}