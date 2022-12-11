# Objective 
Control Onkyo Hardware with RI Jack with NodeRed & Rasperry Pi
# TLDR
U need to **bulild a cable** youslelf with a **3.5mm mono jack**, that has a **10kOhms resistor** in parallell, and connect the other end to a Rasperry Pi's GPIOS

# Getting started
This is a fork of the C++ project [docbender/Onkyo-RI](https://github.com/docbender/Onkyo-RI) 
that was made for the arduino platform, migrating it to the Rasperry Pi platform using
 Wiring Pi. 

# What is OnkyoRi anyway? 
If you have an Onyko Receiver or any other Onkyo hardware, you might find a 3.5mm jack plug on the back.
 When you connect it to another Onkyo device, you will be able to use the same remote for all devices. 
Someone on the internet decoed the protol and and enabled us, to use this interface e.g. via a Rasperry Pi. 
This program is used in [dajuly20/node-red-contrib-onkyo-ri](https://github.com/dajuly20/node-red-contrib-onkyo-ri) 
that wraps it to be A NodeRed Node. So I can enable voice control for legacy hardware. 
![OnkyoRi Port](./img/onkyorihw.png)

# Onkyo-RI-RasperryPi
First of all ... I was myself confused wich approach to choose. There are Python approaches, 
but I found them kind of slow. (Wouldn't be a problem,
 just for setting another source, but when volume control only 
works when on a specific source - for me it's Game/TV when using my TX-SR-605 - then it gets quite annoying if every command needs 1s+ to run)

This is why i foreked [docbender's](https://github.com/ahaack/onkyo-rpi) Onkyo-ri aproach that was originally intended to be used with an Arduino but is is written in C++. I adopted it for use with RasperryPi using WiringPi and added a feature to concatenate commands ... so - if you already know on which source you were on - you can just 
jump back and forth when you want to change the volume
 (or switch off - what for me also only was possible when that source was selected) - if you are a C++er - MR's and suggestions welcome🙈 - I only bended things enough to be working for now. 



## First steps
As the binary is shipted as well, this should work right out of the box. If not, 
or if you want to change the code you will need to (re) build it. 

## Build dependencies
```
sudo apt update 
sudo apt install wiring pi build-essential git```
git clone https://github.com/dajuly20/Onkyo-RI-Rasperrypi/
./build
```
(I haven't worked with C++ for a while, MR for Makefile welcome ) 


## Hardware Connection
You need to make yourself a fitting cable. Nothing too wild. Just a 3.5mm jack paired with a  10kOhms resistor sitting in paralell. 

To connect to the RI port a 3.5mm mono jack is used. Tip is for data signal and sleeve is ground (GND). Data is sent via TTL logic.
 So it is easy to connect a RI device to 5V MCU (Rasperry Pi / [Arduino](https://github.com/ahaack/onkyo-rpi)). 
Just connect the data signal to some output pin and connect GND to a GND pin on the Pi. In case of stereo jack, connect tip to DATA,
 sleeve and ring to GND. 

So for a Rasperry Pi 3 put the tip to Pin 22 (GPIO_GEN6) GPIO25 (tx) and the shield to Pin 20 or another ground  (GND)



![Pi3 Pinout](./img/pi3pinout.svg)
This schema only applies to RasperryPi's. Make sure 

## Protocol
Protocol description could be found at:
*    http://lirc.sourceforge.net/remotes/onkyo/Remote_Interactive

or with grafical representation at:
*    http://fredboboss.free.fr/articles/onkyo_ri.php .

Protocol is pretty simple for implementation. In one message is transfered 12 bit code. This code represents action for target device. Most significant bit is send first.

## Library
There are two Onkyo-RI library:
* blocking **Thats used in this library** - send() method blocks other program execution until whole command is sent. It takes up to 61 ms.
* non-blocking **Ditn't use that here yet** - send() method only start command sending. The execution is handled
 by processing() function. This function must be called periodically with maximum 1 ms period. Function return bool 
value about sending status (true - data is being sent, false - nothing to sent/sending is done). Before the command
 is completely sent other functions can be executed. Library use internaly Arduino micros() function, so no other timer is blocked.

## RI codes
[RI Codes on seperate page](./RiCodes.md)
## Further reading 

## Protocol
Protocol description could be found at:
*    http://lirc.sourceforge.net/remotes/onkyo/Remote_Interactive

or with grafical representation at:
*    http://fredboboss.free.fr/articles/onkyo_ri.php .

#### Notes on volume (as per [ahaack/onkyo-rpi](https://github.com/ahaack/onkyo-rpi))
Volume control codes shown in the table are sent by the receiver out of its RI ports when adjusting the volume using a remote control (they can be found using an oscilloscope).
However, the receiver will not react to these codes when they are sent from an external device, effectively making impossible to control its volume through RI signals.

**Comment to that by Julian Wiche**:

That doesn't seem to be true on every device. I **could** contol the volume on my TX SR 605 when input is set to Game/TV. 
The scan program (see further up) can be used to obtain the codes - it only requires a "bit" of parience. I think that receivers, that have a motor to turn the knob won't work. 

## Test program
If none of the abouve codes works for you. You can walk through any possible code with the test program. See: [ahaack/onkyo-RPI](https://github.com/ahaack/onkyo-RPI) 

NOTE: As per my personal experience: For me testing went thought a testing mode, that made a loud sinus tone.. also it could increase the volume before.. so be careful not to wake anyone when you try this at nights :D 

## Feel free to Contribute 
If it's just Ri Codes for your Hardware by sending a merge request or write me an email. 
if you fork the whole thing and make it your own project. Please share your findings! Before you fork though, you can send me a message - I 've seen it often enough to have the same project in 100 different forks / versions on npm / Node Red. Think of DAUs. They won't know which version to install.  

## On the Horizon
I'd like to read from Onky Ri as-well.. so if anyone has tried that or has input on that - please write me or just continue writing and do MR :-) 

## Sidenode 
When this one works i want to make this availible to [NodeRed](https://github.com/dajuly20/node-red-contrib-onkyo-ri) (see my other repo for that )
