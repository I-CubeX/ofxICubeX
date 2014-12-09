ofxICubeX

Addon to interface with [ICubeX digitizers](http://infusionsystems.com/catalog/index.php/cPath/21). Currently tested with the USB-microDig, WI-microdig and the MIDI Digitizer (first 8 ports). Supports both sensor input and actuator output. 
Linux versions should work pending a serial->midi interface, and will be updated when it is implemented.

On portable platforms, the supported features are limited, as shown in the following table:

Compatibility

| OS       | USB-microDig  | Wi-microDig   |  MIDI Digitizer  |
|----------| ------------- | ------------- | ---------------- |
| Windows  | Yes           | Yes           |  Yes* |
| OSX      | Yes           | Yes           |  Yes* |
| Linux    | Future        | Future        | Future |
| iOS      | Limited       | No            |  Yes* |
| Android  | Limited       | Yes           |  Yes* |

*Requires class compliant USB MIDI adapter

Limited: can only be used in stand alone mode to receive MIDI messages from the digitizer, and requires an existing stand alone configuration on Desktop system using the ICubeX Editor. In this case, the oF app cannot change the settings/operation mode of the digitizer, but once set up (via the desktop Editor), the digitizer can be used as a standard MIDI out device, and messages can be parsed using the inputExample from ofxMidi (TBD: tutorial for this). 

Other (non-default) Addons required:

- ofxMidi [github](https://github.com/danomatika/ofxMidi)
- ofxUI (for the GUI demo), [github](https://github.com/rezaali/ofxUI)

Setup Requirements:

Windows:
- USB-microDig: follow setup instructions [here](http://infusionsystems.com/catalog/info_pages.php/pages_id/216) until BlueMIDI is set up. (No need to proceed further): 
- Wi-microDig: follow setup instructions here: (...)

OSX:
- USB-microDig: 
- - USB Driver (https://www.silabs.com/products/mcu/Pages/USBtoUARTBridgeVCPDrivers.aspx) 
- - ICubeX [Connect](http://infusionsystems.com/catalog/product_info.php/products_id/331)

iOS:
- No special setup is required, other than making sure the required addons are in the path.
- We have tested with the iRig Midi 2 as well as iConnect MIDI adapters; Any generic, class-compliant USB-MIDI cable should work fine.

Android:

The Android version is experimental, and requires the following additional components:
- [AndroidMidi library](https://github.com/nettoyeurny/btmidi)
- Our slightly modified version of ofxMidi, and ofxAndroid addons. (link link)
Make sure you are familiar with the Android/oF toolchain and comfortable compiling generic oF applications for Android before attempting to do this!

Other Notes:

- If you create a blank project with the oF project generator, make sure to manually add the CoreMIDI framework (Build Phases->Link Binary With Libraries) for MAC/iOS, or winmm.lib in Windows.

There are three example applications. Once the driver is loaded and Connect is running, the following apps should work out of the box:

- - exampleICubeX: a more featured GUI application that visually shows sensor data, and allows control of a number of parameters of the digitizer
- - exampleSimpleICubeX: a bare bones app showing the basic commands for talking to the digitizer. if you want to hook up to sensors directly, this is a good place to start as the relevant features are not interlarded with GUI setup code. The setup() function in ofApp.cpp contains comments walking through the process of connecting to, setting up and getting data from the ICubeX digitizers.
- - ofSenseplayDemo: a demo [SensePlay](http://sense-play.com/) application that uses input from two sensors to trigger presense of a person at a public kiosk and navigate interactively through media


Created by Johnty Wang (johnty@infusionsystems.com) Oct 2014

Latest revision 9 Dec 2014
