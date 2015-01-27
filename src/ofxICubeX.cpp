///
///  ofxICubeX.cpp
///
///
///  Created by Johnty Wang on 2014-09-16. johnty@infusionsystems.com
///
///

#include "ofxICubeX.h"

ofxICubeX::ofxICubeX() {
    is_midi_in_connected_ = false;
    is_midi_out_connected_ = false;
    ofSetLogLevel(OF_LOG_SILENT);

    //set debugging options depending on OF Log Level
    if (ofGetLogLevel() <= OF_LOG_NOTICE) {
        SetDebug(true);
    }
    else {
        SetDebug(false);
    }
}

ofxICubeX::~ofxICubeX() {
    if (midi_in_.isOpen()) {
        ofLogNotice("ofxICubeX:", "closing MIDI IN port %s ...",midi_in_.getName().c_str());
        midi_in_.removeListener(this);
        midi_in_.closePort();
        ofLogNotice("ofxICubeX:", "... MIDI Port Closed");
    }
    if (midi_out_.isOpen()) {
        ofLogNotice("ofxICubeX:", "closing MIDI OUT port %s ...",midi_out_.getName().c_str());
        midi_out_.closePort();
    }
}

void ofxICubeX::connectMidiIn(int port_index) {
    ofLogNotice("ofxICubeX:", "opening MIDI in port %i...",port_index);
    midi_in_.openPort(port_index);
    midi_in_.ignoreTypes(false, true, true); //important for receiving SysEx messages!
    midi_in_.addListener(this);
    is_midi_in_connected_ = true;
    ofLogNotice("ofxICubeX:", "... port opened");
}

void ofxICubeX::connectMidiIn(string port_str) {
    ofLogNotice("ofxICubeX:", "opening MIDI in port %s...",port_str.c_str());
    midi_in_.openPort(port_str);
    midi_in_.ignoreTypes(false, true, true); //important for receiving SysEx messages!
    midi_in_.addListener(this);
    is_midi_in_connected_ = true;
    ofLogNotice("ofxICubeX:", "... port opened");
}

void ofxICubeX::connectMidiOut(int port_index) {
    ofLogNotice("ofxICubeX:", "opening MIDI out port %i...",port_index);
    midi_out_.openPort(port_index);
    is_midi_out_connected_ = true;
    ofLogNotice("ofxICubeX:", "... port opened");
    reset();
}

void ofxICubeX::connectMidiOut(string port_str) {
    ofLogNotice("ofxICubeX:", "opening MIDI out port %s...",port_str.c_str());
    midi_out_.openPort(port_str);
    is_midi_out_connected_ = true;
    ofLogNotice("ofxICubeX:", "... port opened");
}

void ofxICubeX::disconnectMidiIn() {
    if (!midi_in_.isOpen()) return;
    ofLogNotice("ofxICubeX:", "closing MIDI in port %s...", midi_in_.getName().c_str());
    midi_in_.closePort();
    is_midi_in_connected_ = midi_in_.isOpen();
}

void ofxICubeX::disconnectMidiOut() {
    if (!midi_out_.isOpen()) return;
    ofLogNotice("ofxICubeX:", "closing MIDI out port %s...",midi_out_.getName().c_str());
    midi_out_.closePort();
    is_midi_out_connected_ = midi_out_.isOpen();
}


void ofxICubeX::toggleMode() {
    int mode = my_digitizer_state_.GetModeState();
    if (mode == ICUBE_SX_MODE_HOST) {
        SendSetMode(true);
        ofLogNotice("ofxICubeX:", "Sending SA mode");
    }
    else {
        SendSetMode(false);
        ofLogNotice("ofxICubeX:", "Sending HOST mode");
    }
}

void ofxICubeX::dumpMode() {
    ofLogNotice("ofxICubeX:", "Sending dump mode cmd");
    SendDumpMode();
}

void ofxICubeX::dumpVersion() {
    ofLogNotice("ofxICubeX:", "Sending dump version cmd");
    SendDumpVersion();
}

