#pragma once

#include "ofMain.h"
#include "ofxiOS.h"
#include "ofxiOSExtras.h"

#include "ofxMidi.h"
#include "ofxICubeX.h"

#include <deque>

class ofApp : public ofxiOSApp, public ofxMidiListener, public ofxMidiConnectionListener {
	
public:
    void setup();
    void update();
    void draw();
    void exit();

    void touchDown(ofTouchEventArgs & touch);
    void touchMoved(ofTouchEventArgs & touch);
    void touchUp(ofTouchEventArgs & touch);
    void touchDoubleTap(ofTouchEventArgs & touch);
    void touchCancelled(ofTouchEventArgs & touch);

    void lostFocus();
    void gotFocus();
    void gotMemoryWarning();
    void deviceOrientationChanged(int newOrientation);
    
    // add a message to the display queue
    void addMessage(string msg);
    
    // midi message callback
    void newMidiMessage(ofxMidiMessage& msg);
    
    // midi device (dis)connection event callbacks
    void midiInputAdded(string name, bool isNetwork);
    void midiInputRemoved(string name, bool isNetwork);
    
    void midiOutputAdded(string nam, bool isNetwork);
    void midiOutputRemoved(string name, bool isNetwork);
    
    vector<ofxMidiIn*> inputs;
    vector<ofxMidiOut*> outputs;
    
    deque<string> messages;
    int maxMessages;
    ofMutex messageMutex; // make sure we don't read from queue while writing
    
    //ICube-X stuff:
    
    ofxICubeX my_icube;
    int my_data[8];
    
    bool isStream;
    string midiDevStr;


};


