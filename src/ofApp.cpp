#include "ofApp.h"

void ofApp::drawData(string name, double value) {
    char drawstr[255];
    sprintf(drawstr, "%s %lf", name.c_str(), value);
    drawFont.drawString(drawstr, ofGetWidth() - 550, ofGetHeight() - 109);
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

void ofApp::drawGaussian(Gaussian& g, double boxw, double boxl, bool selected){
    double gA = g.getgAmp(), galpha = g.getgAlpha();
    double gx = ofMap(g.getgex0(), 0, boxw, 0, ofGetWidth());
    double gy = ofMap(g.getgey0(), 0, boxl, 0, ofGetHeight());
    double scale = g.getScale();
    
    int N_CIRCLES = 10;
    double colourSpacing = 60/double(N_CIRCLES);
    double contourSpacing = 0.8/double(N_CIRCLES);
    
    ofSetCircleResolution(100);
    
    int red = 0;
    if (selected) red = 200;
    
    for (int n = 0; n < N_CIRCLES; n++){
        ofSetColor(red,(190+colourSpacing*n)*scale,255);
        ofDrawCircle(gx, gy, (log(0.2+contourSpacing*n)/galpha)*abs(gA));
    }

}

//--------------------------------------------------------------
void ofApp::setup(){
    double BOX_WIDTH = 15.0;
    double BOX_LENGTH = 10.0;
    double CUTOFF = 3.0;
    double TIMESTEP = 0.005;
    int N_PARTICLES = 50;
    double TEMPERATURE = 20.0;
    
    playbutton.load("play-btn.png");
    
    audioOn = true;
    helpOn = false;
    thermCounter = 0;
    selectedGaussian = -1; // No gaussian selected
    
    double box_ratio = BOX_WIDTH / BOX_LENGTH;
    int n_grid_x = ceil(sqrt(N_PARTICLES * box_ratio));
    int n_grid_y = ceil(sqrt(N_PARTICLES / box_ratio));
    double xspacing = BOX_WIDTH  / (n_grid_x);
    double yspacing = BOX_LENGTH / (n_grid_y);
    
    // Set up grid
    
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
    
    // Set up the system

    theSystem.setConsts(BOX_LENGTH, BOX_WIDTH, CUTOFF, TIMESTEP);
    theSystem.setTemp(TEMPERATURE);
    theSystem.setParticles(N_PARTICLES);
    
    // intialise the system + previous positions
    theSystem.forcesEnergies(N_THREADS);
    
    for (int i = 0; i < 20; ++i) {
        theSystem.integrate(N_THREADS);
    }
    
    // Setup the sound
    int bufferSize = 256;
    
    left.assign(bufferSize, 0.0);
    right.assign(bufferSize, 0.0);
    
    smoothedVol     = 0.0;
    scaledVol		= 0.0;
    
    soundStream.setup(this, 0, 2, 44100, bufferSize, 4);
    
    drawFont.loadFont("Verdana.ttf", 14);
    
    ofBackground(0, 0, 0);
}

//--------------------------------------------------------------
void ofApp::update(){
    theSystem.integrate(N_THREADS);
    drawDataHeight = ofGetHeight() - 5;

    //lets scale the vol up to a 0-1 range, and thermostat
    if (thermCounter % 10 == 0) {
        theSystem.andersen(0.1);
    }
    
    if (audioOn) {
        scaledVol = ofMap(smoothedVol, 0.0, 0.17, 0.0, 1.0, true);
        if ( selectedGaussian > -1)
            theSystem.updateGaussian(selectedGaussian, 50 - scaledVol*100, 1.1 - scaledVol, theSystem.getGaussianX0(selectedGaussian), theSystem.getGaussianY0(selectedGaussian), scaledVol);
    }
    
    thermCounter++;
}

//--------------------------------------------------------------
void ofApp::draw(){
    vector<double> BOX_SIZE;
    BOX_SIZE.push_back(theSystem.getWidth());
    BOX_SIZE.push_back(theSystem.getHeight());
    
    for (int g = 0; g < theSystem.getNGaussians(); g++){
        if ( g == selectedGaussian)
            drawGaussian(theSystem.getGaussian(g), BOX_SIZE[0], BOX_SIZE[1], true);
        else
            drawGaussian(theSystem.getGaussian(g), BOX_SIZE[0], BOX_SIZE[1], false);
    }
    
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
        
        ofSetColor((velx+vely)/2.0, 0, 200);
        
        ofDrawEllipse(posx, posy, accx, accy);
        ofDrawEllipse(pospx, pospy, accx * 0.8, accy * 0.8);
        ofDrawEllipse(posppx, posppy, accx * 0.4, accy * 0.4);
    }
    if (helpOn == true){
        ofSetColor(255, 255, 255, 100);
        //Controls window
        ofDrawRectangle(0, ofGetHeight() - 150, ofGetWidth(), 150);
        ofSetColor(255, 255, 240);
        drawFont.drawString(" key commands: \n audio on/off: 'a' \n change gaussian: 'g' \n remove gaussian: 'k'" ,ofGetWidth() - 270,ofGetHeight()-110);
        
        drawFont.drawString(" temperature:", ofGetWidth() - 1000, ofGetHeight() - 110);
        drawFont.drawString(" particles:", ofGetWidth() - 1000, ofGetHeight() - 88);
        drawFont.drawString(" sensitivity:", ofGetWidth() - 1000, ofGetHeight() - 63);
        //Temperature slider
        ofDrawRectangle(ofGetWidth() - 800,ofGetHeight() - 122, 250, 10);
        
        //Parameter values
        drawData(" ", theSystem.getT());
        
        //Particle slider
        ofDrawRectangle(ofGetWidth() - 800,ofGetHeight() - 100, 250, 10);
        //Sensitivity slider
        ofDrawRectangle(ofGetWidth() - 800,ofGetHeight() - 75, 250, 10);
        ofSetColor(0,0,0);
        ofDrawRectangle((ofGetWidth() - 800) + theSystem.getT(), ofGetHeight() - 122, 5, 10);
        ofDrawRectangle((ofGetWidth() - 800) + theSystem.getN(), ofGetHeight() - 100, 5, 10);
        ofDrawRectangle((ofGetWidth() - 800), ofGetHeight() - 75, 5, 10);
        //buttons
        playbutton.draw(ofGetWidth() - 380, ofGetHeight() - 130, 50, 50);
        

        
    }
    else if (helpOn == false){
        ofSetColor(255, 255, 240);
        drawFont.drawString("press 'h' for controls", ofGetWidth()-1000, ofGetHeight()-40);
    }
    //drawData("E Pot", theSystem.getEPot());
}

