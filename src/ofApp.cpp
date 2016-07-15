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
    optionsButtonUp.load("OptionsButtonUp.png");
    optionsButtonDown.load("OptionsButtonDown.png");
    optionsEnergyButton.load("OptionsEnergyButton.png");
    optionsMainMenuButton.load("OptionsMainMenuButton.png");
    optionsPotentialButton.load("OptionsPotentialButton.png");
    optionsControlsButton.load("OptionsControlsButton.png");
    
    // potential graphics
    
    ljThumbnail.load("LJThumbnail.png");
    squareThumbnail.load("SquareThumbnail.png");
    morseThumbnail.load("MorseThumbnail.png");
    customThumbnail.load("CustomThumbnail.png");
    
    loganLeft.load("david-logan-posing-left.png");
    loganRight.load("david-logan-posing-right.png");
    
    // fonts
    uiFont14.load("Montserrat-Bold.ttf", 14);
    uiFont12.load("Montserrat-Bold.ttf", 12);
    uiFont10.load("Montserrat-Bold.ttf", 10);
    
    // Setup audio stream
    ofSoundStreamSetup(0, 2, this, 44000, 256, 4);
    micInput.setStream(&soundStream);
    
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

    // Setup system UI
    systemUI = gui::UIContainer(0, 0, ofGetWidth(), ofGetHeight());
    
    // Add the gaussian container
    gaussianContainerIndex = systemUI.addIndexedChild(new gui::GaussianContainer(theSystem, circGradient, 30.0, 0, 0, ofGetWidth(), ofGetHeight()));
    
    // And the particles themselves
    systemAtomIndex = systemUI.addIndexedChild(new gui::SystemAtom(theSystem, loganLeft, loganRight, 0, 0, ofGetWidth(), ofGetHeight()));
    
    // Setup graph UI
    graphUI = gui::UIContainer(ofGetWidth()/6, ofGetHeight()/6, 2*ofGetWidth()/3, 2*ofGetHeight()/3);
    
    // Add the graph itself
    graphUI.addChild(new gui::EnergyGraphAtom(theSystem, 0, 0, 2*ofGetWidth()/3, 2*ofGetHeight()/3));
    
    // Add a legend
    graphUI.addChild(new gui::TextAtom("Kinetic Energy", uiFont14, ofColor(200, 0, 0),
                                       POS_RIGHT, 500, 320, 100, 30));
    graphUI.addChild(new gui::TextAtom("Potential Energy", uiFont14, ofColor(255, 255, 255),
                                       POS_RIGHT, 500, 355, 100, 30));
    
    graphUI.makeInvisible();
    
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
    
    
    
    
    potentialUI = gui::UIContainer(50, 50, 985, 600);
    potentialUI.addChild(new gui::RectAtom(bgcolor, 0, 0, 924, 500));
    potentialUI.addChild(new gui::RectAtom(bgcolor, 150, 0, 774, 500));
    
    // Setup potential atoms
    potentialUI.addChild(new gui::PotentialAtom(theSystem, 300, 0.95, 3.0, -2, 2,
                                                150, 0, 774, 500));
    splineContainerIndex = potentialUI.addIndexedChild(new gui::SplineContainer(theSystem.getCustomPotential(), 0.95, 3.0, -2, 2, 12,
                                                                                150, 0, 774, 500));
    
    potentialUI.addChild(new gui::SetColour(textcolor));
    
    potentialUI.addChild(new gui::ButtonAtom([&] () { theSystem.setPotential(md::LENNARD_JONES);
                                                      potentialUI.getChild(splineContainerIndex)->makeInvisible(); },
                                             ljThumbnail, 25, 0, 100, 100));
    potentialUI.addChild(new gui::TextAtom("Lennard-Jones", uiFont12, textcolor, POS_TOP, 0, 100, 150, 25));
    
    potentialUI.addChild(new gui::ButtonAtom([&] () { theSystem.setPotential(md::SQUARE_WELL);
                                                      potentialUI.getChild(splineContainerIndex)->makeInvisible(); },
                                             squareThumbnail, 25, 125, 100, 100));
    potentialUI.addChild(new gui::TextAtom("Square Well", uiFont12, textcolor, POS_TOP, 0, 225, 150, 25));
    
    potentialUI.addChild(new gui::ButtonAtom([&] () { theSystem.setPotential(md::MORSE);
                                                      potentialUI.getChild(splineContainerIndex)->makeInvisible(); },
                                             morseThumbnail, 25, 250, 100, 100));
    potentialUI.addChild(new gui::TextAtom("Morse", uiFont12, textcolor, POS_TOP, 0, 350, 150, 25));
    
    potentialUI.addChild(new gui::ButtonAtom([&] () { theSystem.setPotential(md::CUSTOM);
                                                      potentialUI.getChild(splineContainerIndex)->makeVisible(); },
                                             customThumbnail, 25, 375, 100, 100));
    potentialUI.addChild(new gui::TextAtom("Custom", uiFont12, textcolor, POS_TOP, 0, 475, 150, 25));
    
    potentialUI.makeInvisible();
    potentialUI.getChild(splineContainerIndex)->makeVisible();
    potentialUI.mouseReleased(0, 0, 0);
    
    // start menu as invisible
    menuUI.makeInvisible();
    
    //Options menu
    
    optionsUI = gui::UIContainer(0, 0, 1024, 60);
    optionsUI.addChild(new gui::RectAtom(bgcolor, 0, 0, 1024, 45));
    optionsUI.addChild(new gui::TextAtom("Controls:", uiFont12, textcolor, POS_TOP, 50, 18, 150, 25));
    optionsUI.addChild(new gui::TextAtom("Change potential:", uiFont12, textcolor, POS_TOP, 252, 18, 150, 25));
    optionsUI.addChild(new gui::TextAtom("Display energy graph:", uiFont12, textcolor, POS_TOP, 500, 18, 150, 25));
    optionsUI.addChild(new gui::TextAtom("Main menu:", uiFont12, textcolor, POS_TOP, 740, 18, 150, 25));
    
    optionsUI.addChild(new gui::ButtonAtom([&] () { optionsUI.makeInvisible(); }, optionsButtonUp,
                                        990, 10, 30, 30));
    
    optionsUI.addChild(new gui::ButtonAtom([&] () { if (not potentialUI.getVisible()) {
        menuUI.toggleVisible();}; }, optionsControlsButton, 170, 10, 30, 30));
    
    optionsUI.addChild(new gui::ButtonAtom([&] () { if (not menuUI.getVisible()) {
        potentialUI.toggleVisible();} }, optionsPotentialButton, 408, 10, 30, 30));

    optionsUI.addChild(new gui::ButtonAtom([&] () { graphUI.toggleVisible(); }, optionsEnergyButton,
                                           672, 10, 30, 30));
    optionsUI.addChild(new gui::ButtonAtom([&] () { optionsUI.makeVisible(); }, optionsMainMenuButton,
                                           867, 10, 30, 30));
    optionsUI.makeInvisible();
    optionsUI.mouseReleased(0, 0, 0);
    
    optionsOffUI = gui::UIContainer(985, 0, 40, 40);
    optionsOffUI.addChild(new gui::SetColour(ofColor(255, 255, 255)));
    optionsOffUI.addChild(new gui::ButtonAtom([&] () { optionsUI.makeVisible(); }, optionsButtonDown,
                                           5, 10, 30, 30));
    
    optionsOffUI.makeVisible();
    
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
        systemUI.getChild(gaussianContainerIndex)->audioIn(scaledVol);
        
    }
}

