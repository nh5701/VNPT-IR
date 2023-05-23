#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
// #include <IRremote.hpp>
#include "IRremoteESP8266.h"
#include "IRsend.h"
#include "IRrecv.h"
#include "IRReceive.hpp"
#include <ArduinoJson.h>
#include <Wire.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "PinDefinitionsAndMore.h"

#define DISABLE_CODE_FOR_RECEIVER // Disables restarting receiver after each send. Saves 450 bytes program memory and 269 bytes RAM if receiving functions are not used.
#define MICROS_PER_TICK 2

String s = "";
int sizePre;
int sizeAfter;
int temp;

// Information wifi
const char *ssid = "CAM_TEST2";
const char *password = "1234567890";

// Information MQTT broker IP address
const char *mqttServer = "192.168.88.173";
const int mqttPort = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

const char *topicSub = "data";            // MQTT topic sub
const char *topicPub = "request";         // MQTT topic pub
const char *topicRepToSever = "status"; // MQTT topic pub

 // Define macros for input and output pin etc.
decode_results results;
struct decode_results getMyStructValue();

void removeCommasEndString(String &string);
std::vector<uint16_t> decode(const std::string& source);
std::vector<uint16_t> stringToUint16Array(String &str);
void sendRawData(String &str);
void receiverRawData();
void sendAndReceiverData(String &str);

