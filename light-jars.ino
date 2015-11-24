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
//#include <tables/sin8192_int8.h> 
#include <tables/saw8192_int8.h> 
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
#define NUM_LEDS 20

// For led chips like Neopixels, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806 define both DATA_PIN and CLOCK_PIN
#define DATA_PIN 3
#define CLOCK_PIN 13

// Define the array of leds
CRGB leds[NUM_LEDS];

#define CONTROL_RATE 64

/*
Oscil <8192, AUDIO_RATE> vco1(SIN8192_DATA);; 
Oscil <8192, AUDIO_RATE> vco2(SIN8192_DATA);; 
Oscil <8192, AUDIO_RATE> vco3(SIN8192_DATA);; 
*/

Oscil <8192, AUDIO_RATE> vco1(SAW8192_DATA);; 
Oscil <8192, AUDIO_RATE> vco2(SAW8192_DATA);; 
Oscil <8192, AUDIO_RATE> vco3(SAW8192_DATA);; 

Oscil <8192, AUDIO_RATE> env_vco(SAW8192_DATA);; 


LowPassFilter lpf;

// for triggering the envelope
EventDelay noteDelay;

ADSR <CONTROL_RATE, AUDIO_RATE> envelope;

boolean note_is_on = true;

void setup(){
  FastLED.addLeds<LPD8806, DATA_PIN, CLOCK_PIN, BGR>(leds, NUM_LEDS);
  Serial.begin(9600); // for Teensy 3.1, beware printout can cause glitches
  //Serial.begin(115200);
  randSeed(); // fresh random
  noteDelay.set(2000); // 2 second countdown
  startMozzi(CONTROL_RATE);
  lpf.setResonance(100);

  env_vco.setFreq(4);
}


unsigned int duration, attack, decay, sustain, release_ms;

byte intensity;

int controlTicks = 0;
byte hue;

const int primes[] = { 3, 5, 7, 11, 13, 17, 19, 23 };

byte cutoff_freq = 0; 

void updateControl(){
  
  if(noteDelay.ready()){
    
      // choose envelope levels
      byte attack_level = 200; // rand(128)+127;
      byte decay_level = 0;//rand(128)+127; // rand(255);
      envelope.setADLevels(attack_level,decay_level);

    // generate a random new adsr time parameter value in milliseconds
     unsigned int new_value = rand(300) +100;
     
     // randomly choose one of the adsr parameters and set the new value
     switch (rand(4)){
       case 0:
       attack = new_value;
       break;
       
       case 1:
       decay = new_value;
       break;
       
       case 2:
       sustain = new_value;
       break;
       
       case 3:
       release_ms = new_value;
       break;
     }
     attack = 2500;
     decay = 2500;
     sustain = 0; //rand(4000) + 1000;
     release_ms = 500;
     envelope.setTimes(attack,decay,sustain,release_ms);    
     envelope.noteOn();

     byte midi_note = rand(32);
     hue = midi_note * 8;
     midi_note += 42;

     int baseFreq = (int)mtof(midi_note);
     
     // detuned oscillators
     vco1.setFreq(baseFreq);
     byte beatFrequency = rand(32)+16;
     vco2.setFreq(baseFreq  + primes[rand(8)]);

     // bass
     vco3.setFreq(baseFreq - primes[rand(8)]);

     Serial.print("midi_note\t"); Serial.println(midi_note); 

/*
     // print to screen
     Serial.print("midi_note\t"); Serial.println(midi_note);
     Serial.print("attack_level\t"); Serial.println(attack_level);
     Serial.print("decay_level\t"); Serial.println(decay_level);
     Serial.print("attack\t\t"); Serial.println(attack);
     Serial.print("decay\t\t"); Serial.println(decay);
     Serial.print("sustain\t\t"); Serial.println(sustain);
     Serial.print("release\t\t"); Serial.println(release_ms);
     Serial.println();
*/
     noteDelay.start(attack+decay+sustain+release_ms);

    // map the modulation into the filter range (0-255)

   }
   Serial.println(cutoff_freq);
  controlTicks++;

  if (true) { // controlTicks % 3 == 0){
    int i;

    i  = intensity;

    if (i < 8) {
      i = 0;
    }
    
    CRGB color = CHSV(hue, 255, i);
    // CRGB color = CRGB(pgm_read_byte(&gamma[base.red]), pgm_read_byte(&gamma[base.green]), pgm_read_byte(&gamma[base.blue]));

    for (i=0;i<NUM_LEDS;i++){
      // leds[i] = CRGB(intensity, 0, 0);
      leds[i] = color;
    }
    
    FastLED.show();
  }
  
  envelope.update();
} 


int updateAudio(){
  //intensity = envelope.next();
  intensity = ((int)envelope.next() * (int)env_vco.next()) >> 8;
  cutoff_freq = 100; //(int)(intensity/2) + 50;

  lpf.setCutoffFreq(cutoff_freq);

  int v = vco1.next();
  int beatFreq = lpf.next((vco2.next() + vco3.next() + v + v) >> 2);
  //int beatFreq = (vco2.next() + vco3.next() + v + v) >> 2;
  return (int) (intensity * beatFreq) >> 8;
}


void loop(){
  audioHook(); // required here
}






