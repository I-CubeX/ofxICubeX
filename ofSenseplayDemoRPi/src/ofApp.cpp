#include "ofApp.h"

/// ofSenseplayRPiDemo
///
/// Senseplay Application for interactive advertising
///
/// Requirements:
/// - Raspberry Pi Model B/B+ (ideally running our system image for autoplay, splashscreen, etc)
/// - ttymidi with sysex support (infusion bitbucket)
/// - usb microDig with sensors on port 1 and 8
/// - USB key with videos in /vid and images in /img folders
///
/// Jan 2015
/// johnty@infusionsystems.com
///
///
bool isPlaying = true;
bool isFS = true;

//--------------------------------------------------------------
void ofApp::setup(){

    ofSetLogLevel(OF_LOG_SILENT);

    ofHideCursor();
    ofSetFrameRate(60);
    streamData = true;
    //isHiRes = false;
    isStandalone = false;

    isActive = false;

    setupSensors();


    ofSetFullscreen(isFS);

    //init video settings
    vidSettings.useHDMIForAudio = false;
    vidSettings.enableTexture = true;
    vidSettings.enableLooping = false;
    vidSettings.enableAudio = true;
    vidSettings.listener = this;

    loadVidList();
    loadImgs();
    if (myVidFileList.size()>0) {
        videoCounter = 0;
        vidSettings.videoPath = myVidFileList[videoCounter].path();
        ofLogVerbose() << "loading movie " <<vidSettings.videoPath<<endl;
        vidPlayer.setup(vidSettings);
    }
    ofBackground(0);

}

//--------------------------------------------------------------
void ofApp::update() {
    if (streamData) {
        updateSensorData();
    }
    if (statusOutputStrs.size() > 20) {
        statusOutputStrs.erase(statusOutputStrs.begin());
    }

    if (doLoadNextMovie) {
        loadNextMovie();
    }
}

void ofApp::updateSensorData() {
    //update sensor values
    sensorPresence = myICubeX.getSensorData(0);
    sensorSwipe = myICubeX.getSensorData(7);

    //check for "presence": if person is close enough
    if (sensorPresence > activationThres) {
        isActive = true;
    }
    else {
        isActive = false;
    }

    if (isActive) {
        if ( (mySwipeState == SWIPE_NONE) && (sensorSwipe > activationThres) ) {
            mySwipeState = SWIPE_ENTER;
            printf("enter swipe\n");
        }
        if ( (mySwipeState == SWIPE_ENTER) && (sensorSwipe < (activationThres - HYSTERISIS_OFFSET) ) )  {
            mySwipeState = SWIPE_NONE;
            myClickCount++;
            printf("exit swipe, inc count\n");

        }
    }
}


//--------------------------------------------------------------
void ofApp::draw(){
    //just print output on screen
    int line = 0;
    ofBackground(0);
    ofSetColor(255,255,255);
    if (!isActive) {
        if (vidPlayer.isTextureEnabled) {
            vidPlayer.draw(0, 0, ofGetWidth(), ofGetHeight());
        }
    }
    else { //show slideshow
        ofSetColor(255,0,0);
        ofRect(50, 50, 200, 200);
    }

   // ofSetColor(0, 0, 255);

    if (streamData) {
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == ' ') {
        streamData=!streamData;
        myICubeX.setStream(streamData, 0);
        myICubeX.setStream(streamData, 7);
        myICubeX.setOutput(streamData, 7);
    }
    else if (key == 'h') {
    }
    else if (key == 's') {

    }
    else if (key == 'f') {
        isFS = !isFS;
        ofSetFullscreen(isFS);
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

void ofApp::onVideoEnd(ofxOMXPlayerListenerEventData& e) {
    doLoadNextMovie = true;
    printf("video end; trigger load next\n");
}

void ofApp::loadNextMovie()
{
	if(videoCounter+1<myVidFileList.size())
	{
		videoCounter++;
	}
	else
	{
		videoCounter = 0;
	}
	//skipTimeStart = ofGetElapsedTimeMillis();
	printf("loading new movie...\n");
	vidPlayer.loadMovie(myVidFileList[videoCounter].path());
	printf("...loaded new movie %s\n",myVidFileList[videoCounter].path().c_str());
	//skipTimeEnd = ofGetElapsedTimeMillis();
	//amountSkipped = skipTimeEnd-skipTimeStart;
	//totalAmountSkipped+=amountSkipped;
	doLoadNextMovie = false;
}

void ofApp::loadVidList() {
    string path = "usb/vid";
    ofDirectory dir(path);
    //todo: test these on other platforms!
    dir.allowExt("mp4");
    dir.allowExt("avi");
    dir.allowExt("mov");
    //populate the directory object
    dir.listDir();
    ofLogVerbose() <<" *** Loading Video List ***"<<endl;
    myVidFileList = dir.getFiles();

    for (std::vector<int>::size_type i = 0; i != myVidFileList.size(); i++) {


        ofLogVerbose() <<"added " << "usb/img/"<<myVidFileList[i].getFileName() <<endl;
    }
}

void ofApp::loadImgs() {
    //test directory stuff
    string path = "usb/img";
    ofDirectory dir(path);
    //only show jpg files
    dir.allowExt("jpg");
    //populate the directory object
    dir.listDir();
    vector <ofFile> files = dir.getFiles();
    ofLogVerbose() <<" *** Loading Images ***"<<endl;
    for (std::vector<int>::size_type i = 0; i != files.size(); i++) {
        ofImage img;
        img.loadImage("usb/img/"+files[i].getFileName());
        myImages.push_back(img);
        ofLogVerbose() <<"loaded " << files[i].getFileName() <<endl;
    }
}

bool ofApp::setupSensors() {
    int portInIndex = -1;
	int portOutIndex = -1;

	if (ofxMidiIn::getNumPorts()) {
	    for (int i=0; i<ofxMidiIn::getNumPorts(); i++) {
            string pname = ofxMidiIn::getPortName(i);
            if (ofStringTimesInString(pname, "ICubeX")) {
                printf("port exists!\n");
                portInIndex = i;
            }
        }
	}
	if (ofxMidiOut::getNumPorts()) {
        for (int i=0; i<ofxMidiOut::getNumPorts(); i++) {
            string pname = ofxMidiOut::getPortName(i);
            if (ofStringTimesInString(pname, "ICubeX")) {
                portOutIndex = i; //note: there is a very small chance that when using multiple digitizers
                                // we can have two virtual devices but the in/out port indicies are mixed up?
                                // this is highly unlikely due to use case scenario, but this note is left here
                                // just in case...
            }
        }
	}
	if (portInIndex == -1) return -1;
	if (portOutIndex == -1) return -1;

	myICubeX.connectMidiIn(portInIndex);
	myICubeX.connectMidiOut(portOutIndex);

	//setting sampling interval:
    myICubeX.setSampleInterval(15); //set 15ms sampling interval
    myICubeX.setMode(false); //host mode

    //turn off all the outputs: (by default, they are all on):
    for (int i=0; i<kNUM_ICUBEX_SENSORS; i++)
        myICubeX.setOutput(false, i);


    myICubeX.setStream(streamData, 0);

    return true;

}
