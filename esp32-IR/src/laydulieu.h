// // //CODE NHAN:
// #include <Arduino.h>

// #include "PinDefinitionsAndMore.h" // Define macros for input and output pin etc.

// // #if !defined(RAW_BUFFER_LENGTH)
// // #  if RAMEND <= 0x4FF || RAMSIZE < 0x4FF
// // #define RAW_BUFFER_LENGTH  180  // 750 (600 if we have only 2k RAM) is the value for air condition remotes. Default is 112 if DECODE_MAGIQUEST is enabled, otherwise 100.
// // #  elif RAMEND <= 0x8FF || RAMSIZE < 0x8FF
// // #define RAW_BUFFER_LENGTH  600  // 750 (600 if we have only 2k RAM) is the value for air condition remotes. Default is 112 if DECODE_MAGIQUEST is enabled, otherwise 100.
// // #  else
// // #define RAW_BUFFER_LENGTH  750  // 750 (600 if we have only 2k RAM) is the value for air condition remotes. Default is 112 if DECODE_MAGIQUEST is enabled, otherwise 100.
// // #  endif
// // #endif

// #define MARK_EXCESS_MICROS    20    // Adapt it to your IR receiver module. 20 is recommended for the cheap VS1838 modules.
// #define MICROS_PER_TICK    50L

// #include <IRremote.h>

// #include <IRremote.hpp>
// // // #include<conio.h>
// // // #include<stdio.h>
// #include<string.h>
// // // #include <iostream>
// // // using namespace std;

// struct decode_results getMyStructValue() {
//     struct decode_results myStruct; // Tạo một đối tượng struct MyStruct

//     // Gán giá trị cho các trường của struct
//     myStruct.rawbuf = IrReceiver.decodedIRData.rawDataPtr->rawbuf;
//     myStruct.rawlen = IrReceiver.decodedIRData.rawDataPtr->rawlen;
//     myStruct.address = IrReceiver.decodedIRData.address;
//     myStruct.value = IrReceiver.decodedIRData.command;

//     // Trả về giá trị của struct
//     return myStruct;
// }

// void setup() {
//     pinMode(LED_BUILTIN, OUTPUT);

//     Serial.begin(115200);   // Status message will be sent to PC at 9600 baud
// #if defined(__AVR_ATmega32U4__) || defined(SERIAL_PORT_USBVIRTUAL) || defined(SERIAL_USB) /*stm32duino*/|| defined(USBCON) /*STM32_stm32*/|| defined(SERIALUSB_PID) || defined(ARDUINO_attiny3217)
//     delay(4000); // To be able to connect Serial monitor after reset or power up and before first print out. Do not wait for an attached Serial Monitor!
// #endif
//     // Chỉ để biết chương trình nào đang chạy trên Arduino của tôi
//     // Serial.println(F("START " __FILE__ " from " __DATE__ "\r\nUsing library version " VERSION_IRREMOTE));

//     // Bắt đầu máy thu và nếu không 3. Tham số được chỉ định, hãy lấy mã PIN LED_BUILTIN từ định nghĩa bảng bên trong làm đèn LED phản hồi mặc định
//     IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);

//     Serial.print(F("Ready to receive IR signals of protocols: "));
//     printActiveIRProtocols(&Serial);
//     Serial.println(F("at pin " STR(IR_RECEIVE_PIN)));

//     // infos for receive
//     Serial.print(RECORD_GAP_MICROS);
//     Serial.println(F(" us is the (minimum) gap, after which the start of a new IR packet is assumed"));
//     Serial.print(MARK_EXCESS_MICROS);
//     Serial.println();
//     Serial.println(F("Because of the verbose output (>200 ms at 115200), repeats are probably not dumped correctly!"));
//     Serial.println();
// }

// //+=============================================================================
// // The repeating section of the code
// //
// void loop() {

