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

#include "argon.hpp"
#include "argon_internal.hpp"

using namespace argon;

/*
ROUTINE Initialise:
    Constructor for the app. Sets up the system at the beginning, loads all the assets
    needed for the app (images and fonts), and initialises the audio input stream.
*/
void argon::Initialise() {
    // Load assets
    loading = true;
    
    // graphics
    //splashScreen.load("img/argonsplash.png");
    splashScreen.loadPNG("img/argonsplash.png");
    circGradient.loadPNG("img/circ_gradient.png");
    tutorialButton.loadPNG("img/ButtonTutorial.png");
    playButton.loadPNG("img/ButtonPlay.png");
    pauseButton.loadPNG("img/ButtonPause.png");
    resetButton.loadPNG("img/ButtonReset.png");
    audioOnButton.loadPNG("img/ButtonMic.png");
    audioOffButton.loadPNG("img/ButtonNoMic.png");
    optionsButtonUp.loadPNG("img/OptionsButtonUp.png");
    optionsButtonDown.loadPNG("img/OptionsButtonDown.png");
    optionsEnergyButton.loadPNG("img/OptionsEnergyButton.png");
    optionsMainMenuButton.loadPNG("img/OptionsMainMenuButton.png");
    optionsPotentialButton.loadPNG("img/OptionsPotentialButton.png");
    optionsControlsButton.loadPNG("img/OptionsControlsButton.png");
    optionsAboutButton.loadPNG("img/OptionsAboutButton2.png");
    closeButton.loadPNG("img/CloseButton.png");
    nextButton.loadPNG("img/NextButton.png");
    previousButton.loadPNG("img/PreviousButton.png");
    tmcsLogo.loadPNG("img/tmcslogo.png");
    stargonautsLogo.loadPNG("img/stargonautslogo.png");
    boatLeft.loadPNG("img/boatleft.png");
    boatRight.loadPNG("img/boatright.png");
    argonLogo.loadPNG("img/argonlogo.png");
    resetSplinePointsButton.loadPNG("img/ResetSplinePointsButton.png");
    
    // potential graphics
    
    ljThumbnail.loadPNG("img/LJThumbnail.png");
    squareThumbnail.loadPNG("img/SquareThumbnail.png");
    morseThumbnail.loadPNG("img/MorseThumbnail.png");
    customThumbnail.loadPNG("img/CustomThumbnail.png");
    
    loganLeft.loadPNG("img/david-logan-posing-left.png");
    loganRight.loadPNG("img/david-logan-posing-right.png");
    
    // fonts
    uiFont14.loadTTF("fonts/Montserrat-Bold.ttf", 14);
    uiFont12.loadTTF("fonts/Montserrat-Bold.ttf", 12);
    uiFont10.loadTTF("fonts/Montserrat-Bold.ttf", 10);
    aboutFont12.loadTTF("fonts/Tahoma.ttf", 12);
    
    
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
    RGB bgcolour = RGB(80, 80, 80, 80);
    RGB textcolour = RGB(255, 255, 240);
    RGB optionsColour = RGB(255, 255, 255);

    // Setup system UI
    systemUI = gui::UIContainer(0, 0, screenWidth, screenHeight);
    
    // Add the gaussian container
    gaussianContainerIndex = systemUI.addIndexedChild(new gui::GaussianContainer(theSystem, circGradient, &uiFont10,
                                                                                 &optionsButtonDown, &audioOnButton, &audioOffButton, 30.0, 0, 0, screenWidth, screenHeight));
    
    // And the particles themselves
    systemAtomIndex = systemUI.addIndexedChild(new gui::SystemAtom(theSystem, loganLeft, loganRight, boatLeft, boatRight, 0, 0, screenWidth, screenHeight));
    
    // Info UI
    
    infoUI = gui::UIContainer(256, 310, 750, 380);
    infoUI.addChild(new gui::RectAtom(RGB(80, 80, 80, 180), 0, 0, 750, 380));
    infoUI.addChild(new gui::ButtonAtom([&] () { infoUI.makeInvisible(); }, closeButton, textcolour, 710, 10, 30, 30));
    infoTextIndex = infoUI.addIndexedChild(new gui::TextAtom("Some text goes here", aboutFont12, textcolour, POS_TOP_LEFT, 25, 25, 720, 320));
    infoUI.makeInvisible();
    infoUI.mouseReleased(0, 0, 0);
    
    /*
         Setup menu UI
     */
    
    // setup base container
    controlsUI = gui::UIContainer(250, 0, 774, 209);
    
    // menu background
    controlsUI.addChild(new gui::RectAtom(bgcolour, 0, 0, 774, 209));
    
    // sliders
    optionsIndex = controlsUI.addIndexedChild(new gui::AtomsListAtom(uiFont12, textcolour, 554, 20, 200, 514, 169, 20));
    
    gui::AtomsListAtom* options = (gui::AtomsListAtom *) controlsUI.getChild(optionsIndex);
    options->addOption("Temperature", [&] () {
        gui::TextAtom* t = (gui::TextAtom*) infoUI.getChild(infoTextIndex);
        t->setText(CONTROLS_INFO_TEMP);
    }, new gui::CircularSliderContainer([&] () { return theSystem.getTemp() * 120; },
                                                                                   [&] (double set) { theSystem.setTemp(set / 120.0); },
                                                                                   0, 1000, uiFont14, textcolour, 1, -75, 0, 150, 60, 60, 120));
    
    options->addOption("Particles", [&] () {
        gui::TextAtom* t = (gui::TextAtom*) infoUI.getChild(infoTextIndex);
        t->setText(CONTROLS_INFO_PART);
    }, new gui::CircularSliderContainer([&] () { return theSystem.getNAfterReset(); },
                                                                                 [&] (double set) { theSystem.setNAfterReset(set + 0.5); theSystem.resetSystem(); },
                                                                                 2, 200, uiFont14, textcolour, 0, -75, 0, 150, 60, 60, 120));
    
    options->addOption("Simulation speed", [&] () {
        gui::TextAtom* t = (gui::TextAtom*) infoUI.getChild(infoTextIndex);
        t->setText(CONTROLS_INFO_SIM);
    }, new gui::CircularSliderContainer([&] () { return theSystem.getStepsPerUpdate(); },
                                                                            [&] (double set) { theSystem.setStepsPerUpdate(set + 0.5); },
                                                                            1, 20, uiFont14, textcolour, 0,
                                                                            -75, 0, 150, 60, 60, 120));
    
    options->addOption("Gaussian", [&] () {
        gui::TextAtom* t = (gui::TextAtom*) infoUI.getChild(infoTextIndex);
        t->setText(CONTROLS_INFO_GAUSS);
    }, new gui::CircularSliderContainer([&] () { double rval = 0.0; gui::GaussianContainer* gaussian = (gui::GaussianContainer *) systemUI.getChild(gaussianContainerIndex);
        int gaussianID = gaussian->getSelectedID();
        if (gaussianID > -1) { rval = (50 - theSystem.getGaussianAmp(gaussianID))/100.0;  } return rval; },
                                                                                [&] (double set) { gui::GaussianContainer* gaussian = (gui::GaussianContainer *) systemUI.getChild(gaussianContainerIndex);
                                                                                    int gaussianID = gaussian->getSelectedID();
                                                                                    if (gaussianID > -1) { theSystem.updateGaussian(gaussianID, 50 - set*100, 0.8 - 0.5*set, theSystem.getGaussianX0(gaussianID), theSystem.getGaussianY0(gaussianID)); } },
                                                                                0.0, 1.0, uiFont10, textcolour, 0, -75, 0, 150, 60, 60, 120));
    
    gui::UIContainer* energyGraphContainer = new gui::UIContainer(-45, 0, 514, 169);
    energyGraphContainer->addChild(new gui::EnergyGraphAtom(theSystem, 0, 0, 514, 161));
    RGB KEColour = RGB(200, 0, 0);
    RGB PEColour = RGB(255, 255, 255);
    energyGraphContainer->addChild(new gui::TextAtom("Kinetic Energy", uiFont10, KEColour,
                                                     POS_TOP_LEFT,   5, 166, 100, 20));
    energyGraphContainer->addChild(new gui::TextAtom("Potential Energy", uiFont10, PEColour,
                                                     POS_TOP_LEFT, 125, 166, 100, 20));

    options->addOption("Energy graphs", [&] () {
        gui::TextAtom* t = (gui::TextAtom*) infoUI.getChild(infoTextIndex);
        t->setText(CONTROLS_INFO_GRAPHS);
    }, (gui::UIBase*)energyGraphContainer);
    options->addOption("Maxwell-Boltzmann", [&] () {
        gui::TextAtom* t = (gui::TextAtom*) infoUI.getChild(infoTextIndex);
        t->setText(CONTROLS_INFO_MAXWELL);
    }, new gui::MaxwellGraphAtom(theSystem, -45, 0, 514, 169));
    
    controlsUI.addChild(new gui::ButtonAtom([&] () {
        gui::TextAtom* t = (gui::TextAtom*) infoUI.getChild(infoTextIndex);
        SetInfoText();
        if (!tutorialUI.getVisible()) infoUI.toggleVisible(); }, optionsAboutButton, optionsColour, 10, 10, 30, 30));
    
    controlsUI.makeInvisible();
    controlsUI.mouseReleased(0, 0, 0);
    
    
    potentialUI = gui::UIContainer(50, 50, 924, 500);
    potentialUI.addChild(new gui::PotentialContainer(theSystem, uiFont12, ljThumbnail, squareThumbnail, morseThumbnail, customThumbnail, resetSplinePointsButton));
    
    potentialUI.mouseReleased(0, 0, 0);
    
    // start menu as invisible
    potentialUI.makeInvisible();
    
    //Options menu
    
    optionsUI = gui::UIContainer(0, 0, 250, 370);
    optionsUI.addChild(new gui::RectAtom(bgcolour, 0, 0, 250, 370));
    
    // buttons
    
    optionsUI.addChild(
        new gui::ButtonAtom([&] () {
            optionsUI.makeInvisible();
            controlsUI.makeInvisible();
            potentialUI.makeInvisible();
            graphUI.makeInvisible();
            aboutUI.makeInvisible();
            infoUI.makeInvisible();
            optionsOffUI.makeVisible();
        }, optionsMainMenuButton, optionsColour, 10, 10, 30, 30));
    
    optionsUI.addChild(new gui::TextAtom("Controls", uiFont12, textcolour, POS_LEFT, 20, 55, 100, 25));
    optionsUI.addChild(
        new gui::ButtonAtom([&] () {
            potentialUI.makeInvisible();
            aboutUI.makeInvisible();
            infoUI.makeInvisible();
            controlsUI.toggleVisible();
        }, optionsControlsButton, optionsColour, 200, 50, 30, 30));
    
    optionsUI.addChild(new gui::TextAtom("Potentials", uiFont12, textcolour, POS_LEFT, 20, 95, 100, 25));
    optionsUI.addChild(
        new gui::ButtonAtom([&] () {
            controlsUI.makeInvisible();
            aboutUI.makeInvisible();
            infoUI.makeInvisible();
            optionsUI.makeInvisible();
            optionsOffUI.makeVisible();
            graphUI.makeInvisible();
            potentialUI.toggleVisible();
        }, optionsPotentialButton, optionsColour, 200, 90, 30, 30));
    
    optionsUI.addChild(new gui::TextAtom("About", uiFont12, textcolour, POS_LEFT, 20, 135, 100, 25));
    optionsUI.addChild(
        new gui::ButtonAtom([&] () {
                controlsUI.makeInvisible();
                potentialUI.makeInvisible();
                graphUI.makeInvisible();
                aboutUI.toggleVisible();
                infoUI.makeInvisible();
            }, optionsAboutButton, optionsColour, 200, 130, 30, 30));

    optionsUI.addChild(new gui::TextAtom("Tutorial", uiFont12, textcolour, POS_LEFT, 20, 175, 100, 25));
    optionsUI.addChild(
        new gui::ButtonAtom([&] () {
            controlsUI.makeInvisible();
            potentialUI.makeInvisible();
            graphUI.makeInvisible();
            aboutUI.makeInvisible();
            infoUI.makeInvisible();
            tutorialUI.toggleVisible();
            tutorialHighlightUI.toggleVisible();
        }, tutorialButton, optionsColour, 200, 170, 30, 30));
    
    optionsUI.addChild(new gui::TextAtom("Reset gaussians", uiFont12, textcolour, POS_LEFT, 20, 215, 100, 25));
    optionsUI.addChild(
        new gui::ButtonAtom(
            [&] () {
                ((gui::GaussianContainer *)systemUI.getChild(gaussianContainerIndex))->destroyAllGaussians();
            }, resetButton, optionsColour, 200, 210, 30, 30));
    
    optionsUI.addChild(new gui::TextAtom("Mic on/off", uiFont12, textcolour, POS_LEFT, 20, 255, 100, 25));
    optionsUI.addChild(
        new gui::ButtonToggleAtom(
            [&] () { return getMicActive(); },
            [&] (bool set) { setMicActive(set); },
            audioOnButton, audioOffButton, optionsColour, 200, 250, 30, 30));
    
    optionsUI.addChild(new gui::TextAtom("Reset system", uiFont12, textcolour, POS_LEFT, 20, 295, 100, 25));
    optionsUI.addChild(
        new gui::ButtonAtom(
            [&] () { theSystem.resetSystem(); },
            resetButton, optionsColour, 200, 290, 30, 30));
    
    optionsUI.addChild(new gui::TextAtom("Play / pause", uiFont12, textcolour, POS_LEFT, 20, 335, 100, 25));
    optionsUI.addChild(
        new gui::ButtonToggleAtom(
            [&] () { return theSystem.getRunning(); },
            [&] (bool set) { theSystem.setRunning(set); },
            playButton, pauseButton, optionsColour, 200, 330, 30, 30));
    
    optionsUI.makeInvisible();
    optionsUI.mouseReleased(0, 0, 0);
    
    optionsOffUI = gui::UIContainer(0, 0, 40, 40);
    optionsOffUI.addChild(new gui::ButtonAtom([&] () { optionsUI.makeVisible(); optionsOffUI.makeInvisible(); potentialUI.makeInvisible(); }, optionsMainMenuButton, optionsColour,
                                           10, 10, 30, 30));
    optionsOffUI.addChild(new gui::ButtonAtom([&] () {
        SetInfoText();
        if (!tutorialUI.getVisible()) infoUI.toggleVisible();
    }, optionsAboutButton, optionsColour, 50, 10, 30, 30));
    
    optionsOffUI.makeVisible();
    
    // About UI
    
    aboutUI = gui::UIContainer(257, 150, 610, 300);
    aboutUI.addChild(new gui::RectAtom(RGB(80, 80, 80, 180), 0, 0, 610, 300));
    //aboutUI.addChild(new gui::SetColour(textcolor));
    aboutUI.addChild(new gui::ImageAtom(argonLogo, 0, 5, 390, 170, textcolour));
    aboutUI.addChild(new gui::ButtonAtom([&] () {
        gui::SystemAtom* sys = (gui::SystemAtom*) systemUI.getChild(systemAtomIndex);
        sys->sailTheHighSeas(); },
                                         stargonautsLogo, optionsColour, 395, 5, 205, 170));
    aboutUI.addChild(new gui::TextAtom("A molecular dynamics simulation with interactive external and", aboutFont12, textcolour, POS_LEFT, 25, 195, 600, 20));
    aboutUI.addChild(new gui::TextAtom("interatomic potentials. www.argonmd.co.uk", aboutFont12, textcolour, POS_LEFT, 25, 215, 600, 20));
    aboutUI.addChild(new gui::TextAtom("Copyright 2016 David McDonagh, Robert Shaw, Staszek Welsh", aboutFont12, textcolour, POS_LEFT, 25, 255, 600, 20));
    aboutUI.addChild(new gui::TextAtom("Version 1.0.1", uiFont14, textcolour, POS_CENTRE, 202, 120, 195, 20));
    //aboutUI.addChild(new gui::ImageAtom(tmcsLogo, 129, 138, 164, 95));
    
    aboutUI.makeInvisible();
    aboutUI.mouseReleased(0, 0, 0);
    
    // Tutorial UI
    tutorialUI = gui::UIContainer(0, 0, screenWidth, screenHeight);
    tutorialUI.addIndexedChild(new gui::TutorialContainer(0, 0, screenWidth, screenHeight, uiFont12, nextButton, previousButton, closeButton, tutorialUI, tutorialHighlightUI, tutorialBlockUI, theSystem, graphUI, controlsUI, potentialUI, systemUI, gaussianContainerIndex));
    tutorialUI.makeInvisible();
    tutorialUI.mouseReleased(0, 0, 0);
    
    
    
    // TutorialHighlight UI
    tutorialHighlightUI = gui::UIContainer(-50, 50, 30, 30);
    //tutorialHighlightUI.addChild(new gui::RectAtom(RGB(255,255,255, 80), 0, 0, 30, 30));
    tutorialHighlightUI.addChild(new gui::TutorialHighlightAtom(0, 0, 30, 30, dynamic_cast<gui::TutorialContainer*>(tutorialUI.getChild(0)), tutorialHighlightUI));
    tutorialHighlightUI.makeInvisible();
    tutorialHighlightUI.mouseReleased(0, 0, 0);
    
    tutorialBlockUI = gui::UIContainer(-50, 50, 30, 30);
    //tutorialBlockUI.addChild(new gui::RectAtom(RGB(255,0,255, 80), 0, 0, 30, 30));
    tutorialBlockUI.mouseReleased(0, 0, 0);
    
}

