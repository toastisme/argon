/*
 Argon
 
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
    
    
    // Change root directory
    ofSetDataPathRoot("../Resources/data/");
#if defined(WIN32)
    ofSetDataPathRoot("data");
#endif

    // Load assets
    loading = true;
    
    // graphics
    splashScreen.load("img/argonsplash.png");
    circGradient.load("img/circ_gradient.png");
    tutorialButton.load("img/ButtonTutorial.png");
    playButton.load("img/ButtonPlay.png");
    pauseButton.load("img/ButtonPause.png");
    resetButton.load("img/ButtonReset.png");
    audioOnButton.load("img/ButtonMic.png");
    audioOffButton.load("img/ButtonNoMic.png");
    optionsButtonUp.load("img/OptionsButtonUp.png");
    optionsButtonDown.load("img/OptionsButtonDown.png");
    optionsEnergyButton.load("img/OptionsEnergyButton.png");
    optionsMainMenuButton.load("img/OptionsMainMenuButton.png");
    optionsPotentialButton.load("img/OptionsPotentialButton.png");
    optionsControlsButton.load("img/OptionsControlsButton.png");
    optionsAboutButton.load("img/OptionsAboutButton2.png");
    closeButton.load("img/CloseButton.png");
    nextButton.load("img/NextButton.png");
    tmcsLogo.load("img/tmcslogo.png");
    stargonautsLogo.load("img/stargonautslogo.png");
    boatLeft.load("img/boatleft.png");
    boatRight.load("img/boatright.png");
    argonLogo.load("img/argonlogo.png");
    
    // potential graphics
    
    ljThumbnail.load("img/LJThumbnail.png");
    squareThumbnail.load("img/SquareThumbnail.png");
    morseThumbnail.load("img/MorseThumbnail.png");
    customThumbnail.load("img/CustomThumbnail.png");
    
    loganLeft.load("img/david-logan-posing-left.png");
    loganRight.load("img/david-logan-posing-right.png");
    
    // fonts
    uiFont14.load("fonts/Montserrat-Bold.ttf", 14);
    uiFont12.load("fonts/Montserrat-Bold.ttf", 12);
    uiFont10.load("fonts/Montserrat-Bold.ttf", 10);
    aboutFont12.load("fonts/Tahoma.ttf", 12);
    
    // Setup audio stream
    soundStream.setup(this, 0, 2, 44100, 256, 4);
    micInput.setStream(&soundStream);
    
    // Initialise theSystem with 50 particles at 60K
    theSystem.setTemp(0.5);
    theSystem.setTimestep(0.002);
    theSystem.setCutoff(3.0);
    theSystem.setFreq(0.1);
    theSystem.setStepsPerUpdate(5);
    theSystem.setNAfterReset(50);
    
    // Initialise screen dimensions to default 1024 x 600 pixels
    screenWidth = 1024;
    screenHeight = 600;
    
    double BOX_WIDTH = 17.0;
    double BOX_HEIGHT = BOX_WIDTH / screenWidth * screenHeight;
    theSystem.setBox(BOX_WIDTH, BOX_HEIGHT);
    
    theSystem.resetSystem();
    
    // colours
    ofColor bgcolor = ofColor(80, 80, 80, 80);
    ofColor textcolor = ofColor(255, 255, 240);

    // Setup system UI
    systemUI = gui::UIContainer(0, 0, screenWidth, screenHeight);
    
    // Add the gaussian container
    gaussianContainerIndex = systemUI.addIndexedChild(new gui::GaussianContainer(theSystem, circGradient, &uiFont10,
                                                                                 &optionsButtonDown, &audioOnButton, &audioOffButton, 30.0, 0, 0, screenWidth, screenHeight));
    
    // And the particles themselves
    systemAtomIndex = systemUI.addIndexedChild(new gui::SystemAtom(theSystem, loganLeft, loganRight, boatLeft, boatRight, 0, 0, screenWidth, screenHeight));
    
    // Setup graph UI
    graphUI = gui::UIContainer(624, 60, 400, 300);
    
    // Add the graph itself
    graphUI.addChild(new gui::EnergyGraphAtom(theSystem, 0, 0, 400, 200));
    
    // Add a legend
    graphUI.addChild(new gui::TextAtom("Kinetic Energy", uiFont10, ofColor(200, 0, 0),
                                       POS_TOP_LEFT, 5, 205, 100, 20));
    graphUI.addChild(new gui::TextAtom("Potential Energy", uiFont10, ofColor(255, 255, 255),
                                       POS_TOP_LEFT, 125, 205, 100, 20));
    
    graphUI.makeInvisible();
    
    /*
         Setup menu UI
     */
    
    // setup base container
    controlsUI = gui::UIContainer(250, 0, 774, 160);
    
    // menu background
    controlsUI.addChild(new gui::RectAtom(bgcolor, 0, 0, 774, 160));
    
    // sliders
   /*
    controlsUI.addChild(new gui::SliderContainer("Audio input level",
                                             [&] () { return micInput.getMaxAmplitude(); },
                                             [&] (double set) { micInput.setMaxAmplitude(set + 0.005); },
                                             0.005, 0.1, uiFont12, textcolor, 3,
                                             100, 110, 150, 450, 70, 5, 30)); */
    
    int optionsIndex = controlsUI.addIndexedChild(new gui::AtomsListAtom(uiFont12, textcolor, 524, 30, 200, 400, 130, 30));
    
    gui::AtomsListAtom* options = (gui::AtomsListAtom *) controlsUI.getChild(optionsIndex);
    options->addOption("Temperature", [&] () { }, new gui::CircularSliderContainer([&] () { return theSystem.getTemp() * 120; },
                                                                                   [&] (double set) { theSystem.setTemp(set / 120.0); },
                                                                                   0, 1000, uiFont14, textcolor, 1, 0, -15, 120, 60, 60, 120));
    
    options->addOption("Particles", [&] () { }, new gui::CircularSliderContainer([&] () { return theSystem.getNAfterReset(); },
                                                                                 [&] (double set) { theSystem.setNAfterReset(set + 0.5); theSystem.resetSystem(); },
                                                                                 2, 200, uiFont14, textcolor, 0, 0, -15, 120, 60, 60, 120));
    
    options->addOption("Simulation speed", [&] () { }, new gui::CircularSliderContainer([&] () { return theSystem.getStepsPerUpdate(); },
                                                                            [&] (double set) { theSystem.setStepsPerUpdate(set + 0.5); },
                                                                            1, 20, uiFont14, textcolor, 0,
                                                                            0, -15, 120, 60, 60, 120));
    
    options->addOption("Gaussian", [&] () { }, new gui::CircularSliderContainer([&] () { double rval = 0.0; gui::GaussianContainer* gaussian = (gui::GaussianContainer *) systemUI.getChild(gaussianContainerIndex);
        int gaussianID = gaussian->getSelectedID();
        if (gaussianID > -1) { rval = (50 - theSystem.getGaussianAmp(gaussianID))/100.0;  } return rval; },
                                                                                [&] (double set) { gui::GaussianContainer* gaussian = (gui::GaussianContainer *) systemUI.getChild(gaussianContainerIndex);
                                                                                    int gaussianID = gaussian->getSelectedID();
                                                                                    if (gaussianID > -1) { theSystem.updateGaussian(gaussianID, 50 - set*100, 0.8 - 0.5*set, theSystem.getGaussianX0(gaussianID), theSystem.getGaussianY0(gaussianID)); } },
                                                                                0.0, 1.0, uiFont10, ofColor(0, 0, 0, 0), 2, 0, -15, 120, 0, 0, 120));
    
    
    // framerate counter
    controlsUI.addChild(new gui::ValueAtom([&] () { return ofGetFrameRate(); },
                                       1, uiFont14, textcolor,
                                       POS_BOTTOM_RIGHT, 560, 160, 200, 30));
    controlsUI.makeInvisible();
    controlsUI.mouseReleased(0, 0, 0);
    
    
    
    
    potentialUI = gui::UIContainer(50, 50, 924, 500);
    potentialUI.addChild(new gui::RectAtom(bgcolor, 0, 0, 924, 500));
    potentialUI.addChild(new gui::RectAtom(bgcolor, 150, 0, 774, 500));
    
    // Setup potential atoms
    potentialUI.addChild(new gui::PotentialAtom(theSystem, 300, 0.95, 3.0, -2, 2,
                                                150, 0, 774, 500));
    splineContainerIndex = potentialUI.addIndexedChild(new gui::SplineContainer(theSystem, 0.95, 3.0, -2, 2, 15,
                                                                                150, 0, 774, 500));
    
    potentialUI.addChild(new gui::SetColour(textcolor));
    
    potentialUI.addChild(new gui::ButtonAtom([&] () { theSystem.setPotential(LENNARD_JONES);
                                                      potentialUI.getChild(splineContainerIndex)->makeInvisible(); },
                                             ljThumbnail, 25, 0, 100, 100));
    potentialUI.addChild(new gui::TextAtom("Lennard-Jones", uiFont12, textcolor, POS_TOP, 0, 100, 150, 25));
    
    potentialUI.addChild(new gui::ButtonAtom([&] () { theSystem.setPotential(SQUARE_WELL);
                                                      potentialUI.getChild(splineContainerIndex)->makeInvisible(); },
                                             squareThumbnail, 25, 125, 100, 100));
    potentialUI.addChild(new gui::TextAtom("Square Well", uiFont12, textcolor, POS_TOP, 0, 225, 150, 25));
    
    potentialUI.addChild(new gui::ButtonAtom([&] () { theSystem.setPotential(MORSE);
                                                      potentialUI.getChild(splineContainerIndex)->makeInvisible(); },
                                             morseThumbnail, 25, 250, 100, 100));
    potentialUI.addChild(new gui::TextAtom("Morse", uiFont12, textcolor, POS_TOP, 0, 350, 150, 25));
    
    potentialUI.addChild(new gui::ButtonAtom([&] () { theSystem.setPotential(CUSTOM);
                                                      potentialUI.getChild(splineContainerIndex)->makeVisible(); },
                                             customThumbnail, 25, 375, 100, 100));
    potentialUI.addChild(new gui::TextAtom("Custom", uiFont12, textcolor, POS_TOP, 0, 475, 150, 25));
    
    potentialUI.addChild(new gui::ButtonAtom([&] () { ((gui::SplineContainer *)potentialUI.getChild(splineContainerIndex))->destroyAllPoints(); },
                                             resetButton, 0, 0, 30, 30));
    
    potentialUI.makeInvisible();
    potentialUI.getChild(splineContainerIndex)->makeVisible();
    potentialUI.mouseReleased(0, 0, 0);
    
    // start menu as invisible
    controlsUI.makeInvisible();
    
    //Options menu
    
    optionsUI = gui::UIContainer(0, 0, 250, 390);
    optionsUI.addChild(new gui::RectAtom(bgcolor, 0, 0, 250, 410));
    
    // button text
    optionsUI.addChild(new gui::TextAtom("Controls", uiFont12, textcolor, POS_LEFT, 20, 55, 100, 25));
    optionsUI.addChild(new gui::TextAtom("Potentials", uiFont12, textcolor, POS_LEFT, 20, 95, 100, 25));
    optionsUI.addChild(new gui::TextAtom("Graphs", uiFont12, textcolor, POS_LEFT, 20, 135, 100, 25));
    optionsUI.addChild(new gui::TextAtom("About", uiFont12, textcolor, POS_LEFT, 20, 175, 100, 25));
    optionsUI.addChild(new gui::TextAtom("Play / pause", uiFont12, textcolor, POS_LEFT, 20, 375, 100, 25));
    optionsUI.addChild(new gui::TextAtom("Reset system", uiFont12, textcolor, POS_LEFT, 20, 255, 100, 25));
    optionsUI.addChild(new gui::TextAtom("Mic on/off", uiFont12, textcolor, POS_LEFT, 20, 295, 100, 25));
    optionsUI.addChild(new gui::TextAtom("Reset gaussians", uiFont12, textcolor, POS_LEFT, 20, 335, 100, 25));
    optionsUI.addChild(new gui::TextAtom("Tutorial", uiFont12, textcolor, POS_LEFT, 20, 215, 100, 25));
    
    // buttons
    optionsUI.addChild(new gui::SetColour(ofColor(255, 255, 255)));
    optionsUI.addChild(new gui::ButtonAtom([&] () { optionsUI.makeInvisible(); controlsUI.makeInvisible();potentialUI.makeInvisible();graphUI.makeInvisible(); aboutUI.makeInvisible();
        optionsOffUI.makeVisible();}, optionsMainMenuButton,
                                           10, 10, 30, 30));
    
    optionsUI.addChild(new gui::ButtonAtom([&] () {potentialUI.makeInvisible(); aboutUI.makeInvisible();
        controlsUI.toggleVisible(); }, optionsControlsButton, 200, 50, 30, 30));
    
    optionsUI.addChild(new gui::ButtonAtom([&] () { controlsUI.makeInvisible(); aboutUI.makeInvisible(); optionsUI.makeInvisible(); optionsOffUI.makeVisible();
        potentialUI.toggleVisible(); }, optionsPotentialButton, 200, 90, 30, 30));
    
    optionsUI.addChild(new gui::ButtonAtom([&] () { graphUI.toggleVisible(); }, optionsEnergyButton,
                                           200, 130, 30, 30));
    optionsUI.addChild(new gui::ButtonAtom([&] () {controlsUI.makeInvisible(); potentialUI.makeInvisible(); graphUI.makeInvisible();
        aboutUI.toggleVisible(); }, optionsAboutButton,
                                           200, 170, 30, 30));
    optionsUI.addChild(new gui::ButtonToggleAtom([&] () { return theSystem.getRunning(); }, [&] (bool set) { theSystem.setRunning(set); },
                                                  playButton, pauseButton,
                                                  200, 370, 30, 30));
    optionsUI.addChild(new gui::ButtonAtom([&] () { theSystem.resetSystem(); }, resetButton,
                                            200, 250, 30, 30));
    optionsUI.addChild(new gui::ButtonToggleAtom([&] () { return micInput.getActive(); }, [&] (bool set) { micInput.setActive(set); },
                                                  audioOnButton, audioOffButton,
                                                  200, 290, 30, 30));
    optionsUI.addChild(new gui::ButtonAtom([&] () { ((gui::GaussianContainer *)systemUI.getChild(gaussianContainerIndex))->destroyAllGaussians(); },
                                            resetButton, 200, 330, 30, 30));

    optionsUI.addChild(new gui::ButtonAtom([&] () {controlsUI.makeInvisible(); potentialUI.makeInvisible(); graphUI.makeInvisible(); aboutUI.makeInvisible();
        tutorialUI.toggleVisible(); tutorialHighlightUI.toggleVisible(); }, tutorialButton,
                                           200, 210, 30, 30));
    
    optionsUI.makeInvisible();
    optionsUI.mouseReleased(0, 0, 0);
    
    optionsOffUI = gui::UIContainer(0, 0, 40, 40);
    optionsOffUI.addChild(new gui::SetColour(ofColor(255, 255, 255)));
    optionsOffUI.addChild(new gui::ButtonAtom([&] () { optionsUI.makeVisible(); optionsOffUI.makeInvisible(); potentialUI.makeInvisible(); }, optionsMainMenuButton,
                                           10, 10, 30, 30));
    
    optionsOffUI.makeVisible();
    
    // About UI
    
    aboutUI = gui::UIContainer(257, 150, 610, 300);
    aboutUI.addChild(new gui::RectAtom(ofColor(80, 80, 80, 180), 0, 0, 610, 300));
    aboutUI.addChild(new gui::SetColour(textcolor));
    aboutUI.addChild(new gui::ImageAtom(argonLogo, 0, 5, 390, 170));
    aboutUI.addChild(new gui::ButtonAtom([&] () {
        gui::SystemAtom* sys = (gui::SystemAtom*) systemUI.getChild(systemAtomIndex);
        sys->sailTheHighSeas(); },
                                         stargonautsLogo, 395, 5, 205, 170));
    aboutUI.addChild(new gui::TextAtom("A molecular dynamics simulation with interactive external and", aboutFont12, textcolor, POS_LEFT, 25, 195, 600, 20));
    aboutUI.addChild(new gui::TextAtom("interatomic potentials.", aboutFont12, textcolor, POS_LEFT, 25, 215, 600, 20));
    aboutUI.addChild(new gui::TextAtom("Copyright 2016 David McDonagh, Robert Shaw, Staszek Welsh", aboutFont12, textcolor, POS_LEFT, 25, 255, 600, 20));
    aboutUI.addChild(new gui::TextAtom("Version 0.9.0", uiFont14, textcolor, POS_CENTRE, 202, 120, 195, 20));
    //aboutUI.addChild(new gui::ImageAtom(tmcsLogo, 129, 138, 164, 95));
    
    aboutUI.makeInvisible();
    aboutUI.mouseReleased(0, 0, 0);
    
    // Tutorial UI
    // Placeholder container for the TextBoxContainer
    tutorialUI = gui::UIContainer(0, 0, screenWidth, screenHeight);
    tutorialUI.addChild(new gui::TutorialContainer(0, 0, screenWidth, screenHeight, 250, 0, 774, 200, aboutFont12, nextButton, closeButton, tutorialUI));
    tutorialUI.makeInvisible();
    tutorialUI.mouseReleased(0, 0, 0);
    
    
    // TutorialHighlight UI
    
    tutorialHighlightUI = gui::UIContainer(200, 50, 30, 30);
    tutorialHighlightUI.addChild(new gui::RectAtom(ofColor(255, 255, 255, 80), 0, 0, 30, 30));
    tutorialHighlightUI.makeInvisible();
    tutorialUI.mouseReleased(0, 0, 0);
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
    
    // If the screen size has changed, resize the UI
    if ( screenWidth != ofGetWidth() || screenHeight != ofGetHeight() ) {
        
        float xScale =  ofGetWidth() / ( (float) screenWidth  );
        float yScale = ofGetHeight() / ( (float) screenHeight );
        
        controlsUI.resize(xScale, yScale);
        potentialUI.resize(xScale, yScale);
        optionsUI.resize(xScale, yScale);
        optionsOffUI.resize(xScale, yScale);
        graphUI.resize(xScale, yScale);
        systemUI.resize(xScale, yScale);
        aboutUI.resize(xScale, yScale);
        tutorialUI.resize(xScale, yScale);
        
        screenWidth = ofGetWidth();
        screenHeight = ofGetHeight();
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
    controlsUI.draw();
    potentialUI.draw();
    optionsUI.draw();
    optionsOffUI.draw();
    aboutUI.draw();
    tutorialUI.draw();
    tutorialHighlightUI.draw();
    if (loading) {
        ofColor splashColour = ofColor(255, 255, 255);
        splashColour.a = ofMap(ofGetElapsedTimef(), 3,5, 255, 0, true);
        if ( splashColour.a < 1 ) {
            loading = false;
        }
        ofSetColor(splashColour);
        splashScreen.draw(0, 0, ofGetWidth(), ofGetHeight());
    }
}