//     if (IrReceiver.decode()) {  // Grab an IR code
//         // At 115200 baud, printing takes 200 ms for NEC protocol and 70 ms for NEC repeat
//         Serial.println(); // blank line between entries
//         Serial.println(); // 2 blank lines between entries
//         IrReceiver.printIRResultShort(&Serial);
//         // Check if the buffer overflowed
//         if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_WAS_OVERFLOW) {
//             Serial.println(F("Try to increase the \"RAW_BUFFER_LENGTH\" value of " STR(RAW_BUFFER_LENGTH) " in " __FILE__));

//         } else {
//             String s = "";
//             String s1 = "";

//             if (IrReceiver.decodedIRData.protocol == UNKNOWN) {
//                 Serial.println(F("Unknown protocol"));
//             } else
//             {
//             Serial.println();
//             IrReceiver.printIRSendUsage(&Serial);
//             Serial.println();
//             Serial.println(F("Raw result in internal ticks (50 us) - with leading gap"));
//             IrReceiver.printIRResultRawFormatted(&Serial, false); // Output the results in RAW format
//             Serial.println(F("Raw result in microseconds - with leading gap"));
//             IrReceiver.printIRResultRawFormatted(&Serial, true);  // Output the results in RAW format
//             Serial.println();                               // blank line between entries
//             Serial.print(F("Result as internal 8bit ticks (50 us) array - compensated with MARK_EXCESS_MICROS="));
//             Serial.println(MARK_EXCESS_MICROS);
//             IrReceiver.compensateAndPrintIRResultAsCArray(&Serial, false); // Output the results as uint8_t source code array of ticks
//             Serial.print(F("Result as microseconds array - compensated with MARK_EXCESS_MICROS="));
//             Serial.println(MARK_EXCESS_MICROS);
//             IrReceiver.compensateAndPrintIRResultAsCArray(&Serial, true); // Output the results as uint16_t source code array of micros
//             IrReceiver.printIRResultAsCVariables(&Serial);  // Output address and data as source code variables

//             IrReceiver.compensateAndPrintIRResultAsPronto(&Serial);
//             Serial.println(getMyStructValue().rawlen);
//             Serial.println(getMyStructValue().rawlen - 1);

//             Serial.println(getMyStructValue().address);
//             Serial.println(getMyStructValue().value);
//             Serial.print("0x");
//             Serial.println(getMyStructValue().address, HEX);
//             Serial.print("0x");
//             Serial.println(getMyStructValue().value, HEX);
//             Serial.println("$$$$$$$$$$$$$$$");

//             Serial.println(getMyStructValue().rawbuf[0]* MICROS_PER_TICK, HEX);
//             Serial.println("+++++++++++++++++++++++++++++++++++++++++++++++++++");
//                 for (int i = 1; i <= getMyStructValue().rawlen; i++)
//                 {
//                     s += String(getMyStructValue().rawbuf[i]* MICROS_PER_TICK),HEX + ",";
//                 }

//                 for (int i = 1; i <= getMyStructValue().rawlen -1; i++)
//                 {
//                     s1 += String(getMyStructValue().rawbuf[i]* MICROS_PER_TICK) + ",";
//                 }

//             int j = 0;
//             // s.pop_back();
//             // int n = strlen(s);
//             while(s1[j] != '\0') {
//                 j++;
//             }
//             s1[j-1] ='\0';
//             int k = 0;
//             // s.pop_back();
//             // int n = strlen(s);
//             while(s[k] != '\0') {
//                 k++;
//             }
//             s1[k-1] ='\0';
//             // s.pop_back();
//             // s.erase(s.size() - 1);
//             // s = s.c_str(0, s.size() - 1);
//             Serial.println("RAW DATA : " + s);
//             Serial.println("RAW DATA : " + s1);
//             delay(100);

//             }

//         }
//         IrReceiver.resume();
//     }
// }

