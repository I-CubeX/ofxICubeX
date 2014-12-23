#pragma once

#include "ofMain.h"
#include "ofxMidi.h"
#include "ofxICubeX.h"
#include "ofxOMXPlayer.h"

class ofApp : public ofBaseApp {

public:
    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

private:
    ofxICubeX myICubeX;
    std::vector<string> statusOutputStrs;
    std::vector<string> sensorValueStrs;
    bool streamData;
    bool isHiRes;

    bool isStandalone;
    ofxOMXPlayer omxPlayer;
};
