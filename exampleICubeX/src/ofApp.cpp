#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    colorActive = ofxUIColor(0, 128, 0);
    colorNonActive = ofxUIColor(128, 128, 128);
    x_offset = y_offset = 0;
    x_limit_upper = 0;
    x_limit_lower = -(PANEL_SPACER*2 + PANEL_MAIN_WIDTH * 2);
    setupMainPanel();
    setupMiscPanel();
    setupSensorPanels();
    setupMappingPanel();
    placeSettingPanels();
    placeSensorPanels();

    showMapping = false;
    myAnimationState = UI_ANIMATION_SETTINGS;
    ofSetFrameRate(60);
    
    for (int i=0; i<WAVE_SIZE; i++) {
        wave_buffer[i] = ofNoise(i/100.0);
    }
}

void ofApp::setupMainPanel() {
    ofLogNotice("ofAppF", __FUNCTION__);
    
    mainSettingsPanel = new ofxUISuperCanvas("Settings", OFX_UI_FONT_MEDIUM);
    mainSettingsPanel->setWidth(PANEL_MAIN_WIDTH);
    mainSettingsPanel->setHeight(PANEL_MAIN_HEIGHT);
    
    vector<string> dev_list = ofxMidiIn::getPortList();
    midiInList = new ofxUIDropDownList("MIDI IN           .", dev_list);
    midiInList->setName("MIDI IN");
    mainSettingsPanel->addWidgetDown(midiInList)->setTriggerType(OFX_UI_TRIGGER_END);
    midiInList->setShowCurrentSelected(true);
    midiInList->setAutoClose(true);

    dev_list = ofxMidiOut::getPortList();
    midiOutList = new ofxUIDropDownList("MIDI OUT         .", dev_list);
    midiOutList->setName("MIDI OUT");
    mainSettingsPanel->addWidgetRight(midiOutList)->setTriggerType(OFX_UI_TRIGGER_END);
    //midiOutList = mainSettingsPanel->addDropDownList("MIDI OUT", dev_list, 100);

    midiOutList->setShowCurrentSelected(true);
    midiOutList->setAutoClose(true);
    
    autoConnectBtn = mainSettingsPanel->addLabelButton("Auto Connect", false);
    
    ofAddListener(mainSettingsPanel->newGUIEvent, this, &ofApp::guiEvent);
    
}

void ofApp::setupMiscPanel() {
    ofLogNotice("ofAppF", __FUNCTION__);

    miscSettingsPanel = new ofxUISuperCanvas("Misc", OFX_UI_FONT_MEDIUM);
    miscSettingsPanel->setWidth(PANEL_MAIN_WIDTH);
    miscSettingsPanel->setHeight(PANEL_MAIN_HEIGHT);
    
    sampleIntervalSlider = new ofxUISlider("Sample Interval", 1.0, 1000.0, 0.0, 200, 20);
    miscSettingsPanel->addWidgetDown(sampleIntervalSlider);
    
    versionLabel = new ofxUILabelButton("Get Version", false, 150, 20, 0, 0, OFX_UI_FONT_SMALL);
    miscSettingsPanel->addWidgetRight(versionLabel);
    
    
    ofAddListener(miscSettingsPanel->newGUIEvent, this, &ofApp::guiEvent);
    
    
}

void ofApp::setupSensorPanels() {
    ofLogNotice("ofAppF", __FUNCTION__);
    
    for (int i=0; i<kNUM_ICUBEX_SENSORS; i++) {
        string str_title = "Sensor " + ofToString(i+1);
        sensorPanels[i] = new ofxUISuperCanvas(str_title, OFX_UI_FONT_MEDIUM);
        sensorPanels[i]->autoSizeToFitWidgets();
        
        ofAddListener(sensorPanels[i]->newGUIEvent, this, &ofApp::guiEvent);
        
        str_title = "SensorVal" + ofToString(i);
        sensorValueSliders[i] = new ofxUISlider(str_title, 0.0, 127.0, 0.0, 120, 20);
        //sensorValueSliders[i]->setLabelVisible(false);
        sensorPanels[i]->addWidgetDown(sensorValueSliders[i])->setTriggerType(OFX_UI_TRIGGER_END);
        
        sensorOnToggles[i] = new ofxUIToggle("IsOn", false, 25, 25);
        sensorPanels[i]->addWidgetDown(sensorOnToggles[i]);
        
        actuatorOutToggles[i] = new ofxUIToggle("Out", true, 25, 25);
        sensorPanels[i]->addWidgetRight(actuatorOutToggles[i])->setTriggerType(OFX_UI_TRIGGER_END);
        
        hiresToggles[i] = new ofxUIToggle("HiRes", false, 25, 25);
        sensorPanels[i]->addWidgetRight(hiresToggles[i])->setTriggerType(OFX_UI_TRIGGER_END);
        
    }
    placeSensorPanels();
    
}

