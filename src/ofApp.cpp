/*
 StarredMD
 
 Copyright (c) 2016 David McDonagh, Robert Shaw, Staszek Welsh
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */

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
    
    // Initialise theSystem with 50 particles at 60K
    theSystem.setTemp(0.5);
    theSystem.setTimestep(0.002);
    theSystem.setCutoff(3.0);
    theSystem.setFreq(0.1);
    theSystem.setStepsPerUpdate(5);
    theSystem.setNAfterReset(50);
    
    double BOX_WIDTH = 17.0;
    // NOTE: ofGetWidth and ofGetHeight do not give the right values here if
    // the screen we're trying to draw is to the right values later, but not
    // in ofApp::setup(). If this happens, we get regions where the particles
    // are inside the MD box, but not inside the screen.
    // This is an openFrameworks issue.
    double BOX_HEIGHT = BOX_WIDTH / ofGetWidth() * ofGetHeight();
    theSystem.setBox(BOX_WIDTH, BOX_HEIGHT);
    
    theSystem.resetSystem();
    
    // Set the booleans so that the secret-Logan-mode, energy graphs,
    // potential viewer and custom potential are initially turned off
    loganOn = false;
    graphOn = false;
    drawOn  = false;
    customPotentialOn = false;

    selectedGaussian = -1; // No gaussian selected
    
    /*
         Setup menu UI
     */
    
    // colours
    ofColor bgcolor = ofColor(80, 80, 80, 80);
    ofColor textcolor = ofColor(255, 255, 240);
    
    // setup base container
    menuUI = gui::UIContainer(0, 490, 1024, 110);
    
    // menu background
    menuUI.addChild(new gui::RectAtom(bgcolor, 0, 0, 1024, 110));
    
    // sliders
    menuUI.addChild(new gui::SliderContainer("Temperature (K)",
                                             [&] () { return theSystem.getTemp() * 120; },   // factor of 120 to convert to kelvin
                                             [&] (double set) { theSystem.setTemp(set / 120.0); },
                                             0, 1000, uiFont12, textcolor, 1,
                                             5, 5, 150, 450, 70, 5, 30));
    
    menuUI.addChild(new gui::SliderContainer("Particles",
                                             [&] () { return theSystem.getNAfterReset(); },
                                                    // N is an int: add 0.5 to the argument to set N to round(set) instead of floor(set)
                                             [&] (double set) { theSystem.setNAfterReset(set + 0.5); },
                                             2, 200, uiFont12, textcolor, 0,
                                             5, 40, 150, 450, 70, 5, 30));
    
    menuUI.addChild(new gui::SliderContainer("Simulation speed",
                                             [&] () { return theSystem.getStepsPerUpdate(); },
                                             [&] (double set) { theSystem.setStepsPerUpdate(set + 0.5); },
                                             1, 20, uiFont12, textcolor, 0,
                                             5, 75, 150, 450, 70, 5, 30));
    
    // button text
    menuUI.addChild(new gui::TextAtom("Play / pause:", uiFont10, textcolor,
                                      POS_RIGHT, 690, 5, 100, 30));
    menuUI.addChild(new gui::TextAtom("Reset:", uiFont10, textcolor,
                                      POS_RIGHT, 690, 40, 100, 30));
    menuUI.addChild(new gui::TextAtom("Mic on/off:", uiFont10, textcolor,
                                      POS_RIGHT, 690, 75, 100, 30));
    
    // buttons
    menuUI.addChild(new gui::SetColour(ofColor(255, 255, 255)));
    menuUI.addChild(new gui::ButtonToggleAtom([&] () { return theSystem.getRunning(); }, [&] (bool set) { theSystem.setRunning(set); },
                                              playButton, pauseButton,
                                              800, 5, 30, 30));
    menuUI.addChild(new gui::ButtonAtom([&] () { theSystem.resetSystem(); }, resetButton,
                                              800, 40, 30, 30));
    menuUI.addChild(new gui::ButtonToggleAtom([&] () { return micInput.getActive(); }, [&] (bool set) { micInput.setActive(set); },
                                              audioOnButton, audioOffButton,
                                              800, 75, 30, 30));
    
    // controls list
    menuUI.addChild(new gui::TextAtom("Key Commands", uiFont12, textcolor,
                                      POS_TOP, 850, 5, 174, 20));
    
    menuUI.addChild(new gui::TextAtom("Change gaussian:", uiFont10, textcolor,
                                      POS_RIGHT, 850, 25, 140, 20));
    menuUI.addChild(new gui::TextAtom("g", uiFont10, textcolor,
                                      POS_LEFT, 995, 25, 24, 20));
    
    menuUI.addChild(new gui::TextAtom("Remove gaussian:", uiFont10, textcolor,
                                      POS_RIGHT, 850, 45, 140, 20));
    menuUI.addChild(new gui::TextAtom("k", uiFont10, textcolor,
                                      POS_LEFT, 995, 45, 24, 20));
    
    menuUI.addChild(new gui::TextAtom("Show energies:", uiFont10, textcolor,
                                      POS_RIGHT, 850, 65, 140, 20));
    menuUI.addChild(new gui::TextAtom("e", uiFont10, textcolor,
                                      POS_LEFT, 995, 65, 24, 20));
    
    menuUI.addChild(new gui::TextAtom("Show potentials:", uiFont10, textcolor,
                                      POS_RIGHT, 850, 85, 140, 20));
    menuUI.addChild(new gui::TextAtom("d", uiFont10, textcolor,
                                      POS_LEFT, 995, 85, 24, 20));
    
    // framerate counter
    menuUI.addChild(new gui::ValueAtom([&] () { return ofGetFrameRate(); },
                                       1, uiFont14, textcolor,
                                       POS_BOTTOM_RIGHT, 819, -105, 200, 100));
    menuUI.makeInvisible();
    menuUI.mouseReleased(0, 0, 0);
    
    // Setup Potential UI
    
    potentialUI = gui::UIContainer(0, 0 , 1024, 600);
    
    // Setup potential atoms
    potentialUI.addChild(new gui::PotentialAtom(theSystem, 150, 0.9, 3.0, -2, 2, 146, 75, 1024 - 40 - 146, 600 - 40 - 75));
    
    splineContainerIndex = potentialUI.addIndexedChild(new gui::SplineContainer(theSystem.getCustomPotential(), 0.9, 3.0, -2, 2, 12, 146, 75, 1024 - 40 - 146, 600 - 40 - 75));

    potentialUI.addChild(new gui::RectAtom(bgcolor, 0, 0, 1024, 600)); //
    
    potentialUI.addChild(new gui::TextAtom("Select a pair potential", uiFont14, textcolor, POS_LEFT, 1.2*sideWidth, topHeight/5, 100, 30));
    
    potentialUI.addChild(new gui::TextAtom("Lennard-Jones", uiFont12, textcolor, POS_LEFT, 30, topHeight+1.5*buttonHeight/2, 100, 30));
    potentialUI.addChild(new gui::ButtonAtom([&] () { theSystem.setPotential(md::LENNARD_JONES);
                                                    potentialUI.getChild(splineContainerIndex)->makeInvisible(); },
                                                    ljThumbnail, 30, topHeight, 85, 85));
    
    potentialUI.addChild(new gui::TextAtom("Square Well", uiFont12, textcolor, POS_LEFT, 30, topHeight+3.55*buttonHeight/2, 100, 30));
    potentialUI.addChild(new gui::ButtonAtom([&] () { theSystem.setPotential(md::SQUARE_WELL);
                                                    potentialUI.getChild(splineContainerIndex)->makeInvisible(); },
                                                    squareThumbnail, 30, topHeight+2*buttonHeight/2, 85, 85));
    
    potentialUI.addChild(new gui::TextAtom("Morse", uiFont12, textcolor, POS_LEFT, 30, topHeight+5.4*buttonHeight/2, 100, 30));
    potentialUI.addChild(new gui::ButtonAtom([&] () { theSystem.setPotential(md::MORSE);
                                                    potentialUI.getChild(splineContainerIndex)->makeInvisible(); },
                                                    morseThumbnail, 30, topHeight+4.0*buttonHeight/2, 85, 85));
    
    potentialUI.addChild(new gui::TextAtom("Custom", uiFont12, textcolor, POS_LEFT, 30, topHeight+7.3*buttonHeight/2, 100, 30));
    potentialUI.addChild(new gui::ButtonAtom([&] () { theSystem.setPotential(md::CUSTOM);
                                                    potentialUI.getChild(splineContainerIndex)->makeVisible(); },
                                                    squareThumbnail, 30, topHeight+6.0*buttonHeight/2, 85, 85));
    
    potentialUI.makeInvisible();
    potentialUI.getChild(splineContainerIndex)->makeVisible();
    potentialUI.mouseReleased(0, 0, 0);
    
    // start menu as invisible
    menuUI.makeInvisible();
    
    // 'Press h for menu' text
    // This defaults to visible, but we're adding it to a menu we've just set to invisible
    // This means the text is visible when the rest of the menu is invisible and vice-versa
    // Calling toggleVisible toggles everything correctly
    // This might need to be moved to a separate UI container to work well with other UI elements
    menuUI.addChild(new gui::TextAtom("Press h for menu", uiFont14, textcolor,
                                      POS_BOTTOM_LEFT, 5, 0, 1024, 105));
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
    
    // If not paused, integrate the system
    theSystem.run();
        
    if (micInput.getActive()) {
        // get volume, scaled to between 0 and 1
        double scaledVol = micInput.getVolume();
        
        // Update the currently selected Gaussian, so that quiet-> loud results in
        // a change from an attractive, wide Gaussian, to a repulsive, narrow Gaussian.
        if ( selectedGaussian > -1)
            theSystem.updateGaussian(selectedGaussian, 50 - scaledVol*100, 0.8 - 0.5*scaledVol,
                                     theSystem.getGaussianX0(selectedGaussian),
                                     theSystem.getGaussianY0(selectedGaussian));
    }
}

