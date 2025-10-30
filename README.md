
# OpenHaldex - ESP32 C6
An open-source Generation 1, 2 & 4 Haldex Controller which originates/is a fork from ABangingDonk's 'OpenHaldex T4'.  It has been extended into Gen2 and Gen4 variants, with Gen3 and Gen5 currently unsupported.

Originally based on the Teensy 4.0; the ESP32 features two TWAI (CAN) interfaces as well as WiFi and Bluetooth support; which makes it an ideal candidate for easier interfacing.

### Concept
The basis of the module is to act as a middle man - read the incoming CAN frames destined for the OEM Haldex controller, and, if required, adjust these messages to 'trick' the Haldex into thinking more (or less) lock is required.  

### ESP32 C6
The ESP32 C6 features two TWAI controllers - which allows CANBUS messages to be read, processed and re-transmitted towards the Haldex.  It also supports WiFi and Bluetooth - which makes on-the-fly configuration changes possible.  

The original Teensy had an external HC-05 Bluetooth chip which is limited support wise.  

### The Modes
The controller allows for 4 main modes: Stock (act as OEM), FWD (zero lock), 7525 (some lock) and 5050 (100% lock) at the Haldex differential.  Generation 1, 2 and 4 have been tested on the bench to allow for a full understanding of what the stock CAN messages look like & therefore what messages need to be editted / created. 

The forked code has been tweaked to support the remote screen, standalone modes (so conversations/swaps into non-CANBUS) as well as Gen2 & Gen4 support.

### Uploading Code
For users wishing to customise or edit the code, it is released here for free use.  

For pre-compiled versions, navigate to the latest version and download the 'Teensy.exe' & your generation.  Connect the Haldex controller via. a data micro-USB cable (note some are ONLY power, so this needs to be checked).
Load the .hex file in Teensy.exe (File, Open) and press upload.  Some units may require the small internal white button to be pressed.

It is recommended to check back here regularly to find updates.

### The PCB & Enclosure
The Gerber files for the PCB, should anyone wish to build their own, is under the "PCB" folder.  This is the latest board.
Similarly, the enclosures are also here.
V2 enclosure continued to use the Deusche connector, which meant additional crimping & assembly.  
V3 enclosure moved to an onboard connector which results in a smaller footprint and less crimping.  

>Pinout & functionality remains the same for ALL generations of enclosure.

### Nice to Haves
The board supports broadcasting the Haldex output via. CAN - which allows pairing with the FIS controller to capture (and received) current and new modes.

Flashing LED if there is an issue with writing CAN messages.

Flashing LED to show installed Generation.

### Future / To Do
Incorporate 'live' switching of Generations.

## Disclaimer
It should be noted that this will modify Haldex operation and therefore can only be operated off-road and on a closed course.  

It should always be assumed that the unit may crash/hang or cause the Haldex to operate unprediably and caution should be exercised when in use.

Using this unit in any way will exert more strain on drivetrain components, and while the OEM safety features are still in place, it should be understood that having the Haldex unit locked up permanently may cause acceleated wear.

Forbes-Automotive takes no responsiblity for damages as a result of using this unit or code in any form.
