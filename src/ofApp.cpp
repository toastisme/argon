#include "ofApp.h"

/*
    ROUTINE box2screen_x:
    ROUTINE box2screen_y:
    ROUTINE box2screen:
        Scales coordinates from the box dimensions to the window size, measuring
        positions from a specified (on-screen) origin (defaulting to 0, 0)
 */
double ofApp::box2screen_x(double x, double x0) {
    return x * ofGetWidth() / theSystem.getWidth() - x0;
}

double ofApp::box2screen_y(double y, double y0) {
    return y * ofGetWidth() / theSystem.getWidth() - y0;
}

ofPoint ofApp::box2screen(double x, double y, double x0, double y0) {
    return ofPoint(box2screen_x(x, x0), box2screen_y(y, y0));
}

ofPoint ofApp::box2screen(lj::coord point, lj::coord origin) {
    return box2screen(point.x, point.y, origin.x, origin.y);
}

//--------------------------------------------------------------
// SETUP
//--------------------------------------------------------------

/*
    ROUTINE setup:
        Constructor for the app. Sets up the system at the beginning, loads all the assets
        needed for the app (images and fonts), and initialises the audio input stream.
 */
void ofApp::setup(){
    
    // Default values for system parameters
    numParticles = 50;
    thermFreq = 0.1;
    
    double TEMPERATURE = 0.5;
    double BOX_WIDTH = 15.0;
    double BOX_HEIGHT = BOX_WIDTH / ofGetWidth() * ofGetHeight();
    double TIMESTEP = 0.002;
    double CUTOFF = 3.0;
    
    // openFrameworks initialisation
    ofSetFrameRate(60);
    ofBackground(0, 0, 0);

    // Load assets
    
    // graphics
    circGradient.load("circ_gradient.png");
    playbutton.load("play-btn2.png");
    pausebutton.load("pause-btn2.png");
    
    loganLeft.load("david-logan-posing-left.png");
    loganRight.load("david-logan-posing-right.png");
    loganShiftx = loganLeft.getWidth() / 2;
    loganShifty = loganLeft.getHeight() / 2;
    
    //fonts
    uiFont14.load("Montserrat-Bold.ttf", 14);
    uiFont12.load("Montserrat-Bold.ttf", 12);
    uiFont10.load("Montserrat-Bold.ttf", 10);
    
    // Initialise theSystem
    setupSystem(numParticles, TEMPERATURE, BOX_WIDTH, BOX_HEIGHT, TIMESTEP, CUTOFF);
    
    // Set the booleans so that the audio input is turned on, as is the simulation,
    // but the UI, secret-Logan-mode, and energy graphs are off.
    audioOn = true;
    helpOn  = false;
    loganOn = false;
    graphOn = false;
    playOn  = true;

    selectedGaussian = -1; // No gaussian selected
    selectedSlider = 0; // Temperature slider selected
    
    // Setup the sound
    int bufferSize = 256;
    
    left.assign(bufferSize, 0.0);
    right.assign(bufferSize, 0.0);
    
    smoothedVol = 0.0;
    scaledVol   = 0.0;
    sensitivity = 0.04;
    
    soundStream.setup(this, 0, 2, 44100, bufferSize, 4);
}

/*
    ROUTINE setupSystem:
        Sets up and regrids the system. First clears all particles, then sets basic constants. Then, add the
        particles on a grid, initialise the forces and energies and save initial positions to prevousPositions
 */
void ofApp::setupSystem(int numParticles, double temperature, double box_width, double box_height, double timestep, double cutoff) {
    theSystem.clearSystem();
    
    theSystem.setBox(box_width, box_height);
    theSystem.setTemp(temperature);
    theSystem.setTimestep(timestep);
    theSystem.setCutoff(cutoff);
    
    theSystem.addParticlesGrid(numParticles);
    
    theSystem.forcesEnergies(N_THREADS);
    theSystem.savePreviousValues();
    firstEKin = theSystem.getEKin();
    firstEPot = fabs(theSystem.getEPot());
}

