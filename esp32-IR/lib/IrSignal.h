#ifndef IRSEND_H_
#define IRSEND_H_

#define __STDC_LIMIT_MACROS
#include <stdint.h>
#include "IRremoteESP8266.h"

#if TEST || UNIT_TEST
#define VIRTUAL virtual
#else
#define VIRTUAL
#endif

#endif

#define PULSE_CORRECTION_NANOS (48000L / (F_CPU/MICROS_IN_ONE_SECOND)) // 3000 @16MHz, 666 @72MHz

class Ir_send {
    public: 
        void Ir_start ();
        void sendRawData(const uint16_t aBufferWithMicroseconds[], uint_fast16_t aLengthOfBuffer, uint_fast8_t aIRFrequencyKilohertz);
        void enableIROut(uint_fast8_t aFrequencyKHz);
        void customDelayMicroseconds(unsigned long aMicroseconds);
        void mark(uint16_t aMarkMicros);
        void space(uint16_t aSpaceMicros);
};

class Ir_receive {
    public: 
        void Ir_start ();
};