//--------------------------------------------------------------
// DRAWING
//--------------------------------------------------------------

/*
    ROUTINE draw:
        Part of the infinite update / draw loop.
 */
void ofApp::draw(){
  
    // draw the UI
    graphUI.draw();
    systemUI.draw();
    menuUI.draw();
    potentialUI.draw();
    optionsUI.draw();
    optionsOffUI.draw();
    
    if (optionsUI.getVisible()){optionsOffUI.makeInvisible();}
    else {optionsOffUI.makeVisible();}
    
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
            - l/L           = secret-Logan-mode
            - e/E           = toggle the energy graphs showing in the background
            - h/H           = toggle whether the UI is showing
            - r/R           = reset the system
            - p/P           = pause/restart the simulation
            - d/D           = open/close drawable pair potential
 */
void ofApp::keyPressed(int key){
    
    if (key == 'a' || key == 'A') { // Audio on/off
        micInput.toggleActive();
    }
        
    else if (key == 'l' || key == 'L') { // Secret-Logan-Mode on/off
        gui::SystemAtom* sys = (gui::SystemAtom*) systemUI.getChild(systemAtomIndex);
        sys->toggleTheHorrors();
    }
    
    else if (key == 'e' || key == 'E') { // Show/hide energy graphs
        graphUI.toggleVisible();
    }
    
    else if (key == 'h' || key == 'H'){ // Show/hide UI
        if (not potentialUI.getVisible()) {
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
    } else {
        systemUI.mouseMoved(x, y);
    }
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    if (menuUI.getVisible()) {
        menuUI.mouseMoved(x, y);
    } else if (potentialUI.getVisible()) {
        potentialUI.mouseMoved(x, y);
    } else {
        systemUI.mouseMoved(x, y);
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
    
    
    if (potentialUI.getVisible() && potentialUI.getRect().inside(x, y)) { // Mouse controls drawing UI
        potentialUI.mousePressed(x, y, button);
        
    } else if (menuUI.getVisible() && menuUI.getRect().inside(x, y)) { // Mouse controls menu
        // pass through event to children
        menuUI.mousePressed(x, y, button);
        
    } else if (optionsUI.getVisible() && optionsUI.getRect().inside(x, y)){
        optionsUI.mousePressed(x, y, button);
        
    } else if (optionsOffUI.getVisible() && optionsOffUI.getRect().inside(x, y)){
        optionsOffUI.mousePressed(x, y, button);
        
    } else {
        systemUI.mousePressed(x, y, button);
    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    if (menuUI.getVisible()) {
        menuUI.mouseReleased(x, y, button);
    } else if (potentialUI.getVisible()) {
        potentialUI.mouseReleased(x, y, button);
    } else {
        systemUI.mouseReleased(x, y, button);
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
