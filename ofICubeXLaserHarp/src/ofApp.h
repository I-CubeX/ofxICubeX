#pragma once

#include "ofMain.h"
#include "ofxStk.h"
#include "DrawSynth.h"
#include "ofxGui.h"
#include "ofxICubeX.h"

#ifdef TARGET_RASPBERRY_PI
#include "wiringPi.h"
#include "wiringPiSPI.h"
#endif //TARGET_RASPBERRY_PI

static const char keyMap[] = {'a', 's', 'd', 'f', 'g', 'h', 'j', 'k'};

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
    
    void loadWavePreset(int preset);
    
    void updateSensors();
    
    
    void audioOut(float *output, int bufferSize, int nChannels);
    
    // callback evertime user changes adsr envelope
    void adsrChanged(float &val);
    
    // checks if any keys are being held down;
    bool noNotesDown()const;
    
    // prevents the user from drawing illegal waves
    void cutWaveToPoint(ofPoint cutPoint);
    
    // creates a 5000 size waveTable from the wave drawing
    stk::StkFrames createWaveTableFromDrawing();
    
    // takes the yValue inside the drawing region and converts it to
    // range -1 to 1, where -1 is rext bottom and 1 is rect top
    float transformYValue(float yValue)const ;
    
    ofPolyline wave;
    ofRectangle drawRegion;
    
    DrawSynth drawSynth;
    
    ofxICubeX myICubeX;
    
    bool drawingBegan;
    bool keyDown;
    
    ofxPanel gui;
    ofxFloatSlider attack;
    ofxFloatSlider decay;
    ofxFloatSlider sustain;
    ofxFloatSlider release;
    ofxFloatSlider gain;
    
    ofTrueTypeFont instructions;
    
    bool aDown;
    bool sDown;
    bool dDown;
    bool fDown;
    bool gDown;
    bool hDown;
    bool jDown;
    bool kDown;
    bool lDown;
    
    int sensorVals[kNUM_ICUBEX_SENSORS];
    
#if defined(TARGET_OSX) || defined(TARGET_WIN32)
    ofxICubeX myICube;
#endif // #if defined(TARGET_OSX) || defined(TARGET_WIN32)
    
#if defined(TARGET_RASPBERRY_PI)
    int readADC(int adcnum);
#endif //defined(TARGET_RASPBERRYPI)

};