// // //++++++++++++++++++++===================++++++++++++++++++++++++=================
// // //CODE NHAN
// // /*
// //  * SendRawDemo.cpp - demonstrates sending IR codes with sendRaw
// //  *
// //  * This example shows how to send a RAW signal using the IRremote library.
// //  * The example signal is actually a 32 bit NEC signal.
// //  * Remote Control button: LGTV Power On/Off.
// //  * Hex Value: 0x20DF10EF, 32 bits
// //  *
// //  * If it is a supported protocol, it is more efficient to use the protocol send function
// //  * (here sendNEC) to send the signal.
// //  *
// //  *  This file is part of Arduino-IRremote https://github.com/Arduino-IRremote/Arduino-IRremote.
// //  *
// //  ************************************************************************************
// //  * MIT License
// //  *
// //  * Copyright (c) 2020-2022 Armin Joachimsmeyer
// //  *
// //  * Permission is hereby granted, free of charge, to any person obtaining a copy
// //  * of this software and associated documentation files (the "Software"), to deal
// //  * in the Software without restriction, including without limitation the rights
// //  * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// //  * copies of the Software, and to permit persons to whom the Software is furnished
// //  * to do so, subject to the following conditions:
// //  *
// //  * The above copyright notice and this permission notice shall be included in all
// //  * copies or substantial portions of the Software.
// //  *
// //  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
// //  * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// //  * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// //  * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
// //  * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
// //  * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// //  *
// //  ************************************************************************************
// //  */
// // #include <Arduino.h>

// // #define DISABLE_CODE_FOR_RECEIVER // Disables restarting receiver after each send. Saves 450 bytes program memory and 269 bytes RAM if receiving functions are not used.

// // // #define SEND_PWM_BY_TIMER         // Vô hiệu hóa tạo PWM PWM trong phần mềm và sử dụng (hạn chế) PWM.
// // // #xác định use_no_send_pwm // Không sử dụng PWM của nhà cung cấp, chỉ cần mô phỏng tín hiệu máy thu thấp hoạt động.Ghi đè định nghĩa SEND_PWM_BY_TIMER
// // // #xác định NO_LED_FEEDBACK_CODE // Lưu bộ nhớ chương trình 566 byte
// // // #Xác định User_open_drain_output_for_send_pin // Sử dụng hoặc mô phỏng chế độ đầu ra thoát nước mở tại gửi pin.Chú ý, trạng thái hoạt động của cống mở là thấp, vì vậy hãy kết nối đèn LED gửi giữa nguồn cung cấp dương và gửi PIN!

// // #include "PinDefinitionsAndMore.h" // Define macros for input and output pin etc.
// // #include <IRremote.hpp>

// // void setup()
// // {
// //   pinMode(LED_BUILTIN, OUTPUT);

// //   Serial.begin(115200);
// // #if defined(__AVR_ATmega32U4__) || defined(SERIAL_PORT_USBVIRTUAL) || defined(SERIAL_USB) /*stm32duino*/ || defined(USBCON) /*STM32_stm32*/ || defined(SERIALUSB_PID) || defined(ARDUINO_attiny3217)
// //   delay(4000); // To be able to connect Serial monitor after reset or power up and before first print out. Do not wait for an attached Serial Monitor!
// // #endif
// //   // Just to know which program is running on my Arduino
// //   Serial.println(F("START " __FILE__ " from " __DATE__ "\r\nUsing library version " VERSION_IRREMOTE));
// //   Serial.println(F("Send IR signals at pin " STR(IR_SEND_PIN)));

// //   IrSender.begin(); // Start with IR_SEND_PIN as send pin and enable feedback LED at default feedback LED pin
// // }

