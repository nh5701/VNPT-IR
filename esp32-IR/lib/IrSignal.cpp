#include "IrSignal.h"
#include "Arduino.h"
#include "PinDefinitionsAndMore.h"

void Ir_send::Ir_start(){
    pinMode(IR_SEND_PIN, OUTPUT);
}

void Ir_receive::Ir_start(){
    pinMode(IR_RECEIVE_PIN, INPUT);
}

void Ir_send::sendRawData(const uint16_t aBufferWithMicroseconds[], uint_fast16_t aLengthOfBuffer, uint_fast8_t aIRFrequencyKilohertz) {
// Set IR carrier frequency
    enableIROut(aIRFrequencyKilohertz);
    for (uint_fast16_t i = 0; i < aLengthOfBuffer; i++) {
        if (i & 1) {
            // Odd
            space(aBufferWithMicroseconds[i]);
        } else {
            mark(aBufferWithMicroseconds[i]);
        }
    }
}

void Ir_send::enableIROut(uint_fast8_t aFrequencyKHz) {
#if defined(SEND_PWM_BY_TIMER)
    timerConfigForSend(aFrequencyKHz); 
#endif
}

void Ir_send::customDelayMicroseconds(unsigned long aMicroseconds) {
    if (aMicroseconds > 20000) {
        delay(aMicroseconds / 1000UL);
        delayMicroseconds(static_cast<uint16_t>(aMicroseconds % 1000UL));
    } else {
        unsigned long start = micros();
        while (micros() - start < aMicroseconds) {
        }
    }
}

void Ir_send::space(uint16_t aSpaceMicros) {
    customDelayMicroseconds(aSpaceMicros);
}

void Ir_send::mark(uint16_t aMarkMicros) {

#if defined(SEND_PWM_BY_TIMER) || defined(USE_NO_SEND_PWM)
#  if !defined(NO_LED_FEEDBACK_CODE)
    if (FeedbackLEDControl.LedFeedbackEnabled == LED_FEEDBACK_ENABLED_FOR_SEND) {
        setFeedbackLED(true);
    }
#  endif
#endif

#if defined(SEND_PWM_BY_TIMER)
    /*
     * Generate hardware PWM signal
     */
    enableSendPWMByTimer(); // Enable timer or ledcWrite() generated PWM output
    customDelayMicroseconds(aMarkMicros);
    IRLedOff();// disables hardware PWM and manages feedback LED
    return;
#  endif
}