//--------------------------------------------------------------
// UPDATE
//--------------------------------------------------------------

/*
    ROUTINE update: 
        Part of the infinite update / draw loop.
        Update the status of the application, before the frame is drawn. 
 
        Currently performs the following tasks, when the simulation is not paused (i.e when playOn):
            
            1. Integrates the equations of motion 5 times and thermostats (Berendsen) with a frequency of 0.1
            2. If the audio input is turned on:
                - Calculates the smoothed volume scaled between 0 and 1
                - Updates the amplitude, exponent, and drawing of the selected Gaussian according to 
                    this volume.
 
 */
void ofApp::update(){
    
    if (playOn) { // If not paused
        
        // Integrate 5 times with a thermostat frequency of 0.1
        theSystem.run(5, thermFreq, N_THREADS);
        
        // scale the audio between 0 and 1
        if (audioOn) {
            // Scale smoothedVol between 0 and 1, taking account of the sensitivity
            // to the audioInput.
            scaledVol = ofMap(smoothedVol, 0.0, sensitivity, 0.0, 1.0, true);
            
            // Update the currently selected Gaussian, so that quiet-> loud results in
            // a change from an attractive, wide Gaussian, to a repulsive, narrow Gaussian.
            if ( selectedGaussian > -1)
                theSystem.updateGaussian(selectedGaussian, 50 - scaledVol*100, 0.8 - 0.5*scaledVol,
                                         theSystem.getGaussianX0(selectedGaussian),
                                         theSystem.getGaussianY0(selectedGaussian), scaledVol);
        }
    }
}

//--------------------------------------------------------------
// DRAWING
//--------------------------------------------------------------

/*
    ROUTINE drawData:
        Draws a string name with floating point data value at position x and y. 
 
        Overloaded version has extra argument length which sets the length of the floating point displayed.
 */
void ofApp::drawData(const ofTrueTypeFont &font, string name, double value, int x, int y) { //Converts data as int to string
    char drawstr[255];
    sprintf(drawstr, "%s %lf", name.c_str(), value);
    font.drawString(drawstr, x, y);
}

void ofApp::drawData(const ofTrueTypeFont &font, string name, double value, int x, int y, int length) { //Converts data as int to string
    //TODO: use iomanip
    char drawstr[255];
    sprintf(drawstr, "%s %lf", name.c_str(), value);
    stringstream convert_to_string;
    string drawstr_string;
    convert_to_string << drawstr;
    convert_to_string  >> drawstr_string;
    string data_string = drawstr_string.substr(0, length);
    
    font.drawString(data_string, x, y);
}

/*
    ROUTINE drawParticle:
        Draws a particle, specified by index and a size given either as x and y radii (ellipse)
        or by a single constant radius (circle).
        Optional: set the colour before drawing
        Optional: draws the particle with position nframes frames in the past
 */
void ofApp::drawParticle(int index, double radius_x, double radius_y, ofColor color, int nframes) {
    ofSetColor(color);
    drawParticle(index, radius_x, radius_y, nframes);
}

void ofApp::drawParticle(int index, double radius, ofColor color, int nframes) {
    ofSetColor(color);
    drawParticle(index, radius, nframes);
}

void ofApp::drawParticle(int index, double radius_x, double radius_y, int nframes) {
    lj::coord pos = theSystem.getPos(index, nframes);
    ofDrawEllipse(box2screen(pos), radius_x * 2, radius_y * 2);
}

void ofApp::drawParticle(int index, double radius, int nframes) {
    lj::coord pos = theSystem.getPos(index, nframes);
    ofDrawCircle(box2screen(pos), radius);
}

/*
    ROUTINE drawGaussian:
        Takes a reference to a Gaussian object and whether this Gaussian is the one
        currently in focus (selected), and draws the Gaussian as a circular gradient
        texture, coloured based on the amplitude of the Gaussian
 
 */