/*
ROUTINE Run:
    Part of the infinite update / draw loop.
    Update the status of the application, and then draw a frame.

    Currently performs the following tasks, when the simulation is not paused (i.e when playOn):
        
        1. Integrates the equations of motion 5 times and thermostats (Berendsen) with a frequency of 0.1
        2. If the audio input is turned on:
            - Calculates the smoothed volume scaled between 0 and 1
            - Updates the amplitude, exponent, and drawing of the selected Gaussian according to 
                this volume.

    Then draw all the individual UI Containers.
*/

void argon::Run() {
    // If not paused, integrate the system
    theSystem.run();
        
    if (getMicActive()) {
        // get volume, scaled to between 0 and 1
        double scaledVol = getMicVolume();
        
        // Update the currently selected Gaussian, so that quiet-> loud results in
        // a change from an attractive, wide Gaussian, to a repulsive, narrow Gaussian.
        systemUI.getChild(gaussianContainerIndex)->audioIn(scaledVol);
    }
    
    // If the screen size has changed, resize the UI
    if ( screenWidth != windowWidth() || screenHeight != windowHeight() ) {
        
        float xScale =  windowWidth() / ( (float) screenWidth  );
        float yScale = windowHeight() / ( (float) screenHeight );
        
        controlsUI.resize(xScale, yScale);
        potentialUI.resize(xScale, yScale);
        optionsUI.resize(xScale, yScale);
        optionsOffUI.resize(xScale, yScale);
        graphUI.resize(xScale, yScale);
        systemUI.resize(xScale, yScale);
        aboutUI.resize(xScale, yScale);
        infoUI.resize(xScale, yScale);
        tutorialUI.resize(xScale, yScale);
        tutorialHighlightUI.resize(xScale, yScale);
        tutorialBlockUI.resize(xScale, yScale);
        
        screenWidth = windowWidth();
        screenHeight = windowHeight();
    }
    
    // draw the UI
    graphUI.draw();
    systemUI.draw();
    controlsUI.draw();
    potentialUI.draw();
    optionsUI.draw();
    optionsOffUI.draw();
    aboutUI.draw();
    infoUI.draw();
    tutorialUI.draw();
    tutorialHighlightUI.draw();
    tutorialBlockUI.draw();
    
    if (loading) {
        RGB splashColour = RGB(255, 255, 255);
        splashColour.a = util::map(timeElapsed(), 3, 5, 255, 0, true);
        if ( splashColour.a < 1 ) {
            loading = false;
        }
        splashScreen.draw(0, 0, windowWidth(), windowHeight(), splashColour);
    }
    
}

