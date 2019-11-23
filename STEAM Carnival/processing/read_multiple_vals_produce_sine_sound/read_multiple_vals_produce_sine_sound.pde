/*
 Resonant Bodies Sound Demo Machine 
 for STEAM Carnival at Pittsburgh Children's Museum, Nov. 23rd, 2019
 
 Reads in a string of comma-delimited integers from an Arduino via serial
 stream, and interprets them to play various sounds. This allows for a physical
 input for the demonstration machine.
 
 Includes sound files in "data" folder:
 vibraphone.aiff, included in sample code
 Tribal_Baby.mp3, "Tribal Baby" by Satellite Ensemble, via Free Music Archive
 Kölderen_Polka.mp3, "Kölderen Polka" by Tres Tristes Tangos, via Free Music Archive
 BuckeyeBattleCry-Drums.mp3, "Buckeye Battle Cry (Drums Only)"
 by The Ohio State University Marching Band, from "Chimes and Change",
 Jon Woods, Director, OSU Marching Band
 © 2000 Fidelity Sound Recordings, P. O. Box 802 Redwood City CA 94064
 
 incorporating code from generous Processing Forum internet denizens
 and built-in examples
 */

import processing.sound.*;
import processing.serial.Serial;

// Arduino is at index 13 in the serial list
static final int PORT_INDEX = 13;

int[] vals = {0, 0, 0, 0};

/*
vals position meanings:
 0: volume knob (0–1023)
 1: frequency slider (0–1023)
 2: momentary pushbutton (0,1)
 3: mode switch (0,1)
 4: not used
 */

SinOsc sine;

float freq, amp, pos, rate;

static final int TONE = 0, MUSIC = 1;
int mode = TONE, prevMode = TONE;
boolean switchToTone = false, switchToMusic = false;


SoundFile[] soundfiles = new SoundFile[4];
int songCounter = 0; // index of the currently playing song

String[] songNames = {
  "Vibraphon", 
  "Tribal Baby by Satellite Ensemble", 
  "Kölderen Polka by Tres Tristes Tangos", 
  "Buckeye Battle Cry (Drums Only) by the OSU\nMarching Band"
}; // OSU newline so it'll wrap in the window properly


void setup() {
  size(1500, 1000);
  background(255);
  final String[] ports = Serial.list();
  //printArray(ports);
  new Serial(this, ports[PORT_INDEX], 9600).bufferUntil(ENTER);

  sine = new SinOsc(this);

  soundfiles[0] = new SoundFile(this, "vibraphon.aiff");
  soundfiles[1] = new SoundFile(this, "Tribal_Baby.mp3");
  soundfiles[2] = new SoundFile(this, "Kölderen_Polka.mp3");
  soundfiles[3] = new SoundFile(this, "BuckeyeBattleCry-Drums.mp3");
}

void draw() {
  background(50);

  String sourceText = (prevMode == TONE) ? "sine wave" : songNames[songCounter];
  int volume = int(amp * 100);
  int frequency = int(freq);
  int songPercent = int(rate * 100);
  String rateText;
  if (prevMode == TONE) rateText = "sine wave frequency = " + frequency + "Hz";
  else rateText = "music playing rate = " + songPercent + "% of regular speed";

  textSize(50);
  rectMode(CORNERS);
  text("Tactile Speaker" + 
    "\nnow playing: " + sourceText + 
    "\n\nvolume (amplitude): " + volume + "%" + 
    "\n" + rateText, 
    50, 50);

  // draw a simple sine wave to illustrate 
  float waveHeight = amp * 200;
  float waveWidth = 1000/freq;
  float[] y = new float[width + 1]; // y values of wave points

  // load y values into array
  for (int x = 0; x < width; x++) {
    //point(i, waveHeight * (sin(i/waveWidth)));
    y[x] = waveHeight * sin(x/waveWidth);
  }

  translate(0, 2*height/3); // move wave 2/3 down the screen
  // connect the dots to draw the waves
  stroke(255, 255, 0); // yellow lines
  for (int i = 1; i < y.length - 1; i++) {
    line(i, y[i], i+1, y[i+1]);
  }


  // adjust output volume based on knob
  amp=map(vals[0], 0, 1023, 0.0, 1.0);
  // rescale this to an exponential curve, so lower end of knob travel is more sensitive
  amp = amp * amp;
  sine.amp(amp);
  soundfiles[songCounter].amp(amp);

  // adjust sine frequency based on sliding potentiometer
  freq=map(vals[1], 0, 1023, 0.0, 1.0);
  freq = freq * freq; // rescale on exponential curve
  freq=map(freq, 0.0, 1.0, 1, 1000); // scale up to appropriate range
  if (vals[2] == 1) freq *= pow(2, 4); // a four-octave increase if button pushed
  sine.freq(freq);
  // adjust rate of music play based on sliding potentiometer
  rate = map(vals[1], 0, 1023, 0.0, 1.0);
  rate = rate * rate;
  if (vals[2] != 1) rate = map(rate, 0.0, 1.0, 0.25, 2.0); // if button not pressed
  else rate = map(rate, 0.0, 1.0, 0.1, 10); // much wider range
  soundfiles[songCounter].rate(rate);

  // detect transitions from TONE to MUSIC mode and vice versa
  if (vals[3] == TONE && prevMode == MUSIC) { 
    switchToTone = true;
    prevMode = TONE;
  }
  if (vals[3] == MUSIC && prevMode == TONE) {
    switchToMusic = true;
    prevMode = MUSIC;
  }

  // change output function only once (running sine.play() repeatedly is bad)
  if (switchToTone) {
    soundfiles[songCounter].stop();
    sine.play();
    switchToTone = false;
  }

  if (switchToMusic) {
    sine.stop();

    // advance song counter to play the next song in the queue
    songCounter++;
    songCounter %= soundfiles.length;    
    soundfiles[songCounter].loop();
    switchToMusic = false;
  }
}


void serialEvent(final Serial s) {
  //println(s.readString());
  // read in serial values, split by comma delimiter
  vals = int(splitTokens(s.readString(), ","));
  //printArray(vals);
}