void ofApp::drawGaussian(Gaussian& g, bool selected){
    double gA = g.getgAmp();       // amplitude
    double galpha = g.getgAlpha(); // inverse width
    // Centre
    double gx = g.getgex0();
    double gy = g.getgey0();
    
    // Rescale between box size and window size
    double xscale = ofGetWidth() / theSystem.getWidth();
    double yscale = ofGetHeight() / theSystem.getHeight();
    
    // Determine the colour of the Gaussian, based on the amplitude
    ofColor color;
    u_char hue = 200;
    u_char saturation = gA > 0 ? 255 : 0;
    u_char brightness = 128;
    
    // Brighten if selected
    if (selected) {
        brightness = 255;
    }
    
    color.setHsb(hue, saturation, brightness);
    color.a = ofMap(abs(gA), 0, 50, 100, 255);
    ofSetColor(color);
    
    // Rescale the width and height of the Gaussian
    double scaleFactor = 2.5;
    double width  = scaleFactor * xscale / galpha;
    double height = scaleFactor * yscale / galpha;
    
    double x = gx * xscale - width / 2;
    double y = gy * yscale - height / 2;
    
    // Draw as circGradient
    circGradient.draw(x, y, width, height);
}


/*
    ROUTINE drawUI:
        Draws the user interface, which shows the control sliders for the temperature,
        number of particles, and audio input sensitivity, whilst also displaying the
        various key commands to control the simulation.
 */
