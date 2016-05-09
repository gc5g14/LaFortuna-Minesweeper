Minesweeper implementation for the LaFortuna board

# LaFortuna-Minesweeper
Minesweeper game created for the LaFortuna board used in COMP2215

## Instructions:
1. Run the provided make file and upload hex into LaFortuna memory.
2. Select game size (7x11 selected by default) and press on a difficulty level to start.
3. Use arrows and rotary encoder to navigate.
4. Press center to reveal field.
5. Long press up or down arrow to mark as flagged / uncertain field.
6. Find all the mines!

## Features included:
* 2 map sizes
* 3 levels of difficulty for each map size
* Randomly generated game setups
* Flagging (with counter) and question marks
* Empty field expansion
* Instructions

## Features to implement in the future::
* More map sizes
* Nicer graphics for mines and flags
* Timer and high scores

## FAQ:
* Q: The flag counter went below 0, what does this mean?
  * R: Too many flags have been placed, the number of bombs on the map is the same as the initial flag counter

## LaFortuna hardware:
* avr90usb1286 MCU
* 240x320 screen with ILI9341 driver
* Rotary encoder with 4 directional buttons and a central button

## Some of the code used in this projects belongs to different authors:
* To read switches (routa.c and routa.h) Peter Danneger's code (adapted by Klaus-Peter Zauner) is used
* To drive the display (lcd folder) Steve Gunn's code (adapted by Klaus-Peter Zauner and modified by me) is used

This code is released under the GPLv3 licence
