#pragma once

#include "ofMain.h"
#include "ofxMidi.h"
#include "ofxICubeX.h"
#include "ofxOMXPlayer.h"

#define HYSTERISIS_OFFSET 5

class ofApp : public ofBaseApp, public ofxOMXPlayerListener {

//BEGIN Platform Independent code
enum SwipeState {
    SWIPE_NONE,
    SWIPE_ENTER
};

public:
    void setup();
    void update();
    void updateSensorData();
    void draw();

    void loadImgs();
    void loadVidList();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    void onVideoEnd(ofxOMXPlayerListenerEventData& e);
    void onVideoLoop(ofxOMXPlayerListenerEventData& e){ /*empty*/ };
    void loadNextMovie();

private:
    bool setupSensors();

    ofxICubeX myICubeX;
    std::vector<string> statusOutputStrs;
    bool streamData;

    std::vector<ofImage> myImages;
    vector<ofFile> myVidFileList;

    ofxOMXPlayer vidPlayer;
    ofxOMXPlayerSettings vidSettings;
    //managing playback
    int videoCounter;
    bool doLoadNextMovie;

    int myClickCount;
    int currPlayIndex;
    bool isActive;
    int presenseThres;
    int swipeThres;

    int sensorPresence;
    int sensorSwipe;

    SwipeState mySwipeState;

    bool isStandalone;
    bool isDebug;
};
