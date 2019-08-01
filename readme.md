# notes from 7/22/19 prototying session

Ran the cable diagonally in the Media Lab, played with pure tone generators. Seemed to get a lot more movement out of these than 

Performances are in August and September and are 2-hour blocks, aiming for four fifteen-minute performances during each slot.

Will aim to do: body movement signals on dancers–>Arduino radio transmitter–>Arduino radio receiver–>Processing sketch–>sound synthesis.

# calling car audio places:

## Cazillia 412 351 1660
called 7-25-19 at 1:20pm: on hold for forever, hung up

# notes from 7/31/19 prototyping session

Set up the speaker with diagonal cable, as done previously. Taylor had the interesting idea to hang a sheet of mylar off of it (to capture all the great vibration and shaking) and I got some from Phys Comp. I turned off the MIDI command transmission and adjusted the error reporting from the Arduino side (it doesn't send any more radio failure messages via serial, since those were interfering with the live output from the Processing sketch). The Arduino sketch was `multi_receiver_emit_MIDI` (MIDI commented out), commit `1b78cd2`, and the Processing sketch was `sound_production_from_serial_input.pde`, commit `40d38e5`.

Anna and then Taylor wore just the pressure sensor on their heels, and its analogRead output divided by 10 mapped to the frequency produced on the speaker. (A very fast, very easy way to allow for some interactivity.) The lowest frequencies, around 10 or 20Hz, produced a really great jarring signal that wiggles the wire around a lot. These low speeds seem to produce the greatest amplitude in the travel of the voice coil as well, perhaps because they give the coil so much time to get where it wants to go. (Recall also that the natural frequency of the armature arm is ~37Hz, and that's computed without any additional mass at the end…so it might just like vibrating around 20Hz or so for that reason as well.)

We arranged Source Four lamps to reflect light off the dangling mylar, which went crazy whenever low frequency sounds were played through the cable. Shot lots of interesting weird video, including Anna controlling the frequency in the foreground while Taylor played in the reflection off the mylar in the background.

## for next Tuesday meeting on 8/6/19

Prepare:
* clean up magnet clamps
* pretty up wires (will remove white jacket and twist wires together)
* screw in amp to the top of the wooden base
* build stand for speaker (temporary, but useful)
* solder up remote units
    - three inputs over two radios: 
    - one unit has the pressure sensor
    - the other unit has a laser rangefinder (**needs purchased**) and accelerometer
* software: 
    - implement vibrato (driven by three sensors)