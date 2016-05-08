#include "ofApp.h"


void ofApp::drawData(string name, double value, int x, int y, int length) { //Converts data as int to string
    char drawstr[255];
    sprintf(drawstr, "%s %lf", name.c_str(), value);
    stringstream convert_to_string;
    string drawstr_string;
    convert_to_string << drawstr;
    convert_to_string  >> drawstr_string;
    string data_string = drawstr_string.substr(0, length);
    
    drawFont.drawString(data_string, x, y);
}

void ofApp::randomiseVelocity(vector<double> &vel, double T) {
    double sigma = sqrt(T);
    
    for (int i = 0; i < vel.size(); ++i) {
        double randr = ofRandom(1);
        double randt = ofRandom(1);
        
        randr = sqrt(-2.0 * log(randr));
        randt = sin(2 * PI * randt);
        
        vel[i] = sigma * randr * randt;
    }
}

void ofApp::setupSystem(int numParticles, double temperature, double box_length, double box_width, double cutoff, double timestep) {
    theSystem.clearSystem();
    theSystem.setConsts(box_length, box_width, cutoff, timestep);
    theSystem.setTemp(temperature);
    
    int i = 0, j = 0;
    double posx, posy;
    vector<double> vel = {0, 0};
    
    double box_ratio = box_width / box_length;
    int n_grid_x = ceil(sqrt(numParticles * box_ratio));
    int n_grid_y = ceil(sqrt(numParticles / box_ratio));
    double xspacing = box_width  / (n_grid_x);
    double yspacing = box_length / (n_grid_y);
    
    // grid particles
    for (int n = 0; n < numParticles; ++n){
        posx = xspacing * (i + 0.5);
        posy = yspacing * (j + 0.5);
        
        printf("%d %d %lf %lf\n", i, j, posx, posy);
        randomiseVelocity(vel, temperature);
        theSystem.addParticle(posx, posy, vel[0], vel[1]);
        
        ++i;
        i = i % n_grid_x;
        if (i == 0) ++j;
    }
    
    // intialise the system + previous positions
    theSystem.forcesEnergies(N_THREADS);
    firstEKin = theSystem.getEKin();
    firstEPot = fabs(theSystem.getEPot());
    
    for (int i = 0; i < 20; ++i) {
        theSystem.integrate(N_THREADS);
    }
}

void ofApp::drawGaussian(Gaussian& g, double boxw, double boxl, bool selected){
    double gA = g.getgAmp(), galpha = g.getgAlpha(); // amplitude and alpha variables
    double gx = ofMap(g.getgex0(), 0, boxw, 0, ofGetWidth()); // x values of the visualisation
    double gy = ofMap(g.getgey0(), 0, boxl, 0, ofGetHeight()); // y values of the visualisation
    double scale = g.getScale(); // scale factor
    double transp;
    
    double r;
    double r_max = max(boxw, boxl) / 5.0;
    double fwhm = 2 * sqrt(log(2) / galpha);
    double maxslope = galpha * gA * fwhm * exp(- galpha * fwhm * fwhm);
    
    double scaleFactor = 2.3;
    double xscale = scaleFactor * ofGetWidth() / boxw;
    double yscale = scaleFactor * ofGetHeight() / boxl;
    
    int red = 0;
    if (selected) red = 200;
    
    int n_circles = 50;
    
    for (int i = 1; i < n_circles; ++i) {
        r = ofMap(i, 0, n_circles, 0, r_max);
        transp = galpha * gA * r * exp(- galpha * r * r) / 40;
        transp = ofMap(transp, 0, maxslope, 0, 255);
        ofSetColor(red, 190 * scale, 255, transp);
        ofDrawEllipse(gx, gy, r * xscale, r * yscale);
    }
}

