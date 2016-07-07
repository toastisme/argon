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

ofPoint ofApp::box2screen(coord point, coord origin) {
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
void ofApp::setup() 
{
    
    // Default values for system parameters
    numParticles = 50;
    thermFreq = 0.1;
    
    double TEMPERATURE = 0.5;
    double BOX_WIDTH = 17.0;
    
    // NOTE: ofGetWidth and ofGetHeight do not give the right values here if
    // the screen we're trying to draw is to the right values later, but not
    // in ofApp::setup(). If this happens, we get regions where the particles
    // are inside the MD box, but not inside the screen.
    // This is an openFrameworks issue.
    double BOX_HEIGHT = BOX_WIDTH / ofGetWidth() * ofGetHeight();
    double TIMESTEP = 0.002;
    double CUTOFF = 3.0;
    
    // Set the potential UI parameters
    topHeight = ofGetHeight()/8;
    sideWidth = ofGetWidth()/7;
    buttonHeight = (ofGetHeight() - topHeight)/4;
    
    // openFrameworks initialisation
    ofSetFrameRate(60);
    ofBackground(0, 0, 0);

    // Load assets
    
    // graphics
    circGradient.load("circ_gradient.png");
    playButton.load("ButtonPlay.png");
    pauseButton.load("ButtonPause.png");
    resetButton.load("ButtonReset.png");
    audioOnButton.load("ButtonMic.png");
    audioOffButton.load("ButtonNoMic.png");
    addPointsButton.load("AddPointsButton.png");
    removePointsButton.load("RemovePointsButton.png");
    movePointsButton.load("MovePointsButton.png");
    changeSlopeButton.load("ChangeSlopeButton.png");
    
    // potential graphics
    
    ljThumbnail.load("LJThumbnail.png");
    squareThumbnail.load("SquareThumbnail.png");
    morseThumbnail.load("MorseThumbnail.png");
    
    loganLeft.load("david-logan-posing-left.png");
    loganRight.load("david-logan-posing-right.png");
    loganShiftx = loganLeft.getWidth() / 2;
    loganShifty = loganLeft.getHeight() / 2;
    
    // fonts
    uiFont14.load("Montserrat-Bold.ttf", 14);
    uiFont12.load("Montserrat-Bold.ttf", 12);
    uiFont10.load("Montserrat-Bold.ttf", 10);
    
    // Initialise theSystem
    setupSystem(numParticles, TEMPERATURE, BOX_WIDTH, BOX_HEIGHT, TIMESTEP, CUTOFF);
    // Set the booleans so that the audio input is turned on, as is the simulation,
    // but the UI, secret-Logan-mode, and energy graphs are off.
    audioOn = true;
    controlsOn  = false;
    loganOn = false;
    graphOn = false;
    playOn  = true;
    drawOn  = false;
    customPotentialOn = false;

    selectedGaussian = -1; // No gaussian selected
    
    // Setup the sound
    int bufferSize = 256;
    
    left.assign(bufferSize, 0.0);
    right.assign(bufferSize, 0.0);
    
    smoothedVol = 0.0;
    scaledVol   = 0.0;
    sensitivity = 0.04;
    
    soundStream.setup(this, 0, 2, 44100, bufferSize, 4);
    
    // Setup menu UI
    
    int mt = 490; // top of menu
    ofColor bgcolor = ofColor(80, 80, 80, 80);
    ofColor textcolor = ofColor(255, 255, 240);
    
    menuUI = gui::UIContainer(0, mt, 1024, 600 - mt);
    
    menuUI.addChild(new gui::RectAtom(bgcolor, 0, mt, 1024, 190));
    
    menuUI.addChild(new gui::SliderContainer("Temperature (K)", uiFont12, textcolor,
                                             [&] () { return theSystem.getTemp() * 120; },
                                             [&] (double set) { theSystem.setTemp(set / 120.0); },
                                             0, 1000, "%1.1lf", 5, mt + 5, 150, 450, 70, 30));
    
    menuUI.addChild(new gui::SliderContainer("Particles", uiFont12, textcolor,
                                             [&] () { return numParticles; },
                                             [&] (double set) { numParticles = (int)set; },
                                             2, 200, "%1.0lf", 5, mt + 40, 150, 450, 70, 30));
    
    menuUI.addChild(new gui::SliderContainer("Mic sensitivity", uiFont12, textcolor,
                                             [&] () { return sensitivity; },
                                             [&] (double set) { sensitivity = (float)set; },
                                             0.005, 0.135, "%1.3lf", 5, mt + 75, 150, 450, 70, 30));
    
    menuUI.addChild(new gui::TextAtom("Play / pause:", uiFont10, textcolor,
                                      gui::RIGHT, 690, mt + 5, 100, 30));
    menuUI.addChild(new gui::TextAtom("Reset:", uiFont10, textcolor,
                                      gui::RIGHT, 690, mt + 40, 100, 30));
    menuUI.addChild(new gui::TextAtom("Mic on/off:", uiFont10, textcolor,
                                      gui::RIGHT, 690, mt + 75, 100, 30));
    
    menuUI.addChild(new gui::SetColour(ofColor(255, 255, 255)));
    menuUI.addChild(new gui::ButtonToggleAtom(playOn, playButton, pauseButton,
                                              800, mt + 5, 30, 30));
    menuUI.addChild(new gui::ButtonAtom([&] () { setupSystem(); }, removePointsButton,
                                              800, mt + 40, 30, 30));
    menuUI.addChild(new gui::ButtonToggleAtom(audioOn, audioOnButton, audioOffButton,
                                              800, mt + 75, 30, 30));
    
    menuUI.addChild(new gui::TextAtom("Key Commands", uiFont12, textcolor,
                                      gui::TOP, 850, mt + 5, 174, 20));
    
    menuUI.addChild(new gui::TextAtom("Change gaussian:", uiFont10, textcolor,
                                      gui::RIGHT, 850, mt + 25, 140, 20));
    menuUI.addChild(new gui::TextAtom("g", uiFont10, textcolor,
                                      gui::LEFT, 995, mt + 25, 24, 20));
    
    menuUI.addChild(new gui::TextAtom("Remove gaussian:", uiFont10, textcolor,
                                      gui::RIGHT, 850, mt + 45, 140, 20));
    menuUI.addChild(new gui::TextAtom("k", uiFont10, textcolor,
                                      gui::LEFT, 995, mt + 45, 24, 20));
    
    menuUI.addChild(new gui::TextAtom("Show energies:", uiFont10, textcolor,
                                      gui::RIGHT, 850, mt + 65, 140, 20));
    menuUI.addChild(new gui::TextAtom("e", uiFont10, textcolor,
                                      gui::LEFT, 995, mt + 65, 24, 20));
    
    menuUI.addChild(new gui::TextAtom("Show potentials:", uiFont10, textcolor,
                                      gui::RIGHT, 850, mt + 85, 140, 20));
    menuUI.addChild(new gui::TextAtom("d", uiFont10, textcolor,
                                      gui::LEFT, 995, mt + 85, 24, 20));
    
    menuUI.addChild(new gui::ValueAtom([&] () { return ofGetFrameRate(); },
                                       "%4.1lf", uiFont14, textcolor,
                                       gui::TOP_RIGHT, 819, 5, 200, 100));
    menuUI.makeInvisible();
    menuUI.mouseReleased(0, 0, 0);
    
    // Setup Potential UI
    
    potentialUI = gui::UIContainer(0, 0 , 1024, 600);
    potentialUI.addChild(new gui::RectAtom(bgcolor, 0, 0, 1024, 600)); //
    
    potentialUI.addChild(new gui::TextAtom("Select a pair potential", uiFont14, textcolor, gui::LEFT, 1.2*sideWidth, topHeight/5, 100, 30));
    
    potentialUI.addChild(new gui::TextAtom("Lennard-Jones", uiFont12, textcolor, gui::LEFT, 30, topHeight+1.5*buttonHeight/2, 100, 30));
    potentialUI.addChild(new gui::ButtonAtom([&] () { theSystem.setPotential(md::LENNARD_JONES); }, ljThumbnail, 30, topHeight, 85, 85));
    
    potentialUI.addChild(new gui::TextAtom("Square Well", uiFont12, textcolor, gui::LEFT, 30, topHeight+3.55*buttonHeight/2, 100, 30));
    potentialUI.addChild(new gui::ButtonAtom([&] () {  theSystem.setPotential(md::SQUARE_WELL); }, squareThumbnail, 30, topHeight+2*buttonHeight/2, 85, 85));
    
    potentialUI.addChild(new gui::TextAtom("Morse", uiFont12, textcolor, gui::LEFT, 30, topHeight+5.4*buttonHeight/2, 100, 30));
    potentialUI.addChild(new gui::ButtonAtom([&] () { theSystem.setPotential(md::MORSE); }, morseThumbnail, 30, topHeight+4.0*buttonHeight/2, 85, 85));
    
    potentialUI.addChild(new gui::TextAtom("Custom", uiFont12, textcolor, gui::LEFT, 30, topHeight+7.3*buttonHeight/2, 100, 30));
    potentialUI.addChild(new gui::ButtonAtom([&] () { theSystem.setPotential(md::CUSTOM); }, squareThumbnail, 30, topHeight+6.0*buttonHeight/2, 85, 85));
    potentialUI.addChild(new gui::PotentialAtom(theSystem, 0.5, theSystem.getCutoff(), 150, 146, 75, 1024 - 40 - 146, 600 - 40 - 75));
    
    potentialUI.makeInvisible();
    potentialUI.mouseReleased(0, 0, 0);
    
    // Setup Custom Potential UI
    
    customPotentialUI = gui::UIContainer(0, 0 , 1024, 600);
    customPotentialUI.addChild(new gui::TextAtom("Add points:", uiFont10, textcolor, gui::CENTRE, 3.0*sideWidth, topHeight/4.5, 100, 30));
    //customPotentialUI.addChild(new gui::ButtonAtom([&] () { selectCustomButton(1); }, addPointsButton, 3.65*sideWidth, topHeight/4.5, 30, 30));
    customPotentialUI.addChild(new gui::TextAtom("Move points:", uiFont10, textcolor, gui::CENTRE, 3.90*sideWidth, topHeight/4.5, 100, 30));
    //customPotentialUI.addChild(new gui::ButtonAtom([&] () { selectCustomButton(2); }, movePointsButton, 4.58*sideWidth, topHeight/4.5, 30, 30));
    customPotentialUI.addChild(new gui::TextAtom("Change slope:", uiFont10, textcolor, gui::CENTRE, 4.9*sideWidth, topHeight/4.5, 100, 30));
    //customPotentialUI.addChild(new gui::ButtonAtom([&] () { selectCustomButton(3); }, changeSlopeButton, 5.60*sideWidth, topHeight/4.5, 30, 30));
    customPotentialUI.addChild(new gui::TextAtom("Remove points:", uiFont10, textcolor, gui::CENTRE, 5.95*sideWidth, topHeight/4.5, 100, 30));
    //customPotentialUI.addChild(new gui::ButtonAtom([&] () { selectCustomButton(4); }, removePointsButton, 6.70*sideWidth, topHeight/4.5, 30, 30));
    
    customPotentialUI.makeInvisible();
    customPotentialUI.mouseReleased(0, 0, 0);
}

/*
    ROUTINE setupSystem:
        Sets up and regrids the system. First clears all particles, then sets basic constants. Then, add the
        particles on a grid, initialise the forces and energies and save initial positions to prevousPositions
 
        Overloaded version takes no parameters, just regridding the system based on ofApp.numParticles
 */
void ofApp::setupSystem(int particles, double temperature, double box_width, double box_height, double timestep, double cutoff) {
    theSystem.clearSystem();
    
    theSystem.setBox(box_width, box_height);
    theSystem.setTemp(temperature);
    theSystem.setTimestep(timestep);
    theSystem.setCutoff(cutoff);
    
    theSystem.addParticlesGrid(particles);
    
    theSystem.forcesEnergies(N_THREADS);
    theSystem.savePreviousValues();
}
    
void ofApp::setupSystem() {
    theSystem.clearSystem();
    theSystem.addParticlesGrid(numParticles);
    theSystem.forcesEnergies(N_THREADS);
    theSystem.savePreviousValues();
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
    
    // If not paused, integrate 5 times with a thermostat frequency of 0.1
    if (playOn) { theSystem.run(5, thermFreq, N_THREADS); }
        
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
    coord pos = theSystem.getPos(index, nframes);
    ofDrawEllipse(box2screen(pos), radius_x * 2, radius_y * 2);
}

void ofApp::drawParticle(int index, double radius, int nframes) {
    coord pos = theSystem.getPos(index, nframes);
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
    u_char brightness = 180;
    
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
    ROUTINE drawGraph:
        Draws the kinetic/potential energy graphs as small red/blue circles
        in the background. 
 
        This rescales the values for the graphs using minEKin/Pot and maxEKin/Pot
        as the minimum/maximum values respectively.
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
    float ekinMaxScale = theSystem.getMaxEkin();
    float ekinMinScale = theSystem.getMinEkin();
    float epotMaxScale = theSystem.getMaxEpot();
    float epotMinScale = theSystem.getMinEpot();
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
        ekin = ofMap(theSystem.getPreviousEkin(i), ekinMinScale, ekinMaxScale, 0, 0.9*winHeight);
        ofDrawCircle(xOffset + 5*i, yOffset - ekin, radius);
        
        ofSetColor(255, 255, 255);
        epot = ofMap(fabs(theSystem.getPreviousEpot(i)), epotMinScale, epotMaxScale, 0, 0.9*winHeight);
        ofDrawCircle(xOffset + 5*i, yOffset - epot, radius);

    }
    
    // Label the two graphs in the top left corner.
    ofSetColor(200, 0, 0);
    uiFont14.drawString("Kinetic energy", 0.05*ofGetWidth(), 0.1*ofGetHeight());
    ofSetColor(255, 255, 255);
    uiFont14.drawString("Potential energy", 0.05*ofGetWidth(), 0.15*ofGetHeight() );
    
}


