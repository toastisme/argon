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
    double TEMPERATURE = 2.0;
    thermCounter = 0;
    drawFont.loadFont("verdana.ttf", 32);
    
    int nx = ceil(BOX_WIDTH/(sqrt(N_PARTICLES)));
    int ny = ceil(BOX_LENGTH/(sqrt(N_PARTICLES)));
    double xspacing = BOX_WIDTH/(nx + 1.0);
    double yspacing = BOX_LENGTH/(ny + 1.0);
    
    std::cout << xspacing << " " << yspacing << " " << nx << " " << ny << "\n";
    
    int i = 1, j = 1;
    for (int n = 0; n < N_PARTICLES; n++){
        if (i % nx == 0) {
            j++;
            i = 0;
        }
        
        theSystem.addParticle(i*xspacing, j*yspacing, 0, 0);
        i++;
    }
    
    theSystem.setConsts(BOX_WIDTH, BOX_LENGTH, CUTOFF, TIMESTEP, TEMPERATURE);
    
    
    // intialise the system + previous positions
    theSystem.forcesEnergies(N_THREADS);
    
    for (int i = 0; i < theSystem.getN(); ++i) {
        prevPos[i] = theSystem.getPos(i);
    }
    theSystem.integrate(N_THREADS);
    
    ofBackground(255, 255, 255);
}

//--------------------------------------------------------------
void ofApp::update(){
    // update
    prevPrevPos = prevPos;
    for (int i = 0; i < theSystem.getN(); ++i) {
        prevPos[i] = theSystem.getPos(i);
    }
    
    theSystem.integrate(N_THREADS);
    if (thermCounter % 10 == 0) {
        theSystem.andersen(0.1);
    }
    thermCounter++;
    drawDataHeight = ofGetHeight() - 5;

}

//--------------------------------------------------------------
void ofApp::draw(){
    vector<double> BOX_SIZE = theSystem.getBox();
    float radius = 35;
    double posx, posy;
    double posprevx, posprevy;
    double posprevprevx, posprevprevy;
    double velx, vely;

    
    ofFill();
    vector<double> tempPos;
    vector<double> tempVel;
    double v_avg = 0.25*sqrt(3*theSystem.getN()*theSystem.getT()) ;
    
    
    for (int i = 0; i < theSystem.getN(); i++){
        tempPos = theSystem.getPos(i);
        tempVel = theSystem.getVel(i);
        
        posx = ofMap(tempPos[0], 0, BOX_SIZE[0], 0, ofGetWidth());
        posy = ofMap(tempPos[1], 0, BOX_SIZE[1], 0, ofGetHeight());
        ofDrawCircle(posx, posy, radius);
        
        posprevx = ofMap(prevPos[i][0], 0, BOX_SIZE[0], 0, ofGetWidth());
        posprevy = ofMap(prevPos[i][1], 0, BOX_SIZE[0], 0, ofGetWidth());
        ofDrawCircle(posprevx, posprevy, radius);
        
        posprevprevx = ofMap(prevPrevPos[i][0], 0, BOX_SIZE[0], 0, ofGetWidth());
        posprevprevy = ofMap(prevPrevPos[i][1], 0, BOX_SIZE[0], 0, ofGetWidth());
        ofDrawCircle(posprevprevx, posprevprevy, radius);
        
        velx = ofMap(tempVel[0], 0, v_avg, 0, 255);
        vely = ofMap(tempVel[1], 0, v_avg, 0, 255);
        ofSetColor(150, velx, vely);
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
