#pragma once

#include "ofMain.h"
#include "ofxICubeX.h"
#include "ofxOMXPlayer.h"

#ifdef TARGET_OF_IPHONE
#include "ofxiOS.h"
#include "ofxiOSExtras.h"
#endif //TARGET_OF_IPHONE

#ifdef TARGET_ANDROID
#include "ofxAndroid.h"
#include "ofxAndroidMidiBridge.h"
#endif //TARGET_ANDROID

#define HYSTERISIS_OFFSET 20

#ifdef TARGET_OF_IPHONE
class ofApp : public ofxiOSApp {
#elif defined(TARGET_ANDROID)
    class ofApp : public ofxAndroidApp{
#else
        class ofApp : public ofBaseApp {
#endif
            
        public:
            //BEGIN Platform Independent code
            enum SwipeState {
                SWIPE_NONE,
                SWIPE_ENTER
            };
            
            void setup();
            void update();
            void draw();
            void exit();
            
            void loadImgs();
            void loadVids();
            
            void updateSensorData();
            //END Platform Independent code
            
            //BEGIN Desktop-specific code
            void keyPressed(int key);
            void keyReleased(int key);
            void mouseMoved(int x, int y );
            void mouseDragged(int x, int y, int button);
            void mousePressed(int x, int y, int button);
            void mouseReleased(int x, int y, int button);
            void windowResized(int w, int h);
            void dragEvent(ofDragInfo dragInfo);
            void gotMessage(ofMessage msg);
            //END Desktop-specific code
            
#ifdef TARGET_OF_IPHONE
            //BEGIN iOS-specific code
            void touchDown(ofTouchEventArgs & touch);
            void touchMoved(ofTouchEventArgs & touch);
            void touchUp(ofTouchEventArgs & touch);
            void touchDoubleTap(ofTouchEventArgs & touch);
            void touchCancelled(ofTouchEventArgs & touch);
            
            void lostFocus();
            void gotFocus();
            void gotMemoryWarning();
            void deviceOrientationChanged(int newOrientation);
            //END iOS-specific code
#endif //TARGET_OF_IPHONE
            
#ifdef TARGET_ANDROID
            void touchDown(int x, int y, int id);
            void touchMoved(int x, int y, int id);
            void touchUp(int x, int y, int id);
            void touchDoubleTap(int x, int y, int id);
            void touchCancelled(int x, int y, int id);
            void swipe(ofxAndroidSwipeDir swipeDir, int id);
            
            bool menuItemSelected(char* menu_id);
            
            void pause();
            void stop();
            void resume();
            void reloadTextures();
            
            bool backPressed();
            void okPressed();
            void cancelPressed();
            
            //JNI midi data
            void onArray(char* data, int len);
            ofMutex mylock;
            char* recv_data;
            
#endif //TARGET_ANDROID
            
            //BEGIN Platform Independent code
        private:
            ofxICubeX myICubeX;
            std::vector<string> statusOutputStrs;
            bool streamData;
            
            std::vector<ofImage> myImages;
            //std::vector<ofVideoPlayer> myVideos;
            int myClickCount;
            int currPlayIndex;
            bool isActive;
            int activationThres;
            int sensorPresence;
            
            int sensorSwipe;
            
            SwipeState mySwipeState;
            
            //move the video around (why not)
            float vidX;
            float vidY;
            float dX;
            float dY;
            //END Platform Independent code

	    std::vector<ofxOMXPlayer> myVideos;
            std::vector<ofxOMXPlayerSettings> myVideoSettings;            
        };
