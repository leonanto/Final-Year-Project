#include <gprs.h>

#define APN "ppinternet"
#define HOST "final-year-project-leonanto.c9users.io"
#define PORT 8080
//Irrigation system pin GPIO16
#define IR_PIN 2

//SIM800 TX is connected to ESP8266 GPIO4
//#define SIM800_TX_PIN  4
 
//SIM800 RX is connected to ESP8266 GPIO5
//#define SIM800_RX_PIN  5

int unit1stat = 0;
int unit2stat = 0;
int temp1, temp2, moist1, moist2;
int stat, override;
// Create an instance of the server
// specify the port to listen on as an argument
GPRS gprs;

void gprsSetup(){
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Start setup");
  Serial.println(APN);
  Serial.println(HOST);
  gprs.preInit();
  Serial.println("PreInt success");
  while(0 != gprs.init()) {
      delay(1000);
      Serial.println("init error");
    }
    Serial.println("Int success");
    while(!gprs.join(APN)) {
      Serial.println("gprs join network error");
        delay(2000);
    }
    //Serial.println("Join success");
}

void serverConnect(char* data){
     if(0 != gprs.connectTCP(HOST,PORT)){
        Serial.println("connect TCP error");
        goto STOP;
     }
     if(0 != gprs.sendTCPData(data)){
        Serial.println("send TCP data error");
        goto STOP;
     }
     STOP:
     gprs.closeTCP();
}  

void gprsSendValues(int temp1, int moist1, int temp2, int moist2, int stat){
   char http_cmd[128];
   String response;     
   char c;
   int count = 0;
   while(0 != gprs.connectTCP(HOST, 8080)) {
        break;
    }
    //Serial.println("connection success");
    sprintf(http_cmd, "GET /GV/?t1=%d&m1=%d&t2=%d&m2=%d&s=%d HTTP/1.1\r\nHost: %s\r\n\r\n", temp1, moist1, temp2, moist2, stat, HOST);
    Serial.println("waiting to Send...");
    Serial.println(http_cmd);
    //serverConnect(http_cmd);
    if(0 == gprs.sendTCPData(http_cmd))
    {    
         Serial.println("Sent");       
//       String response;
//       char c;
//       int count = 0;
//        while(c != 'K' ) {
//          if(gprs.serialSIM800.available()){
//              c = gprs.serialSIM800.read();
//              response += c;
//              count++;
//          }
//        }
    }
    delay(2000);
}

void getOverrideStatus(){
  char http_cmd[128];
  String response;     
  char c;
  int count = 0;
  while(0 != gprs.connectTCP(HOST, 8080)) {
        break;
    }
    //Serial.println("connection success");
  sprintf(http_cmd, "GET /OS HTTP/1.1\r\nHost: %s\r\n\r\n", HOST);
  //serverConnect(http_cmd);
   if(0 == gprs.sendTCPData(http_cmd))
    {      
        while(c != '!') {
          if(gprs.serialSIM800.available()){
              c = gprs.serialSIM800.read();
              response += c;
              count++;
          }
        }
      }

       Serial.print("Complete: ");
       Serial.println(response);
       Serial.print("Count: ");
       Serial.println(count);

       
      if(response.indexOf("Status: ON") != -1){
        override = 1;
      }
      else if (response.indexOf("Status: OFF") != -1){
        override = 0;
      }
      delay(2000);
}

void getIrrigationStatus(){
  char http_cmd[128];
  String response;     
  char c;
  int count = 0;
  while(0 != gprs.connectTCP(HOST, 8080)) {
        break;
    }
    //Serial.println("connection success");
  sprintf(http_cmd, "GET /IS HTTP/1.1\r\nHost: %s\r\n\r\n", HOST);
   //serverConnect(http_cmd);
   if(0 == gprs.sendTCPData(http_cmd))
    { 
        while(c != '!') {
          if(gprs.serialSIM800.available()){
              c = gprs.serialSIM800.read();
              response += c;
              count++;
          }
        }
       }

       Serial.print("Complete: ");
       Serial.println(response);
//       Serial.print("Count: ");
//       Serial.println(count);

      
      if(response.indexOf("Status: ON") != -1){
        digitalWrite(IR_PIN, HIGH);
        stat = 1;
      }
      else if(response.indexOf("Status: OFF") != -1){
        digitalWrite(IR_PIN, LOW);
        stat = 0;
      }
    delay(2000);      
}

void setup() {
    gprsSetup();
    pinMode(2, OUTPUT);
    Serial.begin(9600);
    delay(10);
    temp1 = 0;
    temp2 = 0;
    moist1 = 0;
    moist2 = 0;
    Serial.println(temp1);
    Serial.println(temp2);
    Serial.println(moist1);
    Serial.println(moist2);
}

void loop() {
  Serial.println("Begin");
  getOverrideStatus();
  temp1++;
  temp2++;
  moist1++;
  moist2++;
  Serial.println("Prep values");
  Serial.println(temp1);
  Serial.println(temp2);
  Serial.println(moist1);
  Serial.println(moist2);
  gprsSendValues(temp1, moist2, temp1, moist2, stat);
  if(override == 0){
      if((moist1+moist2)/2 > 30){
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
}