//--------------------------------------------------------------------
void ofApp::audioIn(float * input, int bufferSize, int nChannels){
    
    float curVol = 0.0;
    
    // samples are "interleaved"
    int numCounted = 0;
    
    //lets go through each sample and calculate the root mean square which is a rough way to calculate volume
    for (int i = 0; i < bufferSize; i++){
        left[i]		= input[i*2]*0.5;
        right[i]	= input[i*2+1]*0.5;
        
        curVol += left[i] * left[i];
        curVol += right[i] * right[i];
        numCounted+=2;
    }
    
    //this is how we get the mean of rms :)
    curVol /= (float)numCounted;
    
    // this is how we get the root of rms :)
    curVol = sqrt( curVol );
    
    smoothedVol *= 0.93;
    smoothedVol += 0.07 * curVol;
    
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == 'a' || key == 'A') // Audio on/off
        audioOn = !audioOn;
    else if (key == 'g' || key == 'G') // Change gaussian
        selectedGaussian = (selectedGaussian+1)%theSystem.getNGaussians();
    else if (key == 'k' || key == 'K') // Kill gaussian
        if (selectedGaussian > -1) {
            theSystem.removeGaussian(selectedGaussian);
            selectedGaussian--;
        }
    if (key == 'h' || key == 'H'){
        helpOn = !helpOn;
    }
    if (helpOn == true){
        if (key == OF_KEY_RIGHT){
            theSystem.setTemp(theSystem.getT()+1);
        }
        else if (key == OF_KEY_LEFT){
            theSystem.setTemp(theSystem.getT()-1);
        }
        
        
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
    double GAMP = 50.0;
    double GALPHA =0.3;
    
    double scaled_x = x * theSystem.getWidth()/ofGetWidth();
    double scaled_y = y * theSystem.getHeight()/ofGetHeight();
    
    theSystem.addGaussian(GAMP, GALPHA, scaled_x, scaled_y);
    selectedGaussian = theSystem.getNGaussians() - 1;
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
