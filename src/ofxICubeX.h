///
///  ofxICubeX.h
///
///  ofxAddon for interfacing with I-CubeX digitizers
///
///  Created by Johnty Wang on 2014-09-16. johnty@infusionsystems.com
///
///

#ifndef __ofxICubeX__
#define __ofxICubeX__

#include <iostream>
#include "ICubeXInteface.h"
#include "ofxMidi.h"

#ifdef TARGET_ANDROID
#include "ofxAndroidMidiBridge.h"
#endif

///
///  \file ofxICubeX.h
///
///  The header file for the openFrameworks addon that interfaces with I-CubeX digitizers
///  It is a thin shell wrapped around the C++ API class (ICubeXInterface), and
///  makes use of class-independent MIDI capabilities of ofxMidi and the openFrameworks platform
///  to create a relatively portable instances of applications using I-CubeX family of sensors and
///  digitizers.
///
///  On Android, we skip the ofxMidi class, and instead rely on the ofxAndroidMidiBridge implementation
///  that performs the MIDI interfacing via the Android (Java) application, which then passes through
///  MIDI data via JNI directly.
///
///  Created by Johnty Wang on 2014-09-16. johnty@infusionsystems.com
///
///


class ofxICubeX : public ICubeXInterface, public ofxMidiListener {

public:
    
    ofxICubeX();
    ~ofxICubeX();
    
    void sendSysExCmd();
    //midi message received from ofxMidi
    void newMidiMessage(ofxMidiMessage& msg);
    //manually triggered message from custom interface
    void newMidiMessage(char* data, int len);
    //manually triggered sysex data
    void newSysExStrippedData(char* data, int len);
    
    void listMidiIn() {midi_in_.listPorts();}
    void listMidiOut() {midi_out_.listPorts();}
    
    void connectMidiIn(int port_index);
    void connectMidiOut(int port_index);
    
    void connectMidiIn(string port_str);
    void connectMidiOut(string port_str);
    
    bool isMidiInConnected() {return is_midi_in_connected_;}
    bool isMidiOutConnected() {return is_midi_out_connected_;}
    
    void disconnectMidiIn();
    void disconnectMidiOut();
    
    
    //TODO: most of these could be inline functions.
    void dumpMode();
    void dumpVersion();
    
    void setSampleInterval(int time_ms);
    
    void setMode(bool is_standalone);
    void toggleMode();
    
    void setMute(bool is_muted);
    void toggleMute();
    
    void reset();
    
    void setStream(bool is_on, int port);
    void toggleStream(int port);
    void setHiRes(bool is_hi, int port);
    void setOutput(bool is_on, int port);
    void toggleOutput(int port);
    
    int getSensorData(int port);
    bool isSensorOn(int port) {return my_digitizer_state_.GetIsSensorOnState(port);}
    
    bool isActuatorOn(int port) {return my_digitizer_state_.GetIsActuatorOnState(port);}
    
    string getVersionString();
    
private:
    ofxMidiIn midi_in_;
    ofxMidiOut midi_out_;
    
    bool is_midi_in_connected_;
    bool is_midi_out_connected_;
    
};

#endif /* defined(__ofxICubeX__) */