void ofApp::setupMappingPanel() {
    mappingPanel = new ofxUISuperCanvas("Example Mapping", OFX_UI_FONT_MEDIUM);
    mappingPanel->setPosition(2*PANEL_SPACER, 2*PANEL_SPACER);
    mappingPanel->setWidth(ofGetWidth() - 4*PANEL_SPACER);
    mappingPanel->setHeight(ofGetHeight() - 4*PANEL_SPACER);
    mappingPanel->setVisible(false);
    mappingPanel->addWaveform("WAVEFORM", wave_buffer, WAVE_SIZE, -1.0, 1.0);
    
}

void ofApp::placeSensorPanels() {
    y_offset = PANEL_MAIN_HEIGHT + PANEL_SPACER;
    for (int i=0; i<kNUM_ICUBEX_SENSORS; i++) {
        sensorPanels[i]->setWidth(PANEL_SENSOR_WIDTH);
        sensorPanels[i]->setHeight(PANEL_SENSOR_HEIGHT);
        sensorPanels[i]->setPosition(x_offset + PANEL_SPACER + (i%4)*(PANEL_SENSOR_WIDTH+PANEL_SPACER),
                                     y_offset + PANEL_SPACER + (i/4)*(PANEL_SENSOR_HEIGHT+PANEL_SPACER));
    }
    //this is how much vertical space the above sensors took:
    y_offset += (kNUM_ICUBEX_SENSORS/4) * PANEL_SENSOR_HEIGHT + PANEL_SPACER;
}

void ofApp::placeSettingPanels() {
    y_offset = PANEL_SPACER;
    mainSettingsPanel->setPosition(x_offset + PANEL_SPACER, y_offset);
    miscSettingsPanel->setPosition(x_offset + PANEL_MAIN_WIDTH + 2*PANEL_SPACER, y_offset);
}

void ofApp::exit() {
    //free UI stuff
    ofLogNotice("ofAppF", __FUNCTION__);
    for (int i=0; i<kNUM_ICUBEX_SENSORS; i++) {
        if (sensorPanels[i] != NULL) {
            ofRemoveListener(sensorPanels[i]->newGUIEvent, this, &ofApp::guiEvent);
            delete sensorPanels[i];
        }
    }
}

//--------------------------------------------------------------
void ofApp::update(){

    //refresh internal state to UI
    // 1.) sensor values
    // 2.) on/off indicator
    // 3.) hi/low res indicator
    // 4.) UI stuf
    
    if ( (myAnimationState == UI_ANIMATION_M2S) || (myAnimationState == UI_ANIMATION_S2M) ) {
        if (myAnimationState == UI_ANIMATION_M2S) {
            //moving from Mapping to Settings
            x_offset += UI_ANIMATION_SPEED;
            if (x_offset > x_limit_upper) {
                myAnimationState = UI_ANIMATION_SETTINGS;
                x_offset = 0;
                mappingPanel->setVisible(false);
            }
        }
        else {
            x_offset -= UI_ANIMATION_SPEED;
            if (x_offset < x_limit_lower) {
                myAnimationState = UI_ANIMATION_MAPPING;
                x_offset = x_limit_lower;
                mappingPanel->setVisible(true);
            }

            
        }
        mappingAlpha = - 255 * (float) x_offset / (float) (x_limit_upper - x_limit_lower);
        placeSettingPanels();
        placeSensorPanels();
    }
    if (myAnimationState == UI_ANIMATION_MAPPING) {
        for(int i = 0; i < WAVE_SIZE; i++) {
            int f =172.265625; //this gets a single period in 256 samples at 44.1k
            if (myICubeX.isMidiInConnected()) {
                if (myICubeX.isSensorOn(0)) //if sensor connected
                    f = 2 * f* (float)myICubeX.getSensorData(0) / 127.0 + 440;
                //todo: some basic graphical mapping here.
            }
            wave_buffer[i] = sin((float)i*f*2*pi/(44100));
        }
    }
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    for (int i=0; i<kNUM_ICUBEX_SENSORS; i++) {
        if (myICubeX.isSensorOn(i)) {
            sensorPanels[i]->setColorBack(colorActive);
            sensorOnToggles[i]->setValue(true);
            sensorValueSliders[i]->setValue(myICubeX.getSensorData(i));
        }
        else {
            sensorPanels[i]->setColorBack(colorNonActive);
            sensorOnToggles[i]->setValue(false);
        }
        actuatorOutToggles[i]->setValue(myICubeX.isActuatorOn(i));
    }
    if (mappingAlpha > 0) {
        drawMapping();
    }

}

