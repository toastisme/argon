#include "ofApp.h"

void ofApp::drawData(string name, double value) {
    char drawstr[255];
    sprintf(drawstr, "%s %lf", name.c_str(), value);
    drawFont.drawString(drawstr, 5, drawDataHeight);
    drawDataHeight -= drawFont.stringHeight(drawstr) + 5;
}

//--------------------------------------------------------------
void ofApp::setup(){
    double BOX_WIDTH = 15.0;
    double BOX_LENGTH = 10.0;
    double CUTOFF = 3.0;
    double TIMESTEP = 0.01;
    int N_PARTICLES = 15;
    thermCounter = 0;
    drawFont.loadFont("verdana.ttf", 32);
    
    for (int i = 0; i < N_PARTICLES; i++) theSystem.addParticle((i+5)/2, ofRandom(1, 9), 0, 0);
    
    theSystem.setConsts(BOX_WIDTH, BOX_LENGTH, CUTOFF, TIMESTEP);
    
    theSystem.forcesEnergies(N_THREADS);
    
    ofBackground(255, 255, 255);
}

//--------------------------------------------------------------
void ofApp::update(){
    theSystem.integrate(N_THREADS);
    if (thermCounter % 10 == 0) {
        theSystem.andersen(1.0, 0.1);
    }
    thermCounter++;
    drawDataHeight = ofGetHeight() - 5;

}

//--------------------------------------------------------------
void ofApp::draw(){
    vector<double> BOX_SIZE = theSystem.getBox();
    float radius = 35;
    double posx, posy;
    
    ofFill();
    vector<double> tempPos;
    for (int i = 0; i < theSystem.getN(); i++){
        tempPos = theSystem.getPos(i);
        posx = ofMap(tempPos[0], 0, BOX_SIZE[0], 0, ofGetWidth());
        posy = ofMap(tempPos[1], 0, BOX_SIZE[1], 0, ofGetHeight());
        ofSetColor(ofRandom(0, 255), 200, ofRandom(0, 255));
        ofDrawCircle(posx, posy, radius);
    }
    
    ofSetColor(0, 0, 0);
    tempPos = theSystem.getPos(1);
    
    drawData("E Kin", theSystem.getEKin());
    drawData("E Pot", theSystem.getEPot());
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

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
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

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
