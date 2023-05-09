#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <IRremote.hpp>
#include "IRReceive.hpp"
#include <ArduinoJson.h>
#include <Wire.h>
#include <iostream>
#include <string>
#include <vector>

#define DISABLE_CODE_FOR_RECEIVER // Disables restarting receiver after each send. Saves 450 bytes program memory and 269 bytes RAM if receiving functions are not used.

// String sTest = "9050,4500,600,500,600,550,600,1650,600,550,600,500,650,500,600,550,600,500,600,1700,600,1650,600,500,650,1650,550,1700,600,1650,600,1650,650,1650,600,500,600,550,600,500,650,1650,600,550,600,500,600,550,600,550,550,1700,600,1650,550,1750,550,550,550,1750,550,1700,550,1700,550,1700,600";
String s = "";
int sizePre;
int sizeAfter;
int temp;
// Thong tin ket noi mang wifi
const char *ssid = "CAM_TEST2";
const char *password = "1234567890";

// Thong tin ket noi toi MQTT broker IP address
const char *mqttServer = "192.168.88.173";
const int mqttPort = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

const char *topicSub = "testPubData";            // MQTT topic sub
const char *topicPub = "testSubRequest";         // MQTT topic pub
const char *topicRepToSever = "testRepToServer"; // MQTT topic pub

#include "PinDefinitionsAndMore.h" // Define macros for input and output pin etc.

struct decode_results getMyStructValue();
// void printData(const uint16_t *data, int size);

void removeCommasEndString(String &string);
std::vector<uint16_t> stringToUint16Array(String &str);
void sendRawData(String &str);
void receiverRawData();
void sendAndReceiverData(String &str);

void conectWifi();
void reconnect();
void connectToMqttBroker();
// void callback();
// void jsonRep(String &command, int &status);

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  IrSender.begin(IR_SEND_PIN); // Start with IR_SEND_PIN as send pin and enable feedback LED at default feedback LED pin
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);

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

void receiverRawData()
{
  String s1 = "";
  while (s1.isEmpty())
  {
    // sendRawData(s);
    if (IrReceiver.decode())
    { // Grab an IR code
      if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_WAS_OVERFLOW)
      {
        Serial.println();
      }
      else
      {
        if (IrReceiver.decodedIRData.protocol == UNKNOWN || IrReceiver.decodedIRData.protocol == RC5)
        {
          // Serial.println(F("Nhận được tin hieu nhieu hoặc giao thức không xác định (hoặc chưa được bật)"));
        }
        else
        {
          Serial.println();
          for (int i = 1; i <= getMyStructValue().rawlen - 1; i++)
          {
            s1 += String(getMyStructValue().rawbuf[i] * MICROS_PER_TICK) + ",";
          }
          sizeAfter = getMyStructValue().rawlen - 1;
          // Serial.println("ADDRESS: " + String(getMyStructValue().address));
          Serial.println("COMMAD: " + IrReceiver.decodedIRData.command);
          Serial.printf("\n[Protocol] %s - [Address] %hu - [Command] %hu\n", ProtocolNames[IrReceiver.decodedIRData.protocol], ProtocolNames[IrReceiver.decodedIRData.address], ProtocolNames[IrReceiver.decodedIRData.command]);
          Serial.printf("\nName of protocol: %s\n", ProtocolNames[IrReceiver.decodedIRData.protocol]);

          // Serial.println("RAW DATA: " + s1);
        }
      }
      removeCommasEndString(s1);
      Serial.println("RAW DATA SAU: " + s1);
    }
    IrReceiver.resume();
  }
}

void sendRawData(String &str)
{
  Serial.println("Phat tin hieu hong ngoai");
  Serial.flush();

  std::vector<uint16_t> myArray = stringToUint16Array(str);

  const uint16_t *rawData = myArray.data();
  int size = myArray.size();
  sizePre = size;

  // printData(rawData, size);

  // Lưu ý cách tiếp cận được sử dụng để tự động tính toán kích thước của mảng.
  // do
  // {
  //   // send raw data taked from server
  //   IrSender.sendRaw(rawData, size, NEC_KHZ);
  //   if (IrReceiver.decode())
  //   {
  //     /** Check overflow */
  //     if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_WAS_OVERFLOW)
  //     {
  //       Serial.println("Overflow");
  //     }
  //     /**  */
  //     else
  //     {
  //       sStoredIRData.receivedIRData = IrReceiver.decodedIRData;
  //       IrReceiver.printIRResultShort(&Serial);
  //       IrReceiver.compensateAndPrintIRResultAsCArray(&Serial, true); // Output the results as uint16_t source code array of micros
  //     }
  //   }
  //   IrReceiver.resume();
  // } while (sStoredIRData.receivedIRData.rawDataPtr->rawlen - 1 != sizeAfter);
}

