#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "IRremoteESP8266.h"
#include "IRsend.h"
#include "IRrecv.h"
#include <ArduinoJson.h>
#include <Wire.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "PinDefinitionsAndMore.h"

#define DISABLE_CODE_FOR_RECEIVER 
#define MICROS_PER_TICK 2

String s = "";
int sizePre;
int sizeAfter;
int temp;

// Information wifi
const char *ssid = "CAM_TEST2";
const char *password = "1234567890";

// Information MQTT broker IP address
// const char *mqttServer = "192.168.88.173";  //ifconfig of Duong
const char *mqttServer = "192.168.88.107";  //ipconfig of DucLM
const int mqttPort = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

const char *topicSub = "data";            // MQTT topic sub
const char *topicPub = "request";         // MQTT topic pub
const char *topicRepToSever = "status"; // MQTT topic pub

 // Define macros for input and output pin etc.
decode_results results;
struct decode_results getMyStructValue();
/*Declared function*/
void callback(char *topic, byte *payload, unsigned int length);
void removeCommasEndString(String &string);
std::vector<uint16_t> decode(const std::string& source);
std::vector<uint16_t> stringToUint16Array(String &str);
void sendRawData(String &str);
void receiverRawData();
void sendAndReceiverData(String &str);
String createJsonTemplate(String status);
void connectWifi();
void reconnect();
void connectMqtt();
std::vector<uint16_t> decodeRawData(const char* encodedRaw);

IRsend irsend(IR_SEND_PIN);
IRrecv irrecv(IR_RECEIVE_PIN);

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  irsend.begin();
  irrecv.enableIRIn();
  // IrSender.begin(IR_SEND_PIN); // Start with IR_SEND_PIN as send pin and enable feedback LED at default feedback LED pin
  // IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);

  connectWifi();
  connectMqtt();
  client.subscribe(topicSub);
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
    Serial.print("+");
  }
  client.loop();
}

/*Setting Wifi and MQTT*/
void connectWifi()
{
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.printf(".");
  }
  Serial.println();
  Serial.println("Connected to Wifi!");
}

void connectMqtt()
{
  client.setServer(mqttServer, mqttPort);
  client.setBufferSize(512); 
  // client.setCallback(callback);
  client.setCallback(callback);
  if (!client.connected())
  {
    reconnect();
  }
}

void reconnect()
{
  while (!client.connected())
  {
    if (client.connect("esp32-client"))
    {
      Serial.println("Connected to MQTT broker");
      client.subscribe(topicSub);
    }
    else
    {
      Serial.println("Failed to connect to MQTT broker, retrying...");
      delay(2000);
    }
  }
}

void sendAndReceiverData(String &str){
  String data;
  // std::string stdString = std::string(str.c_str());
  const char * data_to_decode = str.c_str();
  std::vector<uint16_t> rawDataIsDecoded = decodeRawData(data_to_decode);

  const uint16_t *rawData = rawDataIsDecoded.data();
  int size = rawDataIsDecoded.size();
  irsend.sendRaw(rawData,size,38);
  sizePre = size;

  delay(500);

  while (results.rawlen != size){
    irrecv.decode(&results); 
  }

  if(irrecv.decode(&results))
  { 
    // if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_WAS_OVERFLOW)
    if(results.overflow)
    {
      Serial.println("\nOverflow!");
    }
    else {
      if (results.decode_type == UNKNOWN || results.decode_type == RC5){
        data = createJsonTemplate("0");
        const char * data_str = data.c_str();
        client.publish(topicRepToSever, data_str);
      }
      else {
        data = createJsonTemplate("1");
        const char * data_str = data.c_str();
        client.publish(topicRepToSever, data_str);
      }
    }

    temp = sizeAfter * sizePre;
    // IrReceiver.resume();
    irrecv.resume();
  }
}

void removeCommasEndString(String &string)
{
  int i = 0;
  while (string[i] != '\0')
  {
    i++;
  }
  string[i - 1] = '\0';
}

std::vector<uint16_t> stringToUint16Array(String &str)
{
  std::vector<uint16_t> arr;
  std::string temp;

  for (size_t i = 0; i < str.length(); i++)
  {
    if (isdigit(str[i]))
    {
      temp += str[i];
    }
    else if (!temp.empty())
    {
      uint16_t num = std::stoi(temp);
      arr.push_back(num);
      temp.clear();
    }
  }
  // Handle the last number
  if (!temp.empty())
  {
    uint16_t num = std::stoi(temp);
    arr.push_back(num);
  }

  return arr;
}

/*FUNCTION Data*/

// Put value IR into struct
struct decode_results getMyStructValue()
{
  struct decode_results dataInfo;

