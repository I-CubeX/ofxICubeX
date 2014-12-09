#include "ofApp.h"

/// ofSenseplayDemoIOS: openFrameworks demo app for interfacing with iCubeX digitizers
///
/// iOS Version: tested with iPad retina + iRig MIDI 2 inteface + USB Digitizer
///
/// Make sure the USB driver is installed, and the ICubeX Connect application is running!
///
/// This is a demo SensePlay application example showing how to connect to, configure, and get data
/// from an ICubeX digitizer to control the playback of images and videos.
///
/// On the desktop version, it works with a USB-microDig, Wi-microDig, and
/// the MIDI Digitizer as well. By default it connects to the first available MIDI port.
/// See the results of the ofxMidiX::listPorts() for available ports in your system.
///
///
/// Nov 2014
/// johnty@infusionsystems.com
///
///

//--------------------------------------------------------------
//BEGIN Platform independent code
void ofApp::setup(){
    
    //todo: port management - gui perhaps?
    int portInIndex = 0;
    int portOutIndex = 0;
    
#ifdef TARGET_OF_IPHONE
    ofSetOrientation(OF_ORIENTATION_90_RIGHT);
    portInIndex = 1; //0 is always network midi!
    portOutIndex = 1;
#endif
    
    vidX = 0;
    vidY = 0;
    dX = ofRandom(0.5, 1);
    dY = ofRandom(0.5, 1);
    
    ofSetFullscreen(true);
    
    ofSetLogLevel(OF_LOG_VERBOSE);
    myICubeX.SetDebug(false);
    
    myClickCount = 0;
    isActive = false;
    activationThres = 50;
    mySwipeState = SWIPE_NONE;
    
    loadImgs();
    loadVids();
    
    ofSetFrameRate(60);
    streamData = false;
    
#ifdef TARGET_ANDROID
    //Android init: slightly different since we're using AndroidMIDI java on the other side,
    // and the port connection is handled by the Java code/Android GUI
    
    // Specifically, we may not have obtained MIDI port immediately on init, so we need to set configuration
    // of the digitizer after we've connected the port on the Android side.
    // (it would be possible to auto-connect to available ports on the java side, but still between initiation,
    // permission granting, and actual availability of port there will be some delay and we can't guarantee
    //
    
#else
    //OSX, Windows, iOS case: this is more "standard" use of ofxMidi as we have the same library
    // functionalities to work with....
    
    if ( (portInIndex < ofxMidiIn::getNumPorts()) && (portOutIndex < ofxMidiOut::getNumPorts()) ) {
        //only try if we have MIDI ports avaiable...
        
        ofxMidiIn::listPorts();
        ofxMidiOut::listPorts();
        
        //When you have the driver installed and ICubeX Connect running, MIDI ports are created
        // for each digitizer plugged in.
        
        // we need both midi in (getting data from the digitizer)
        //  as well as midi out (setting parameters, querying the digitizer)
        
        // NOTE: by default we simply connect to the first in and out port available.
        // on iOS the first input port is often the "Network MIDI" device. So the connected
        // USB MIDI hardware will use port 1 for input, and port 0 for output.
        //
        //
        
        string line_str = "Connecting to Midi In Port " + ofxMidiIn::getPortList().at(portInIndex);
        statusOutputStrs.push_back(line_str);
        myICubeX.connectMidiIn(portInIndex);
        
        line_str = "Connecting to Midi Out Port " + ofxMidiOut::getPortList().at(portOutIndex);
        statusOutputStrs.push_back(line_str);
        myICubeX.connectMidiOut(portOutIndex);
        
        //now that the ports are connected, we can push some settings to it:
        
        //setting sampling interval:
        myICubeX.setSampleInterval(15); //set 15ms sampling interval
        myICubeX.setMode(false); //host mode
        
        //turn off all the outputs: (by default, they are all on):
        for (int i=0; i<kNUM_ICUBEX_SENSORS; i++)
            myICubeX.setOutput(false, i);
        
        //see the keyPressed function
        statusOutputStrs.push_back("Press SPACE (or \"double tap\" on mobile device) to start/stop streaming data...");
        
    }
    else {
        statusOutputStrs.push_back("No MIDI Ports available!");
    }
#endif
}

