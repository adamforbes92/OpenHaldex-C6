
# OpenHaldex - ESP32 C6
An open-source Generation 1, 2 & 4 Haldex Controller which originates/is a fork from ABangingDonk's 'OpenHaldex T4'.  It has been extended into Gen2 and Gen4 variants, with Gen3 and Gen5 currently unsupported - CAN reads of these systems would be awesome!

Originally based on the Teensy 4.0; the ESP32 features two TWAI (CAN) interfaces as well as WiFi and Bluetooth support; which makes it an ideal candidate for easier interfacing.

![OpenHaldex-C6](/Images/BoardOverview.png)

### Purchase
If you want to purchase an assembled OpenHaldex C6 controller, you can do so here: [OpenHaldex C6 Controller - Forbes Automotive](https://forbes-automotive.com/products/openhaldex-t4-controller)

### Concept
The basis of the module is to act as a middle man - read the incoming CAN frames destined for the OEM Haldex controller, and, if required, adjust these messages to 'trick' the Haldex into thinking more (or less) lock is required.  

Generation 1, 2 and 4 are all available as 'standalone' systems - which means that no other ECUs have to be present and OpenHaldex will create the necessary frames for Haldex operation.

### ESP32 C6
The ESP32 C6 features two TWAI controllers - which allows CANBUS messages to be read, processed and re-transmitted towards the Haldex.  It also supports WiFi and Bluetooth - which makes on-the-fly configuration changes possible.  

The original Teensy had an external HC-05 Bluetooth chip which is limited support wise.  

### The Modes
The controller allows for 4 main modes: Stock (act as OEM), FWD (zero lock), 7525 (some lock) and 5050 (100% lock) at the Haldex differential.  Generation 1, 2 and 4 have been tested on the bench to allow for a full understanding of what the stock CAN messages look like & therefore what messages need to be editted / created. 

These modes are displayed as colours using the 5mm PCB LED: Red (Stock), Green (FWD), Cyan (7525) and Blue (5050).  Custom modes are purple.

The modes can be toggled with the onboard 'Mode' button or changed via. WiFi.

Disabling at low throttle inputs or high speed inputs are also configurable.

### WiFi Setup
WiFi setup and configuration is always active.  Connect to 'OpenHaldexC6' by searching in WiFi devices and searching for 192.168.1.1 in a browser.  All settings are available for editing.  Should the WiFi page hang, a long press on the 'mode' button will reset the WiFi connection.

### Pinouts
The MX23A12NF connector pinout is:
> Pin 1: Vbatt (12v)
> Pin 2: Ground
> Pin 3: Chassis CAN Low
> Pin 4: Chassis CAN High
> Pin 5: Haldex CAN LOW
> Pin 6: Haldex CAN High
> Pin 7: Switch Mode Exteral (+12v to activate)
> Pin 8: Brake Switch In (+12v to activate)
> Pin 9: Brake Switch Out
> Pin 10: Handbrake Switch In (+12v to activate)
> Pin 11: Handbrake Switch Out

### Uploading Code
For users wishing to customise or edit the code, it is released here for free use.  Connect the Haldex controller via. a data USB-C cable (note some are ONLY power, so this needs to be checked).

It is recommended to check back here regularly to find updates.  An Over-The-Air (OTA) updater would be cool to implement!

### The PCB & Enclosure
The Gerber files for the PCB, should anyone wish to build their own, is under the "PCB" folder.  This is the latest board.
Similarly, the enclosures are also here.

>Pinout & functionality remains the same for ALL generations of enclosure.

![OpenHaldex-C6](/Images/BoardTop.png)

![OpenHaldex-C6](/Images/BoardBottom.png)

### Nice to Haves
The board supports broadcasting the Haldex output via. CAN - which allows pairing with the FIS controller to capture (and received) current and new modes.

Flashing LED if there is an issue with writing CAN messages.

Follow Brake/Handbrake signals (via. hard-wired signals).  The Haldex controller can 'pass-through' the brake/handbrake signals to enable/disable the controller or they can be ignored so that it is always active.

### Shout Outs
Massive thanks to Arwid Vasilev for re-designing the PCB!

## Disclaimer
It should be noted that this will modify Haldex operation and therefore can only be operated off-road and on a closed course.  

It should always be assumed that the unit may crash/hang or cause the Haldex to operate unprediably and caution should be exercised when in use.

Using this unit in any way will exert more strain on drivetrain components, and while the OEM safety features are still in place, it should be understood that having the Haldex unit locked up permanently may cause acceleated wear.

Forbes-Automotive takes no responsiblity for damages as a result of using this unit or code in any form.