void ofApp::drawUI()
{
    ofSetColor(255, 255, 255, 30);
    //Controls window
    ofDrawRectangle(0, ofGetHeight() - 150, ofGetWidth(), 150);
    ofSetColor(255, 255, 240);
    //drawFont.loadFont("Montserrat-Bold.ttf", 12);
    uiFont12.drawString("Key Commands", ofGetWidth() - 230, ofGetHeight()-130);
    //drawFont.loadFont("Montserrat-Bold.ttf", 10);
    uiFont10.drawString("Audio        /         'a'" ,ofGetWidth() - 225,ofGetHeight()-110);
    
    //Highlight audio on/off
    if (audioOn) {
        uiFont10.drawString("off" ,ofGetWidth() - 153,ofGetHeight()-110);
        ofSetColor(0, 200, 0);
        uiFont10.drawString("on" ,ofGetWidth() - 178,ofGetHeight()-110);
    } else{
        uiFont10.drawString("on" ,ofGetWidth() - 178,ofGetHeight()-110);
        ofSetColor(255, 0, 0);
        uiFont10.drawString("off" ,ofGetWidth() - 153,ofGetHeight()-110);
    }
    ofSetColor(255, 255, 240);
    uiFont10.drawString("Change gaussian 'g'" ,ofGetWidth() - 230,ofGetHeight()-90);
    uiFont10.drawString("Remove gaussian 'k'" ,ofGetWidth() - 230,ofGetHeight()-70);
    uiFont10.drawString("Show energies 'e'" ,ofGetWidth() - 225,ofGetHeight()-50);
    uiFont10.drawString("Pause 'p'" ,ofGetWidth() - 190,ofGetHeight()-30);
    uiFont10.drawString("Reset 'r'" ,ofGetWidth() - 190,ofGetHeight()-10);
    uiFont10.drawString("Change slider 'tab'", 50, ofGetHeight() - 30);
    uiFont10.drawString("Move slider up/down 'left/right arrow'", 250, ofGetHeight()-30);
    
    //Highlight the selected parameter
    int slider1, slider2, slider3;
    slider1 = (selectedSlider == 0 ? 0 : 1);
    slider2 = (selectedSlider == 1 ? 0 : 1);
    slider3 = (selectedSlider == 2 ? 0 : 1);
    //drawFont.loadFont("Montserrat-Bold.ttf", 12);
    ofSetColor(255, 255*slider1, 240*slider1);
    uiFont12.drawString(" Temperature (K):", ofGetWidth() - 1000, ofGetHeight() - 110);
    
    ofSetColor(255, 255*slider2, 240*slider2);
    uiFont12.drawString(" Particles:", ofGetWidth() - 1000, ofGetHeight() - 88);
    
    ofSetColor(255, 255*slider3, 240*slider3);
    uiFont12.drawString(" Sensitivity:", ofGetWidth() - 1000, ofGetHeight() - 63);
    
    ofSetColor(255, 255, 240);
    
    //Temperature slider
    ofDrawRectangle(ofGetWidth() - 800,ofGetHeight() - 122, 250, 10);
    
    //Parameter values
    drawData(uiFont12, " ", theSystem.getT()*120, ofGetWidth()-545, ofGetHeight() - 109, 5); // Temperature
    drawData(uiFont12, " ", numParticles, ofGetWidth()-545, ofGetHeight() - 87, 5); // Number of particles
    drawData(uiFont12, " ", sensitivity, ofGetWidth()-545, ofGetHeight() - 62, 5); // Sensitivity
    
    //Particle slider
    ofDrawRectangle(ofGetWidth() - 800,ofGetHeight() - 100, 250, 10);
    //Sensitivity slider
    ofDrawRectangle(ofGetWidth() - 800,ofGetHeight() - 75, 250, 10);
    //Slider cursors
    ofSetColor(0,0,0);
    ofDrawRectangle((ofGetWidth() - 800) + 30*theSystem.getT(), ofGetHeight() - 122, 5, 10);
    ofDrawRectangle((ofGetWidth() - 800) + numParticles, ofGetHeight() - 100, 5, 10);
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

/*
    ROUTINE drawGraph:
        Draws the kinetic/potential energy graphs as small red/blue circles
        in the background. 
 
        This rescales the values for the graphs using firstEKin/Pot and maxEKin/Pot
        as the minimum/maximum values respectively, which seems to be giving 
        unsatisfactory results in some circumstances. Needs looking in to.
 */
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
    
    // Loop over all data points stored in the previous energy arrays in theSystem
    // and draw them as small circles.
    for (int i = 0; i < theSystem.getNEnergies(); i++){
        ofSetColor(200, 0, 0);
        ekin = ofMap(theSystem.getPreviousEkin(i), firstEKin, ekinScale, 0, 0.9*winHeight, true);
        ofDrawCircle(xOffset + 5*i, yOffset - ekin, radius);
        
        ofSetColor(255, 255, 255);
        epot = ofMap(fabs(theSystem.getPreviousEpot(i)), firstEPot, epotScale, 0, 0.9*winHeight, true);
        ofDrawCircle(xOffset + 5*i, yOffset - epot, radius);
    }
    
    // Label the two graphs in the top left corner.
    ofSetColor(200, 0, 0);
    uiFont14.drawString("Kinetic energy", 0.05*ofGetWidth(), 0.1*ofGetHeight());
    ofSetColor(255, 255, 255);
    uiFont14.drawString("Potential energy", 0.05*ofGetWidth(), 0.15*ofGetHeight() );
    
}

/*
    ROUTINE draw:
        Part of the infinite update / draw loop.
        Draws the frame in the following sequence:
 
            1. Draws the frame rate in top left corner.
            2. If graphOn, draws the energy graphs in the background.
            3. Draws the Gaussian external potentials; selectedGaussian is at front of these.
            4. For each particle in the system, draws:
                - An ellipse of the particle,
                - coloured by its velocity,
                - with width and height determined by the x/y forces acting on it.
                - Trails of the 10th and 15th previous positions.
            5. If helpOn (UI showing), draws the UI. Otherwise, draws message
                on how to turn on UI in bottom left corner.
 */
