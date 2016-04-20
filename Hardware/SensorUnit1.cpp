#include <OneWire.h>
#include <ESP8266WiFi.h>
#include <DallasTemperature.h>

#define TEMP_PIN    2 //GPIO2 used to sense temperature
#define MOIST_PIN   A0 //ADC pin used to sense moisture

const char* ssid = "LiFi";//SSID of the Acess point
const char* password = "leonardo";//Password of the Acess point
const char* host = "192.168.4.1";//Server IP address
const int httpPort = 80;//Server pott
int temp, moist;
OneWire  ds(2);

OneWire  onewire(TEMP_PIN);

DallasTemperature temp_sensor(&onewire);

//Read Temperature from the DSB temperature sensor using the Dallas temperature library
int readTemp(){
  temp_sensor.requestTemperatures();
  return (temp_sensor.getTempCByIndex(0));
}

//Read Moisture from the moisture sensor
int readMoist(){
  int sensorValue = analogRead(MOIST_PIN);
  float voltage = sensorValue * (1.0 / 1023.0);
  float percentage = voltage * 100;
  return percentage;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(10);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);

   while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
}

void loop(){
  delay(6000);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  temp = readTemp();
  moist = readMoist();

  // We now create a URI for the request
  String url = "/read/";
  url += 1;
  url += "t";
  url += temp;
  url += "t";
  url += moist;

  Serial.print("Requesting URL: ");
  Serial.println(url);

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  int timeout = millis() + 5000;
  while (client.available() == 0) {
    if (timeout - millis() < 0) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  Serial.println();
  Serial.println("closing connection");
}