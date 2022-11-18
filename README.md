# What is OnkyoRi anyway? 
If you have an Onyko Receiver or any other Onkyo hardware, you might find a 3.5mm jack plug on the back. When you connect it to another Onky device, you will be able to use the same remote for all devices. Someone on the internet decoed the protol and and enabled us, to use this interface e.g. via a Rasperry Pi. My usecase is, to feed "this" by NodeRed / Alexa. So I can enable voice control for legacy hardware. 
![OnkyoRi Port](./img/onkyorihw.png)



# Onkyo-RI-RasperryPi
First of all ... I was myself confused wich approach to choose. There are Python approaches, but I found them kind of slow. (Wouldn't be a problem, just for setting another source, but when volume control only works when on a specific source - for me it's GAM when using my TX-SR-605 - then it gets quite annoying if every command needs 1s+ to run :D)

This is why i foreked [docbender's](https://github.com/ahaack/onkyo-rpi) Onkyo-ri aproach that was originally intended to be used with an Arduino but is is written in C++. I adopted it for use with RasperryPi using WiringPi and added a feature to concatenate commands ... so - if you already know on which source you were on - you can just jump back and forth when you want to change the volume (or switch off - what for me also only was possible when that source was selected) - if you are a C++er - MR's and suggestions welcome🙈 - i only bended things enough to be work. 


## First steps

## Software
clone :)