//--------------------------------------------------------------
void ofApp::update(){
    
    if (streamData) {
        updateSensorData();
    }
    if (statusOutputStrs.size() > 20) {
        statusOutputStrs.erase(statusOutputStrs.begin());
    }
    
    for (std::vector<ofVideoPlayer>::iterator it = myVideos.begin(); it != myVideos.end(); ++it) {
        if (it->isPlaying()) {
            it->update();
        }
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
    
    int img_idx = myClickCount % myImages.size();
    
    ofSetBackgroundColor(128, 128, 128);
    if (myImages.size() > 0) {
        myImages.at(img_idx).draw(0, 0, ofGetWidth(), ofGetHeight());
    }
    
    int line = 0;
    ofSetColor(0, 0, 255);
    //Draw the status messages (for testing)
    for (std::vector<string>::iterator it = statusOutputStrs.begin(); it !=statusOutputStrs.end(); ++it) {
        ofDrawBitmapString(it->c_str(), 10, 20+line*25);
        line++;
    }
    if (streamData) {
        line = 0;
        ofSetColor(255,0,0);
        string line_str;
        line_str = "input value = " + ofToString(sensorPresence) + " " + ofToString(sensorSwipe);
        ofDrawBitmapString(line_str, 420, 20+line*25);
        
        //Color of circle indicates "presence".
        if (isActive)
            ofSetColor(255, 0, 0);
        else
            ofSetColor(0, 255, 0);
        
        ofSetCircleResolution(50);
        ofCircle(0, 0, 50);
        
        if (myVideos.size() > 0) {
            int vid_curr_idx = (2+currPlayIndex) % myVideos.size();
            int vid_idx = (2+myClickCount) % myVideos.size();
            if (!isActive) {
                myVideos.at(vid_curr_idx).stop();
            }
            if (vid_idx != vid_curr_idx) { // we need to trigger swapping of videos
                myVideos.at(vid_curr_idx).stop();
                myVideos.at(vid_curr_idx).setPosition(0.0);
                myVideos.at(vid_idx).play();
            }
            ofSetColor(255);
            if (myVideos.at(vid_idx).isPlaying()) {
                vidX+=dX;
                vidY+=dY;
                myVideos.at(vid_idx).draw(vidX, vidY,
                                          myVideos.at(vid_idx).getWidth()/2, myVideos.at(vid_idx).getHeight()/2);
                if ( (vidX<0) || (vidX+myVideos.at(vid_idx).getWidth()/2 > ofGetWidth()) ) dX = - dX;
                if ( (vidY<0) || (vidY+myVideos.at(vid_idx).getHeight()/2 > ofGetHeight()) ) dY = -dY;
            }
        }
        
    }
    ofSetColor(255);
    
    currPlayIndex = myClickCount;
    
}

void ofApp::exit() {
    //deallocate videos and images
    for (std::vector<ofVideoPlayer>::iterator it = myVideos.begin(); it != myVideos.end(); ++it) {
        if (it->isPlaying()) {
            it->stop();
        }
        it->close();
    }
    
    for (std::vector<ofImage>::iterator it = myImages.begin(); it != myImages.end(); ++it) {
        it->clear(); //probably not necessary, but anyhoo...
    }
    
    printf("exit app\n");
    
}

void ofApp::loadImgs() {
    //test directory stuff
    string path = "img";
    ofDirectory dir(path);
    //only show jpg files
    dir.allowExt("jpg");
    //populate the directory object
    dir.listDir();
    vector <ofFile> files = dir.getFiles();
    
    for (std::vector<int>::size_type i = 0; i != files.size(); i++) {
        ofImage img;
        img.loadImage("img/"+files[i].getFileName());
        myImages.push_back(img);
        ofLogNotice() <<"loaded " << files[i].getFileName() <<endl;
    }
}

void ofApp::loadVids() {
    //test directory stuff
    string path = "vid";
    ofDirectory dir(path);
    //todo: test these on other platforms!
    dir.allowExt("mp4");
    dir.allowExt("avi");
    dir.allowExt("mov");
    //populate the directory object
    dir.listDir();
    vector <ofFile> files = dir.getFiles();
    
    for (std::vector<int>::size_type i = 0; i != files.size(); i++) {
        ofVideoPlayer vid;
        vid.loadMovie("vid/"+files[i].getFileName());
        vid.setLoopState(OF_LOOP_NORMAL);
        myVideos.push_back(vid);
        ofLogNotice() <<"loaded " << files[i].getFileName() <<endl;
    }
}
//END Platform independent code

//BEGIN Desktop specific code

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == ' ') {
        streamData=!streamData;
        //turn on inputs 1 and 8 (0 and 7)
        myICubeX.setStream(streamData, 0);
        myICubeX.setStream(streamData, 7);
        
        //use output on port 8 to show streaming status (why not)
        myICubeX.setOutput(streamData, 7);
    }
    if (key == 'p') {
        myClickCount++;
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
//END Desktop only code

//BEGIN iOS specific code
//--------------------------------------------------------------
void ofApp::touchDown(ofTouchEventArgs & touch){
    printf("%f %f\n", touch.x, touch.y);
    
}

//--------------------------------------------------------------
void ofApp::touchMoved(ofTouchEventArgs & touch){
    printf("%f %f\n", touch.x, touch.y);
}

//--------------------------------------------------------------
void ofApp::touchUp(ofTouchEventArgs & touch){
    
}

//--------------------------------------------------------------
void ofApp::touchDoubleTap(ofTouchEventArgs & touch){
    streamData=!streamData;
    //turn on inputs 1 and 8 (0 and 7)
    myICubeX.setStream(streamData, 0);
    myICubeX.setStream(streamData, 7);
    
    //use output on port 8 to show streaming status (why not)
    myICubeX.setOutput(streamData, 7);
}

//--------------------------------------------------------------
void ofApp::touchCancelled(ofTouchEventArgs & touch){
    
}

//--------------------------------------------------------------
void ofApp::lostFocus(){
    
}

//--------------------------------------------------------------
void ofApp::gotFocus(){
    
}

//--------------------------------------------------------------
void ofApp::gotMemoryWarning(){
    
}

//--------------------------------------------------------------
void ofApp::deviceOrientationChanged(int newOrientation){
    
}
//END iOS specific code

#ifdef TARGET_ANDROID
//BEGIN ANDROID-specific
//--------------------------------------------------------------
void ofApp::touchDown(int x, int y, int id){
    //output_msg += ofToString(x)+":"+ofToString(y)+"\n";
    //ofxAndroidMidiBridge::testTrigger();
    myICubeX.setMode(false); //host mode
    myICubeX.setStream(true, 7);
}

//--------------------------------------------------------------
void ofApp::touchMoved(int x, int y, int id){
}

//--------------------------------------------------------------
void ofApp::touchUp(int x, int y, int id){
    
}

//--------------------------------------------------------------
void ofApp::touchDoubleTap(int x, int y, int id){
    streamData=!streamData;
    //turn on inputs 1 and 8 (0 and 7)
    myICubeX.setStream(streamData, 0);
    myICubeX.setStream(streamData, 7);
    
    //use output on port 8 to show streaming status (why not)
    myICubeX.setOutput(streamData, 7);
}

//--------------------------------------------------------------
void ofApp::touchCancelled(int x, int y, int id){
}

//--------------------------------------------------------------
void ofApp::swipe(ofxAndroidSwipeDir swipeDir, int id){
}

bool ofApp::menuItemSelected(char* menu_id) {
    return true;
}

//--------------------------------------------------------------
void ofApp::pause(){
}

//--------------------------------------------------------------
void ofApp::stop(){
    
}

//--------------------------------------------------------------
void ofApp::resume(){
}

//--------------------------------------------------------------
void ofApp::reloadTextures(){
    
}

//--------------------------------------------------------------
bool ofApp::backPressed(){
    return false;
}

//--------------------------------------------------------------
void ofApp::okPressed(){
    
}

//--------------------------------------------------------------
void ofApp::cancelPressed(){
    
}

void ofApp::onArray(char* data, int len) {
    int data_len = len;
    mylock.lock();
    if (recv_data != NULL)
        delete recv_data;
    recv_data = new char[data_len];
    memcpy(recv_data, data, len);
    mylock.unlock();
    myICubeX.newSysExStrippedData(data, len);
    
}
//END ANDROID-specific
#endif //TARGET_ANDROID