# Pic 18 Seven segment display clock

An attempt to revive old microcontrollers with a retro approach.
This project leverages 6 old red common anode 7 segment display to show a display by scanning each display

Information is managed as a rambuffer, allowing background update of the message while guaranteeing no tearing
in the image (just update data on index per display)

Keyboard scanning is done by leveraging seven segment display and using just 1 input line

Timing mechanism is based on a 18.432Mhz clock which is easily divided into exact milliseconds

# 3D model
The case of the clock has been modeled with tinkercad and is freely available at [Tinkercad](https://www.tinkercad.com/things/5GmCri4DHj3-fnaf-7-segment-clock)

![image](https://github.com/user-attachments/assets/ee61ad8e-cb4f-4b4d-a115-1af30fa0fa02)

The pcb was not yet designed but the schematic is available [here](https://github.com/mlorenzati/retroIoT/blob/main/PIC18/clock/doc/watch_schematics.pdf)

# C Compiler
The compiler to use is and [HITECH PICC-18 9.5 and above](https://ww1.microchip.com/downloads/en/DeviceDoc/PICC_18_9_80_manual.pdf), for the size of the project you can use the evaluation mode
