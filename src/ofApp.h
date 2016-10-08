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

#pragma once

#include <sstream>
#include <string>

// Magic include to fix Microsoft C++ compatibility
#include <ciso646>

#include "ofMain.h"
#include "utilities.hpp"
#include "mdforces.hpp"
#include "gaussian.hpp"
#include "gui_base.hpp"
#include "gui_derived.hpp"
#include "cubicspline.hpp"
#include "potentials.hpp"
#include "audio.hpp"


#define N_THREADS 1 // Number of threads to be used in the forces calculations

class ofApp : public ofBaseApp
{
public:
    
    void setup(); // Constructor for app
    
    // Infinite loop
    void update();
    void draw();

    // Events
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    void audioIn(ofSoundBuffer &buffer);

private:
   
    md::MDContainer theSystem; // The MD simulation system
    
    // Font and images needed for UI
    ofTrueTypeFont uiFont14;
    ofTrueTypeFont uiFont12;
    ofTrueTypeFont uiFont10;
    ofTrueTypeFont aboutFont12;
    
    ofImage playButton;
    ofImage pauseButton;
    ofImage resetButton;
    ofImage tutorialButton;
    ofImage audioOnButton;
    ofImage audioOffButton;
    ofImage menuButton;
    ofImage circGradient;
    ofImage ljThumbnail;
    ofImage squareThumbnail;
    ofImage morseThumbnail;
    ofImage customThumbnail;
    ofImage addPointsButton;
    ofImage removePointsButton;
    ofImage movePointsButton;
    ofImage changeSlopeButton;
    ofImage optionsButtonUp;
    ofImage optionsButtonDown;
    ofImage optionsEnergyButton;
    ofImage optionsMainMenuButton;
    ofImage optionsPotentialButton;
    ofImage optionsControlsButton;
    ofImage optionsAboutButton;
    ofImage splashScreen;
    ofImage stargonautsLogo;
    ofImage boatLeft;
    ofImage boatRight;
    ofImage tmcsLogo;
    ofImage argonLogo;
    ofImage closeButton;
    ofImage nextButton;
    ofImage previousButton;
    
    // Booleans
    bool loading;
    
    // Audio data
    AudioStream micInput;
    ofSoundStream soundStream;
    
    // ~Trivial~ variables
    ofImage loganLeft, loganRight;
    
    // UI Containers
    gui::UIContainer controlsUI;
    gui::UIContainer gaussianUI;
    gui::UIContainer potentialUI;
    gui::UIContainer systemUI;
    gui::UIContainer graphUI;
    gui::UIContainer optionsUI;
    gui::UIContainer optionsOffUI;
    gui::UIContainer aboutUI;
    gui::UIContainer tutorialHighlightUI;
    gui::UIContainer tutorialBlockUI; // This second container just adds a bit for flexibility in blocking mouse pressed events
    gui::UIContainer tutorialUI;
    gui::UIContainer infoUI;
    
    int splineContainerIndex; // Index of spline container in potentialUI
    int gaussianContainerIndex; // Index of gaussian container in systemUI
    int systemAtomIndex; // Index of the system atom in systemUI
    int infoTextIndex; // Index of the text atom in infoUI
    
    // Store current screen dimensions so that resizing can occur in update if they change
    int screenWidth, screenHeight;

};
