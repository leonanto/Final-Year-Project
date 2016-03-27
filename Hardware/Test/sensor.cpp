#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <gprs.h>

#define TEMP_PIN   2 //GPIO2 used to sense temperature
#define MOIST_PIN   A0 //ADC pin used to sense moisture
#define APN "ppinternet"

#define SIM800_TX_PIN 1//SIM800 TX is connected to ESP8266 GPIO1
 
#define SIM800_RX_PIN 3//SIM800 RX is connected to ESP8266 GPIO3

const char* ssid = "ESP8266 Leo";//SSID of the Acess point
const char* password = "leonardo";//Password of the Acess point 
float temp = 0.0;

GPRS gprs;

OneWire  onewire(TEMP_PIN);

DallasTemperature temp_sensor(&onewire);

SoftwareSerial serialSIM800(SIM800_TX_PIN,SIM800_RX_PIN);

float readTemp(){
  temp_sensor.requestTemperature();
  return (temp_sensor.getTempCByIndex(0));
}

float readMoist(){
  
}

void gprsSetup(){
  gprs.preInit();
  while(0 != gprs.init()) {
     delay(1000);
     Serial.println("init error");
  }
  while(!gprs.join("ppinternet")) {
      Serial.println("gprs join network error");
      delay(2000);
  }
   while(0 != gprs.connectTCP("mbed.org", 80)) {
      Serial.println("connect error");
  }
}

void gprsPostvalues(float temp){
  char http_cmd[] = "POST  HTTP/1.1\r\nHost: \r\nContent-Type:application\json\r\n\r\n";
  gprsSetup();
  Serial.println("waiting to post...");
  if(0 == gprs.sendTCPData(http_cmd))
  {      
    gprs.serialDebug();
  }
}

bool gprsGet(){
  char http_cmd[] = "GET /media/uploads/mbed_official/hello.txt HTTP/1.1\r\nHost: \r\n\r\n";
  gprsSetup();
  Serial.println("waiting to post...");
  if(0 == gprs.sendTCPData(http_cmd))
  {      
    gprs.serialDebug();
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(10);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
}

void loop() {
  // put your main code here, to run repeatedly:
  temp = readTemp();
  Serial.print("The temperature is" + temp +"C \n");
}