execute build with ./build  (I haven't worked with C++ for a while, MR for Makefile welcome :D ) 




## Hardware Connection
You need to make yourself a fitting cable. Nothing too wild. Just a 3.5mm jack paired with a  10kOhms resistor sitting in paralell. 

To connect to the RI port a 3.5mm mono jack is used. Tip is for data signal and sleeve is ground (GND). Data is sent via TTL logic. So it is easy to connect a RI device to 5V MCU (Rasperry Pi / [Arduino](https://github.com/ahaack/onkyo-rpi)). Just connect the data signal to some output pin and connect GND to a GND pin on the Pi. In case of stereo jack, connect tip to DATA, sleeve and ring to GND. 

So for a Rasperry Pi 3/4 to put the tip to Pin 22 (GPIO_GEN6) GPIO25 (tx) and the shield to Pin 20 or another ground  (GND)



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
* non-blocking **Ditn't use that here yet** - send() method only start command sending. The execution is handled by processing() function. This function must be called periodically with maximum 1 ms period. Function return bool value about sending status (true - data is being sent, false - nothing to sent/sending is done). Before the command is completely sent other functions can be executed. Library use internaly Arduino micros() function, so no other timer is not blocked.

## RI codes
At mentioned sites are also listed codes for Onkyo devices. Unfortunnately none of the codes is not valid for my receiver TX-8020. To determine the valid codes I wrote a simple loop for Arduino (more below) that goes through the whole 12bit code range (0x0-0xFFF). Results are listed below commands.

### TX-SR605
<table>
  <tr><td><b>Action</b></td><td><b>Command</b></td><td><b>Notes</b></td></tr>
  <tr><td>Input CD</td><td>0x20</td><td>Switch input to CD channel</td></tr>
  <tr><td>Turn On + CD</td><td>0x2F</td><td>Turn ON receiver and select CD as input channel</td></tr>
  <tr><td>Input TAPE</td><td>0x70</td><td>Switch input to TAPE channel</td></tr>
  <tr><td>Turn On + TAPE</td><td>0x7F</td><td>Turn ON receiver and select TAPE as input channel</td></tr>
  <tr><td>Input DVD</td><td>0x120</td><td>Switch input to DVD channel</td></tr>
  <tr><td>Turn On + DVD</td><td>0x12F</td><td>Turn ON receiver and select DVD as input channel</td></tr>
  <tr><td>Input DOCK</td><td>0x170</td><td>Switch input to DOCK channel</td></tr>
  <tr><td>Turn On + DOCK</td><td>0x17F</td><td>Turn ON receiver and select DOCK as input channel</td></tr>  
  <tr><td>Dimmer Hi</td><td>0x2B0</td><td>Set dimmer brightness to highest level</td></tr>
  <tr><td>Dimmer Mid</td><td>0x2B1</td><td>Set dimmer brightness to mid level</td></tr>  
  <tr><td>Dimmer Lo</td><td>0x2B2</td><td>Set dimmer brightness to lowest level</td></tr>  
  <tr><td>Dimmer Hi</td><td>0x2B8</td><td>Set dimmer brightness to highest level</td></tr>
  <tr><td>Dimmer Lo</td><td>0x2BF</td><td>Set dimmer brightness to lowest level</td></tr>    
  <tr><td>Turn Off</td><td>0x1AE</td><td>Turn OFF(set into standby) receiver</td></tr>  
  <tr><td>Test mode</td><td>0x421 - 0x424</td><td>Some kind of test modes. Leave test mode is possible by code 0x420 (Turn Off). Test modes provides clear of receiver setting.</td></tr>
  <tr><td>Switch to Radio</td><td>0x423</td><td>Switch to FB</td></tr>  
  <tr><td>Radio search next</td><td>0x430</td><td>Tune next radio station when radio is selected.</td></tr>  
  <tr><td>Radio search previous</td><td>0x431</td><td>Tune previous radio station when radio is selected.</td></tr>  
  <tr><td>Radio Stereo/Mono</td><td>0x432</td><td>Switch between Stereo and Mono when FM radio is selected.</td></tr>  
  <tr><td>Radio station next</td><td>0x433</td><td>Jump to next stored radio station when radio is selected.</td></tr>  
  <tr><td>Radio station previous</td><td>0x434</td><td>Jump to previous stored radio station when radio is selected.</td></tr>
  
  <tr><td>Vol Up</td><td>0x1A2</td><td>Volume Up (only when input is set to Video 3)</td></tr>
  <tr><td>Vol Down</td><td>0x1A3</td><td>Volume Down (only when input is set to Video 3)</td></tr>
  <tr><td>Mute</td><td>0x1A4</td><td>Mute (only when input is set to Video 3)</td></tr>
</table>


### TX-8020 receiver 
Codes are valid for TX-8020 receiver. With a high probability it will work with other Onkyo receivers.
<table>
  <tr><td><b>Action</b></td><td><b>Command</b></td><td><b>Notes</b></td></tr>
  <tr><td>Input CD</td><td>0x20</td><td>Switch input to CD channel</td></tr>
  <tr><td>Turn On + CD</td><td>0x2F</td><td>Turn ON receiver and select CD as input channel</td></tr>
  <tr><td>Input TAPE</td><td>0x70</td><td>Switch input to TAPE channel</td></tr>
  <tr><td>Turn On + TAPE</td><td>0x7F</td><td>Turn ON receiver and select TAPE as input channel</td></tr>
  <tr><td>Input BD/DVD</td><td>0x120</td><td>Switch input to BD/DVD channel</td></tr>
  <tr><td>Turn On + BD/DVD</td><td>0x12F</td><td>Turn ON receiver and select BD/DVD as input channel</td></tr>
  <tr><td>Input DOCK</td><td>0x170</td><td>Switch input to DOCK channel</td></tr>
  <tr><td>Turn On + DOCK</td><td>0x17F</td><td>Turn ON receiver and select DOCK as input channel</td></tr>  
  <tr><td>Dimmer Hi</td><td>0x2B0</td><td>Set dimmer brightness to highest level</td></tr>
  <tr><td>Dimmer Mid</td><td>0x2B1</td><td>Set dimmer brightness to mid level</td></tr>  
  <tr><td>Dimmer Lo</td><td>0x2B2</td><td>Set dimmer brightness to lowest level</td></tr>  
  <tr><td>Dimmer Hi</td><td>0x2B8</td><td>Set dimmer brightness to highest level</td></tr>
  <tr><td>Dimmer Lo</td><td>0x2BF</td><td>Set dimmer brightness to lowest level</td></tr>    
  <tr><td>Turn Off</td><td>0x420</td><td>Turn OFF(set into standby) receiver</td></tr>  
  <tr><td>Test mode</td><td>0x421 - 0x424</td><td>Some kind of test modes. Leave test mode is possible by code 0x420 (Turn Off). Test modes provides clear of receiver setting.</td></tr>
  <tr><td>Radio search next</td><td>0x430</td><td>Tune next radio station when radio is selected.</td></tr>  
  <tr><td>Radio search previous</td><td>0x431</td><td>Tune previous radio station when radio is selected.</td></tr>  
  <tr><td>Radio Stereo/Mono</td><td>0x432</td><td>Switch between Stereo and Mono when FM radio is selected.</td></tr>  
  <tr><td>Radio station next</td><td>0x433</td><td>Jump to next stored radio station when radio is selected.</td></tr>  
  <tr><td>Radio station previous</td><td>0x434</td><td>Jump to previous stored radio station when radio is selected.</td></tr>
</table>

### TX-SR304 receiver 
Thanks to lonejeeper 
<table>
  <tr><td><b>Action</b></td><td><b>Command</b></td><td><b>Notes</b></td></tr>
  <tr><td>Input CD</td><td>0x20</td><td>Switch input to CD channel</td></tr>
  <tr><td>Input TAPE</td><td>0x70</td><td>Switch input to TAPE channel</td></tr>
  <tr><td>Input BD/DVD</td><td>0x120</td><td>Switch input to BD/DVD channel</td></tr>
  <tr><td>Input HDD</td><td>0x170</td><td>Switch input to HDD channel</td></tr>
  <tr><td>Input Video2</td><td>0x1A0</td><td>Switch input to Video2 channel</td></tr>  
  <tr><td>Vol Up</td><td>0x1A2</td><td>Volume Up</td></tr>
  <tr><td>Vol Down</td><td>0x1A3</td><td>Volume Down</td></tr>
  <tr><td>Mute</td><td>0x1A4</td><td>Mute</td></tr>
  <tr><td>Power OFF</td><td>0x1AE</td><td>Power OFF</td></tr>
  <tr><td>Power ON</td><td>0x1AF</td><td>Power ON</td></tr>
  <tr><td>Dim</td><td>0x2B0</td><td>Set dimmer brightness to highest level</td></tr>
  <tr><td>Dimer</td><td>0x2B1</td><td>Set dimmer brightness to mid level</td></tr>  
  <tr><td>Dimmest</td><td>0x2B2</td><td>Set dimmer brightness to lowest level</td></tr>  
  <tr><td>Dimmer Daytime</td><td>0x2B8</td><td>Set daytime brightness</td></tr>
  <tr><td>Dimmer Nighttime</td><td>0x2BF</td><td>Set nighttime brightness</td></tr>    
  <tr><td>Test mode</td><td>0x421 - 0x423, 0x430</td><td>RDS TEST</td></tr>  
</table>

### TX-SR313 receiver 
Thanks to wongcz
<table>
  <tr><td><b>Action</b></td><td><b>Command</b></td><td><b>Notes</b></td></tr>
  <tr><td>Input Aux</td><td>0x20</td><td>Switch input to AUX channel</td></tr>
  <tr><td>Turn ON + AUX</td><td>0x2F</td><td>Turn ON receiver and select AUX as input channel</td></tr>
  <tr><td>Input TV(/CD)</td><td>0x70</td><td>Switch input to TV(/CD) channel</td></tr>
  <tr><td>Turn ON + input TV(/CD)</td><td>0x7F</td><td>Turn ON receiver and select TV(/CD) as input channel</td></tr>
  <tr><td>Input BD/DVD</td><td>0x120</td><td>Switch input to BD/DVD channel</td></tr>
  <tr><td>Turn ON + input BD/DVD</td><td>0x12F</td><td>Turn ON receiver and select BD/DVD as input channel</td></tr>
  <tr><td>Turn ON + input DOCK</td><td>0x17F</td><td>Turn ON receiver and select DOCK as input channel</td></tr>
  <tr><td>Dim</td><td>0x2B0</td><td>Set dimmer brightness to highest level</td></tr>
  <tr><td>Dimer</td><td>0x2B1</td><td>Set dimmer brightness to mid level</td></tr>  
  <tr><td>Dimmest</td><td>0x2B2</td><td>Set dimmer brightness to lowest level</td></tr>  
  <tr><td>Dimmer Daytime</td><td>0x2B8</td><td>Set daytime brightness</td></tr>
  <tr><td>Dimmer Nighttime</td><td>0x2BF</td><td>Set nighttime brightness</td></tr>    
  <tr><td>Test mode</td><td>0x421 - 0x423, 0x430</td><td>RDS TEST</td></tr>  
</table>

### TX-SR333 receiver 
Thanks to ogrady
<table>
  <tr><td><b>Action</b></td><td><b>Command</b></td><td><b>Notes</b></td></tr>
  <tr><td>Input TV/CD</td><td>0x20</td><td>Switch input to TV/CD channel</td></tr>
  <tr><td>Input BD/DVD</td><td>0x120</td><td>Switch input to BD/DVD channel</td></tr>  
  <tr><td>Dim</td><td>0x2B0</td><td>Set dimmer brightness to highest level</td></tr>
  <tr><td>Dimer</td><td>0x2B1</td><td>Set dimmer brightness to mid level</td></tr>  
  <tr><td>Dimmest</td><td>0x2B2</td><td>Set dimmer brightness to lowest level</td></tr>  
  <tr><td>Test mode set</td><td>0x421 - 0x424</td><td>Test 1-00,2-00,3-00,4-00</td></tr>  
  <tr><td>Test</td><td>0x425</td><td>Test x-01</td></tr>
  <tr><td>Test</td><td>0x426</td><td>Test x-02</td></tr>
  <tr><td>Test</td><td>0x427</td><td>WiFi test</td></tr>
  <tr><td>Test</td><td>0x428</td><td>Route test</td></tr>
  <tr><td>Test</td><td>0x42a</td><td>Key Test Mode</td></tr>
  <tr><td>Test</td><td>0x42b</td><td>Test x-07</td></tr>
  <tr><td>Test</td><td>0x42c</td><td>Test x-08</td></tr>
  <tr><td>Test</td><td>0x42d</td><td>Test x-09</td></tr>
  <tr><td>Test</td><td>0x42e</td><td>Test x-00</td></tr>
  <tr><td>Test mode set</td><td>0x43e</td><td>Test 5-00</td></tr>
  <tr><td>Test</td><td>0x43f</td><td>CEC test</td></tr>
</table>

### TX-SR600 receiver 
Thanks to kelvinlaw 
<table>
  <tr><td><b>Action</b></td><td><b>Command</b></td><td><b>Notes</b></td></tr>
  <tr><td>Input CD</td><td>0x20</td><td>Switch input to CD channel</td></tr>
  <tr><td>Turn On + CD</td><td>0x2F</td><td>Turn ON receiver and select CD as input channel</td></tr>
  <tr><td>Input TAPE</td><td>0x70</td><td>Switch input to TAPE channel</td></tr>
  <tr><td>Turn On + TAPE</td><td>0x7F</td><td>Turn ON receiver and select TAPE as input channel</td></tr>  
  <tr><td>Input BD/DVD</td><td>0x120</td><td>Switch input to BD/DVD channel</td></tr>
  <tr><td>Turn On + BD/DVD</td><td>0x12F</td><td>Turn ON receiver and select BD/DVD as input channel</td></tr>
  <tr><td>Input Video3</td><td>0x1A0</td><td>Switch input to Video3 channel</td></tr>
  <tr><td>Turn On + Input Video3</td><td>0x1AF</td><td>Turn ON receiver and select Video3 as input channel</td></tr>
  <tr><td>Power Off</td><td>0x1AE</td><td>Power OFF (only works if you are currently in Video 3!!)</td></tr>
</table>

### TX-SR603 receiver 
Thanks to jimtng 
<table>
  <tr><td><b>Action</b></td><td><b>Command</b></td><td><b>Notes</b></td></tr>
  <tr><td>Input Video3</td><td>0x1A0</td><td>Switch input to Video3 channel</td></tr>
  <tr><td>Vol Up</td><td>0x1A2</td><td>Volume Up (only when input is set to Video 3)</td></tr>
  <tr><td>Vol Down</td><td>0x1A3</td><td>Volume Down (only when input is set to Video 3)</td></tr>
  <tr><td>Mute</td><td>0x1A4</td><td>Mute (only when input is set to Video 3)</td></tr>
  <tr><td>Unmute</td><td>0x1A5</td><td>Unmute (only when input is set to Video 3)</td></tr>
  <tr><td>Power Off</td><td>0x1AE</td><td>Power OFF (only works if you are currently in Video 3!!)</td></tr>
  <tr><td>Turn On + Input Video3</td><td>0x1AF</td><td>Turn ON receiver and select Video3 as input channel</td></tr>
</table>

### TX-SR606 receiver 
<table>
  <tr><td><b>Action</b></td><td><b>Command</b></td><td><b>Notes</b></td></tr>
  <tr><td>Input CD</td><td>0x20</td><td>Switch input to CD channel</td></tr>
  <tr><td>Turn On + CD</td><td>0x2F</td><td>Turn ON receiver and select CD as input channel</td></tr>
  <tr><td>Input TAPE</td><td>0x70</td><td>Switch input to TAPE channel</td></tr>
  <tr><td>Turn On + TAPE</td><td>0x7F</td><td>Turn ON receiver and select TAPE as input channel</td></tr>  
  <tr><td>Input DVD</td><td>0x120</td><td>Switch input to DVD channel</td></tr>
  <tr><td>Turn On + DVD</td><td>0x12F</td><td>Turn ON receiver and select DVD as input channel</td></tr>
  <tr><td>Input GAME/TV</td><td>0x1A0</td><td>Switch input to GAME/TV channel</td></tr>
  <tr><td>Vol Up</td><td>0x1A2</td><td>Volume Up (only when input is set to GAME/TV)</td></tr>
  <tr><td>Vol Down</td><td>0x1A3</td><td>Volume Down (only when input is set to GAME/TV)</td></tr>
  <tr><td>Mute</td><td>0x1A4</td><td>Mute (only when input is set to GAME/TV)</td></tr>
  <tr><td>Unmute</td><td>0x1A5</td><td>Unmute (only when input is set to GAME/TV)</td></tr>
  <tr><td>Power Off</td><td>0x1AE</td><td>Power OFF (only when input is set to GAME/TV)</td></tr>
  <tr><td>Turn On + GAME/TV</td><td>0x1AF</td><td>Turn ON receiver and select GAME/TV as input channel</td></tr>
  <tr><td>Setup</td><td>0x420-0x424</td><td>Displayed: SETUP. Playing loud noisy sounds</td></tr>
  <tr><td>CEC test</td><td>0x43E</td><td>Displayed: CEC TEST OK</td></tr>
  <tr><td>TEST 5-00</td><td>0x43F</td><td>Displayed: TEST 5-00</td></tr>
</table>
  
### A-803 receiver
<table>
  <tr><td><b>Action</b></td><td><b>Command</b></td><td><b>Notes</b></td></tr>
  <tr><td>Volume +</td><td>0x2</td><td>See notes on volume</td></tr>
  <tr><td>Volume -</td><td>0x3</td><td>See notes on volume</td></tr>
  <tr><td>Power On/Power Off</td><td>0x4</td><td>The receiver actually enters/exits standby since the only way to properly shut it down is through the power switch located on the front of the receiver itself.</td></tr>
  <tr><td>Mute</td><td>0x5</td><td></td></tr>
  <tr><td>Input Aux/Video</td><td>0x6</td><td></td></tr>
  <tr><td>Input Tape-2 Monitor</td><td>0x7</td><td></td></tr>
  <tr><td>Input Tape-1/DAT</td><td>0x8</td><td>Alternative code: 0x70.</td></tr>
  <tr><td>Input CD</td><td>0x9</td><td>Alternative code: 0x20.</td></tr>
  <tr><td>Input Phono</td><td>0xA</td><td>Alternative code: 0x30.</td></tr>
  <tr><td>Input Tuner</td><td>0xB</td><td>Alternative code: 0xE0.</td></tr>
  <tr><td>Source direct</td><td>0x13</td><td></td></tr>
  <tr><td>OFF</td><td>0xEA</td><td>This code will switch the receiver into stanby mode, but it will not switch it back on.</td></tr>
</table>

#### Notes on volume
Volume control codes shown in the table are sent by the receiver out of its RI ports when adjusting the volume using a remote control (they can be found using an oscilloscope).
However, the receiver will not react to these codes when they are sent from an external device, effectively making impossible to control its volume through RI signals - Thats what was in docbenders readme - I think for most receivers it is in fact possible to control the volume - as long as they have a "Doc"-support for I-Pads. On my TX-SR 605 I can switch to GAM source, what enables me to send the Volume commands. Maybe your harware works different. Again - happy for input on that :) 

## Test program
Did't implement that yet. But I used one scan program in one of the python approaches to scan for my inputs. If you have harsware other then listet, you should do one, to see what your Onkyo Hw reacts to. 

TODO: Add link to Python lib for scanner

## On the Horizon
I'd like to read from Onky Ri as-well.. so if anyone has tried that or has input on that - please write me or just continue writing and do MR :-) 

## Sidenode 
When this one works i want to make this availible to [NodeRed](https://github.com/dajuly20/node-red-contrib-onkyo-ri) (see my other repo for that )