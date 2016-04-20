#include <EEPROM.h>
#include <ESP8266WiFi.h>

const char* ssid = "LiFi";
const char* WiFiAPPSK = "leonardo";
unit1stat = 0;
unit2stat = 0;
int temp[1], moist[1];
// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  delay(10);
  
  //Create AP
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, WiFiAPPSK);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
  
  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();
  
  // Match the request
  String clientValues = req.substring(4);

  int delim_1 = clientValues.indexOf('t');
  //Search for the next comma just after the first
  int delim_2 = clientValues.indexOf('t', delim_1+1);

  String unit = clientValues.substring(0, delim_1);
  String tempString = clientValues.substring(delim_1+1, delim_2);
  String moistString = clientValues.substring(delim_2);
  int u = unit.toInt();

  if(u == 1){
    unit1stat = 1;
  }
  else if(u == 2){
    unit2stat =1;
  }

  temp[u-1] = tempString.toInt();
  moist[u-1] = moistString.toInt();
  
  client.flush();

  // Prepare the response
  //String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nGPIO is now ";
  //s += "</html>\n";
  String s = "200";

  // Send the response to the client
  client.print(s);
  delay(1);
  Serial.println("Client disonnected");
  Serial.println(temp[0]);
  Serial.println(temp[1]);
  Serial.println(moist[0]);
  Serial.println(moist[1]);

  // The client will actually be disconnected 
  // when the function returns and 'client' object is detroyed

  if(unit1stat == 1 && unit2stat == 1){
    unit1stat = 0;
    unit2stat = 0;
  }
}
