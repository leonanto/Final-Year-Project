#include <ESP8266WiFi.h>

const char* ssid = "LiFi";
const char* password = "leonardo";
int unit1stat = 0;
int unit2stat = 0;
int temp[2], moist[2];
// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  delay(10);
  
  //Create AP
  WiFi.softAP(ssid, password);
 
  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.softAPIP());
}

void loop() {

  //Serial.println("Ready");
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
  if (req.indexOf("/favicon.ico") != -1){
    Serial.println("invalid request");
    client.stop();
    return;
  }
  //"/read/"
  int r_begin = req.indexOf('d');
  int r_end = req.indexOf('H');
  String clientValues = req.substring(r_begin+2, r_end-1);
  Serial.println(clientValues);

  int delim_1 = clientValues.indexOf('t');
  //Search for the next comma just after the first
  int delim_2 = clientValues.indexOf('t', delim_1+1);

  String unit = clientValues.substring(0, delim_1);
  String tempString = clientValues.substring(delim_1+1, delim_2);
  String moistString = clientValues.substring(delim_2+1);
  int u = unit.toInt();
  Serial.print("Unit ");
  Serial.println(u);
  Serial.print("Temp ");
  Serial.println(tempString);
  Serial.print("Moist ");
  Serial.println(moistString);

  if(u == 1){
    unit1stat = 1;
    temp[0] = tempString.toInt();
    moist[0] = moistString.toInt();
  }
  else if(u == 2){
    unit2stat = 1;
    temp[1] = tempString.toInt();
    moist[1] = moistString.toInt();
  }
  
  client.flush();

  // Prepare the response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nGPIO is now ";
  s += "</html>\n";
  
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