//--------------------------------------------------------------------
// INPUT & EVENT HANDLING
//--------------------------------------------------------------------

void ofApp::audioIn(ofSoundBuffer &buffer){
    micInput.audioIn(buffer);
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
            controlsUI.toggleVisible();
        }
    }
    
    else if (key == 'r' || key == 'R') { // Reset the system to have the current values of the sliders
        theSystem.resetSystem();
    }
    
    else if (key == 'p' || key == 'P') { // Play/pause the simulation
        theSystem.toggleRunning();
    }
    
    else if (key == 'd' || key == 'D') { // Drawing interface
        if (not controlsUI.getVisible()) {
            potentialUI.toggleVisible();
        }
    }
    
    else if (key == 'x' || key == 'X') { // Skip loading
        loading = false;
    }
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    tutorialUI.mouseMoved(x, y);
    controlsUI.mouseMoved(x, y);
    potentialUI.mouseMoved(x, y);
    systemUI.mouseMoved(x, y);
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    mouseMoved(x, y);
}

//--------------------------------------------------------------

/*
    EVENT mousePressed:
        When the mouse is clicked anywhere in the window, this uses the
        (x, y) position of the click to create a Gaussian at that position.
 
        Scope to change this to only be on left mouse button click, to free up
        other mouse uses?
 */