// Custom potential function
void ofApp::drawCustomPotential(float min_x, float min_y, float max_x, float max_y, std::vector<float> xpoints, std::vector<float> ypoints, std::vector<float> partx,std::vector<float> party){
    
    float x,y;
    
    CustomPotential& customPotential = theSystem.getCustomPotential();
    
    // Rescale the default position of the four-point spline
    // Only occurs the first time the custom potential is selected
    if (customPotentialOn == false){
        double x0 = ofMap(min_x*1.025, min_x, max_x,sideWidth + 40, ofGetWidth() - 40, true);
        double x_end = ofMap(max_x, min_x, max_x, sideWidth + 40, ofGetWidth() - 40, true);
        double y0 = ofGetHeight()/5.5;
        double y_end = ofGetHeight()/1.8;
    
        // Move the two points to the scaled position;
        customPotential.getSpline().movePoint(1, x_end, y_end, 0);
        customPotential.getSpline().movePoint(0, x0, y0, 0);
        
        // After calling the custom potential the first time, skip this block
        customPotentialOn = !customPotentialOn;
    }
    
    // Once the initial two-point spline has been rescaled, update the spline each iteration
    else {
        // Set color, position of each point in the spline
        for (int i = 1; (i < customPotential.getSpline().points()+1); i++){
            ofSetColor(0, 200, 240,200);
            ofFill();
            ofDrawCircle(customPotential.getSpline().getPoint(i-1).x, customPotential.getSpline().getPoint(i-1).y, 10);
            ofSetColor(255, 255, 255,90);
            ofNoFill();
            ofDrawCircle(customPotential.getSpline().getPoint(i-1).x, customPotential.getSpline().getPoint(i-1).y, 10);
        }
        
        // Map every x value to the UI and obtain y, add each value to ypoints
        for (int i = 0; i < xpoints.size(); i++){
            xpoints[i] = ofMap(xpoints[i], min_x, max_x, sideWidth + 40, ofGetWidth() - 40, true );
            y = customPotential.getSpline().value(xpoints[i]);
            // If y is out of the y limits, set y to the limit
            max_y = ( y > max_y ? y : max_y);
            min_y = ( y < min_y ? y : min_y);
            ypoints.push_back(y);
        }
        
        // Map every x particle value to the UI and obtain y, add each value to party
        for (int i = 0; i < theSystem.getN() - 1; i++){
            partx[i] = ofMap(partx[i], min_x, max_x, sideWidth + 40, ofGetWidth() - 40, true);
            y = customPotential.getSpline().value(partx[i]);
            party.push_back(y);
            
        }
        
        // Draw the potential
        ofSetColor(255,255,255, 220);
        for (int i = 1; i < xpoints.size() - 1; i++){
            ofDrawLine(xpoints[i], ypoints[i], xpoints[i+1], ypoints[i+1]);
        }
        
        // Draw the particles on the potential
        ofSetColor(0, 100, 220, 220);
        for (int i = 0; i < theSystem.getN() - 1; i++){
            ofFill();
            ofDrawCircle(partx[i], party[i], 4);
        }

    }
    
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
            5. If controlsOn (UI showing), draws the UI. Otherwise, draws message
                on how to turn on UI in bottom left corner.
 */
void ofApp::draw(){
    
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
    coord tempVel;
    coord tempAcc;
    
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
        
        hue = ofMap(abs(tempVel.x) + abs(tempVel.y), 0, 3 * v_avg, 170, 210, true);
        particleColor.setHsb(hue, 255, 255);
        
        radius_x = ofMap(log(1.0 + abs(tempAcc.x)), 0, 10, 10, 25);
        radius_y = ofMap(log(1.0 + abs(tempAcc.y)), 0, 10, 10, 25);
        radius = (radius_x + radius_y) / 2;
        
        if (loganOn) {
            ofSetColor(particleColor);
            coord pos = theSystem.getPos(i);
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
    

    if (drawOn) {
        potentialUI.draw();
    }
    // Draw the UI if controlsOn, otherwise draw message on how to turn the UI on.
    else if (not controlsOn) {
        ofSetColor(255, 255, 240);
        uiFont14.drawString("press 'h' for controls", 10, ofGetHeight()-10);
    }
    
    menuUI.draw();
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
            - d/D           = open/close drawable pair potential
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
        if (!drawOn) {
            controlsOn = !controlsOn;
            menuUI.toggleVisible();
        }
    }
    
    else if (key == 'r' || key == 'R') { // Reset the system to have the current values of the sliders
        coord box = theSystem.getBox();
        setupSystem();
        //setupSystem(numParticles, theSystem.getTemp(), box.x, box.y, theSystem.getTimestep(), theSystem.getCutoff());
    }
    
    else if (key == 'p' || key == 'P') { // Pause/restart the simulation
        playOn = !playOn;
    }
    
    else if (key == 'd' || key == 'D') { // Drawing interface
        if (!controlsOn) {
            drawOn = !drawOn;
            potentialUI.toggleVisible();
        }
    }
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    if (controlsOn) {
        menuUI.mouseMoved(x, y);
    }
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    if (controlsOn) {
        menuUI.mouseMoved(x, y);
    }
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
    
    
    if ( drawOn ) { // Mouse controls drawing UI
        potentialUI.mousePressed(x, y, button);

        //theSystem.setPotential(&customPotential);

        
    } else if (controlsOn && menuUI.getRect().inside(x, y)) { // Mouse controls menu
        // pass through event to children
        menuUI.mousePressed(x, y, button);
        
    } else { // Mouse controls Gaussian placement
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
    /*
    if (selectedPotential == 4){ // Mouse controls custom potential modification
        customPotentialUI.mousePressed(x,y,button);
        // Add customPotential points at clicked location when 'add points' button is on
        if (customPotentialButton == 1){
            // Ensure the point is created within the box
                    }
    
        // Remove clicked customPotential points when the 'remove points' button is on
        else if (customPotentialButton == 4){
        }
    }
     */
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    if (controlsOn) {
        menuUI.mouseReleased(x, y, button);
    }
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
