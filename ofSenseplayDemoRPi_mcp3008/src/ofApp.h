#pragma once

#include "ofMain.h"
#include "ofxOMXPlayer.h"
#include "ofxXmlSettings.h"
#include "wiringPi.h"
#include "wiringPiSPI.h"

#define PING_INTERVAL_MINS 60

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
    int readADC(int ch);
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

    string getTimeStamp();

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
    bool isCounting; //timer before triggering "isActive"
    bool isActive;   //are we in slideshow mode?
    int presenseThres;
    int swipeThres;

    unsigned long long lastTime;
    unsigned long long nextTrigger;
    unsigned waitTime_ms;

    int sensorPresence;
    int sensorSwipe;
    int sensorHyst;

    SwipeState mySwipeState;

    bool isStandalone;
    bool isDebug;
};