void ofApp::mousePressed(int x, int y, int button) {
    // pass through mouse press to UI elements
    // stop when the first function returns true and the event is handled
    // slight abuse of short-circuiting boolean or, but it avoids an ugly ifelse tree
    
    
    if (tutorialUI.getVisible()){
        if (tutorialHighlightUI.mousePressed(x, y, button)){
            potentialUI.mousePressed(x, y, button)  ||
            aboutUI.mousePressed(x, y, button)      ||
            controlsUI.mousePressed(x, y, button)       ||
            optionsUI.mousePressed(x, y, button)    ||
            optionsOffUI.mousePressed(x, y, button) ||
            systemUI.mousePressed(x, y, button);
        }
    }
    
    tutorialUI.mousePressed(x, y, button) ||
    potentialUI.mousePressed(x, y, button)  ||
    aboutUI.mousePressed(x, y, button)      ||
    controlsUI.mousePressed(x, y, button)       ||
    optionsUI.mousePressed(x, y, button)    ||
    optionsOffUI.mousePressed(x, y, button) ||
    systemUI.mousePressed(x, y, button);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    tutorialUI.mouseReleased(x, y, button);
    controlsUI.mouseReleased(x, y, button);
    potentialUI.mouseReleased(x, y, button);
    systemUI.mouseReleased(x, y, button);
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
