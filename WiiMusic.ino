/*  Example playing a sinewave at a set frequency,
    using Mozzi sonification library.

    Demonstrates the use of Oscil to play a wavetable.

    Circuit: Audio output on digital pin 9 on a Uno or similar, or
    DAC/A14 on Teensy 3.1, or
    check the README or http://sensorium.github.com/Mozzi/

    Mozzi help/discussion/announcements:
    https://groups.google.com/forum/#!forum/mozzi-users

    Tim Barrass 2012, CC by-nc-sa.
*/

//#include <ADC.h>  // Teensy 3.1 uncomment this line and install http://github.com/pedvide/ADC
#include <MozziGuts.h>
#include <Oscil.h> // oscillator template
#include <tables/sin2048_int8.h> // sine table for oscillator
#include "pitches.h"
#include "WiiChuck.h"
#include <ADSR.h>
#include <tables/cos8192_int8.h>
#include <tables/envelop2048_uint8.h>

WiiChuck chuck = WiiChuck();

// use: Oscil <table_size, update_rate> oscilName (wavetable), look in .h file of table #included above
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin(SIN2048_DATA);
Oscil <COS8192_NUM_CELLS, AUDIO_RATE> aCarrier(COS8192_DATA);
Oscil <COS8192_NUM_CELLS, AUDIO_RATE> aModulator(COS8192_DATA);
Oscil <COS8192_NUM_CELLS, AUDIO_RATE> aModWidth(COS8192_DATA);
Oscil <COS8192_NUM_CELLS, CONTROL_RATE> kModFreq1(COS8192_DATA);
Oscil <COS8192_NUM_CELLS, CONTROL_RATE> kModFreq2(COS8192_DATA);
Oscil <ENVELOP2048_NUM_CELLS, AUDIO_RATE> aEnvelop(ENVELOP2048_DATA);





// use #define for CONTROL_RATE, not a constant
#define CONTROL_RATE 128 // powers of 2 please

ADSR <CONTROL_RATE, CONTROL_RATE> envelope;

int gain = 1;
unsigned int duration, attack, decay, sustain, release_ms;


void setup() {
  Serial.begin(115200);
  chuck.begin();
  chuck.update();
  startMozzi(CONTROL_RATE); // set a control rate of 64 (powers of 2 please)
  aSin.setFreq(440); // set the frequency
  attack = 50;
  decay = 50;
  sustain = 200;
  release_ms = 1000;
  envelope.setADLevels(255, 100);
  envelope.setTimes(attack, decay, sustain, release_ms);
  aCarrier.setFreq(aSin.next());
  kModFreq1.setFreq(.45f);
  kModFreq2.setFreq(0.087f);
  aModWidth.setFreq(0.0717f);
  aEnvelop.setFreq(9.0f);
}


void updateControl() {
  chuck.update();

  int Pitch = map(chuck.readPitch(), 0, 170, 1, 16); // map the Pitch to 1-16 to pick notes of G7 Arpeggio (V7 chord)
  if (chuck.buttonZ) {
    switch (Pitch) {
      case 1:
        aSin.setFreq(NOTE_G3);
        break;
      case 2:
        aSin.setFreq(NOTE_B3);
        break;
      case 3:
        aSin.setFreq(NOTE_D4);
        break;
      case 4:
        aSin.setFreq(NOTE_F4);
        break;
      case 5:
        aSin.setFreq(NOTE_G4);
        break;
      case 6:
        aSin.setFreq(NOTE_B4);
        break;
      case 7:
        aSin.setFreq(NOTE_D5);
        break;
      case 8:
        aSin.setFreq(NOTE_F5);
      case 9:
        aSin.setFreq(NOTE_G5);
        break;
      case 10:
        aSin.setFreq(NOTE_B5);
        break;
      case 11:
        aSin.setFreq(NOTE_D6);
        break;
      case 12:
        aSin.setFreq(NOTE_F6);
        break;
      case 13:
        aSin.setFreq(NOTE_G6);
        break;
      case 14:
        aSin.setFreq(NOTE_B6);
        break;
      case 15:
        aSin.setFreq(NOTE_G7);
        break;
      case 16:
        aSin.setFreq(NOTE_A7);
        break;
      default:
        aSin.setFreq(NOTE_G5);
        break;
    }

  }

  else {
    switch (Pitch) {     //Map notes to Cmaj7 Arpeggio (I chord)
      case 1:
        aSin.setFreq(NOTE_C3);
        break;
      case 2:
        aSin.setFreq(NOTE_E3);
        break;
      case 3:
        aSin.setFreq(NOTE_G3);
        break;
      case 4:
        aSin.setFreq(NOTE_C4);
        break;
      case 5:
        aSin.setFreq(NOTE_E4);
        break;
      case 6:
        aSin.setFreq(NOTE_G4);
        break;
      case 7:
        aSin.setFreq(NOTE_B4);
        break;
      case 8:
        aSin.setFreq(NOTE_C5);
      case 9:
        aSin.setFreq(NOTE_E5);
        break;
      case 10:
        aSin.setFreq(NOTE_G5);
        break;
      case 11:
        aSin.setFreq(NOTE_B5);
        break;
      case 12:
        aSin.setFreq(NOTE_C6);
        break;
      case 13:
        aSin.setFreq(NOTE_E6);
        break;
      case 14:
        aSin.setFreq(NOTE_G6);
        break;
      case 15:
        aSin.setFreq(NOTE_B6);
        break;
      case 16:
        aSin.setFreq(NOTE_C7);
        break;
      default:
        aSin.setFreq(NOTE_C4);
        break;
    }
  }
    // Now we will turn on the phase mod envelope if the user is holding down the C-Button
  if (chuck.buttonC) {   
    gain = chuck.readJoyY() + 114;
    int roll = map(chuck.readRoll(), -180, 180, 1, 7);
    aEnvelop.setFreq(roll); //Map Roll coordinates to between 4 and 20 Hz for Modulator width
    aModulator.setFreq(aSin.next() + 0.4313f * kModFreq1.next() + kModFreq2.next());
  }
    // Use the joystick's Y-value to change the volume of a note up or down
  gain = chuck.readJoyY() + 114;
    
  envelope.update(); // Make sure any updated parameters are saved


}


int updateAudio() {
  if (chuck.buttonC) {
    int asig = aSin.phMod((int)aModulator.next() * (260u + aModWidth.next()));
    return (asig * (byte)aEnvelop.next()) >> 8;
  }
  else {
    return ((aSin.next() * gain) >> 8); // return an int signal centred around 0
  }
}

void loop() {
  audioHook(); // required here
}



