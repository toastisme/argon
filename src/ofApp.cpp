#include "ofApp.h"

/*
    ROUTINE drawData:
        Draws a string name with floating point data value at position x and y. 
 
        Overloaded version has extra argument length which sets the length of the floating point displayed.
 */
void ofApp::drawData(string name, double value, int x, int y) { //Converts data as int to string
    char drawstr[255];
    sprintf(drawstr, "%s %lf", name.c_str(), value);
    drawFont.drawString(drawstr, x, y);
}

void ofApp::drawData(string name, double value, int x, int y, int length) { //Converts data as int to string
    //TODO: use iomanip
    char drawstr[255];
    sprintf(drawstr, "%s %lf", name.c_str(), value);
    stringstream convert_to_string;
    string drawstr_string;
    convert_to_string << drawstr;
    convert_to_string  >> drawstr_string;
    string data_string = drawstr_string.substr(0, length);
    
    drawFont.drawString(data_string, x, y);
}


/*
    ROUTINE randomiseVelocity:
        Sets the initial velocities of a velocity vector vel to randomised velocities
        sampled from a Maxwell distribution corresponding to the temperature T
 */
void ofApp::randomiseVelocity(vector<double> &vel, double T) {
    double sigma = sqrt(T); // Std. dev. of normal distribution
    
    // Sample random numbers from normal distribution with mean 0 and std. dev. sigma
    for (int i = 0; i < vel.size(); ++i) {
        double randr = ofRandom(1);
        double randt = ofRandom(1);
        
        randr = sqrt(-2.0 * log(randr));
        randt = sin(2 * PI * randt);
        
        vel[i] = sigma * randr * randt;
    }
}


/*
    ROUTINE setupSystem:
        Clears the system (removes all particles, and resets all parameters), before resetting the system
        to have the parameters listed in the arguments. Places the particles on a grid, and gives them random velocities
        sampled from a Maxwell distribution of the chosen temperature. Calculates the initial forces and energies
        before integrating 20 times to begin the simulation. 
 */
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


/*
    ROUTINE drawGaussian:
        Takes a reference to a Gaussian object, the box width and length (boxw, boxl),
        and whether this Gaussian is the one currently in focus (selected), and draws
        the Gaussian as blended concentric circles, coloured by the scaling factor `scale'
        stored in the Gaussian object, which depends on the audio input.
 
 */
