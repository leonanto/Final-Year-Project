#include <ESP8266WiFi.h>
#include <gprs.h>
#include <SoftwareSerial.h>

const char* ssid = "LiFi";
const char* password = "leonardo";
#define APN "ppinternet"
#define HOST ""

//Irrigation system pin GPIO16
#define IR_PIN 16

//SIM800 TX is connected to ESP8266 GPIO4
//#define SIM800_TX_PIN  4
 
//SIM800 RX is connected to ESP8266 GPIO5
//#define SIM800_RX_PIN  5

//Create software serial object to communicate with SIM800
SoftwareSerial serialSIM800(SIM800_TX_PIN,SIM800_RX_PIN);

int unit1stat = 0;
int unit2stat = 0;
float temp[2], moist[2];
int stat;
// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);//default server ip 192.168.4.1

GPRS gprs;

void gprsSetup(){
	gprs.preInit();
	while(0 != gprs.init()) {
    	delay(1000);
    	Serial.println("init error");
  	}
  	while(!gprs.join(APN)) {
    	Serial.println("gprs join network error");
      	delay(2000);
  	}
   	while(0 != gprs.connectTCP(HOST, 80)) {
      	Serial.println("connect error");
  	}
}

void gprsSendValues(float temp1, float moist1, float temp2, float moist2, int stat){
 	char http_cmd[128];
  	sprintf(http_cmd, "GET /PostValues/?temp1=%f&moist1=%f&temp2=%f&moist2=%f&stat=%d HTTP/1.1\r\nHost:%s\r\nConnection:keep-alive\r\n\r\n", temp1, moist1, temp2, moist2, stat, HOST);
  	Serial.println("waiting to Send...");
  	if(0 == gprs.sendTCPData(http_cmd))
  	{      
    	gprs.serialDebug();
  	}
}

void getIrrigationStatus(){
	char http_cmd[128];
	sprintf(http_cmd, "GET /IrrigationStatus HTTP/1.1\r\nHost:%s\r\nConnection:keep-alive\r\n\r\n", HOST);
	 if(0 == gprs.sendTCPData(http_cmd))
  	{      
    	String response = gprs.serialDebug();

    	if(response.indexOf("Status: ON") != -1){
    		digitalWrite(IR_PIN, HIGH);
    		stat = 1;
  		}
  		else(response.indexOf("Status: OFF") != -1){
  			digitalWrite(IR_PIN, LOW);
  			stat = 0;
  		}
  	}
}

void setup() {
  	Serial.begin(115200);
  	delay(10);
  	gprsSetup();
  	//Create AP
  	WiFi.softAP(ssid, password);
 
	// Start the server
  	server.begin();
  	Serial.println("Server started");

  	// Print the IP address
  	Serial.println(WiFi.softAPIP());
}

void loop() {
 	// Check irrigation System Status 

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
    	temp[0] = tempString.toFloat();
    	moist[0] = moistString.toFloat();
  	}
  	else if(u == 2){
    	unit2stat = 1;
    	temp[1] = tempString.toFloat();
    	moist[1] = moistString.toFloat();
  	}
  
  	client.flush();

  	// Prepare the response
  	String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nData recieved ";
  	s += "</html>\n";
  	//String s = "200";

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
   		gprsSetup();
    	gprsSendValues(temp[0], moist[0], temp[1], moist[1], stat);
  	}
}
