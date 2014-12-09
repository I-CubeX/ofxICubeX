#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    
    
    //most of the setup is from the example in ofxMidi
    //allow network MIDI for testing (esp thru simulator!)
    
    isStream = false;
    midiDevStr = "";
    
    maxMessages = 10;
    
    ofxMidi::enableNetworking();
    
    // list the number of available input & output ports
    ofxMidiIn::listPorts();
    ofxMidiOut::listPorts();
    midiDevStr+= "numInputs="+ofToString(ofxMidiIn::getNumPorts());
    
    // create and open input ports
    for(int i = 0; i < ofxMidiIn::getNumPorts(); ++i) {
        
        // new object
        inputs.push_back(new ofxMidiIn);
        inputs[i]->openPort(i);
        
        // set this class to receive incoming midi events
        inputs[i]->addListener(this);
        
        // open input port via port number
        
        if (i == 1) { // USB midi should be second port
            midiDevStr+= "\ndevice " + ofToString(i)+ " = " + inputs[i]->getName()+"\n";
            my_icube.connectMidiIn(1);
            my_icube.connectMidiOut(1);
            messages.push_back("ICubeX input connected. \n tap to toggle streaming on port 1 and 8...");
            my_icube.reset();
        }
        
    }
    
    // create and open output ports
    for(int i = 0; i < ofxMidiOut::getNumPorts(); ++i) {
        
        // new object
        outputs.push_back(new ofxMidiOut);
        
        // open input port via port number
        outputs[i]->openPort(i);
    }
    if ( ofxMidiIn::getNumPorts() < 2) {
        midiDevStr+= "\nNo USB MIDI device detected!";
    }
    
    
    // set this class to receieve midi device (dis)connection events
    ofxMidi::setConnectionListener(this);

    

}

//--------------------------------------------------------------
void ofApp::update(){
    for (int i=0; i<8; i++) {
        my_data[i] = my_icube.getSensorData(i);
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(0);
    
    ofDrawBitmapString("Input:", 10, 20);
    int x = 10, y = 34;
    
    ofDrawBitmapString("midi devices:", 10, 50);
    ofDrawBitmapString(midiDevStr, 10, 80);
    
    //we won't draw the stuff from App's midiIn, but ICube stuff (to follow)
    /*
    messageMutex.lock();
    deque<string>::iterator iter = messages.begin();
    for(; iter != messages.end(); ++iter) {
        ofDrawBitmapString((*iter), x, y);
        y += 14;
    }
    messageMutex.unlock();
     */
    string msg_str = "iCube Data = ";
    for (int i=0; i<8; i++) {
        msg_str+= ofToString(my_data[i])+ " ";
    }
    ofDrawBitmapString(msg_str, x,y);
    
    ofDrawBitmapString("Output:", 10, ofGetHeight()-42);
    

	
}

//--------------------------------------------------------------
void ofApp::exit(){

}

//--------------------------------------------------------------
void ofApp::touchDown(ofTouchEventArgs & touch){

    isStream = !isStream;
    printf("toggle port 1 and 8: on=%i\n",isStream);
    my_icube.setStream(isStream, 0);
    my_icube.setHiRes(true, 0);
    my_icube.setOutput(isStream, 7);
}

//--------------------------------------------------------------
void ofApp::touchMoved(ofTouchEventArgs & touch){

}

//--------------------------------------------------------------
void ofApp::touchUp(ofTouchEventArgs & touch){

}

//--------------------------------------------------------------
void ofApp::touchDoubleTap(ofTouchEventArgs & touch){
    printf("reset!\n");
    if (my_icube.isMidiOutConnected())
        my_icube.reset();

}

//--------------------------------------------------------------
void ofApp::touchCancelled(ofTouchEventArgs & touch){
    
}

//--------------------------------------------------------------
void ofApp::lostFocus(){

}

//--------------------------------------------------------------
void ofApp::gotFocus(){

}

//--------------------------------------------------------------
void ofApp::gotMemoryWarning(){

}

//--------------------------------------------------------------
void ofApp::deviceOrientationChanged(int newOrientation){

}

void ofApp::newMidiMessage(ofxMidiMessage& msg) {
    addMessage(msg.toString());
}

//--------------------------------------------------------------
void ofApp::addMessage(string msg) {
    messageMutex.lock();
    cout << msg << endl;
    messages.push_back(msg);
    while(messages.size() > maxMessages)
        messages.pop_front();
    messageMutex.unlock();
}

//--------------------------------------------------------------
void ofApp::midiInputAdded(string name, bool isNetwork) {
    stringstream msg;
    msg << "ofxMidi: input added: " << name << " network: " << isNetwork;
    addMessage(msg.str());
    
    // create and open a new input port
    ofxMidiIn * newInput = new ofxMidiIn;
    newInput->openPort(name);
    newInput->addListener(this);
    inputs.push_back(newInput);
}

//--------------------------------------------------------------
void ofApp::midiInputRemoved(string name, bool isNetwork) {
    stringstream msg;
    msg << "ofxMidi: input removed: " << name << " network: " << isNetwork << endl;
    addMessage(msg.str());
    
    // close and remove input port
    vector<ofxMidiIn*>::iterator iter;
    for(iter = inputs.begin(); iter != inputs.end(); ++iter) {
        ofxMidiIn * input = (*iter);
        if(input->getName() == name) {
            input->closePort();
            input->removeListener(this);
            delete input;
            inputs.erase(iter);
            break;
        }
    }
}

//--------------------------------------------------------------
void ofApp::midiOutputAdded(string name, bool isNetwork) {
    stringstream msg;
    msg << "ofxMidi: output added: " << name << " network: " << isNetwork << endl;
    addMessage(msg.str());
    
    // create and open new output port
    ofxMidiOut * newOutput = new ofxMidiOut;
    newOutput->openPort(name);
    outputs.push_back(newOutput);
}

//--------------------------------------------------------------
void ofApp::midiOutputRemoved(string name, bool isNetwork) {
    stringstream msg;
    msg << "ofxMidi: output removed: " << name << " network: " << isNetwork << endl;
    addMessage(msg.str());
    
    // close and remove output port
    vector<ofxMidiOut*>::iterator iter;
    for(iter = outputs.begin(); iter != outputs.end(); ++iter) {
        ofxMidiOut * output = (*iter);
        if(output->getName() == name) {
            output->closePort();
            delete output;
            outputs.erase(iter);
            break;
        }
    }
}