void ofApp::draw(){
    
    // 1. Draw the frame rate in the top left
    drawData(uiFont14, "framerate", ofGetFrameRate(), 5, 25);
    
    // Set the resolution and fill settings for drawing the energy graphs if necessary
    ofFill();
    ofSetCircleResolution(10);
    // 2. Draw graphs in background if turned on.
    if (graphOn) drawGraph();
    
    // 3. Draw gaussians, with selected on top
    if (theSystem.getNGaussians() > 0) {
        for (int g = 0; g < theSystem.getNGaussians(); g++){
            if (g != selectedGaussian)
                drawGaussian(theSystem.getGaussian(g), false);
        }
        drawGaussian(theSystem.getGaussian(selectedGaussian), true);
    }
    
    // Setup temporary placeholders
    ofColor particleColor;
    lj::coord tempVel;
    lj::coord tempAcc;
    
    double radius_x;
    double radius_y;
    double radius;
    
    double hue;
    
    double v_avg = theSystem.getVAvg(); // Get average velocity for scaling purposes
    ofSetCircleResolution(20);
    
    // Draw all the particles and trails
    for (int i = 0; i < theSystem.getN(); ++i) {
        tempVel = theSystem.getVel(i);
        tempAcc = theSystem.getForce(i);
        
        hue = ofMap(tempVel.x + tempVel.y, 0, 3 * v_avg, 170, 210, true);
        particleColor.setHsb(hue, 255, 255);
        
        radius_x = ofMap(log(1.0 + abs(tempAcc.x)), 0, 10, 10, 25);
        radius_y = ofMap(log(1.0 + abs(tempAcc.y)), 0, 10, 10, 25);
        radius = (radius_x + radius_y) / 2;
        
        if (loganOn) {
            ofSetColor(particleColor);
            lj::coord pos = theSystem.getPos(i);
            if (tempVel.x >= 0)
                loganRight.draw(box2screen(pos.x, pos.y, loganShiftx, loganShifty), radius_x * 4, radius_y * 4);
            else
                loganLeft.draw( box2screen(pos.x, pos.y, loganShiftx, loganShifty), radius_x * 4, radius_y * 4);
        } else {
            //trail
            if (theSystem.getNPrevPos() >= 15) {
                particleColor.a = 100;
                drawParticle(i, radius * 0.25, particleColor, 14);
            }
            if (theSystem.getNPrevPos() >= 10) {
                particleColor.a = 150;
                drawParticle(i, radius * 0.5,  particleColor, 9);
            }
            if (theSystem.getNPrevPos() >= 5) {
                particleColor.a = 200;
                drawParticle(i, radius * 0.75, particleColor, 4);
            }
            
            //particle
            particleColor.a = 255;
            drawParticle(i, radius_x, radius_y, particleColor);
        }
    }
    
    
    // Draw the UI if helpOn, otherwise draw message on how to turn the UI on.
    if (helpOn) {
        drawUI();
    } else {
        ofSetColor(255, 255, 240);
        uiFont14.drawString("press 'h' for controls", 10, ofGetHeight()-10);
    }
}

//--------------------------------------------------------------------
// INPUT & EVENT HANDLING
//--------------------------------------------------------------------

/*
    EVENT audioIn:
        If audio is coming in to the primary input (built-in microphone by default
        on a Mac), this is triggered. It takes the amplitudes of the left and right
        audio streams and determines the root mean square averaged volume and then
        updates the previous value of the volume in a 93:7 ratio of old:new, so that
        volume changes are smoothed. Places this value in smoothedVol.
 */
void ofApp::audioIn(float * input, int bufferSize, int nChannels){
    
    float curVol = 0.0;
    
    // samples are "interleaved"
    int numCounted = 0;
    
    //lets go through each sample and calculate the root mean square which is a rough way to calculate volume
    for (int i = 0; i < bufferSize; i++){
        left[i]  = input[i*2]*0.5;
        right[i] = input[i*2+1]*0.5;
        
        curVol += left[i] * left[i];
        curVol += right[i] * right[i];
        numCounted+=2;
    }
    
    //this is how we get the mean of rms
    curVol /= (float)numCounted;
    
    // this is how we get the root of rms
    curVol = sqrt( curVol );
    
    smoothedVol *= 0.93;
    smoothedVol += 0.07 * curVol;
    
}


