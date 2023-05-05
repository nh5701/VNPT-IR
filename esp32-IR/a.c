#include <WiFi.h>
#include <PubSubClient.h>
#include "IRremote.hpp"
#include "IRReceive.hpp"

// #define PUB_TOPIC "testSubRequest1" 
#define SUB_TOPIC "testPubData"
#define LED_SENSE 2           // blink when sense IR coming
#define LED_START 23          // ON to notify start receive IR signal
#define LED_SEND 4           // Config LED to send IR signal
#define LED_RECIEVE 15        // Config LED to receive IR signal

const char* ssid = "CAM_TEST2";
const char* password = "1234567890";
// const char* ssid = "P407";
// const char* password = "17052000";
const char* mqtt_server = "192.168.88.173";
// const char* mqtt_server = "192.168.1.2";      
int i=0;
int numberOfElements = 0;

void handleData(char str[]);
void stringToUint16(char s[], int sizeArray);
int countElement (char s[]);
uint16_t *data_recieve;
void connectBroker();
void callback(char* topic, byte *payload, unsigned int length);
void connectWifi (void);
void receiveData (int num);

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  IrSender.begin(LED_SEND);
  IrReceiver.begin(LED_RECIEVE,ENABLE_LED_FEEDBACK);
  pinMode(LED_SENSE, OUTPUT);
  pinMode(LED_START, OUTPUT);
  connectWifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  connectBroker();
}

void loop() {
  client.loop();
  if (!client.connected()) {
    connectBroker();
  }
  // int value = countElement();
  receiveData(numberOfElements);
}

void connectBroker() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
      client.subscribe("testPubData");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void callback(char* topic, byte *payload, unsigned int length) {
  char data[1000];
  Serial.println("\n-------New message from broker-----");
  // Serial.print("Topic: ");
  // Serial.println(topic);
  // Serial.print("Message: ");
  // Serial.write(payload, length);
  Serial.println();
  for(int i=0; i<length;i++){
    data[i] = *(payload+i);
  }
  // Serial.printf("  Data = %s\n",data);
  handleData(data);
  // receiveData(numberOfElements);
}

void connectWifi (void){
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.printf("IP address: %s",WiFi.localIP());
}

void handleData(char str[]){
  int size_array = countElement(str);
  // printf("Size = %d\n", size_array);
  char* token;

  data_recieve = (uint16_t*) calloc(size_array, sizeof(uint16_t));

  token = strtok(str, ",");
  while (token != NULL) {
      stringToUint16(token,size_array);
      token = strtok(NULL, ",");
  }
  IrSender.sendRaw(data_recieve,size_array, 38);
  numberOfElements = size_array;
  free(data_recieve);
}

void stringToUint16(char s[], int sizeArray) {
  unsigned int num = strtoul(s, NULL, 10);
  uint16_t num16 = (uint16_t)num;
  data_recieve[i] = num16;
  // printf("rawdata[%d] = %u\t", i,data_recieve[i]);
  // printf("%u  ", data_recieve[i]);
  if(i >= sizeArray-1)
    i=0;
  else 
    i++;
}

int countElement (char s[]){
  int count_element = 0;
  for(int i=0;i<strlen(s); i++){
      if(s[i] == ','){
          count_element++;
      }
  }
  return count_element+1;
}

void receiveData (int num){
  // Serial.printf("\nInside receiveData function\n");
  if (IrReceiver.decode()) {
    Serial.printf("State = %d\n", IrReceiver.decode());
    int rawlen = IrReceiver.decodedIRData.rawDataPtr->rawlen-1;
    Serial.printf("\nLength of array = %d", num);
    Serial.printf("\nLength = %d", rawlen);
    if(num == rawlen){
      uint8_t rawCode[RAW_BUFFER_LENGTH];
      IrReceiver.compensateAndStoreIRResultInArray(rawCode);
      // for (int i = 0; i < rawlen; i++) {
      //   printf("%u ", rawCode[i]);
      // } 
      // Serial.printf("Data = %", temp.substring(0,temp.length()-1));
      Serial.printf("\n[Protocol] %s - [Address] %hu - [Command] %hu\n", ProtocolNames[IrReceiver.decodedIRData.protocol],ProtocolNames[IrReceiver.decodedIRData.address],ProtocolNames[IrReceiver.decodedIRData.command]); 
      IrReceiver.printIRResultShort(&Serial);
    }
    else{
      Serial.printf("\nName of protocol: %s\n", ProtocolNames[IrReceiver.decodedIRData.protocol]); 
      printf("The signal was noise! Try again...\n");
    }
    IrReceiver.resume();
  }
  // Serial.printf("State = %d\n", IrReceiver.decode());
}

// // #include "stdio.h"
// // #include "stdint.h"

// // int main() {
// //     uint32_t a = 0x40;
// //     uint32_t b = 0x12;
// //     uint64_t c = 0x2fd48b7;
// //     uint64_t d = 0x48b7;

// //     uint64_t e = 0x2fd;
// //     uint64_t f = 0x14;

// //     printf("a = %d\n",a);
// //     printf("b = %d\n",b);
// //     printf("c = %ld\n",c);
// //     printf("e = %ld\n",e);
// //     printf("d = %ld\n",d);
// //     printf("f = %ld\n",f);

// //     return 0;
// // }