void ofApp::drawUI()
{
    ofSetColor(255, 255, 255, 30);
    //Controls window
    ofDrawRectangle(0, ofGetHeight() - 150, ofGetWidth(), 150);
    ofSetColor(255, 255, 240);
    drawFont.loadFont("Montserrat-Bold.ttf", 12);
    drawFont.drawString("Key Commands" ,ofGetWidth() - 230,ofGetHeight()-130);
    drawFont.loadFont("Montserrat-Bold.ttf", 10);
    drawFont.drawString("Audio        /         'a'" ,ofGetWidth() - 225,ofGetHeight()-110);
    
    //Highlight audio on/off
    if (audioOn){
        drawFont.drawString("off" ,ofGetWidth() - 153,ofGetHeight()-110);
        ofSetColor(0, 200, 0);
        drawFont.drawString("on" ,ofGetWidth() - 178,ofGetHeight()-110);
        
    }
    else{
        drawFont.drawString("on" ,ofGetWidth() - 178,ofGetHeight()-110);
        ofSetColor(255, 0, 0);
        drawFont.drawString("off" ,ofGetWidth() - 153,ofGetHeight()-110);
    }
    ofSetColor(255, 255, 240);
    drawFont.drawString("Change gaussian 'g'" ,ofGetWidth() - 230,ofGetHeight()-90);
    drawFont.drawString("Remove gaussian 'k'" ,ofGetWidth() - 230,ofGetHeight()-70);
    drawFont.drawString("Show energies 'e'" ,ofGetWidth() - 225,ofGetHeight()-50);
    drawFont.drawString("Pause 'p'" ,ofGetWidth() - 190,ofGetHeight()-30);
    drawFont.drawString("Reset 'r'" ,ofGetWidth() - 190,ofGetHeight()-10);
    drawFont.drawString("Change slider 'tab'", 50, ofGetHeight() - 30);
    drawFont.drawString("Move slider up/down 'left/right arrow'", 250, ofGetHeight()-30);
    
    //Highlight the selected parameter
    int slider1, slider2, slider3;
    slider1 = (selectedSlider == 0 ? 0 : 1);
    slider2 = (selectedSlider == 1 ? 0 : 1);
    slider3 = (selectedSlider == 2 ? 0 : 1);
    drawFont.loadFont("Montserrat-Bold.ttf", 12);
    ofSetColor(255, 255*slider1, 240*slider1);
    drawFont.drawString(" Temperature (K):", ofGetWidth() - 1000, ofGetHeight() - 110);
    
    ofSetColor(255, 255*slider2, 240*slider2);
    drawFont.drawString(" Particles:", ofGetWidth() - 1000, ofGetHeight() - 88);
    
    ofSetColor(255, 255*slider3, 240*slider3);
    drawFont.drawString(" Sensitivity:", ofGetWidth() - 1000, ofGetHeight() - 63);
    
    ofSetColor(255, 255, 240);
    
    //Temperature slider
    ofDrawRectangle(ofGetWidth() - 800,ofGetHeight() - 122, 250, 10);
    
    //Parameter values
    drawData(" ", theSystem.getT()*120, ofGetWidth()-545, ofGetHeight() - 109, 5); // Temperature
    drawData(" ", N_PARTICLES, ofGetWidth()-545, ofGetHeight() - 87, 5); // Number of particles
    drawData(" ", sensitivity, ofGetWidth()-545, ofGetHeight() - 62, 5); // Sensitivity
    
    //Particle slider
    ofDrawRectangle(ofGetWidth() - 800,ofGetHeight() - 100, 250, 10);
    //Sensitivity slider
    ofDrawRectangle(ofGetWidth() - 800,ofGetHeight() - 75, 250, 10);
    //Slider cursors
    ofSetColor(0,0,0);
    ofDrawRectangle((ofGetWidth() - 800) + 30*theSystem.getT(), ofGetHeight() - 122, 5, 10);
    ofDrawRectangle((ofGetWidth() - 800) + N_PARTICLES, ofGetHeight() - 100, 5, 10);
    ofDrawRectangle((ofGetWidth() - 810) + 1900*sensitivity, ofGetHeight() - 75, 5, 10);
    
    //buttons
    if (playOn) {
        pausebutton.draw(ofGetWidth() - 350, ofGetHeight() - 132, 55, 55);
        ofSetColor(0, 200, 0);
        playbutton.draw(ofGetWidth() - 420, ofGetHeight() - 130, 50, 50);
        
    }
    else{
        playbutton.draw(ofGetWidth() - 420, ofGetHeight() - 130, 50, 50);
        ofSetColor(0,230,255);
        pausebutton.draw(ofGetWidth() - 350, ofGetHeight() - 132, 55, 55);
    }
}