//--------------------------------------------------------------

/*
    EVENT keyPressed:
 
        Reacts to the keyboard being used. Currently, we have the
        following mappings:
        
            - a/A           = toggle audio on or off
            - g/G           = change which Gaussian is currently selected
            - k/K           = kill the currently selected Gaussian if any exist
            - l/L           = secret-Logan-mode
            - e/E           = toggle the energy graphs showing in the background
            - h/H           = toggle whether the UI is showing
            - r/R           = reset the system
            - p/P           = pause/restart the simulation
            - TAB           = switch which UI slider has focus
            - RIGHT/LEFT    = move the currently selected slider up/down
 */
void ofApp::keyPressed(int key){
    
    if (key == 'a' || key == 'A') { // Audio on/off
        audioOn = !audioOn;
    }
    
    else if ((key == 'g' || key == 'G') && theSystem.getNGaussians() > 0) { // Change selected gaussian
        selectedGaussian = (selectedGaussian+1)%theSystem.getNGaussians();
    }
    
    else if (key == 'k' || key == 'K') { // Kill currently selected gaussian
        
        // Remove the selected Gaussian and select the one before it, if it is not
        // the zeroth Gaussian
        if (selectedGaussian > 0) {
            theSystem.removeGaussian(selectedGaussian);
            selectedGaussian--;
        } else if (selectedGaussian == 0) {
            // Otherwise, remove the zeroth Gaussian, without changing
            // the selected Gaussian, unless there are none left
            // selectedGaussian = -1 implies there are no Gaussians
            theSystem.removeGaussian(selectedGaussian);
            if (theSystem.getNGaussians() == 0) {
                selectedGaussian = -1;
            }
        }
    }
    
    else if (key == 'l' || key == 'L') { // Secret-Logan-Mode on/off
        loganOn = !loganOn;
    }
    
    else if (key == 'e' || key == 'E') { // Show/hide energy graphs
        graphOn = !graphOn;
    }
    
    else if (key == 'h' || key == 'H'){ // Show/hide UI
        helpOn = !helpOn;
    }
    
    else if (key == 'r' || key == 'R') { // Reset the system to have the current values of the sliders
        lj::coord box = theSystem.getBox();
        setupSystem(numParticles, theSystem.getT(), box.x, box.y, theSystem.getTimestep(), theSystem.getCutoff());
    }
    
    else if (key == 'p' || key == 'P') { // Pause/restart the simulation
        playOn = !playOn;
    }
    
    // If the UI is showing, the sliders are in focus
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
                    if (numParticles + 5 <= 245) { //Bounds the upper end of the particle slider
                        numParticles += 5;
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
                    if ((numParticles - 5) >= 0 ) { //Bounds the lower end of the particle slider
                        numParticles -= 5;
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

/*
    EVENT mousePressed:
        When the mouse is clicked anywhere in the window, this uses the
        (x, y) position of the click to create a Gaussian at that position.
 
        Scope to change this to only be on left mouse button click, to free up
        other mouse uses?
 */
void ofApp::mousePressed(int x, int y, int button){
    
    // Default values for the amplitude and exponent of a Gaussian
    // Should they really be stored here?
    double GAMP =  50.0;
    double GALPHA = 0.3;
    
    // Rescale the (x, y) coordinates of the mouse input so that they
    // are within the dimensions of the box
    double scaled_x = x * theSystem.getWidth()/ofGetWidth();
    double scaled_y = y * theSystem.getHeight()/ofGetHeight();
    
    // Add a Gaussian external potential to the system, and make this new
    // Gaussian be the one currently in focus.
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