void ofxICubeX::setMode(bool is_standalone) {
    SendSetMode(is_standalone);
}

void ofxICubeX::setSampleInterval(int time_ms) {
    SendSampleInterval(time_ms);
}

void ofxICubeX::setMute(bool is_muted) {
    SendMute(is_muted);
}

void ofxICubeX::toggleMute() {
    SendToggleMute();
}

void ofxICubeX::reset() {
    SendReset();
}

void ofxICubeX::setStream(bool is_on, int port) {
    SendStream(is_on, port);
}

void ofxICubeX::toggleStream(int port) {
    bool on = my_digitizer_state_.GetIsSensorOnState(port);
    SendStream(!on, port);
}

void ofxICubeX::setHiRes(bool is_hi, int port) {
    SendSetHires(is_hi, port);
}

void ofxICubeX::setOutput(bool is_on, int port) {
    SendSetOutput(is_on, port);
}

void ofxICubeX::toggleOutput(int port) {
    bool on = my_digitizer_state_.GetIsActuatorOnState(port);
    SendSetOutput(!on, port);
}

int ofxICubeX::getSensorData(int port) {
    return my_digitizer_state_.GetSensorValState(port);
}


void ofxICubeX::newMidiMessage(ofxMidiMessage &msg) {
    if (msg.bytes.at(0) == 0xF0) {
        //we have SysEx... let interface class handle it
        //TODO: look at ID before parsing for multiple digitizer support?

        //debug output what the thing looks like
        ofLogVerbose("rawSysEx") << "rcv data size = "<< msg.bytes.size()<<endl << "data = ";
        if (ofGetLogLevel() <= OF_LOG_VERBOSE) {
            for (int i=0; i<msg.bytes.size(); i++) {
                std::cout<<"0x"<< std::hex << (int) msg.bytes.at(i)<<" ";
            }
        }


        int pos = 0;
        //break up into individual messages


        std::vector<unsigned char> curr;
        for (std::vector<unsigned char>::iterator it = msg.bytes.begin(); it != msg.bytes.end(); ++it) {
            curr.push_back(*it);
            if (*it == 0xf7) {
            //send it to the sysex parser
                if (ofGetLogLevel() <= OF_LOG_VERBOSE) {
                    std::cout<<" !!! " <<endl;
                }
                ParseSysEx(curr);
                curr.clear();
            }
        }

    }
    else {
        ofLogNotice("rMidi") << msg.channel<<" "<<msg.control<<" "<<msg.value<<endl;

    }

}

void ofxICubeX::newMidiMessage(char *data, int len) {
    if ((unsigned int)data[0] == 0xF0) { //if sysex
        // assemble into vector and feed into parser
        std::vector<unsigned char> data_v;
        for (int i=0; i<len; i++) {
            data_v.push_back(data[i]);
        }
        ParseSysEx(data_v);
    }
}

void ofxICubeX::newSysExStrippedData(char *data, int len) {
    std::vector<unsigned char> data_v;
    data_v.push_back(0xF0);
    for (int i=0; i<len; i++) {
        data_v.push_back(data[i]);
    }
    data_v.push_back(0xF7);
    ParseSysEx(data_v);
}

string ofxICubeX::getVersionString() {
    string res;
    res = "FW: "+ ofToString(my_digitizer_state_.GetVersionFWState()) +
            " HW: " + ofToString(my_digitizer_state_.GetVersionHWState() );
    return res;
}

void ofxICubeX::sendSysExCmd() {
    std::vector<unsigned char> sendBuff = getSysExBuffer();
#ifdef TARGET_ANDROID
    int len = sendBuff.size();
    char data[len];
    for (int i=0; i<len; i++) {
        data[i] = sendBuff.at(i);
    }
    ofxAndroidMidiBridge::sendData(data, len);
#else
    midi_out_.sendMidiBytes(sendBuff);
#endif
}