void ofApp::drawGaussian(Gaussian& g, double boxw, double boxl, bool selected){
    double gA = g.getgAmp();       // amplitude
    double galpha = g.getgAlpha(); // inverse width
    // Centre
    double gx = g.getgex0();
    double gy = g.getgey0();
    // Volume scaling factor
    double volume = g.getScale();
    
    // Rescale between box size and window size
    double xscale = ofGetWidth() / boxw;
    double yscale = ofGetHeight() / boxl;
    
    // Determine the colour of the Gaussian, based on the amplitude
    ofColor color;
    float hue = 200;
    float saturation = gA > 0 ? 255 : 0;
    float brightness = 128;
    
    if (selected) {
        brightness *= 2.0f;
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
    drawFont.drawString("Kinetic energy", 0.05*ofGetWidth(), 0.1*ofGetHeight());
    ofSetColor(255, 255, 255);
    drawFont.drawString("Potential energy", 0.05*ofGetWidth(), 0.15*ofGetHeight() );
    
}

//--------------------------------------------------------------
/*
    ROUTINE setup:
        Constructor for the app. Sets up the system at the beginning, loads all the assets
        needed for the app (images and fonts), and initialises the audio input stream.
 */
void ofApp::setup(){
    
    // Default values for system parameters
    BOX_WIDTH = 15.0;
    BOX_LENGTH = BOX_WIDTH / ofGetWidth() * ofGetHeight();
    CUTOFF = 3.0;
    TIMESTEP = 0.002;
    N_PARTICLES = 50;
    TEMPERATURE = 0.5;

    // Load images and fonts for the UI
    circGradient.load("circ_gradient.png");
    playbutton.load("play-btn2.png");
    pausebutton.load("pause-btn2.png");
    drawFont.loadFont("Montserrat-Bold.ttf", 14);
    
    // initialise openFrameworks stuff
    ofSetFrameRate(60);
    
    // intitialise the system
    setupSystem(N_PARTICLES, TEMPERATURE, BOX_LENGTH, BOX_WIDTH, CUTOFF, TIMESTEP);
    
    // Set the booleans so that the audio input is turned on, as is the simulation,
    // but the UI, secret-Logan-mode, and energy graphs are off.
    audioOn = true;
    helpOn = false;
    loganOn = false;
    graphOn = false;
    playOn = true;

    // Initialise the timstep counter
    thermCounter = 0;
    
    selectedGaussian = -1; // No gaussian selected
    
    selectedSlider = 0; // Temperature slider selected
    
    // Setup the sound
    int bufferSize = 256;
    
    left.assign(bufferSize, 0.0);
    right.assign(bufferSize, 0.0);
    
    smoothedVol     = 0.0;
    scaledVol		= 0.0;
    sensitivity     = 0.04;
    
    soundStream.setup(this, 0, 2, 44100, bufferSize, 4);
    
    // Setup Logan image
    loganLeft.load("david-logan-posing-left.png");
    loganRight.load("david-logan-posing-right.png");
    loganShiftx = loganLeft.getWidth() / 2;
    loganShifty = loganLeft.getHeight() / 2;
    
    // Set the background to black
    ofBackground(0, 0, 0);
}

//--------------------------------------------------------------
/*
    ROUTINE update: 
        Part of the infinite update / draw loop.
        Update the status of the application, before the frame is drawn. 
 
        Currently performs the following tasks, when the simulation is not paused (i.e when playOn):
            
            1. Integrates the equations of motion 5 times.
            2. Thermostats (Berendsen) the system every 10 steps.
            3. Increments the (REDUNDANT) timestep counter, thermCounter.
            4. If the audio input is turned on:
                - Calculates the smoothed volume scaled between 0 and 1
                - Updates the amplitude, exponent, and drawing of the selected Gaussian according to 
                    this volume.
 
 */
void ofApp::update(){
    
    if (playOn) { // If not paused
        
        // Integrate 5 times, and thermostat every 10 steps
        for (int i = 0; i < 5; ++i) {
            theSystem.integrate(N_THREADS);

            //lets scale the vol up to a 0-1 range, and thermostat
            if (thermCounter % 10 == 0) {
                theSystem.berendsen(1.0);
            }
            thermCounter++; // Increment the timestep counter
        }
        
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
    
    // This really should be put somewhere else/removed, it seems unnecessary
    // to make a temporary vector of the box dimensions every frame, when the values
    // are stored in theSystem?!
    vector<double> BOX_SIZE;
    BOX_SIZE.push_back(theSystem.getWidth());
    BOX_SIZE.push_back(theSystem.getHeight());
    
    // 1. Draw the frame rate in the top left
    drawData("framerate", ofGetFrameRate(), 5, 25);
    
    // Set the resolution and fill settings for drawing the energy graphs if necessary
    ofFill();
    ofSetCircleResolution(10);
    // 2. Draw graphs in background if turned on.
    if (graphOn) drawGraph();
    
    // 3. Draw gaussians, with selected on top
    if (theSystem.getNGaussians() > 0) {
        ofSetCircleResolution(50);
        for (int g = 0; g < theSystem.getNGaussians(); g++){
            if (g != selectedGaussian)
                drawGaussian(theSystem.getGaussian(g), BOX_SIZE[0], BOX_SIZE[1], false);
        }
        drawGaussian(theSystem.getGaussian(selectedGaussian), BOX_SIZE[0], BOX_SIZE[1], true);
    }
    
    // Set up a load of temporary placeholders
    // There has to be a cleaner way to do this - this whole section
    // could probably be factored elsewhere?
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
    
    // Draw all the particles and their trails
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
        
        // Scale the velocities of particle i so that they can be mapped onto a colour range
        velx = ofMap(abs(tempVel[0]), 0, 1.5*v_avg, 0, 255);
        vely = ofMap(abs(tempVel[1]), 0, 1.5*v_avg, 0, 255);
        
        // Scale the forces so that they can deform the shapes of the ellipses by suitable amounts
        accx = ofMap(log(1.0+abs(tempAcc[0])), 0, 10, 20, 50);
        accy = ofMap(log(1.0+abs(tempAcc[1])), 0, 10, 20, 50);
        
        // Set the size of the trail depending on the size of the ellipses
        trailSize = (accx + accy) / 4.0;
        
        // Set the hue (colour) of the scaled velocity of the particle
        hue = ofMap((velx + vely) / 2, 0, 300, 170, 210, true);
        particleColor.setHsb(hue, 255, 255);
        
        trailColor.set(particleColor);
        
        ofSetCircleResolution(20);

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
    
    // Draw the UI if helpOn, otherwise draw message on how to turn the UI on.
    if (helpOn == true){
        drawUI();
    }
    else if (helpOn == false){
        ofSetColor(255, 255, 240);
        drawFont.drawString("press 'h' for controls", ofGetWidth()-1000, ofGetHeight()-40);
    }
}

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
        left[i]		= input[i*2]*0.5;
        right[i]	= input[i*2+1]*0.5;
        
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
        setupSystem(N_PARTICLES, TEMPERATURE, BOX_LENGTH, BOX_WIDTH, CUTOFF, TIMESTEP);
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
