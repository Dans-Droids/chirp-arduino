/*
    CHIRP version 3.0
    This software is provided by Asio Ltd trading as Chirp

        http://chirp.io

    and is distributed freely and without warranty under the Creative Commons licence CC BY-NC 3.0
  
        https://creativecommons.org/licenses/by-nc/3.0/
  
    Have fun
*/


#ifndef CHIRP_H
#define CHIRP_H

#include "Synth.h"

class Chirp {
protected:
    static const byte CHIRP_STRING_LENGTH = 18;
    static const uint16_t BLOCK_TIME = 5450; // in number of pulses: 87.2ms = 5450 * 16µs
    static const byte DEFAULT_MAX_VOLUME = 255;
    
    static const int MINIMUM_FREE_RAM = 100; // need just enough for stack while playing
    
    static const uint32_t PROGMEM phaseSteps[32];

    static const uint16_t MIN_SUSTAIN_TIME = BLOCK_TIME / 2; // somewhat arbitrary
    static const uint16_t MAX_RAMP_TIME = (BLOCK_TIME - MIN_SUSTAIN_TIME) / 2;
    static const byte DEFAULT_MIN_VOLUME = 128;
    static const uint16_t DEFAULT_RAMP_TIME = 240; // 12ms = 750 * 16µs
    
    uint16_t rampTime;
    uint16_t sustainTime;
    byte minVolume;
    
    uint32_t lastPhaseStep;
    
    byte maxVolume;
    
    uint32_t phaseStepForCharCode(const char charCode);
    
    virtual int16_t numberOfFrames();
    bool enoughSpaceFor(size_t size);
 
    virtual void head(uint32_t phaseStep);
    virtual void append(uint32_t phaseStep);
    virtual void tail(uint32_t phaseStep);
    
public:
    static const byte BAD_CHIRP_CODE_WARNING = 1;
    static const byte CHIRP_STRING_LENGTH_WARNING = 2;
    static const byte TOO_LITTLE_RAM_WARNING = 4;

    Chirp(byte minVolume = DEFAULT_MIN_VOLUME, byte maxVolume = DEFAULT_MAX_VOLUME, uint16_t rampTime = DEFAULT_RAMP_TIME);
    void setVolume(byte volume = DEFAULT_MAX_VOLUME);
    void setParameters(byte minVolume = DEFAULT_MIN_VOLUME, byte maxVolume = DEFAULT_MAX_VOLUME, uint16_t rampTime = DEFAULT_RAMP_TIME);

    
    byte chirp(const char *chirpStr);
    byte chirp(const char *chirpStr, char *enoughSpaceForFrames);
    byte chirp(const char *chirpStr, int16_t nFrames, SynthFrame *frames);
};


#endif