void ofApp::guiEvent(ofxUIEventArgs &e) {
    ofLogNotice("ofAppF", "UIEvent from %s", e.getName().c_str());
    if (e.getName() == "IsOn") {
        for (int i=0; i<kNUM_ICUBEX_SENSORS; i++) {
            if (e.getToggle() == sensorOnToggles[i]) {
                ofLogNotice("UI", "toggle %i\n",i);
                myICubeX.toggleStream(i);
                return; //no need to check rest; we're done.
            }
        }
    }
    else if (e.getName() == "Out") {
        for (int i=0; i<kNUM_ICUBEX_SENSORS; i++) {
            if (e.getToggle() == actuatorOutToggles[i]) {
                myICubeX.toggleOutput(i);
                return; 
            }
        }
    }
    else if (e.getName() == "HiRes") {
        for (int i=0; i<kNUM_ICUBEX_SENSORS; i++) {
            if (e.getToggle() == hiresToggles[i]) {
                myICubeX.setHiRes(hiresToggles[i]->getValue(), i);
                if (hiresToggles[i]->getValue()) {
                    //need to change scale of scrollbar
                    sensorValueSliders[i]->setMax(1023);
                }
                else {
                    sensorValueSliders[i]->setMax(127);
                    
                }
                return;
            }
        }
    }
    else if (e.getName() == "Auto Connect") {
        autoConnectMicroDig();
    }
    //TODO: the droplist behaviour for port selection creates some redundant extra open/closes; take a look at this.
    else if (e.widget == midiInList) {
        vector<ofxUIWidget *> &selected = midiInList->getSelected();
        for(int i = 0; i < selected.size(); i++)
        {
            if (myICubeX.isMidiInConnected()) {
                myICubeX.disconnectMidiIn();
            }
            ofLogNotice("ofAppF", "Connecting to midi in %s\n", selected[i]->getName().c_str());
            myICubeX.connectMidiIn(selected[i]->getName());
        }
    }
    else if (e.getName() == "Sample Interval") {
        int val = (int) sampleIntervalSlider->getValue();
        myICubeX.setSampleInterval(val);
    }
    else if (e.getName() == "Get Version") {
        if (myICubeX.isMidiInConnected() && myICubeX.isMidiOutConnected()) {
            string label_str = ofToString(myICubeX.getVersionString());
            versionLabel->setLabelText(label_str);
        }
    }
    else if (e.widget == midiOutList) {
        vector<ofxUIWidget *> &selected = midiOutList->getSelected();
        for(int i = 0; i < selected.size(); i++)
        {
            if (myICubeX.isMidiOutConnected()) {
                myICubeX.disconnectMidiOut();
            }
            ofLogNotice("ofAppF", "Connecting to midi out %s\n", selected[i]->getName().c_str());
            myICubeX.connectMidiOut(selected[i]->getName());
            myICubeX.reset();
            myICubeX.setMode(false);
            
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == '2') {
        myAnimationState = UI_ANIMATION_S2M;
    }
    else if (key == '1') {
        myAnimationState = UI_ANIMATION_M2S;
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

void ofApp::autoConnectMicroDig() {
    //connect to the first "USB-microDig XXXX" device found
    vector<string> inList = ofxMidiIn::getPortList();
    vector<string> outList = ofxMidiOut::getPortList();
    
    int input_index = -1;
    for (int i=0; i<inList.size(); i++) {
        string curr_str = inList[i];
        if (curr_str.find("USB-microDig") != string::npos) {
            input_index = i;
            ofLogNotice("ofAppF:", "%s: Found USB-microDig input port!\n", __FUNCTION__);
        }
    }
    int output_index = -1;
    if (input_index >= 0) {
        for (int i=0; i<outList.size(); i++) {
            string curr_str = inList[i];
            if (curr_str.find("USB-microDig") != string::npos) {
                output_index = i;
                ofLogNotice("ofAppF:", "%s: Found USB-microDig output port!\n", __FUNCTION__);
            }
        }
    }
    if ( (input_index >= 0) && (output_index >= 0) ) {
        midiInList->setName(inList[input_index]);
        midiOutList->setName(outList[output_index]);
        myICubeX.connectMidiIn(input_index);
        myICubeX.connectMidiOut(input_index);
        myICubeX.setSampleInterval(100);
        sampleIntervalSlider->setValue(100);
        myICubeX.SendDumpVersion();
    }
}

void ofApp::drawMapping() {

    ofSetColor(25, 25, 25, mappingAlpha);
    ofRect(2*PANEL_SPACER, 2*PANEL_SPACER, ofGetWidth() - 4*PANEL_SPACER, ofGetHeight()-4*PANEL_SPACER);
}