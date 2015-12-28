/*  Example applying an ADSR envelope to an audio signal
    with Mozzi sonification library.  This shows 
    internal updates at CONTROL_RATE, using update() in updateControl(),
    with interpolation and output using next() at AUDIO_RATE in updateAudio().
    This is the "ordinary" way to use ADSR for smooth amplitude transitions while
    maintaining reasonable efficiency by updating internal states in updateControl().
    
    Demonstrates a simple ADSR object being controlled with
    noteOn() and noteOff() instructions.
  
    Tim Barrass 2013, CC by-nc-sa.
*/

//#include <ADC.h>  // Teensy 3.1 uncomment this line and install http://github.com/pedvide/ADC
#include <MozziGuts.h>
#include <Oscil.h>
#include <EventDelay.h>
#include <ADSR.h>
#include <tables/sin8192_int8.h> 
//#include <tables/saw8192_int8.h> 
//#include <tables/phasor256_int8.h> 
#include <mozzi_rand.h>
#include <mozzi_midi.h>
#include <LowPassFilter.h>
#include "FastLED.h"

const uint8_t PROGMEM gamma[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };

// How many leds in your strip?
#define NUM_LEDS 16

// sound on pin 9
#define DATA_PIN 3
#define CLOCK_PIN 4

#define SENSOR_PIN 5

// Define the array of leds
CRGB leds[NUM_LEDS];

#define CONTROL_RATE 64

Oscil <8192, AUDIO_RATE> vco1(SIN8192_DATA);
Oscil <8192, AUDIO_RATE> vco2(SIN8192_DATA);

// for triggering the envelope
EventDelay noteTimer;

ADSR <CONTROL_RATE, AUDIO_RATE> envelope;

boolean noteOn = true;

void setup(){
  FastLED.addLeds<LPD8806, DATA_PIN, CLOCK_PIN, BGR>(leds, NUM_LEDS);
  // Serial.begin(9600); // for Teensy 3.1, beware printout can cause glitches
  //Serial.begin(115200);
  randSeed(); // fresh random
  noteTimer.set(200); // 200ms countdown
  startMozzi(CONTROL_RATE);
  // env_vco.setFreq(0.5f);
  pinMode(SENSOR_PIN, INPUT);

}


unsigned int duration, attack, decay, sustain, release_ms;

byte intensity;

int controlTicks = 0;
byte hue;

const int primes[] = { 3, 5, 7, 11, 13, 17, 19, 23 };

const int blues_scale[] = { 0, 3, 5, 6, 7, 10, 12, 15, 17, 18, 19, 22, 24 };

byte cutoff_freq = 0; 

void updateControl(){
  int sensor = digitalRead(SENSOR_PIN);

  if (!noteOn && sensor) {
    startNote();
  }
  
  if(noteTimer.ready()){
    stopNote();
  }

  controlTicks++;

  byte i = intensity;
  if (i < 8) {
    i = 0;
  }
  if (!noteOn) {
    i = 0;
  }
  fill_solid(leds, NUM_LEDS, CHSV(hue, 255, i));
  FastLED.show();

  // Important bit
  envelope.update();
}

void startNote() {
  noteOn = true;
  
  // choose envelope levels
  byte attack_level = 255; // rand(128)+127;
  byte decay_level = 192;//rand(128)+127; // rand(255);
  envelope.setADLevels(attack_level,decay_level);
  
  attack = 100;
  decay = 100;
  sustain = rand(200) + 100;
  release_ms = 150;
  envelope.setTimes(attack,decay,sustain,release_ms);    
  envelope.noteOn();
  
  byte midi_note = rand(24);
  hue = (int) midi_note * 255 / 24;
  midi_note += 52;
  
  int baseFreq = (int)mtof(midi_note);
  
  // baseFreq = 400;
  
  // detuned oscillators
  vco1.setFreq(baseFreq);
  byte beatFrequency = rand(32);
  vco2.setFreq(baseFreq + beatFrequency);

  noteTimer.start(attack+decay+sustain+release_ms);
}

void stopNote() {
  noteOn = false;
}

int updateAudio(){
  intensity = envelope.next();

  if (!noteOn) {
    intensity = 0;
  }
  
  int v = (vco1.next() + vco2.next()) >> 1;
  return (int) (intensity * v) >> 8;
}


void loop(){
  audioHook(); // required here
}






