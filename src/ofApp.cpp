#include "ofApp.h"

void ofApp::drawData(string name, double value) {
    char drawstr[255];
    sprintf(drawstr, "%s %lf", name.c_str(), value);
    drawFont.drawString(drawstr, 5, drawDataHeight);
    drawDataHeight -= drawFont.stringHeight(drawstr) + 5;
}

void randomiseVelocity(vector<double> &vel, double T) {
    double sigma = sqrt(T);
    
    for (int i = 0; i < vel.size(); ++i) {
        double randr = ofRandom(1);
        double randt = ofRandom(1);
        
        randr = sqrt(-2.0 * log(randr));
        randt = sin(2 * PI * randt);
        
        vel[i] = sigma * randr * randt;
    }
}

//--------------------------------------------------------------
void ofApp::setup(){
    double BOX_WIDTH = 15.0;
    double BOX_LENGTH = 10.0;
    double CUTOFF = 3.0;
    double TIMESTEP = 0.01;
    int N_PARTICLES = 100;
    double TEMPERATURE = 2.0;
    thermCounter = 0;
    drawFont.loadFont("verdana.ttf", 32);
    
    double box_ratio = BOX_WIDTH / BOX_LENGTH;
    int n_grid_x = ceil(sqrt(N_PARTICLES * box_ratio));
    int n_grid_y = ceil(sqrt(N_PARTICLES / box_ratio));
    double xspacing = BOX_WIDTH  / (n_grid_x);
    double yspacing = BOX_LENGTH / (n_grid_y);
    
    std::cout << xspacing << " " << yspacing << " " << n_grid_x << " " << n_grid_y << "\n";
    
    int i = 0, j = 0;
    double posx, posy;
    vector<double> vel = {0, 0};
    for (int n = 0; n < N_PARTICLES; ++n){
        posx = xspacing * (i + 0.5);
        posy = yspacing * (j + 0.5);
        
        printf("%d %d %lf %lf\n", i, j, posx, posy);
        randomiseVelocity(vel, TEMPERATURE);
        theSystem.addParticle(posx, posy, vel[0], vel[1]);
        
        ++i;
        i = i % n_grid_x;
        if (i == 0) ++j;
    }
    
    theSystem.setConsts(BOX_LENGTH, BOX_WIDTH, CUTOFF, TIMESTEP, TEMPERATURE);
    
    
    // intialise the system + previous positions
    theSystem.forcesEnergies(N_THREADS);
    
    for (int i = 0; i < 20; ++i) {
        theSystem.integrate(N_THREADS);
    }
    
    ofBackground(255, 255, 255);
}

//--------------------------------------------------------------
void ofApp::update(){
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
    double posx, posy;
    double pospx, pospy;
    double posppx, posppy;
    double velx, vely;
    double accx, accy;

    
    ofFill();
    vector<double> tempPos;
    vector<double> tempPosPrev;
    vector<double> tempPosPrevPrev;
    vector<double> tempVel;
    vector<double> tempAcc;
    double v_avg = 0.25*sqrt(3*theSystem.getN()*theSystem.getT()) ;
    
    for (int i = 0; i < theSystem.getN(); i++){
        tempPos = theSystem.getPos(i);
        tempPosPrev = theSystem.getPreviousPositions(i, 15);
        tempPosPrevPrev = theSystem.getPreviousPositions(i, 10);
        tempVel = theSystem.getVel(i);
        tempAcc = theSystem.getForces(i);
        
        posx = ofMap(tempPos[0], 0, BOX_SIZE[0], 0, ofGetWidth());
        posy = ofMap(tempPos[1], 0, BOX_SIZE[1], 0, ofGetHeight());
        
        pospx = ofMap(tempPosPrev[0], 0, BOX_SIZE[0], 0, ofGetWidth());
        pospy = ofMap(tempPosPrev[1], 0, BOX_SIZE[1], 0, ofGetHeight());
        
        posppx = ofMap(tempPosPrevPrev[0], 0, BOX_SIZE[0], 0, ofGetWidth());
        posppy = ofMap(tempPosPrevPrev[1], 0, BOX_SIZE[1], 0, ofGetHeight());
        
        velx = ofMap(abs(tempVel[0]), 0, v_avg, 0, 255);
        vely = ofMap(abs(tempVel[1]), 0, v_avg, 0, 255);
        
        accx = ofMap(log(1.0+abs(tempAcc[0])), 0, 10, 20, 50);
        accy = ofMap(log(1.0+abs(tempAcc[1])), 0, 10, 20, 50);
        
        ofSetColor(150, velx, vely);
        
        ofDrawEllipse(posx, posy, accx, accy);
        ofDrawEllipse(pospx, pospy, accx * 0.8, accy * 0.8);
        ofDrawEllipse(posppx, posppy, accx * 0.4, accy * 0.4);
    }
    
    ofSetColor(0, 0, 0);
    
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
