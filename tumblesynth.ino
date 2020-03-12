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
#include <tables/triangle_warm8192_int8.h> // sawe table for oscillator
#include <tables/chum9_int8.h> // recorded audio wavetable
#include <RollingAverage.h>
#include <ControlDelay.h>
#include <LowPassFilter.h>

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
Oscil <TRIANGLE_WARM8192_NUM_CELLS, AUDIO_RATE> aTri2(TRIANGLE_WARM8192_DATA);
Oscil <TRIANGLE_WARM8192_NUM_CELLS, AUDIO_RATE> aTri3(TRIANGLE_WARM8192_DATA);

// use: RollingAverage <number_type, how_many_to_average> myThing
RollingAverage <int, 8> kAverage; // how_many_to_average has to be power of 2
int averagedX;
int averagedY;
int averagedZ;

LowPassFilter lpf;

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
  Serial.print(bumpy_inputX);
  Serial.print("\t"); // prints a tab
  Serial.print("Y = ");
  Serial.print(bumpy_inputY);
  Serial.print("\t"); // prints a tab
  Serial.print("Z = ");
  Serial.print(bumpy_inputZ);
  Serial.print("\t"); // prints a tab
  averagedX = kAverage.next((bumpy_inputX-250)*3.5);
  averagedY = kAverage.next((bumpy_inputY-240)*3.5);
  averagedZ = kAverage.next((bumpy_inputZ-300)*3.5);
  Serial. println();  
  aTri0.setFreq((averagedZ));
  aTri1.setFreq(kDelay.next(averagedX));
  aTri2.setFreq(kDelay.read(averagedY));
  aTri3.setFreq(kDelay.read(averagedZ));
}


int updateAudio(){
  return 3*((int)aTri0.next()+aTri1.next()+(aTri2.next()>>1)
    +(aTri3.next()>>2)) >>3;
}


void loop(){
  audioHook();
}