// // /*
// //  * NEC address=0xFB0C, command=0x18
// //  *
// //  * This is data in byte format.
// //  * The uint8_t/byte elements contain the number of ticks in 50 us.
// //  * The uint16_t format contains the (number of ticks * 50) if generated by IRremote,
// //  * so the uint16_t format has exact the same resolution but requires double space.
// //  * With the uint16_t format, you are able to modify the timings to meet the standards,
// //  * e.g. use 560 (instead of 11 * 50) for NEC or use 432 for Panasonic. But in this cases,
// //  * you better use the timing generation functions e.g. sendNEC() directly.
// //  */
// // // const uint8_t rawDataP[]
// // // #if defined(__AVR__)
// // // PROGMEM
// // // #endif
// // // = { 180, 90 /*Start bit*/, 11, 11, 11, 11, 11, 34, 11, 34/*0011 0xC of 16 bit address LSB first*/, 11, 11, 11, 11, 11, 11, 11,
// // //         11/*0000*/, 11, 34, 11, 34, 11, 11, 11, 34/*1101 0xB*/, 11, 34, 11, 34, 11, 34, 11, 34/*1111*/, 11, 11, 11, 11, 11, 11, 11,
// // //         34/*0001 0x08 of command LSB first*/, 11, 34, 11, 11, 11, 11, 11, 11/*1000 0x01*/, 11, 34, 11, 34, 11, 34, 11,
// // //         11/*1110 Inverted 8 of command*/, 11, 11, 11, 34, 11, 34, 11, 34/*0111 inverted 1 of command*/, 11 /*stop bit*/};

// // void loop()
// // {

// //   // #if !(defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__))
// //   /*
// //    * Send hand crafted data from RAM
// //    * The values are NOT multiple of 50, but are taken from the NEC timing definitions
// //    */
// //   Serial.println(F("Gửi NEC 16 bit địa chỉ = 0xFB04 và lệnh 0x08 với thời gian chính xác (định dạng mảng 16 bit)"));
// //   Serial.flush();

// //   // const uint16_t rawData[] = { 9000, 4500/*Start bit*/, 560, 560, 560, 560, 560, 1690, 560,
// //   //         560/*0010 0x4 of 16 bit address LSB first*/, 560, 560, 560, 560, 560, 560, 560, 560/*0000*/, 560, 1690, 560, 1690, 560,
// //   //         560, 560, 1690/*1101 0xB*/, 560, 1690, 560, 1690, 560, 1690, 560, 1690/*1111*/, 560, 560, 560, 560, 560, 560, 560,
// //   //         1690/*0001 0x08 of command LSB first*/, 560, 560, 560, 560, 560, 560, 560, 560/*0000 0x00*/, 560, 1690, 560, 1690, 560,
// //   //         1690, 560, 560/*1110 Inverted 8 of command*/, 560, 1690, 560, 1690, 560, 1690, 560, 1690/*1111 inverted 0 of command*/,
// //   //         560 /*stop bit*/}; // Using exact NEC timing
// //   // const uint16_t rawData[] =
// //   //         {2400,600,1200,600,600,600,600,600,1200,600,600,600,600,600,550,650,1200,600,600,600,600,600,600,600,600};

// //   const uint16_t rawData[] =
// //       {3980,3920, 530,1970, 530,1970, 530,1970, 530,1920, 580,920, 530,970, 480,2020, 480,1020, 480,1970, 480,1020, 480,2020, 480,1020, 480,1020, 480,1020, 430,1020, 480,1020, 480,2020, 430,2020, 480,1070, 430,2020, 480,1020, 430,2070, 430,1020, 480,2020, 480};

// //   IrSender.sendRaw(rawData, sizeof(rawData) / sizeof(rawData[0]), 40); // Lưu ý cách tiếp cận được sử dụng để tự động tính toán kích thước của mảng.

// //   delay(5000); // delay must be greater than 5 ms (RECORD_GAP_MICROS), otherwise the receiver sees it as one long signal
// //   // #endif

// //   //     /*
// //   //      * Gửi dữ liệu byte trực tiếp từ flash
// //   //      * Lưu ý cách tiếp cận được sử dụng để tự động tính toán kích thước của mảng.
// //   //      */
// //   //     Serial.println(F("Gửi địa chỉ NEC 16 bit 0xfb0c và dữ liệu 0x18 với (50 US) Thời gian giải quyết đánh dấu (định dạng mảng 8 bit) "));
// //   //     Serial.flush();
// //   //     // IrSender.sendRaw_P(rawDataP, sizeof(rawDataP) / sizeof(rawDataP[0]), NEC_KHZ);