void sendAndReceiverData(String &str)
{
  String s1 = "";
  String command;

  // while(s1.isEmpty())
  // {
  // Serial.println("Phat tin hieu hong ngoai");
  // Serial.flush();

  std::vector<uint16_t> myArray = stringToUint16Array(str);

  const uint16_t *rawData = myArray.data();
  int size = myArray.size();

  // printData(rawData, size);

  IrSender.sendRaw(rawData, size, NEC_KHZ);
  sizePre = size;
  // sStoredIRData.receivedIRData = IrReceiver.decodedIRData;
  // Serial.println(sStoredIRData.rawCode);

  // Serial.println(.rawcode);

  delay(1000);
  if (IrReceiver.decode())
  { // Lấy mã IR
    if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_WAS_OVERFLOW)
    {
      Serial.println();
    }
    else
    {
      if (IrReceiver.decodedIRData.protocol == UNKNOWN || IrReceiver.decodedIRData.protocol == RC5)
      {
        // Serial.println(F("Nhận được tin hieu nhieu hoặc giao thức không xác định (hoặc chưa được bật)"));
      }
      else
      {
        Serial.println();
        for (int i = 1; i <= getMyStructValue().rawlen - 1; i++)
        {
          s1 += String(getMyStructValue().rawbuf[i] * MICROS_PER_TICK) + ",";
        }
        sizeAfter = int(getMyStructValue().rawlen - 1);
        // Serial.println("ADDRESS: " + String(getMyStructValue().address));
        Serial.println("COMMAD: " + IrReceiver.decodedIRData.command);
        Serial.printf("\n[Protocol] %s - [Address] %hu - [Command] %hu\n", ProtocolNames[IrReceiver.decodedIRData.protocol], ProtocolNames[IrReceiver.decodedIRData.address], ProtocolNames[IrReceiver.decodedIRData.command]);
        Serial.printf("\nName of protocol: %s\n", ProtocolNames[IrReceiver.decodedIRData.protocol]);
        command = getMyStructValue().value;
        // Serial.println("RAW DATA: " + s1);
        int status;

        if (sizeAfter != 0 || sizePre != 0)
        {
          if (sizeAfter == sizePre)
          {
            status = 1;
            StaticJsonDocument<200> doc;
            doc["command"] = command;
            doc["status"] = status;

            // Chuyển JSON object sang chuỗi JSON
            char jsonBuffer[512];
            serializeJson(doc, jsonBuffer);

            // Gửi chuỗi JSON lên MQTT
            client.publish(topicRepToSever, jsonBuffer);
          }
          else
          {
            status = 0;
            StaticJsonDocument<200> doc;
            doc["command"] = command;
            doc["status"] = status;

            // Chuyển JSON object sang chuỗi JSON
            char jsonBuffer[512];
            serializeJson(doc, jsonBuffer);

            // Gửi chuỗi JSON lên MQTT
            client.publish(topicRepToSever, jsonBuffer);
          }
          client.publish(topicPub, "OK");
        }
      }
    }
    // removeCommasEndString(s1);
    // Serial.println("RAW DATA SAU: " + s1);
    // Serial.println(IrReceiver.read());
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
  // Xử lý số cuối cùng
  if (!temp.empty())
  {
    uint16_t num = std::stoi(temp);
    arr.push_back(num);
  }

  return arr;
}

/*Cac ham xu ly Data*/

// struct tao value ir
struct decode_results getMyStructValue()
{
  struct decode_results myStruct; // Tạo một đối tượng struct MyStruct

  // Gán giá trị cho các trường của struct
  myStruct.rawbuf = IrReceiver.decodedIRData.rawDataPtr->rawbuf;
  myStruct.rawlen = IrReceiver.decodedIRData.rawDataPtr->rawlen;
  myStruct.address = IrReceiver.decodedIRData.address;
  myStruct.value = IrReceiver.decodedIRData.command;
  // myStruct.name = IrReceiver.decodedIRData.protocol;

  // Trả về giá trị của struct
  return myStruct;
}
void printData(const uint16_t *data, int size)
{
  for (int i = 0; i < size; i++)
  {
    std::cout << data[i] << " ";
  }
  std::cout << std::endl;
}

/*Cac ham xu ly MQTT*/

// Ket noi toi wwifi
void conectWifi()
{
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Dang ket noi voi WiFi...");
  }
  Serial.println("Ket noi voi wifi thanh cong!");
}

// Ham callback MQTT
void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message đã nhận: ");
  String dataReceived = "";
  for (int i = 0; i < length; i++)
  {
    // Serial.print((char)payload[i]);
    dataReceived += String((char)payload[i]);
  }
  // Serial.println(dataReceived);
  Serial.println("NHAN++++++++");
  s = dataReceived;

  do
  {
    sendAndReceiverData(s);
  } while (temp == 0);
  s = "";
  sizeAfter = 0;
  sizePre = 0;
}

// Ket noi den MQTT
void connectToMqttBroker()
{

  // Thiết lập thông tin về MQTT broker
  client.setServer(mqttServer, mqttPort);
  client.setBufferSize(512); 
  client.setCallback(callback);
  // Kết nối đến MQTT broker
  if (!client.connected())
  { // Kiểm tra kết nối
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
      delay(5000);
    }
  }
}