  // dataInfo.rawbuf = IrReceiver.decodedIRData.rawDataPtr->rawbuf;
  dataInfo.rawbuf = results.rawbuf;
  dataInfo.rawlen = results.rawlen;
  dataInfo.address = results.address;
  dataInfo.command = results.command;
  // dataInfo.rawlen = IrReceiver.decodedIRData.rawDataPtr->rawlen;
  // dataInfo.address = IrReceiver.decodedIRData.address;
  // dataInfo.value = IrReceiver.decodedIRData.command;
  // dataInfo.name = IrReceiver.decodedIRData.protocol;
  return dataInfo;
}


void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message received: ");
  String dataReceived = "";
  for (int i = 0; i < length; i++)
  {
    // Serial.print((char)payload[i]);
    dataReceived += String((char)payload[i]);
  }
  // Serial.println(dataReceived);
  Serial.println("NHAN++++++++");
  
  s = dataReceived;
  // std::string dataStringToStdString = std::string(s.c_str());
  do
  {
    // std::vector<uint16_t> decode(const std::string& source);
    sendAndReceiverData(s);
  } while (temp == 0);
  s = "";
  sizeAfter = 0;
  sizePre = 0;
}

std::vector<uint16_t> decodeRawData(const char* encodedRaw)
{
    std::vector<uint16_t> rawDataArray;
    std::vector<uint16_t> mappingRaw;

    String numberString = "";
    uint16_t alphabetCOunt = 0;

    for (int16_t i = 0; i < strlen(encodedRaw); i++) {
        if (encodedRaw[i] == '+' || encodedRaw[i] == '-') {
            // if character + or -
            if (numberString.length()) {
                uint16_t value = numberString.toInt();
                mappingRaw.push_back(value);
                rawDataArray.push_back(value);
            }
            numberString = "";
        } else if ((uint8_t)encodedRaw[i] >= 48 && (uint8_t)encodedRaw[i] <= 57) {
            // if number
            numberString += encodedRaw[i];
        } else {
            // if alphabet
            if (numberString.length()) {
                mappingRaw.push_back(numberString.toInt());
                rawDataArray.push_back(numberString.toInt());
            }
            // if lowercase
            if ((uint8_t)encodedRaw[i] >= 97 && (uint8_t)encodedRaw[i] <= 122) {
                rawDataArray.push_back(mappingRaw[(uint8_t)encodedRaw[i] - 97]);
            } else {
                // else upper case
                rawDataArray.push_back(mappingRaw[(uint8_t)encodedRaw[i] - 65]);
            }
            numberString = "";
        }
    }

    return rawDataArray;
}

std::string encodeRawData(uint8_t* rawData, uint8_t rawLength)
{
    std::map<char, uint16_t> map_pair;
    std::map<char, uint16_t>::iterator it = map_pair.begin();
    std::string encodedRaw = "";

    bool isDup = false;

    char startU = 'A';

    for (uint8_t i = 0; i < rawLength; i++) {
        // if positive (carrier)
        if (i % 2 == 0) {
            while (it != map_pair.end()) {
                // compare value
                if ((it->second - rawData[i] * MICROS_PER_TICK) == 0) {
                    encodedRaw += it->first;
                    isDup = true;
                    break;
                }
                ++it;
            }
            if (!isDup) {
                encodedRaw += "+" + std::to_string(rawData[i] * MICROS_PER_TICK);
                map_pair[startU++] = rawData[i] * MICROS_PER_TICK;
            }
            // reset for new iteration
            it = map_pair.begin();
            isDup = false;
        }
        // if negative (no carrier)
        else {
            while (it != map_pair.end()) {
                if ((it->second - rawData[i] * MICROS_PER_TICK) == 0) {
                    encodedRaw += (char)(it->first + 32); // convert 'A' to 'a'
                    isDup = true;
                    break;
                }
                ++it;
            }
            if (!isDup) {
                encodedRaw += "-" + std::to_string(rawData[i] * MICROS_PER_TICK);
                map_pair[startU++] = rawData[i] * MICROS_PER_TICK;
            }
            // reset for new iteration
            it = map_pair.begin();
            isDup = false;
        }
    }
    return encodedRaw;
}

String createJsonTemplate(String status){
  String data = "{\"code\":[";
            
  for (uint16_t i = 1; i < results.rawlen; i++) {
      data += String(results.rawbuf[i]) + ",";
  }
  
  data.substring(0, data.length() - 2); // cut comma in tail of raw string
  data += "],";

  data += "\"mapping_code\":\"";
  data += String(results.decode_type) + "_" + String(results.command) + "_" + String(results.address) + "_" + String(results.value) + "\",";
  data += "\"status\":";
  data += status;
  data += "}";

  return data;
}