// //   //     // delay(1000); // delay must be greater than 5 ms (RECORD_GAP_MICROS), otherwise the receiver sees it as one long signal

// //   //     Serial.println(F("Send NEC 16 bit address 0x0102, 8 bit data 0x34 with generated timing"));
// //   //     Serial.flush();
// //   //     IrSender.sendNEC(0x0102, 0x34, 0);

// //   //     delay(5000);
// // }

// // MQTT-ESP=================================================
// // #include <Arduino.h>
// // #include <ArduinoJson.h>
// // #include <WiFi.h>
// // #include <PubSubClient.h>
// // #include <Wire.h>
// // #include <iostream>
// // #include <stdio.h>

// // const char* ssid = "CAM_TEST2";
// // const char* password =  "1234567890";
// // const char* mqttServer = "192.168.88.173";
// // const int mqttPort = 1883;
// // #define topic "dataIR"

// // WiFiClient espClient;
// // PubSubClient client(espClient);

// // const uint16_t rawData[] =
// //        {3980,3920, 530,1970, 530,1970, 530,1970, 530,1920, 580,920, 530,970, 480,2020, 480,1020, 480,1970, 480,1020, 480,2020, 480,1020, 480,1020, 480,1020, 430,1020, 480,1020, 480,2020, 430,2020, 480,1070, 430,2020, 480,1020, 430,2070, 430,1020, 480,2020, 480};

// // void setup() {

// //   Serial.begin(115200);
// //   Serial.println();

// //   WiFi.begin(ssid, password);

// //   while (WiFi.status() != WL_CONNECTED) {
// //     delay(500);
// //     Serial.println("Connecting to WiFi..");
// //   }

// //   Serial.println("Connected to the WiFi network");

// //   client.setServer(mqttServer, mqttPort);

// //   while (!client.connected()) {
// //     Serial.println("Connecting to MQTT...");

// //     if (client.connect("ESP32Client")) {

// //       Serial.println("connected");
// //       client.subscribe(topic);
// //     } else {

// //       Serial.print("failed with state ");
// //       Serial.print(client.state());
// //       delay(2000);

// //     }
// //   }

// // }

// // void checkDataJson() {

// // }

// // void dataJson() {
// //   int length = sizeof(rawData)/ sizeof(uint16_t);
// //   Serial.println(sizeof(rawData)/ sizeof(uint16_t));
// //   StaticJsonDocument<2000> doc;
// //   JsonArray array = doc.createNestedArray("rawData");
// //   Serial.println(sizeof(array));
// //   // Serial.println(typeof(length));

// //   for (int i = 0; i < length; i++) {
// //     array.add(rawData[i]);

// //   }
// //     char jsonBuffer[2000];
// //     serializeJson(doc, jsonBuffer);
// //     client.publish(topic, jsonBuffer);
// // }

// // void loop() {
// //   // dataJson();
// //   client.loop();
// //   delay(5000);
// // }
// //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// // #include <WiFi.h>
// // #include <PubSubClient.h>
// // #include <IRremote.h>

// // const char* ssid = "CAM_TEST2";
// // const char* password = "1234567890";

// // /** Define Macros */
// // #define MQTT_SERVER "192.168.88.173"
// // #define MQTT_PORT 1883
// // #define SUB_TOPIC "control"
// // #define PUB_TOPIC "data"

// // #define LED_SENSE 2   // blink when sense IR coming
// // #define LED_START 23  // ON to notify start receive IR signal

// // #define IR_RECEIVE_PIN 15

// // #define MARK_EXCESS_MICROS  20
// // #define MICROS_PER_TICK     50L // must be with L to get 32 bit results if multiplied with rawbuf[] content.

// // WiFiClient wifiClient;
// // PubSubClient client(wifiClient);
// // String data = "";

// // struct storedIRDataStruct {
// //     IRData receivedIRData;
// //     // extensions for sendRaw
// //     uint8_t rawCode[RAW_BUFFER_LENGTH]; // The durations if raw
// //     uint8_t rawCodeLength; // The length of the code
// // } sStoredIRData;

