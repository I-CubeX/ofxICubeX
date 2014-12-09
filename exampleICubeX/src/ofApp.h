#pragma once

#include "ofMain.h"
#include "ofxUI.h"
#include "ofxMidi.h"
#include "ofxICubeX.h"
#include "Math.h"

static const int PANEL_SENSOR_WIDTH = 200;
static const int PANEL_SENSOR_HEIGHT = 300;
static const int PANEL_SPACER = 15;
static const int PANEL_MAIN_WIDTH = 2*PANEL_SENSOR_WIDTH + PANEL_SPACER;
static const int PANEL_MAIN_HEIGHT = 85;

static const float UI_ANIMATION_SPEED = 45.0;
static const int WAVE_SIZE = 256;

enum UIAnimationState {
    UI_ANIMATION_SETTINGS,
    UI_ANIMATION_S2M,
    UI_ANIMATION_MAPPING,
    UI_ANIMATION_M2S
};

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    void exit();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    //ofxUI specific:
    void guiEvent(ofxUIEventArgs &e);


private:
    
    void setupSensorPanels();
    void setupMainPanel();
    void setupMiscPanel();
    void placeSensorPanels();
    void placeSettingPanels();
    
    void setupMappingPanel();
    
    void autoConnectMicroDig();
    
    void drawMapping();
    
    //for Main Panel:
    ofxUISuperCanvas* mainSettingsPanel;
    ofxUIDropDownList* midiInList;
    ofxUIDropDownList* midiOutList;
    ofxUILabelButton* autoConnectBtn;

    //for Misc Settings Panel
    ofxUISuperCanvas* miscSettingsPanel;
    ofxUISlider* sampleIntervalSlider;
    ofxUILabelButton* versionLabel;
    
    //for sensor panels
    ofxUISuperCanvas* sensorPanels[kNUM_ICUBEX_SENSORS];
    ofxUISlider* sensorValueSliders[kNUM_ICUBEX_SENSORS];
    ofxUIToggle* sensorOnToggles[kNUM_ICUBEX_SENSORS];
    ofxUIToggle* actuatorOutToggles[kNUM_ICUBEX_SENSORS];
    ofxUIToggle* hiresToggles[kNUM_ICUBEX_SENSORS];
    
    //for example mapping
    ofxUISuperCanvas* mappingPanel;
    ofxUIWaveform* waveformPanel;
    float wave_buffer[WAVE_SIZE];
    
    ofxICubeX myICubeX;
    
    int y_offset; //for keeping track of vertical GUI placement of items
    float x_offset;
    float x_limit_lower;
    float x_limit_upper;
    
    ofxUIColor colorActive;
    ofxUIColor colorNonActive;
    
    int mappingAlpha;
    
    bool showMapping;
    UIAnimationState myAnimationState;
		
};
