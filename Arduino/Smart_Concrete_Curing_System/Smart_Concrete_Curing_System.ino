/*************************************************************
Smart Concrete Curing System
In the present era, water scarcity is the biggest problem. In Construction potable water
is required for mixing and curing of concrete. The curing process is required for 28 days. So the
water requirement in the construction field for curing purposes is very large. Due to various
reasons, potable water availability is decreasing day by day. So it is required to spend a
considerable amount on the procurement of water. To control the wastage of water for curing it is
necessary to use water in a controlled manner by adopting advanced technology. It is known as a
smart concrete curing system. The smart concrete curing system is developed to create an
automatic curing mechanism to supply water for curing depending on the availability of moisture
in the concrete and surrounding temperature using moisture sensor. The system will be connected
to the internet using Wi-Fi. The current moisture content level of the concrete structure and the
pump status will be pushed to the cloud. A mobile app will access this data from the cloud. So
that the curing process monitoring can be done remotely. Results shown that strength of the cube
with smart concrete curing system is more than the strength of the cube with immersion curing.

Author- Pavan Kumar Pacharla
Date - 27/12/2022
 *************************************************************/

// Template ID, Device Name and Auth Token are provided by the Blynk.Cloud
// See the Device Info tab, or Template settings
#define BLYNK_TEMPLATE_ID "****************" //commented
#define BLYNK_DEVICE_NAME "Smart Concrete Curing System"
#define BLYNK_AUTH_TOKEN "****************" //commented


// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Honor";
char pass[] = "pavan961";

BlynkTimer timer;

#define waterPumpPin D5
#define moistureSensorPin A0
#define temperatureSensorPin D7
// This function is called every time the Virtual Pin 0 state changes
// BLYNK_WRITE(V0)
// {
//   // Set incoming value from pin V0 to a variable
//   int value = param.asInt();

//   // Update state
//   Blynk.virtualWrite(V4, value);
// }

// This function is called every time the device is connected to the Blynk.Cloud
// BLYNK_CONNECTED()
// {
//   // Change Web Link Button message to "Congratulations!"
//   Blynk.setProperty(V5, "offImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations.png");
//   Blynk.setProperty(V5, "onImageUrl",  "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations_pressed.png");
//   Blynk.setProperty(V5, "url", "https://docs.blynk.io/en/getting-started/what-do-i-need-to-blynk/how-quickstart-device-was-made");
// }

// This function sends Arduino's uptime every second to Virtual Pin 2.
void myTimerEvent()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V1, millis() / 1000);
}

void setup()
{
  // Debug console
  Serial.begin(115200);
  pinMode(waterPumpPin, OUTPUT);
pinMode(moistureSensorPin, INPUT);
pinMode(temperatureSensorPin, INPUT);

  Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);

  // Setup a function to be called every second
  timer.setInterval(1000L, myTimerEvent);
}

void loop()
{
  Blynk.run();
  timer.run();
  int temperatureValue = getConcreteTemperatureValue();
  int moistureValue = getConcreteMoistureValue();
  if (moistureValue > 400){
    //turn off the water pump
    digitalWrite(waterPumpPin,LOW);
     Blynk.virtualWrite(V3,"Motor OFF");

  }else if (moistureValue < 100){
    //turn on the water pump
    digitalWrite(waterPumpPin,HIGH);
     Blynk.virtualWrite(V3,"Motor ON");

  }
  Blynk.virtualWrite(V2,temperatureValue);
 
}

int getConcreteTemperatureValue(){
  int tempValue = analogRead(temperatureSensorPin);
  return tempValue;
}
int getConcreteMoistureValue(){
  int moistValue = analogRead(moistureSensorPin);
  return moistValue;
}