#include <OneWire.h>
#include <ESP8266WiFi.h>
#include <DallasTemperature.h>

#define TEMP_PIN     13//GPIO13 used to sense temperature
#define MOIST_PIN   A0 //ADC pin used to sense moisture
#define HOST        "192.168.4.1"////Server IP address
#define PORT        80//Server port

const char* ssid = "LiFi";//SSID of the Acess point
const char* password = "leonardo";//Password of the Acess point
int temp, moist;
OneWire ds(2);

OneWire onewire(TEMP_PIN);

DallasTemperature temp_sensor(&onewire);

//Read Temperature from the DSB temperature sensor using the Dallas temperature library
int readTemp(){
  temp_sensor.requestTemperatures();
  return (temp_sensor.getTempCByIndex(0));
}

//Read Moisture from the moisture sensor
int readMoist(){
  int sensorValue = analogRead(MOIST_PIN);
  Serial.println("sensorValue ");
  Serial.println(sensorValue);
  float voltage = sensorValue / 1023.0;
  Serial.println("Voltage ");
  Serial.println(voltage);
  float raw =100 -(voltage * 100);
  float percentage = (raw - 5)*(3+(1/3));
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

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

   while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(".");
  Serial.println("Connected");
}

void loop(){
  delay(5000);

  temp = readTemp();
  moist = readMoist();

  Serial.println(temp);
  Serial.println(moist);

  // We now create a URI for the request
  String url = "/read/";
  url += 1;
  url += "t";
  url += temp;
  url += "t";
  url += moist;

  Serial.print("Requesting URL: ");
  Serial.println(url);

  // Use WiFiClient class to create TCP connections
    WiFiClient client;

    if (!client.connect(HOST, PORT)) {
        Serial.println("connection failed");
        Serial.println("wait 5 sec...");
        delay(5000);
        return;
    }

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + HOST+ "\r\n" + 
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
  client.stop();
  delay(30000);
}

