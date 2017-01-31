# Chirp for Arduino

*Version 3.0, January 2017*

## Overview

Chirp is a library enabling Arduino-based devices to output chirp audio via attached audio hardware. You'll need:

 * An Arduino Uno or an Arduino Mega 2560
 * A compatible speaker, earbud or other device to provide sound output
 * A list of Encoded Identifiers to send. (test identifiers are provided in the examples)
 * an iOS or Android device running a Chirp-enabled app to receive the chirps transmitted

You'll need an Arduino Uno (or a board with an equivalent processor such as the Arduino Ethernet) an Arduino Mega 2560 or an Engduino. For sound output you will need a compatible speaker or other device (we have found just connecting an earbud directly to the pins works, although it is likely to be overdriving the earbud), chirp codes from chirp.io (test codes are provided in the examples), and something such as smartphone running a chirp-enabled app to listen out for the chirps emitted.

## Installing Chirp

Chirp is written for the Arduino IDE versions 1.0.6 and above, including 1.6.3 which is the most recent at the time of writing. 

Install Chirp as a library. For instructions, see 

[http://arduino.cc/en/Guide/Libraries](http://arduino.cc/en/Guide/Libraries)

Once installed, you can access the example programs from the menu :

```File > Examples > Chirp > example ```

and you can include the headers to use Chirp in your own code by using :

```Sketch > Import Library > Chirp```

## Using Chirp

Hardware: Connect a speaker or other audio device to your board. Use ground plus pin 3 on the Uno, ground plus pin 9 on the Mega, or ground plus pin I/O_0 on the Engdiuno.

Software: create a Chirp  and ask it to chirp. 

For instance, File > Examples > Chirp > Example contains this complete program:

      #include <Chirp.h>
      Chirp chirp;
      void setup() {}
      void loop() {
          chirp.chirp("parrotbilllllahcm4");
          delay(2000);
      }

## Audio output

On the Uno, the main sound signal is supplied on digital pin 3, and on the Mega it's on digital pin 9. Use pin I/O_0 on the Engduino. Connect this pin and a ground pin to your compatible speaker or audio device.

The signal supplied is a 62.5kHz, pulse width modulated (PWM) approximation to an ideal audio waveform. It’s 2.7V on the Engduino and 5V on the other Arduinos. This PWM technique is explained further below but it's important to understand that it's up to your attached audio hardware to smooth this signal. Ideally, this smoothing would be performed by additional filter circuitry before the result is fed to your audio device, however in practice the 62.5kHz pulse train is at too high a frequency for audio hardware to pass unchanged, and so simple speakers will naturally smooth the signal.

Although amplification & smoothing circuitry would be ideal, if you want to keep things simple you can attach a small speaker or an earbud directly to the Arduino pins. The exact nature of the smoothing, and so the fine characteristics of the sound, will depend on the particular hardware that you attach. We have had success directly connecting the pins to a 3.5mm audio lead and plugging this into a device such as an old radio. Please be aware that such a signal won't match the official audio jack electrical specifications (so keep the volume low), and you will certainly be overdriving an earbud if you connect it this way (don’t put it in your ear!). Whatever hardware you connect to the Arduino you do at your own risk; it's your responsibility to check for compatibility.

#### Pulse Width Modulation

The signal generated approximates a sine wave that is changing continuously as appropriate for a chirp. However, the Arduino can't supply a true variable analog output; at any one time a pin may only ever be switched on (5V or 2.7V depending on device) or off (0V). So to approximate a sine wave the pin is pulsed rapidly, with 62500 pulses being issued every second (one every 16 microseconds) under the assumption that this pulse train will be smoothed (averaged over time). The targeted Arduinos run at 16MHz so a new pulse is issued every 256 clock cycles, the Engduino runs at 8MHz so issue one every 128 clock cycles. In each 16-microsecond period the pin is only turned on for a fraction of the time, from 0% (0V, not turned on at all) to 100% (5V on an Arduino, turned on the whole time). For instance, to approximate a 3.75V signal (75% of 5V) the pulse will be switched on for 192 clock cycles, that is 12 microseconds (75% of the time). Since we are approximating a continuously varying sinusoidal signal, each successive pulse is likely to be on for a different time than its predecessor. This description is provided only to help you to understand the output signal; the generation of these pulses is fully handled for you by the synthesiser. 

## Chirps Identifiers

Chirps are specified by strings containing 18 characters. Each character selects one of 32 tones; it must be either a digit (0-9) or one of the first 22 lower-case letters (a-v). Eg: ```"ntdb982ilj6etj6e3l"```

When chirps are played, two extra tones (corresponding to tones h and j) are prepended to the 18 characters you supply to make 20 in total. Chirp codes from chirp.io contain some data and some error correction characters based on the data. 

Chirp will play any string of the correct length and characters, but only strings from chirp.io contain the valid error correction codes that let listening devices receive them properly, and only very particular strings link to content.
 
## Example

Once Chirp is installed as a library the examples supplied are available in the IDE menu ```File > Examples > Chirp > Example```. They are designed to be explored in approximately the following order:

A minimal program. A Chirp object is created and is used to chirp a specific code over and
over, with two-second gaps. A good test to check your library is installed and functioning.

### Trouble

Demonstrates using the warning codes to help troubleshoot problems.

chirp returns a warnings value that you can ignore if you wish, or can pay attention to if you are investigating problems. While all is well warnings is zero, but whenever a problem is spotted a bit is set in the corresponding field.

Trouble and the subsequent example write to the Arduino IDE serial monitor at 9600 baud so you should open the monitor and set it to this speed to see the messages. Serial output is sent asynchronously using interrupts after the print command returns, but chirping turns off these interrupts so serial output may be delayed or lost. As demonstrated here, the simplest way to avoid this is to use delay to wait a while to allow the serial output to finish before issuing a chirp.

***

This document is part of the [Chirp developer resources](http://developers.chirp.io).

All content copyright &copy; Asio Ltd, 2013-2017. All rights reserved.
