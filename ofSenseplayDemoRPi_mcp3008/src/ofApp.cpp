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

    ofSetLogLevel(OF_LOG_NOTICE);

    ofHideCursor();
    ofSetFrameRate(60);
    streamData = true;
    //isHiRes = false;
    isStandalone = false;

    isActive = false;

    presenseThres = 15;
    swipeThres = 15;
    mySwipeState = SWIPE_NONE;
    waitTime_ms = 2000;
    isCounting = false;


    //try load from settings file
    ofxXmlSettings settings;
    bool haveSettingsFile = settings.loadFile("usb/sp_settings.xml");
    if (haveSettingsFile) { //check settings file exists
            presenseThres = settings.getValue("settings:PresenseThres", 15);
            swipeThres = settings.getValue("settings:SwipeThres", 15);
            waitTime_ms = settings.getValue("settings:WaitTime", 1500);
            sensorHyst = settings.getValue("settings:Hyst", 2);
            ofLogNotice() << "loaded .xml values: " << presenseThres<<" " << swipeThres<< " " << waitTime_ms <<" " << sensorHyst<<endl;
    }
    else {
        ofLogNotice()<< "usbroot/sp_settings.xml not found; using default values"<<endl;
    }

    if (setupSensors()) {
        printf("wiringPi SPI setup successful!\n");
    }


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
    nextTrigger = ofGetElapsedTimeMillis()+PING_INTERVAL_MINS*60*1000;

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
    if (ofGetElapsedTimeMillis()>=nextTrigger) {
        nextTrigger+=PING_INTERVAL_MINS*60*1000;
        ofLogNotice() << "an hour passed" <<endl;
    }
}

void ofApp::updateSensorData() {
    //update sensor values
    sensorPresence = readADC(0);//myICubeX.getSensorData(0);
    sensorSwipe = readADC(7);//myICubeX.getSensorData(7);
    string outLine = ofToString(sensorPresence)+":"+ofToString(sensorSwipe);
    //statusOutputStrs.push_back(outLine);
    //printf("sensors data = %s\n",outLine.c_str());
    ofLogVerbose() << sensorPresence<<":"<<sensorSwipe;

    //check for "presence": if person is close enough
    //todo: perhaps create an ofxSmartThresh object?
    if ( (sensorPresence > presenseThres) && !isActive) {
        if (!isCounting) { //start counting
            isCounting = true;
            //printf("starting enter timer\n");
            lastTime = ofGetElapsedTimeMillis();
        }
        else { //check if we've remained high for enough
            unsigned long long elapsed = ofGetElapsedTimeMillis() - lastTime;
            if (elapsed >= waitTime_ms) {
                isActive = true;
                isCounting = false;
                string output = getTimeStamp()+ ": user entered";
                //printf("%s\n",output.c_str());
                ofLogNotice() << output<<endl;
                //vidPlayer.setPaused(true);
                myClickCount = 0;
            }
        }

    }
    // low presense sensor value case
    if ( (sensorPresence < (presenseThres - sensorHyst)) ){
        if (isActive) {
            //start timer here, if not counting
            if (!isCounting) {
                isCounting = true;
                lastTime = ofGetElapsedTimeMillis();
            }
            else {
                unsigned long long elapsed = ofGetElapsedTimeMillis() - lastTime;
                if (elapsed >= waitTime_ms) {
                    isActive = false;
                    //printf("user exit\n");
                    string output = getTimeStamp() + ": user exit";
                    //printf("%s\n", output.c_str());
                    ofLogNotice() << output<<endl;
                }

            }

        }
        else { //not active, but counting
            if (isCounting) {
                unsigned long long elapsed = ofGetElapsedTimeMillis() - lastTime;
                if (elapsed >= waitTime_ms) {
                    ofLogVerbose()<<"counter timeout no trigger; reset timer"<<endl;
                    isCounting = false;
                }
            }
        }
    }


    if (isActive) {
        if ( (mySwipeState == SWIPE_NONE) && (sensorSwipe > swipeThres) ) {
            mySwipeState = SWIPE_ENTER;
            ofLogVerbose() << "enter swipe"<<endl;
        }
        if ( (mySwipeState == SWIPE_ENTER) && (sensorSwipe < (swipeThres - sensorHyst) ) )  {
            mySwipeState = SWIPE_NONE;
            myClickCount++;
            string output = getTimeStamp() + ": user swipe";
            //printf("%s\n", output.c_str());
            ofLogNotice() << output<<endl;

        }
    }
    else {
        vidPlayer.setPaused(false);
    }
}


//--------------------------------------------------------------
void ofApp::draw(){
    //just print output on screen
    int line = 0;
    ofBackground(0);
    ofSetColor(255,255,255);
    {
        if (vidPlayer.isTextureEnabled) {
            vidPlayer.draw(0, 0, ofGetWidth(), ofGetHeight());
        }
    }
    if (isActive) {
        if (myClickCount > 0) { //start slideshow on 2nd swipe
             int imgIdx = (myClickCount-1) % myImages.size();
            ofImage* img = &myImages.at(imgIdx);
            img->draw(0, 0, ofGetWidth(), ofGetHeight());
        }
    }
    //set colour of tiny rectangle in corner depending on mode
    if (isActive && isCounting)
        ofSetColor(0, 0, 255); //exiting
    if (isActive && !isCounting)
        ofSetColor(255,0,0); //active
    if (!isActive && isCounting)
        ofSetColor(0, 255, 0); //entering
    if (!isActive && !isCounting)
        ofSetColor(128, 128, 128); //idle
    ofRect(ofGetWidth()-10, ofGetHeight()-10, 5, 5);
    ofSetColor(255,255,255);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == ' ') {
        streamData=!streamData;
        //myICubeX.setStream(streamData, 0);
        //myICubeX.setStream(streamData, 7);
        //myICubeX.setOutput(streamData, 7);
    }
    else if (key == 'h') {
    }
    else if (key == 's') {
    }
    else if (key == 'd') {

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
    ofLogVerbose() << "vid end, trigger next" << endl;
    //printf("video end; trigger load next\n");
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
	//printf("loading new movie... vidC = %i\n", videoCounter);
	vidPlayer.loadMovie(myVidFileList[videoCounter].path());
	//printf("...loaded new movie %s\n",myVidFileList[videoCounter].path().c_str());
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
    dir.sort();
    ofLogVerbose() <<" *** Loading Video List ***"<<endl;
    myVidFileList = dir.getFiles();

    for (std::vector<int>::size_type i = 0; i != myVidFileList.size(); i++) {

        printf("file %i = %s\n", i, myVidFileList[i].getFileName().c_str() );
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
    dir.sort();
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
    if (wiringPiSPISetup (0, 1000000) < 0) 
        return false;
    else 
        return true;

}

int ofApp::readADC(int adcnum)
{
    uint8_t buff[3];
    int adc;
    if ((adcnum > 7) || (adcnum < 0))
        return -1;
    buff[0] = 1;
    buff[1] = (8+adcnum)<<4;
    buff[2] = 0;
    wiringPiSPIDataRW(0, buff, 3);
    adc = ((buff[1]&3) << 8) + buff[2];
    return adc;
}

string ofApp::getTimeStamp() {
    string output_str = ofToString(ofGetYear())+"-"+ofToString(ofGetMonth())+"-"+ofToString(ofGetDay())+"_"+ofToString(ofGetHours())+":"+ofToString(ofGetMinutes())+":"+ofToString(ofGetSeconds());
    return output_str;
}
