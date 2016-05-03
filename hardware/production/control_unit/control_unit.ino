#include <ESP8266WiFi.h>
//#include <gprs.h>

const char* ssid = "LiFi";
const char* password = "leonardo";
const char* connectssid = "MonaConnect";
//#define APN "ppinternet"
#define HOST "final-year-project-leonanto.c9users.io"
#define PORT 8080
//Irrigation system pin GPIO16
#define IR_PIN 16

int unit1stat = 0;
int unit2stat = 0;     
int temp1, temp2, moist1, moist2;
int stat, override;
// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);//default server ip 192.168.4.1


void gprsSendValues(int temp1, int moist1, int temp2, int moist2, int stat){
   // Use WiFiClient class to create TCP connections
  WiFiClient client;
  if (!client.connect(HOST, PORT)) {
    Serial.println("connection failed");
    return;
  }
  Serial.println("Connected GV");
   char http_cmd[128];
   String response;
   //HTTP GET Request to send data to the server    
   sprintf(http_cmd, "/GV/?t1=%d&m1=%d&t2=%d&m2=%d&s=%d HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", temp1, moist1, temp2, moist2, stat, HOST);
   Serial.println(http_cmd);
   client.print(String("GET ") + http_cmd );
  int timeout = 50000;
  int countdown = 0;
  while (client.available() == 0) {
    if (timeout - countdown < 0) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      break;
    }
    countdown++;
  }
  Serial.println("Data sent");
  while(client.available()){
    response = client.readStringUntil('\r');
  }
  Serial.print("Complete: ");
  Serial.println(response);

   if(response.indexOf("Received") != -1){
    unit1stat = 0;
    unit2stat = 0;
  }   
}

void getOverrideStatus(){
  delay(1000);
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  if (!client.connect(HOST, PORT)) {
    Serial.println("connection failed");
    return;
  }
  Serial.println("Connected to OS");
  char http_cmd[128];
  String response;
  //HTTP GET Request to GET override status from the server
  sprintf(http_cmd, "/OS HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", HOST);
  client.print(String("GET ") + http_cmd );
  Serial.println("Get request sent");
  int timeout = 50000;
  int countdown = 0;
  while (client.available() == 0) {
    if (timeout - countdown < 0) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      break;
    }
    countdown++;
  }
  Serial.println("OS Recieved");
  while(client.available()){
    response = client.readStringUntil('\r');
  }
  Serial.print("Complete: ");
  Serial.println(response);
   
  if(response.indexOf("Status: ON") != -1){
    override = 1;
  }
  else if (response.indexOf("Status: OFF") != -1){
    override = 0;
  }
}

void getIrrigationStatus(){
  delay(1000);
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  if (!client.connect(HOST, PORT)) {
    Serial.println("connection failed");
    return;
  }
  Serial.println("IS Recieved");
  Serial.println("Connected IR");
  char http_cmd[128];
  String response;     
  char c;
  int count = 0;
  //HTTP GET Request to GET irrigation status from the server
  sprintf(http_cmd, "/IS HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", HOST);
  client.print(String("GET ") + http_cmd );
  int timeout = 50000;
  int countdown = 0;
  while (client.available() == 0) {
    if (timeout - countdown < 0) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      break;
    }
    countdown++;
  }
  
  while(client.available()){
    response = client.readStringUntil('\r');
  }
  Serial.print("Complete: ");
  Serial.println(response);

  if(response.indexOf("Status: ON") != -1){
    digitalWrite(IR_PIN, HIGH);
    stat = 1;
  }
  
  else if(response.indexOf("Status: OFF") != -1){
    digitalWrite(IR_PIN, LOW);
    stat = 0;
  }          
}

void setup() {
  pinMode(IR_PIN, OUTPUT);
  Serial.begin(115200);
  delay(10);

  //Connect to AP
  WiFi.begin(connectssid);

   while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");    
  }
  Serial.println("Connected");
  //Create AP
  WiFi.softAP(ssid, password);
 
  // Start the server
  Serial.println("Server begin");
  server.begin();
  //Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.softAPIP());
}

void loop() {
  Serial.println("Connect to server");
  Serial.println("Connected ");
  //Get override status from the server
  getOverrideStatus();
  if(override == 0){
      if((moist1+moist2)/2 < 50){
        digitalWrite(IR_PIN, HIGH);
        stat = 1; 
      }
      else{
        digitalWrite(IR_PIN, LOW);
        stat = 0;
      }
  }
  else{
    getIrrigationStatus();
  }
  
  Serial.println("Get Results");
  // Check if a client has connected
  WiFiClient client2 = server.available();
  if(!client2) {
    return;
  }
  Serial.println("Get Results 2");
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client2.available()){
    delay(1);
  }
  
    // Read the first line of the request
    String req = client2.readStringUntil('\r');
    //Serial.println(req);
    client2.flush();
    
    // Match the request
    if (req.indexOf("/favicon.ico") != -1){
      //Serial.println("invalid request");
      client2.stop();
      return;
    }
    //"/read/"
    int r_begin = req.indexOf('d');
    int r_end = req.indexOf('H');
    String clientValues = req.substring(r_begin+2, r_end-1);
   // Serial.println(clientValues);
    
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
      temp1 = tempString.toInt();
      moist1 = moistString.toInt();
    }
    else if(u == 2){
      unit2stat = 1;
      temp2 = tempString.toInt();
      moist2 = moistString.toInt();
    }
    
    client2.flush();
    
    // Prepare the response
    String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nUnit ";
    s += u;
    s += " received";
    s += "</html>\n\r";
    //String s = "200";
    
    // Send the response to the client
    client2.print(s);
    delay(1);
    Serial.println("Client disonnected");
    Serial.println(temp1);
    Serial.println(temp2);
    Serial.println(moist1);
    Serial.println(moist2);
    
    // The client will actually be disconnected 
    // when the function returns and 'client' object is detroyed
 
  if(unit1stat == 1 && unit2stat == 1){
    
    gprsSendValues(temp1, moist1, temp2, moist2, stat);
  }
  delay(1000);
}

