#if defined(ESP32)
#include <Arduino.h>

// tone() được bao gồm trong lõi ESP32 vì 2.0.2
#if !defined(ESP_ARDUINO_VERSION_VAL)
#define ESP_ARDUINO_VERSION_VAL(major, minor, patch) 12345678
#endif
#if ESP_ARDUINO_VERSION  <= ESP_ARDUINO_VERSION_VAL(2, 0, 2)
#define TONE_LEDC_CHANNEL        1  // Using channel 1 makes tone() independent of receiving timer -> No need to stop receiving timer.
void tone(uint8_t aPinNumber, unsigned int aFrequency){
    ledcAttachPin(aPinNumber, TONE_LEDC_CHANNEL);
    ledcWriteTone(TONE_LEDC_CHANNEL, aFrequency);
}
void tone(uint8_t aPinNumber, unsigned int aFrequency, unsigned long aDuration){
    ledcAttachPin(aPinNumber, TONE_LEDC_CHANNEL);
    ledcWriteTone(TONE_LEDC_CHANNEL, aFrequency);
    delay(aDuration);
    ledcWriteTone(TONE_LEDC_CHANNEL, 0);
}
void noTone(uint8_t aPinNumber){
    ledcWriteTone(TONE_LEDC_CHANNEL, 0);
}
#endif // ESP_ARDUINO_VERSION  <= ESP_ARDUINO_VERSION_VAL(2, 0, 2)

#define IR_RECEIVE_PIN          15  // D15
#define IR_SEND_PIN              4  // D4
#define TONE_PIN                27  // D27 25 & 26 are DAC0 and 1
#define APPLICATION_PIN         16  // RX2 pin

#endif // defined(ESP8266)

#if defined(ESP32) || defined(ARDUINO_ARCH_RP2040) || defined(PARTICLE) || defined(ARDUINO_ARCH_MBED)
#define SEND_PWM_BY_TIMER // Chúng tôi không có giới hạn PIN cho CPU này, vì vậy hãy sử dụng PWM phần cứng để gửi tín hiệu nhà cung cấp
#else
# if defined(SEND_PWM_BY_TIMER)
#undef IR_SEND_PIN // SendPin is determined by timer! This avoids warning in IRTimer.hpp
#  endif
#endif

#if !defined (FLASHEND)
#define FLASHEND 0xFFFF // Giá trị giả cho các nền tảng nơi Flashend không được xác định
#endif
#if !defined (RAMEND)
#define RAMEND 0xFFFF //Giá trị giả cho các nền tảng nơi Ramend không được xác định
#endif
#if !defined (RAMSIZE)
#define RAMSIZE 0xFFFF // Giá trị giả cho các nền tảng nơi RamSize không được xác định
#endif

/*
 * Macro trợ giúp để nhận định nghĩa macro dưới dạng chuỗi
 */
#if !defined(STR_HELPER)
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#endif
