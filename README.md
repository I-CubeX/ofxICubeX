ofxICubeX

Addon to interface with ICubeX digitizers. Currently tested with the USB-microDig, but should work with Wi-microDig as well, and the Digitizer (first 8 ports) too.

Requirements:

- USB microDig from Infusion Systems, and some sensors of your choice http://infusionsystems.com/catalog/index.php

- Silicon Labs USB-serial Driver http://www.silabs.com/products/mcu/Pages/USBtoUARTBridgeVCPDrivers.aspx

- ICubeX Connect Software (Mac) http://infusionsystems.com/catalog/product_info.php/products_id/331 
     or BlueMIDI (Windows) http://infusionsystems.com/catalog/product_info.php/products_id/332

- Our modified version of ofxMidi and ofxAndroid addons (only necessary for Android)

openFrameworks addons:

- ofxXmlSettings (built in)

- ofxMidi https://github.com/danomatika/ofxMidi (or modified version if on Android)

- ofxUI https://github.com/rezaali/ofxUI

If you create a blank project with the oF project generator, make sure to add the CoreMIDI framework (Build Phases->Link Binary With Libraries) for MAC/iOS, or winmm.lib in Windows.

There are two example applications. Once the driver is loaded and Connect is running, the following apps should work out of the box:

- exampleICubeX: a more featured GUI application that visually shows sensor data, and allows control of a number of parameters of the digitizer

- exampleSimpleICubeX: a bare bones app showing the basic commands for talking to the digitizer. if you want to hook up to sensors directly, this is a good place to start as the relevant features are not interlarded with GUI setup code. The setup() function in ofApp.cpp contains comments walking through the process of connecting to, setting up and getting data from the ICubeX digitizers.

The interface has also been tested on iOS (wired MIDI only - Digitizer or USB-microDig with MIDI adapter), with a working version of the GUI example (exampleICubeX) available as an oF iOS project. The same list of oF addons above are required.

Tested configurations:

- Android (using our modified ofxAndroid and ofxMidi libraries)

- iOS

- Windows

- OSX

Created by Johnty Wang (johnty@infusionsystems.com) Oct 2014

Latest revision 1 Dec 2014
