#include "ofApp.h"

/// exampleSimpleICubeX: openFrameworks demo app for interfacing with iCubeX digitizers
///
/// Make sure the USB driver is installed, and the ICubeX Connect application is running!
///
/// This is a very bare-bones example showing how to connect to, configure, and get data
/// from an ICubeX digitizer. It should work with a USB-microDig, Wi-microDig, and
/// the MIDI Digitizer as well (although for the latter, only the first 8 ports are indexed in the current
/// version of the ofxAddon).
///
///
/// Oct 2014
/// johnty@infusionsystems.com
///
///

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetFrameRate(60);
    streamData = false;
    isHiRes = false;
    
    if ( ofxMidiIn::getNumPorts() && ofxMidiOut::getNumPorts() ) {
        //only try if we have MIDI ports avaiable...
        
        ofxMidiIn::listPorts();
        ofxMidiOut::listPorts();
        
        //When you have the driver installed and ICubeX Connect running, MIDI ports are created
        // for each digitizer plugged in.
        
        // we need both midi in (getting data from the digitizer)
        //  as well as midi out (setting parameters, querying the digitizer)
        
        //In this example, assume its the first port; check the console printout from above if not...
        int portIndex = 0;
        string line_str = "Connecting to Midi In Port " + ofxMidiIn::getPortList().at(portIndex);
        statusOutputStrs.push_back(line_str);
        myICubeX.connectMidiIn(portIndex);
        
        line_str = "Connecting to Midi Out Port " + ofxMidiOut::getPortList().at(portIndex);
        statusOutputStrs.push_back(line_str);
        myICubeX.connectMidiOut(portIndex);
        
        //now that the ports are connected, we can push some settings to it:
        
        //setting sampling interval:
        myICubeX.setSampleInterval(10); //set 10ms sampling interval (100Hz)
        
        //turn off all the outputs: (by default, they are all on):
        for (int i=0; i<kNUM_ICUBEX_SENSORS; i++)
            myICubeX.setOutput(false, i);
        
        //see the keyPressed function
        statusOutputStrs.push_back("Press SPACE to start/stop streaming data...");
        
        statusOutputStrs.push_back("Press h to toggle hi-resolution...");
        
        statusOutputStrs.push_back("HiRes: OFF");
    }
    else {
        statusOutputStrs.push_back("No MIDI Ports available!");
    }

}

//--------------------------------------------------------------
void ofApp::update(){
    if (streamData) {
        string line_str;
        line_str = "input value = " + ofToString(myICubeX.getSensorData(0));
        sensorValueStrs.push_back(line_str);
    }
    if (sensorValueStrs.size() > 20) { //keep list at 20...
        sensorValueStrs.erase(sensorValueStrs.begin());
    }
    
    if (statusOutputStrs.size() > 20) {
        statusOutputStrs.erase(statusOutputStrs.begin());
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    //just print output on screen
    int line = 0;
    ofSetColor(0, 0, 255);
    for (std::vector<string>::iterator it = statusOutputStrs.begin(); it !=statusOutputStrs.end(); ++it) {
        ofDrawBitmapString(it->c_str(), 10, 20+line*25);
        line++;
    }
    if (streamData) {
        line = 0;
        ofSetColor(255,0,0);
        for (std::vector<string>::iterator it = sensorValueStrs.begin(); it !=sensorValueStrs.end(); ++it) {
            ofDrawBitmapString(it->c_str(), 420, 20+line*25);
            line++;
        }
        
        //map the sensor value to some drawing:
        //for Hi-res mode, sensor input is 0-1023
        // for low-res, sensor input is 0-127
        
        float r;
        if (isHiRes) //we have to scale down the hi-res value
            r = 127 * (float)myICubeX.getSensorData(0)/(float)1023;
        else
            r = myICubeX.getSensorData(0);
        ofSetColor(0,255-r,0+r);
        ofSetCircleResolution(200);
        ofCircle(200, 300, 50 + r);
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == ' ') {
        streamData=!streamData;
        myICubeX.setStream(streamData, 0);
        //use output on port 8 to show streaming status
        myICubeX.setOutput(streamData, 7);
    }
    else if (key == 'h') {
        //toggle resolution
        isHiRes = !isHiRes;
        myICubeX.setHiRes(isHiRes, 0);
        statusOutputStrs.pop_back();
        if (isHiRes) {
            statusOutputStrs.push_back("HiRes: ON");
        }
        else {
            statusOutputStrs.push_back("HiRes: OFF");
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