// // void setup_wifi();
// // void connect_to_broker();
// // void callback(char* topic, byte *payload, unsigned int length);
// // String readIR();

// // void setup() {
// //   Serial.begin(115200);
// //   Serial.setTimeout(500);

// //   pinMode(LED_SENSE, OUTPUT);
// //   pinMode(LED_START, OUTPUT);
// //   IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);

// //   setup_wifi();
// //   client.setServer(MQTT_SERVER, MQTT_PORT);
// //   client.setCallback(callback);
// //   connect_to_broker();
// // }

// // void loop() {
// //   client.loop();
// //   if (!client.connected()) {
// //     connect_to_broker();
// //   }
// // }

// // /**
// //     Connect wifi
// // */
// // void setup_wifi() {
// //   Serial.print("Connecting to ");
// //   Serial.println(ssid);
// //   WiFi.begin(ssid, password);
// //   while (WiFi.status() != WL_CONNECTED) {
// //     delay(500);
// //     Serial.print(".");
// //   }
// //   Serial.println("");
// //   Serial.println("WiFi connected");
// //   Serial.println("IP address: ");
// //   Serial.println(WiFi.localIP());
// // }

// // /**
// //   ===== Connect & Subcribe =====
// // */
// // void connect_to_broker() {
// //   while (!client.connected()) {
// //     Serial.print("Attempting MQTT connection...");
// //     String clientId = "slave";

// //     if (client.connect(clientId.c_str())) {
// //       Serial.println("connected");
// //       client.subscribe(SUB_TOPIC);
// //     }

// //     else {
// //       Serial.print("failed, rc=");
// //       Serial.print(client.state());
// //       Serial.println(" try again in 3 seconds");
// //       delay(3000);
// //     }
// //   }
// // }

// // /**
// //   ===== Callback func when SUB_TOPIC have value =====
// // */
// // void callback(char* topic, byte *payload, unsigned int length) {
// //   Serial.println();
// //   Serial.println();
// //   Serial.println();
// //   Serial.println("-------new message from broker-----");
// //   Serial.print("topic: ");
// //   Serial.println(topic);
// //   Serial.print("message: ");
// //   Serial.write(payload, length);

// //   // if have command from server
// //   if(payload)
// //   {
// //     // turn on LED to notify start to listen signal
// //     digitalWrite(LED_START, 1);

// //     String data = readIR(); // cut "," in tail of packet
// //     Serial.println(data);

// //     // char dataChar[data.length() + 1];
// //     // data.toCharArray(dataChar, sizeof(dataChar));

// //       Serial.println("Success receive rawData !!!");

// //       client.publish(PUB_TOPIC, data.c_str());
// //       Serial.println("Publish to server is completed ---");
// //       digitalWrite(LED_START, 0);
// //     }

// //   }

// // /**
// //   ===== READ IR =====
// // */
// // String readIR(){
// //   String temp = "";
// //   /* While loop until temp take raw Data */
// //   while (temp.isEmpty()){
// //     if (IrReceiver.decode()) {
// //           /** Check overflow */
// //           if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_WAS_OVERFLOW) {
// //               Serial.println("Overflow");
// //           }
// //           /**  */
// //           else {
// //               sStoredIRData.receivedIRData = IrReceiver.decodedIRData;

// //               if (sStoredIRData.receivedIRData.protocol == UNKNOWN) {
// //                 Serial.println("Unknown !");
// //               } else {
// //                 sStoredIRData.rawCodeLength = IrReceiver.decodedIRData.rawDataPtr->rawlen - 1;
// //                 /*
// //                 * Store the current raw data in a dedicated array for later usage
// //                 */
// //                 IrReceiver.compensateAndStoreIRResultInArray(sStoredIRData.rawCode);
// //                 Serial.println("Length: " + sStoredIRData.rawCodeLength);
// //                 for (int i = 0 ; i < sStoredIRData.rawCodeLength; i++){
// //                   temp += String(sStoredIRData.rawCode[i] * MICROS_PER_TICK) + ",";
// //                 }
// //               }
// //           }
// //       }
// //     IrReceiver.resume();
// //   }
// //   Serial.println(ProtocolNames[IrReceiver.decodedIRData.protocol]);
// //   return temp.substring(0, temp.length() - 1);
// // }
// code:+++++++++++=====================
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

