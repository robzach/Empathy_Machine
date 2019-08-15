// basis of Graphing sketch from tom igoe //<>//

/* 2/11/15 end of day progress: sketch talks to arduino, tells it to move, and then doesn't display
 anything other than a black background. 
 Tried trimming it so it would only see and parse the intended data (starting data lines with N 
 as a header for numbers to follow), but no dice so far.
 FIXED IT! Stayed late and figured out to use split() instead of parseInt(). Yeesh.
 
 */

/* 7-25-19 picking up where I left off four years ago!
 
 now using this as a basis for graphing the data output from the distance sensing array
 
 the Arduino will transmit a line which is tab delimited of five different data points,
 something like:
 122  522  6000  40  872
 with a newline at the end. Each data point will be a bar graph to start.
 
 7-28-19
 * changed bar graphs into line graphs which move up and down inside bounded areas
 * moved all drawing operations into draw() or subroutines (doing any drawing inside
 the serial event isn't recommended and was introducing occasional errors)
 * any values of 0 (min) or 1000 (max) back from the sensor are painted red
 * added better legend, etc.
 * added array of checkboxes and x's for constant 10cm and 50cm diagnostic tests
 * started adding trials ("put your hand at 30cm") with voice prompts, *very* incomplete
 
 
 7-31-19 forking from Amelia project!
 * adding sound
 
 8-6-19 updating with mouse input for vibrato
 * deleted lots of unused code
 * 
 
 */

int NUM_OF_SENSORS = 3;

String inString = "1"; // serial read string
int[] positions = new int[NUM_OF_SENSORS]; // incoming position data
int[] barHeight = new int[NUM_OF_SENSORS]; // scaled data for graphing

int MINGRAPHHEIGHT, MAXGRAPHHEIGHT, GRAPHRANGE;

int counter = 0;

public long timer;
public float speedDivisor = 50;


import processing.serial.*;
Serial myPort;
String portName = "/dev/cu.usbmodemMIDI1";

import processing.sound.*;
SinOsc sine;
float freq=400;
float amp=0.5;
float pos;
float vibratoMultiplier;

public int MAXFREQ = 1000;
public int MINFREQ = 20;

void setup () {

  // set the window size
  // you can adjust height as wanted, and the graphs will scale appropriately
  size(1200, 500); 

  // set graph heights
  MINGRAPHHEIGHT = 20;
  MAXGRAPHHEIGHT = height - 20;
  GRAPHRANGE = MAXGRAPHHEIGHT - MINGRAPHHEIGHT;

  myPort = new Serial(this, portName, 9600);
  // don't generate a serialEvent() unless you get a newline character:
  myPort.bufferUntil('\n');

  sine = new SinOsc(this);
  //Start the Sine Oscillator. 
  sine.play();

  background(0);
  fill(255);
}


void draw () {
  background(0);
  drawLegend();
  drawGraphLines();
  
  // start with a standard frequency to change from
  // this as per Anna's request for a different sort of interaction
  float frequency = 100;
  
  
  // multiply it by a value between 100% and 1% based on the amount of squeeze from the remove device
  float frequencyMultiplier = map(positions[2], 0, 1023, 1, 0.01);
  
  //if (positions[2] < 5) frequency = 0;  

  frequency *= (1+vibratoMultiplier());
  
  frequency *= frequencyMultiplier;

  sine.freq(frequency); // drive the sound
  
  float amplitude = map(mouseY, height, 0, 0, 1);
  sine.amp(amplitude);

  //println(frequency);
}

float vibratoMultiplier() {
  counter++;

  float vibratoDivisor = map(mouseX, 0, width, 1, 10);

  float vibratoMultiplier = 0.1 * (1+sin(counter/vibratoDivisor));
  //println(vibratoMultiplier);
  return vibratoMultiplier;
}



void drawGraphLines() {
  for (int i = 0; i<NUM_OF_SENSORS; i++) {
    pushMatrix();
    translate(0, barHeight[i]);
    if (barHeight[i] == 0 || barHeight[i] == GRAPHRANGE) {
      stroke(255, 0, 0);
      strokeWeight(3);
    } else {
      stroke(255);
      strokeWeight(3);
    }
    line((i+1)*75, MINGRAPHHEIGHT, 50+((i+1)*75), MINGRAPHHEIGHT);
    //text(positions[i], (i+1)*75, 0);
    popMatrix();
  }
}

void drawLegend() {
  fill(255);
  text("0", 40, MAXGRAPHHEIGHT); // graph minimum legend (at bottom, hence "max" height)
  text("1023", 20, MINGRAPHHEIGHT); // graph maximum legend (at top, hence "min" height)

  // Y axis legend
  pushMatrix();
  translate(50, height/2 + 25);
  rotate(-PI/2);
  text("sensor raw value", 0, 0);
  popMatrix();

  for (int i = 0; i<NUM_OF_SENSORS; i++) {
    strokeWeight(1);
    stroke(128);
    noFill();
    rectMode(CORNERS);
    // bounding rectangles for each graph line
    rect((i+1)*75, MINGRAPHHEIGHT, 50+((i+1)*75), MAXGRAPHHEIGHT);
    fill(255);
    text("#" + i, 10+((i+1)*75), height-10); // sensor number at bottom of each rectangle
  }
}

void serialEvent (Serial myPort) {
  while (myPort.available() > 0) { 
    // load serial data into inString
    inString = myPort.readStringUntil('\n');
    println(inString);
  }

  // then parse input into five values
  positions = int(split(inString, '\t'));

  // console printing of incoming data
  /*
  for (int i = 0; i<NUM_OF_SENSORS; i++) {
   print(i + ": " + positions[i] + "     ");
   if (i == NUM_OF_SENSORS-1) println();
   }
   */

  // clamp inputs to between 0 and 1023, then map those 0 to GRAPHRANGE for graphing
  for (int i = 0; i<NUM_OF_SENSORS; i++) {
    positions[i] = constrain(positions[i], 0, 1023);
    barHeight[i] = (int)map(positions[i], 1023, 0, 0, GRAPHRANGE);
  }
}