void ofApp::drawGraph()
{
    // Draw `window'
    float winLeft = ofGetWidth()/6;
    float winTop = ofGetHeight()/6;
    float winWidth = 2*ofGetWidth()/3;
    float winHeight = 2*ofGetHeight()/3;
    float xOffset = 1.1*winLeft;
    float yOffset = 7*winHeight/6;
    float ekinScale = theSystem.getMaxEkin();//8*theSystem.getN()*theSystem.getT();
    float epotScale = theSystem.getMaxEpot();//8*firstEPot;
    //ofFill();
    //ofSetColor(255, 255, 255, 100);
    //ofDrawRectangle(winLeft, winTop, winWidth, winHeight);
    
    // Draw graph
    float radius = 3;
    float ekin, epot;
    
    for (int i = 0; i < theSystem.getNEnergies(); i++){
        ofSetColor(200, 0, 0);
        ekin = ofMap(theSystem.getPreviousEkin(i), firstEKin, ekinScale, 0, 0.9*winHeight, true);
        ofDrawCircle(xOffset + 5*i, yOffset - ekin, radius);
        
        ofSetColor(255, 255, 255);
        epot = ofMap(fabs(theSystem.getPreviousEpot(i)), firstEPot, epotScale, 0, 0.9*winHeight, true);
        ofDrawCircle(xOffset + 5*i, yOffset - epot, radius);
    }
    
    ofSetColor(200, 0, 0);
    drawFont.drawString("Kinetic energy", 0.05*ofGetWidth(), 0.1*ofGetHeight());
    ofSetColor(255, 255, 255);
    drawFont.drawString("Potential energy", 0.05*ofGetWidth(), 0.15*ofGetHeight() );
    
}

//--------------------------------------------------------------
void ofApp::setup(){
    BOX_WIDTH = 15.0;
    BOX_LENGTH = BOX_WIDTH / ofGetWidth() * ofGetHeight();
    CUTOFF = 3.0;
    TIMESTEP = 0.005;
    N_PARTICLES = 50;
    TEMPERATURE = 0.5;

    playbutton.load("play-btn2.png");
    pausebutton.load("pause-btn2.png");
    
    // initialise openFrameworks stuff
    ofSetCircleResolution(100);
    ofSetFrameRate(60);
    
    // intitialise the system
    setupSystem(N_PARTICLES, TEMPERATURE, BOX_LENGTH, BOX_WIDTH, CUTOFF, TIMESTEP);
    
    audioOn = true;
    helpOn = false;
    loganOn = false;
    graphOn = false;
    playOn = true;

    thermCounter = 0;
    selectedGaussian = -1; // No gaussian selected
    selectedSlider = 0;
    drawFont.loadFont("Montserrat-Bold.ttf", 14);
    sensitivity = 0.04;
    
    // Setup the sound
    int bufferSize = 256;
    
    left.assign(bufferSize, 0.0);
    right.assign(bufferSize, 0.0);
    
    smoothedVol     = 0.0;
    scaledVol		= 0.0;
    
    soundStream.setup(this, 0, 2, 44100, bufferSize, 4);
    
    ofBackground(0, 0, 0);
    
    // Setup Logan image
    loganLeft.load("david-logan-posing-left.png");
    loganRight.load("david-logan-posing-right.png");
    loganShiftx = loganLeft.getWidth() / 2;
    loganShifty = loganLeft.getHeight() / 4;
}

