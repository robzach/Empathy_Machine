/*
This is a sine-wave oscillator. The method .play() starts the oscillator. There
 are several setters like .amp(), .freq(), .pan() and .add(). If you want to set all of them at
 the same time use .set(float freq, float amp, float add, float pan)
 */

import processing.sound.*;
import processing.serial.Serial;

static final int PORT_INDEX = 13, BAUDS = 9600;

int[] vals = {0,0,0,0};
//float[] vals = {};

SinOsc sine;

float freq=400;
float amp=0.5;
float pos;

void setup() {
  size(640, 360);
  background(255);
  final String[] ports = Serial.list();
  printArray(ports);
  new Serial(this, ports[PORT_INDEX], BAUDS).bufferUntil(ENTER);

  // Create and start the sine oscillator.

  sine = new SinOsc(this);

  //Start the Sine Oscillator. 
  sine.play();
}

void draw() {

  // Map mouseY from 0.0 to 1.0 for amplitude
  amp=map(vals[0], 0, 1023, 0.0, 1.0);
  sine.amp(amp);

  // Map mouseX from 20Hz to 1000Hz for frequency  
  freq=map(vals[1], 0, 1023, 1.0, 1000.0);
  if (vals[2] == 1) freq *= 2^10;
  sine.freq(freq);

  // Map mouseX from -1.0 to 1.0 for left to right 
  pos=map(mouseX, 0, width, -1.0, 1.0);
  sine.pan(pos);
}


void serialEvent(final Serial s) {
  //println(s.readString());
  vals = int(splitTokens(s.readString(), ","));
  printArray(vals);
  //redraw = true;
}