/*
    Current key mappings:
        a/A = toggle audio on or off
        l/L = secret-Logan-mode
        e/E = toggle the energy graphs showing in the background
        h/H = toggle whether the UI is showing
        r/R = reset the system
        p/P = pause/restart the simulation
        d/D = open/close drawable pair potential
        x/X = skip loading fade-in animation
 */
void argon::KeyPress(unsigned char key) {
    if (key == 'a' || key == 'A') { // Audio on/off
        toggleMicActive();
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

void argon::KeyRelease(unsigned char key) {
}

void argon::MouseMove(int x, int y) {
    tutorialUI.mouseMoved(x, y);
    controlsUI.mouseMoved(x, y);
    potentialUI.mouseMoved(x, y);
    systemUI.mouseMoved(x, y);
}

void argon::MousePress(int x, int y, int button) {
    // pass through mouse press to UI elements
    // stop when the first function returns true and the event is handled
    // slight abuse of short-circuiting boolean or, but it avoids an ugly ifelse tree
    
    // when the tutorial is running, only allow mouse events in the highlighted area
    if (button == 3) { printf("%d %d\n", x, y); }
    
    if (tutorialUI.getVisible()){
        if (!tutorialBlockUI.mousePressed(x,y, button) && tutorialHighlightUI.mousePressed(x, y, button)){
            tutorialBlockUI.mousePressed(x, y, button) ||
            potentialUI.mousePressed(x, y, button)  ||
            controlsUI.mousePressed(x, y, button)       ||
            optionsUI.mousePressed(x, y, button)    ||
            optionsOffUI.mousePressed(x, y, button) ||
            systemUI.mousePressed(x, y, button);
        }
    }
    
    /* USEFUL FOR DEBUGGING WHEN THINGS ARE EATING MOUSE EVENTS
    if (tutorialUI.mousePressed(x, y, button))
        std::cout << "tutorialUI";
    else if (potentialUI.mousePressed(x, y, button))
        std::cout << "potentialUI";
    else if (aboutUI.mousePressed(x, y, button))
        std::cout << "aboutUI";
    else if (infoUI.mousePressed(x, y, button))
        std::cout << "infoUI";
    else if (controlsUI.mousePressed(x, y, button))
        std::cout << "controlsUI";
    else if (optionsUI.mousePressed(x, y, button))
        std::cout << "optionsUI";
    else if (optionsOffUI.mousePressed(x, y, button))
        std::cout << "optionsOffUI";
    else if (systemUI.mousePressed(x, y, button))
        std::cout << "systemUI"; */
    
    tutorialUI.mousePressed(x, y, button) ||
    infoUI.mousePressed(x, y, button) ||
    potentialUI.mousePressed(x, y, button)  ||
    aboutUI.mousePressed(x, y, button)      ||
    controlsUI.mousePressed(x, y, button)       ||
    optionsUI.mousePressed(x, y, button)    ||
    optionsOffUI.mousePressed(x, y, button) ||
    systemUI.mousePressed(x, y, button);
    
    if (potentialUI.getVisible()) SetInfoText();
}


void argon::MouseRelease(int x, int y, int button) {
    tutorialUI.mouseReleased(x, y, button);
    controlsUI.mouseReleased(x, y, button);
    potentialUI.mouseReleased(x, y, button);
    systemUI.mouseReleased(x, y, button);
}

void argon::AudioIn(double input) {
    setMicVolume(input);
}

// Worker function to set info text
void argon::SetInfoText() {
    gui::TextAtom* t = (gui::TextAtom*) infoUI.getChild(infoTextIndex);
    std::string control_string = "";
    if (potentialUI.getVisible()) {
        switch( theSystem.getPotential().getType() ) {
            case LENNARD_JONES: {
                control_string = POTENTIALS_INFO_LENNARD;
                break;
            }
            
            case MORSE: {
                control_string = POTENTIALS_INFO_MORSE;
                break;
            }
            
            case SQUARE_WELL: {
                control_string = POTENTIALS_INFO_SQUARE;
                break;
            }
            
            case CUSTOM: {
                control_string = POTENTIALS_INFO_CUSTOM;
                break;
            }
                
            default:
                control_string = "";
        }
        control_string += POTENTIALS_INFO_GENERAL;
        
    } else if (controlsUI.getVisible()) {
        
        gui::AtomsListAtom* o = (gui::AtomsListAtom *) controlsUI.getChild(optionsIndex);
        switch(o->getSelectedOption()) {
            case 0: { // Temperature
                control_string = CONTROLS_INFO_TEMP;
                break;
            }
                
            case 1: { // No. of particles
                control_string = CONTROLS_INFO_PART;
                break;
            }
                
            case 2: { // Simulation speed
                control_string = CONTROLS_INFO_SIM;
                break;
            }
                
            case 3: { // Gaussian
                control_string = CONTROLS_INFO_GAUSS;
                break;
            }
                
            case 4: { // Energy graphs
                control_string = CONTROLS_INFO_GRAPHS;
                break;
            }
                
            case 5: { // Maxwell-Boltzmann
                control_string = CONTROLS_INFO_MAXWELL;
                break;
            }
                
            default:
                control_string = "There should be some info about the controls here...";
        }

    }
    else control_string = SYSTEM_INFO_TEXT;
    
    t->setText(control_string);
}