void conectWifi();
void reconnect();
void connectToMqttBroker();

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

  conectWifi();

  connectToMqttBroker();
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
void sendAndReceiverData(String &str)
{
  String s1 = "";
  String command;
  // std::vector<uint16_t> data = decode(std::string(s.c_str()));
  // String arduinoString = "Your Arduino String";
  std::string stdString = std::string(str.c_str());

  std::vector<uint16_t> myArray = decode(stdString);

  const uint16_t *rawData = myArray.data();
  int size = myArray.size();
  // irsend.sendNEC((uint64_t)rawData, size, 38);
  irsend.sendRaw(rawData,size,38);
  // IrSender.sendRaw(rawData, size, NEC_KHZ);
  sizePre = size;

  delay(500);
  // if (IrReceiver.decode())
  if(irrecv.decode(&results))
  { 
    // if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_WAS_OVERFLOW)
    if(results.overflow)
    {
      Serial.println("\nOverflow!");
    }
    else
    {
      if (results.decode_type == UNKNOWN || results.decode_type == RC5)
      {
        Serial.println(F("UNKNOWN OR RC5!"));
      }
      else
      {
        Serial.println();
        for (int i = 1; i <= getMyStructValue().rawlen - 1; i++)
        {
          s1 += String(getMyStructValue().rawbuf[i] * MICROS_PER_TICK) + ",";
        }
        sizeAfter = int(getMyStructValue().rawlen - 1);
        Serial.println("COMMAD: " + results.command);
        Serial.printf("\n[Protocol] %s - [Address] %hu - [Command] %hu\n",results.decode_type, result.address, results.command);
        // command = getMyStructValue().value;
        command = results.command;
        String status;
        String data;
        if (sizeAfter != 0 || sizePre != 0)
        {
          if (sizeAfter == sizePre)
          {
            data = "{\"code\":[";
            status = "1";
            
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
            const char * data_str = data.c_str();
            // send JSON to MQTT
            client.publish(topicRepToSever, data_str);
          }
          else
          {
            data = "{\"code\":[";
            status = "0";
            
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
            const char * data_str = data.c_str();
            // send JSON to MQTT
            client.publish(topicRepToSever, data_str);
          }
          client.publish(topicPub, "OK");
        }
      }
    }
  }
  temp = sizeAfter * sizePre;
  IrReceiver.resume();
  // }
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

  // Tách các số thành chuỗi con
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

/*Setting Wifi and MQTT*/
void conectWifi()
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

void connectToMqttBroker()
{
  client.setServer(mqttServer, mqttPort);
  client.setBufferSize(512); 
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



/* giảiMã Tasmota raw data to array uint16_t raw data to send */
std::vector<uint16_t> decode(const std::string& source)
{
    Serial.printf("\nSource: %s", source.c_str());

    std::map<char, uint16_t> map_pair;
    std::vector<uint16_t> raw;

    int16_t prev = 0;
    int16_t currentPos = 0;
    int16_t prev_2 = 0;
    int16_t currentPos_2 = 0;
    // Serial.printf("Source: %s", source);

    char startU = 'A';
    char startL = 'a';

    // Declare delimiters
    const std::string delimeters = "+-";
    const std::string tastomaCode = "qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM";

    // main loop to push sub std::string between two delimiter to <vector> result
    while ((currentPos = source.find_first_of(delimeters, prev)) != std::string::npos) {
        if (currentPos > prev) {
            std::string temp = source.substr(prev, currentPos - prev);

            /* Handle temp std::string, parse tastoma code */
            prev_2 = 0;
            currentPos_2 = 0;
            while ((currentPos_2 = temp.find_first_of(tastomaCode, prev_2)) != std::string::npos) {
                // if currentPos_2 != prev_2, to push (interger raw)
                if (currentPos_2 - prev_2 != 0) {
                    // if integer value --> map pair
                    uint16_t uintRaw = (uint16_t)stoi(temp.substr(prev_2, currentPos_2 - prev_2));
                    // Serial.printf("\n%u", uintRaw);
                    map_pair[startU++] = uintRaw;
                    map_pair[startL++] = uintRaw;
                    raw.push_back(uintRaw);
                }

                // std::string --> const char* --> (char*) type cast --> *(char*) to take char value
                raw.push_back(map_pair[*((char*)temp.substr(currentPos_2, 1).c_str())]); // push tastoma code into vector
                prev_2 = currentPos_2 + 1;
            }
            if (prev_2 < temp.length()) {
                // if integer value --> map pair
                uint16_t uintRaw = (uint16_t)stoi(temp.substr(prev_2, currentPos_2 - prev_2));
                map_pair[startU++] = uintRaw;
                map_pair[startL++] = uintRaw;
                raw.push_back(uintRaw); // push from prev_2 to currentPos to vector
            }
            /***********************/
            // results.push_back(source.substr(prev, currentPos - prev));
        }
        // next move to next delimitter
        prev = currentPos + 1;
    }

    // check if prev < length source --> push last substd::string (from last delimiter to the end of source) to results
    if (prev < source.length()) {
        std::string temp = source.substr(prev);

        /* Handle temp std::string, parse tastoma code */
        prev_2 = 0;
        currentPos_2 = 0;
        while ((currentPos_2 = temp.find_first_of(tastomaCode, prev_2)) != std::string::npos) {
            // if currentPos_2 != prev_2, to push (interger raw)
            if (currentPos_2 - prev_2 != 0) {
                // if integer value --> map pair
                uint16_t uintRaw = (uint16_t)stoi(temp.substr(prev_2, currentPos_2 - prev_2));
                map_pair[startU++] = uintRaw;
                map_pair[startL++] = uintRaw;
                raw.push_back(uintRaw);
            }

            // std::string --> const char* --> (char*) type cast --> *(char*) to take char value
            raw.push_back(map_pair[*((char*)temp.substr(currentPos_2, 1).c_str())]); // push tastoma code into vector
            prev_2 = currentPos_2 + 1;
        }
        if (prev_2 < temp.length()) {
            // if integer value --> map pair
            uint16_t uintRaw = (uint16_t)stoi(temp.substr(prev_2, currentPos_2 - prev_2));
            map_pair[startU++] = uintRaw;
            map_pair[startL++] = uintRaw;
            raw.push_back(uintRaw); // push from prev_2 to currentPos to vector
        }
        /***********************/
    }

    return raw;
}

std::string encodeRaw(uint8_t* rawData, uint8_t rawLength)
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