//--------------------------------------------------------------
// DRAWING
//--------------------------------------------------------------

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
            5. Draw the menu UI
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

    // draw the menu
    menuUI.draw();
}

//--------------------------------------------------------------------
// INPUT & EVENT HANDLING
//--------------------------------------------------------------------

void ofApp::audioIn(float * input, int bufferSize, int nChannels){
    
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
        micInput.toggleActive();
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
            menuUI.toggleVisible();
        }
    }
    
    else if (key == 'r' || key == 'R') { // Reset the system to have the current values of the sliders
        theSystem.resetSystem();
    }
    
    else if (key == 'p' || key == 'P') { // Play/pause the simulation
        theSystem.toggleRunning();
    }
    
    else if (key == 'd' || key == 'D') { // Drawing interface
        if (not menuUI.getVisible()) {
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
    if (menuUI.getVisible()) {
        menuUI.mouseMoved(x, y);
    } else if (potentialUI.getVisible()) {
        potentialUI.mouseMoved(x, y);
    }
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    if (menuUI.getVisible()) {
        menuUI.mouseMoved(x, y);
    } else if (potentialUI.getVisible()) {
        potentialUI.mouseMoved(x, y);
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
    
    
    if (potentialUI.getVisible()) { // Mouse controls drawing UI
        potentialUI.mousePressed(x, y, button);
        
    } else if (menuUI.getVisible() && menuUI.getRect().inside(x, y)) { // Mouse controls menu
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
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    if (menuUI.getVisible()) {
        menuUI.mouseReleased(x, y, button);
    } else if (potentialUI.getVisible()) {
        potentialUI.mouseReleased(x, y, button);
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
