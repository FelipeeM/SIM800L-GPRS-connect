// FelipeeM
// https://github.com/FelipeeM/SIM800L-GPRS-connect

#include <SoftwareSerial.h>
#include <Wire.h>

#define TX_PIN 10
#define RX_PIN 11
#define RESET_PIN 4

SoftwareSerial sim800lSerial(TX_PIN, RX_PIN); //RX, TX Pins
String apn = "APN";                           //APN
String apn_u = "APN-Username";                //APN-Username
String apn_p = "APN-Password";                //APN-Password
String url = "http://YOUR-API.com/";          //URL of API

boolean send_command_sim800l(String command, int timeout = 1000) {
  Serial.print("Command => ");
  sim800lSerial.println(command);
  String response = "";
  boolean result = false;
  unsigned long previous;
  for (previous = millis(); (millis() - previous) < timeout;) {
    while (sim800lSerial.available()) {
      response = sim800lSerial.readString();
      if (response.indexOf("ERROR") < 0) {
        result = true;
        goto OUTSIDE;
      }
    }
  }
  OUTSIDE:
    if (response != "") { Serial.println(response); }
    return result;
}
void gprs_config() {
  send_command_sim800l("AT+SAPBR=3,1,CONTYPE,GPRS", 10000);          
  send_command_sim800l("AT+SAPBR=3,1,\"APN\",\"" + apn + "\"");      
  send_command_sim800l("AT+SAPBR=3,1,\"USER\",\"" + apn_u + "\"");  
  send_command_sim800l("AT+SAPBR=3,1,\"PWD\",\"" + apn_p + "\"");   
  send_command_sim800l("AT+SAPBR=2,1");                             
  send_command_sim800l("AT+SAPBR=1,1", 5000);
  send_command_sim800l("AT+HTTPINIT", 1000);
  send_command_sim800l("AT+HTTPPARA=\"CID\",1", 1000);
}
void setup() {
  Serial.begin(9600);
  sim800lSerial.begin(9600);

  pinMode(RESET_PIN, OUTPUT);
  digitalWrite(RESET_PIN, LOW);
  delay(100);
  digitalWrite(RESET_PIN, HIGH);
  delay(30000);  //Wait 30 seconds for start configs sim800l
  while (!send_command_sim800l("AT", 1000)) {
    Serial.println(">> Wait Sim800l Response <<");
  }
  gprs_config();
}

void gprs_send_get(String payload) {
  Serial.println(">>> Send HTTP Get Request <<<");
  send_command_sim800l("AT+HTTPPARA=URL," + url + payload, 2000);
  send_command_sim800l("AT+HTTPACTION=0",2000);
  send_command_sim800l("AT+HTTPREAD",2000);
  // send_command_sim800l("AT+HTTPTERM"); // Close http connection
}
void loop() {
  gprs_send_get("TEST");
  Serial.println("Wait one minute for next HTTP Get");
  delay(60000);  //Wait one minute
}
