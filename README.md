ofxICubeX

Addon to interface with ICubeX digitizers. Currently tested with the USB-microDig, WI-microdig and the MIDI Digitizer (first 8 ports). Supports both sensor input and actuator output. 
Linux versions should work, but have not yet been tested.

On portable platforms, the supported features are limited, as shown in the following table:

Compatibility

| OS       | USB-microDig  | Wi-microDig   |  MIDI Digitizer  |
|----------| ------------- | ------------- | ---------------- |
| Windows  | Yes           | Yes           |  Yes* |
| OSX      | Yes           | Yes           |  Yes* |
| Linux    | Most Likey    | Most Likey    |  Most Likely |
| iOS      | Limited       | No            |  Yes* |
| Android  | Limited       | Yes           |  Yes* |

*Requires class compliant USB MIDI adapter
Limited: can only be used in stand alone mode to receive MIDI messages *from* the digitizer, requires existing configuration on Desktop system using the ICubeX Editor. In this case, simply connect the digitizer using the USB interface and parse MIDI messages directly using ofxMidi example (tutorial for this TBD).

Other (non-default) Addons required:

- ofxMidi
- ofxUI (for the GUI demo)

Setup Requirements:

Windows:
- USB-microDig: follow setup instructions until BlueMIDI is set up. (No need to proceed further): http://infusionsystems.com/catalog/info_pages.php/pages_id/216
- Wi-microDig: follow setup instructions here: (...)

OSX:
- USB-microDig: 
- - USB Driver (https://www.silabs.com/products/mcu/Pages/USBtoUARTBridgeVCPDrivers.aspx) 
- - ICubeX Connect http://infusionsystems.com/catalog/product_info.php/products_id/331

iOS:
- No special setup is required, other than making sure the required addons are in the path.
- We have tested with the iRig Midi 2 as well as iConnect MIDI adapters; Any generic, class-compliant USB-MIDI cable should work fine.

Android:

The Android version is experimental, and requires the following additional components:
- AndroidMidi library (https://github.com/nettoyeurny/btmidi)
- Our slightly modified version of ofxMidi, and ofxAndroid addons. (link link)
Make sure you are familiar with the Android/oF toolchain and comfortable compiling generic oF applications for Android before attempting to do this!

openFrameworks addons:

- ofxXmlSettings (built in)

- ofxMidi https://github.com/danomatika/ofxMidi (or modified version if on Android)

- ofxUI https://github.com/rezaali/ofxUI

Other Notes:

- If you create a blank project with the oF project generator, make sure to manually add the CoreMIDI framework (Build Phases->Link Binary With Libraries) for MAC/iOS, or winmm.lib in Windows.

- There are two example applications. Once the driver is loaded and Connect is running, the following apps should work out of the box:

- - exampleICubeX: a more featured GUI application that visually shows sensor data, and allows control of a number of parameters of the digitizer

- - exampleSimpleICubeX: a bare bones app showing the basic commands for talking to the digitizer. if you want to hook up to sensors directly, this is a good place to start as the relevant features are not interlarded with GUI setup code. The setup() function in ofApp.cpp contains comments walking through the process of connecting to, setting up and getting data from the ICubeX digitizers.


Created by Johnty Wang (johnty@infusionsystems.com) Oct 2014

Latest revision 9 Dec 2014
