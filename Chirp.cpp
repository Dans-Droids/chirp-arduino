/*
    CHIRPINO SING version 3.0
    This software is provided by Asio Ltd trading as Chirp

        http://chirp.io

    and is distributed freely and without warranty under the Creative Commons licence CC BY-NC 3.0
  
        https://creativecommons.org/licenses/by-nc/3.0/
  
    Have fun
*/

#include <alloca.h>
#include "Chirp.h"


/*
// Storing precalculated phaseSteps avoids load of bulky floating point library

// 512 samples in one period of sine wave table
// one pulse emitted every 256 clock cycles, clock runs at 16000000Hz
// so step to advance = freq / (16000000 / (512 * 256)) = freq * 128 / 15625
// phase steps are stored as fixed point binary, with 16 fractional bits

void CreatePhaseStepData() {
    for(int i = 0; i < 32; i++) {
        float frequency = 440.0 * pow(2, (i + 93 - 69) / 12.0);
        uint32_t phaseStep = round(((frequency * 128) / 15625.0) * (1UL << 16));
        Serial.print("0x"); Serial.print(phaseStep, HEX); Serial.print("UL");
        if(i < 31) {
            Serial.print(",");
            if(i % 6 == 5) {
                Serial.println();
            }
            else {
                Serial.print(" ");
            }
        }
    }
    Serial.println();
}

*/
const uint32_t PROGMEM Chirp::phaseSteps[32] = {
    0xE6AFDUL,  0xF4677UL,  0x102EFEUL, 0x112559UL, 0x122A5AUL, 0x133EE0UL,
    0x1463D8UL, 0x159A3CUL, 0x16E314UL, 0x183F7AUL, 0x19B097UL, 0x1B37A8UL,
    0x1CD5F9UL, 0x1E8CEEUL, 0x205DFDUL, 0x224AB2UL, 0x2454B5UL, 0x267DC2UL,
    0x28C7B0UL, 0x2B3477UL, 0x2DC628UL, 0x307EF3UL, 0x33612FUL, 0x366F50UL,
    0x39ABF3UL, 0x3D19DCUL, 0x40BBF9UL, 0x449565UL, 0x48A969UL, 0x4CFB80UL,
    0x518F5FUL, 0x5668EEUL
};


Chirp::Chirp(byte minVolume, byte maxVolume, uint16_t rampTime) {
    setParameters(minVolume, maxVolume, rampTime);
}

void Chirp::setVolume(byte volume) {
    maxVolume = volume;
}

void Chirp::setParameters(byte minVolume, byte maxVolume, uint16_t rampTime) {
    this->minVolume = minVolume;
    this->maxVolume = maxVolume;
    if(rampTime > MAX_RAMP_TIME) {
        // clamp rampTime to max
        rampTime = MAX_RAMP_TIME;
    }
    this->rampTime = rampTime;
    this->sustainTime = BLOCK_TIME - 2 * rampTime;
}



uint32_t Chirp::phaseStepForCharCode(const char charCode) {
    byte index;
    
    if(charCode >= 'a' && charCode <= 'v') {
        index = charCode - 'a' + 10;
    }
    else if(charCode >= '0' && charCode <= '9') {
        index = charCode - '0';
    }
    else {
        // BAD_CHIRP_CODE_WARNING;
        return 0L;
    }
    
    return pgm_read_dword_near(phaseSteps + index);
}


void Chirp::head(uint32_t phaseStep) {
    lastPhaseStep = 0;
}


void Chirp::append(uint32_t phaseStep) {
    if(rampTime) {
        if(lastPhaseStep) {
            uint32_t boundaryPhaseStep = (lastPhaseStep + phaseStep) / 2; // average the values
            TheSynth.addFrame(rampTime, lastPhaseStep, boundaryPhaseStep, maxVolume, minVolume); // last frame for *previous* block 
            TheSynth.addFrame(rampTime, boundaryPhaseStep, phaseStep, minVolume, maxVolume); // first frame for this block
        }
        else { // we're at the start, or following a silent block
            TheSynth.addFrame(rampTime, phaseStep, phaseStep, 0, maxVolume); // first frame for this block no phaseStep ramping
        }
    }
    
    TheSynth.addSustainFrame(sustainTime, phaseStep, maxVolume); // central frame for this block
    
    lastPhaseStep = phaseStep;
}


void Chirp::tail(uint32_t phaseStep) {
    if(rampTime && phaseStep) {
        TheSynth.addFrame(rampTime, phaseStep, phaseStep, maxVolume, 0); // last frame for *previous* block
    }
}


// (2 front door, & 18 data/error codes) * 3 frames for each, plus 1 end marker: (20*3+1 = 61)
int16_t Chirp::numberOfFrames() {
    return 61;
}


bool Chirp::enoughSpaceFor(size_t size)
{
    extern int __heap_start, *__brkval;
    int freeRAM = (int) &freeRAM - (__brkval == 0 ? (int) &__heap_start: (int) __brkval);
    return freeRAM >= MINIMUM_FREE_RAM + (int) size;
}


byte Chirp::chirp(const char *chirpStr) {
    int16_t nFrames = numberOfFrames();
    size_t frameStoreSize = sizeof(SynthFrame[nFrames]);
    
    if(enoughSpaceFor(frameStoreSize)) {
        SynthFrame *frames = (SynthFrame *) alloca(frameStoreSize);
        return chirp(chirpStr, nFrames, frames);
    }
    else {
        return TOO_LITTLE_RAM_WARNING;
    }
}


byte Chirp::chirp(const char *chirpStr, char *enoughSpaceForFrames) {
   return chirp(chirpStr, numberOfFrames(), (SynthFrame *) enoughSpaceForFrames);
}


byte Chirp::chirp(const char *chirpStr, int16_t nFrames, SynthFrame *frames) {
    static const uint32_t hPhaseStep = pgm_read_dword_near(phaseSteps + 17);
    static const uint32_t jPhaseStep = pgm_read_dword_near(phaseSteps + 19);
    uint32_t phaseStep = jPhaseStep;
    
    // check length and return warning if incorrect
    const char *cs = chirpStr;
    byte count = CHIRP_STRING_LENGTH;
    while(*cs++ && --count) {
    }
    if(count || *cs) {
        return CHIRP_STRING_LENGTH_WARNING;
    }
    
    TheSynth.beginFrameSequence(nFrames, frames);
    
    head(hPhaseStep);
    
    // all chirps start with these two tones
    append(hPhaseStep);
    append(jPhaseStep);

    while(const char code = *chirpStr++) { // assignment, not comparison
        phaseStep = phaseStepForCharCode(code);
        if(phaseStep) {
            append(phaseStep);
        }
        else {
            return BAD_CHIRP_CODE_WARNING;
        }
    }
        
    tail(phaseStep);
    TheSynth.endFrameSequence();
    
    return TheSynth.play();
}