//--------------------------------------------------------------
void ofApp::update(){
    if (playOn) {
        theSystem.integrate(N_THREADS);

        //lets scale the vol up to a 0-1 range, and thermostat
        if (thermCounter % 10 == 0) {
            theSystem.berendsen(1.0);
        }
        thermCounter++;
        
        if (audioOn) {
            scaledVol = ofMap(smoothedVol, 0.0, sensitivity, 0.0, 1.0, true);
            if ( selectedGaussian > -1)
                theSystem.updateGaussian(selectedGaussian, 50 - scaledVol*100, 0.8 - 0.5*scaledVol, theSystem.getGaussianX0(selectedGaussian), theSystem.getGaussianY0(selectedGaussian), scaledVol);
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    vector<double> BOX_SIZE;
    BOX_SIZE.push_back(theSystem.getWidth());
    BOX_SIZE.push_back(theSystem.getHeight());
    
    ofFill();
    if (graphOn) drawGraph();
    
    for (int g = 0; g < theSystem.getNGaussians(); g++){
        if ( g == selectedGaussian)
            drawGaussian(theSystem.getGaussian(g), BOX_SIZE[0], BOX_SIZE[1], true);
        else
            drawGaussian(theSystem.getGaussian(g), BOX_SIZE[0], BOX_SIZE[1], false);
    }
    
    double posx, posy;
    double pospx, pospy;
    double posppx, posppy;
    double pospppx, pospppy;
    double velx, vely;
    double accx, accy;
    
    vector<double> tempPos;
    vector<double> tempPosPrev;
    vector<double> tempPosPrevPrev;
    vector<double> tempPosPrevPrevPrev;
    vector<double> tempVel;
    vector<double> tempAcc;
    
    double trailSize;
    double hue;
    ofColor particleColor;
    ofColor trailColor;
    
    double v_avg = theSystem.getVAvg(); // Get average velocity for scaling purposes
    
    for (int i = 0; i < theSystem.getN(); i++){
        tempPos = theSystem.getPos(i);
        tempPosPrev = theSystem.getPreviousPositions(i, 15);
        tempPosPrevPrev = theSystem.getPreviousPositions(i, 10);
        tempPosPrevPrevPrev = theSystem.getPreviousPositions(i, 5);
        tempVel = theSystem.getVel(i);
        tempAcc = theSystem.getForces(i);
        
        posx = ofMap(tempPos[0], 0, BOX_SIZE[0], 0, ofGetWidth());
        posy = ofMap(tempPos[1], 0, BOX_SIZE[1], 0, ofGetHeight());
        
        pospx = ofMap(tempPosPrev[0], 0, BOX_SIZE[0], 0, ofGetWidth());
        pospy = ofMap(tempPosPrev[1], 0, BOX_SIZE[1], 0, ofGetHeight());
        
        posppx = ofMap(tempPosPrevPrev[0], 0, BOX_SIZE[0], 0, ofGetWidth());
        posppy = ofMap(tempPosPrevPrev[1], 0, BOX_SIZE[1], 0, ofGetHeight());
        
        pospppx = ofMap(tempPosPrevPrevPrev[0], 0, BOX_SIZE[0], 0, ofGetWidth());
        pospppy = ofMap(tempPosPrevPrevPrev[1], 0, BOX_SIZE[1], 0, ofGetHeight());
        
        velx = ofMap(abs(tempVel[0]), 0, 1.5*v_avg, 0, 255);
        vely = ofMap(abs(tempVel[1]), 0, 1.5*v_avg, 0, 255);
        
        accx = ofMap(log(1.0+abs(tempAcc[0])), 0, 10, 20, 50);
        accy = ofMap(log(1.0+abs(tempAcc[1])), 0, 10, 20, 50);
        trailSize = (accx + accy) / 4.0;
        
        hue = ofMap((velx + vely) / 2, 0, 300, 170, 210, true);
        particleColor.setHsb(hue, 255, 255);
        
        trailColor.set(particleColor);
        
        // draw trail
        if (!loganOn) {
            trailColor.a = 100;
            ofSetColor(trailColor);
            ofDrawCircle(pospppx, pospppy, trailSize * 0.25);
            
            trailColor.a = 150;
            ofSetColor(trailColor);
            ofDrawCircle(posppx, posppy, trailSize * 0.5);
            
            trailColor.a = 200;
            ofSetColor(trailColor);
            ofDrawCircle(pospx, pospy, trailSize * 0.75);
        }
        
        // draw particle
        ofSetColor(particleColor);
        if (loganOn){
            if (tempVel[0] >= 0)
                loganRight.draw(posx - loganShiftx, posy - loganShifty, accx*2, accy*2);
            else
                loganLeft.draw(posx - loganShiftx, posy - loganShifty, accx*2, accy*2);
        }
        else {
            ofDrawEllipse(posx, posy, accx, accy);
        }
    }
    if (helpOn == true){
        drawUI();
    }
    else if (helpOn == false){
        ofSetColor(255, 255, 240);
        drawFont.drawString("press 'h' for controls", ofGetWidth()-1000, ofGetHeight()-40);
    }
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
    if (key == 'a' || key == 'A') { // Audio on/off
        audioOn = !audioOn;
    }
    else if ((key == 'g' || key == 'G') && theSystem.getNGaussians() > 0) { // Change gaussian
        selectedGaussian = (selectedGaussian+1)%theSystem.getNGaussians();
    } else if (key == 'k' || key == 'K') { // Kill gaussian
        if (selectedGaussian > 0) {
            theSystem.removeGaussian(selectedGaussian);
            selectedGaussian--;
        } else if (selectedGaussian == 0) {
            theSystem.removeGaussian(selectedGaussian);
            if (theSystem.getNGaussians() == 0) {
                selectedGaussian = -1;
            }
        }
    }
    else if (key == 'l' || key == 'L') {
        loganOn = !loganOn;
    }
    else if (key == 'e' || key == 'E') {
        graphOn = !graphOn;
    }
    else if (key == 'h' || key == 'H'){
        helpOn = !helpOn;
    }
    else if (key == 'r' || key == 'R') {
        setupSystem(N_PARTICLES, TEMPERATURE, BOX_LENGTH, BOX_WIDTH, CUTOFF, TIMESTEP);
    }
    else if (key == 'p' || key == 'P') {
        playOn = !playOn;
    }
    if (helpOn == true){
        if (key == OF_KEY_TAB)  // Switch slider
            selectedSlider = (selectedSlider+1)%3;
        if (key == OF_KEY_RIGHT){
            switch(selectedSlider) {
                case 0: { // Temperature
                    if (((theSystem.getT()*120) + 10.0) <= 980){ //Bounds the upper end of the temperature slider
                        theSystem.setTemp(theSystem.getT()+(10/120.0));
                    }
                    break;
                }
                case 1: { // N particles
                    if (N_PARTICLES + 5 <= 245) { //Bounds the upper end of the particle slider
                        N_PARTICLES += 5;
                    }
                    break;
                }
                case 2: { // Sensitivity
                    if (sensitivity + 0.005 < 0.140){ //Bounds the upper end of the sensitivity slider
                        sensitivity += 0.005;
                    }
                    break;
                }
                default:
                {}
            }
        }
        else if (key == OF_KEY_LEFT){
            switch(selectedSlider) {
                case 0: { // Temperature
                    if ((theSystem.getT() - (10/120.0)) > 0){ //Bounds the lower end of the temperature slider
                        theSystem.setTemp(theSystem.getT()-(10/120.0));
                    }
                    break;
                }
                case 1: { // N particles
                    if ((N_PARTICLES - 5) >= 0 ) { //Bounds the lower end of the particle slider
                        N_PARTICLES -= 5;
                    }
                    break;
                }
                case 2: { // Sensitivity
                    if (sensitivity - 0.005 >= 0.000){ //Bounds the lower end of the sensitivity slider
                        sensitivity -= 0.005;
                    }
                    break;
                }
                default:
                {}
            }
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
