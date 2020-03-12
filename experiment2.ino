/*  Example of a simple light-sensawg theremin-like
    instrument with long echoes,
    usawg Mozzi sonification library.
  
    Demonstrates ControlDelay() for echoing control values,
    and smoothing an analog input from a sensor
    signal with RollingAverage().
  
    The circuit: 
   
       Audio output on digital pin 9 on a Uno or similar, or
      DAC/A14 on Teensy 3.0/3.1, or 
       check the README or http://sensorium.github.com/Mozzi/
  
    Light dependent resistor (LDR) and 5.1k resistor on analog pin 1:
       LDR from analog pin to +5V (3.3V on Teensy 3.0/3.1)
       5.1k resistor from analog pin to ground
    
    Mozzi help/discussion/announcements:
    https://groups.google.com/forum/#!forum/mozzi-users
  
    Tim Barrass 2013, CC by-nc-sa.
*/

//#include <ADC.h>  // Teensy 3.0/3.1 uncomment this line and install http://github.com/pedvide/ADC
#include <MozziGuts.h>
#include <Oscil.h> // oscillator template
#include <tables/sin8192_int8.h> // sawe table for oscillator
#include <tables/triangle_warm8192_int8.h> // recorded audio wavetable

#include <RollingAverage.h>
#include <ControlDelay.h>

#define X_PIN 0 // analog control input
#define Y_PIN 1 // analog control input
#define Z_PIN 2 // analog control input

unsigned int echo_cells_1 = 32;
unsigned int echo_cells_2 = 60;
unsigned int echo_cells_3 = 127;

#define CONTROL_RATE 64
ControlDelay <128, int> kDelay; // 2seconds

// oscils to compare bumpy to averaged control input

Oscil <TRIANGLE_WARM8192_NUM_CELLS, AUDIO_RATE> aTri0(TRIANGLE_WARM8192_DATA);
Oscil <TRIANGLE_WARM8192_NUM_CELLS, AUDIO_RATE> aTri1(TRIANGLE_WARM8192_DATA);
Oscil <SIN8192_NUM_CELLS, AUDIO_RATE> aTri2(SIN8192_DATA);
Oscil <SIN8192_NUM_CELLS, AUDIO_RATE> aTri3(SIN8192_DATA);

// use: RollingAverage <number_type, how_many_to_average> myThing
RollingAverage <int, 16> kAverage; // how_many_to_average has to be power of 2
int averagedX;
int averagedY;
int averagedZ;
int mappedX;
int mappedY;
int mappedZ;


byte volume;


void setup(){
  Serial.begin(115200); 
  kDelay.set(echo_cells_1);
  startMozzi();
  
}


void updateControl(){
  int bumpy_inputX = mozziAnalogRead(X_PIN);
  int bumpy_inputY = mozziAnalogRead(Y_PIN);
  int bumpy_inputZ = mozziAnalogRead(Z_PIN);
  Serial.print("X = ");
  Serial.print((bumpy_inputX-270)*3);
  Serial.print("\t"); // prints a tab
  Serial.print("Y = ");
  Serial.print((bumpy_inputY-270)*3);
  Serial.print("\t"); // prints a tab
  Serial.print("Z = ");
  Serial.print((bumpy_inputZ-270)*3);
  Serial.print("\t"); // prints a tab
  mappedX = map(bumpy_inputX, 260, 450, 0, 1023);
  mappedY = map(bumpy_inputY, 260, 450, 0, 1023);
  mappedZ = map(bumpy_inputZ, 260, 450, 0, 1023);

  
  averagedX = kAverage.next(mappedX*1.5);
  averagedY = kAverage.next(mappedY);
  averagedZ = kAverage.next(mappedZ*.75);
  Serial. println();  
  aTri0.setFreq((averagedX));
  aTri1.setFreq(kDelay.next(averagedY));
  aTri2.setFreq(kDelay.read(averagedZ));
  aTri3.setFreq(kDelay.read(averagedX+averagedZ));
  
}


int updateAudio(){
  return 4*((int)aTri0.next()+aTri1.next()+(aTri2.next()>>1)
    +(aTri3.next()>>2)) >>3;
}


void loop(){
  audioHook();
}