// String s = "9050,4500,600,500,600,550,600,1650,600,550,600,500,650,500,600,550,600,500,600,1700,600,1650,600,500,650,1650,550,1700,600,1650,600,1650,650,1650,600,500,600,550,600,500,650,1650,600,550,600,500,600,550,600,550,550,1700,600,1650,550,1750,550,550,550,1750,550,1700,550,1700,550,1700,600";
String s = "";
int sizePre;
// Thong tin ket noi mang wifi
const char *ssid = "CAM_TEST2";
const char *password = "1234567890";

// Thong tin ket noi toi MQTT broker IP address
const char *mqttServer = "192.168.88.173";
const int mqttPort = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

const char *topicSub = "testPubData";     // MQTT topic dang ky
const char *topicPub = "testSubRequest1"; // MQTT topic

#include "PinDefinitionsAndMore.h" // Define macros for input and output pin etc.

struct decode_results getMyStructValue();
// void printData(const uint16_t *data, int size);

void removeCommasEndString(String &string);
std::vector<uint16_t> stringToUint16Array(String &str);
void sendRawData(String &str);
void receiverRawData();

void conectWifi();
void connectToMqttBroker();

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
  int sizeAfter;
  client.loop();
  receiverRawData();
  s = "";
}

void receiverRawData()
{
  String s1 = "";
  if (IrReceiver.decode())
  { // Grab an IR code
    if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_WAS_OVERFLOW)
    {
      Serial.println();
    }
    else
    {
      if (IrReceiver.decodedIRData.protocol == UNKNOWN)
      {
        Serial.println(F("Nhận được tin hieu nhieu hoặc giao thức không xác định (hoặc chưa được bật)"));
      }
      else
      {
        Serial.println();
        for (int i = 1; i <= getMyStructValue().rawlen - 1; i++)
        {
          s1 += String(getMyStructValue().rawbuf[i] * MICROS_PER_TICK) + ",";
        }
        // sizeA
        // Serial.println("ADDRESS: " + String(getMyStructValue().address));
        // Serial.println("ADDRESS: " + String(getMyStructValue().value));
        Serial.printf("\n[Protocol] %s - [Address] %hu - [Command] %hu\n", ProtocolNames[IrReceiver.decodedIRData.protocol], ProtocolNames[IrReceiver.decodedIRData.address], ProtocolNames[IrReceiver.decodedIRData.command]);
        Serial.printf("\nName of protocol: %s\n", ProtocolNames[IrReceiver.decodedIRData.protocol]);

        Serial.println("RAW DATA: " + s1);
      }
    }
    removeCommasEndString(s1);
    // Serial.println("RAW DATA SAU: " + s1);
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

  IrSender.sendRaw(rawData, size, NEC_KHZ); // Lưu ý cách tiếp cận được sử dụng để tự động tính toán kích thước của mảng.
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

void convertStringToUint16(String &string_to_uint_16)
{
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
  Serial.println(dataReceived);
  Serial.println("NHAN++++++++");
  s = dataReceived;
  sendRawData(s);
  // delay(1000);
}

// Ket noi den MQTT
void connectToMqttBroker()
{

  // Thiết lập thông tin về MQTT broker
  client.setServer(mqttServer, mqttPort);
  client.setBufferSize(512); //
  client.setCallback(callback);
  // Kết nối đến MQTT broker
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

// Tao doi tuong JSON
void jsonTrue()
{
  // Tạo đối tượng JSON
  StaticJsonDocument<200> doc;
  doc["name"] = "John";
  doc["age"] = 30;
  doc["city"] = "New York";
}