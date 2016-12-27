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

// Defines all the instantiated objects we need to store somewhere
// This should be included nowhere other than argon.cpp

#ifndef argon_internal_hpp
#define argon_internal_hpp

#include "platform.hpp"
#include "utilities.hpp"
#include "mdforces.hpp"
#include "gaussian.hpp"
#include "gui_base.hpp"
#include "gui_derived.hpp"
#include "cubicspline.hpp"
#include "potentials.hpp"
#include "info_text.h"

// Magic include to fix Microsoft C++ compatibility
#include <ciso646>

#define N_THREADS 1 // Number of threads to be used in the forces calculations

namespace argon {
    md::MDContainer theSystem; // The MD simulation system
    
    int splineContainerIndex; // Index of spline container in potentialUI
    int gaussianContainerIndex; // Index of gaussian container in systemUI
    int systemAtomIndex; // Index of the system atom in systemUI
    int infoTextIndex; // Index of the text atom in infoUI
    int optionsIndex; // Index of the atoms list atom in controlsUI
    
    bool loading; // are we still loading?
    
    // Store current screen dimensions so that resizing can occur in update if they change
    int screenWidth, screenHeight;
    
    /*
        UI CONTAINERS
     */
    
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
    
    /*
        ASSETS
     */
    
    // these all need to become ArgonFonts
    ofTrueTypeFont uiFont14;
    ofTrueTypeFont uiFont12;
    ofTrueTypeFont uiFont10;
    ofTrueTypeFont aboutFont12;
    
    // these all need to become ArgonImages
    ArgonImage splashScreen;
    ArgonImage boatLeft;
    ArgonImage boatRight;
    ArgonImage loganLeft;
    ArgonImage loganRight;
    ArgonImage argonLogo;
    ofImage stargonautsLogo;
    ofImage tmcsLogo;
    ofImage circGradient;
    
    ofImage playButton;
    ofImage pauseButton;
    ofImage resetButton;
    ofImage tutorialButton;
    ofImage audioOnButton;
    ofImage audioOffButton;
    ofImage menuButton;
    //ofImage addPointsButton;
    //ofImage removePointsButton;
    //ofImage movePointsButton;
    //ofImage changeSlopeButton;
    
    ofImage optionsButtonUp;
    ofImage optionsButtonDown;
    ofImage optionsEnergyButton;
    ofImage optionsMainMenuButton;
    ofImage optionsPotentialButton;
    ofImage optionsControlsButton;
    ofImage optionsAboutButton;
    
    ofImage closeButton;
    ofImage nextButton;
    ofImage previousButton;
    
    ofImage ljThumbnail;
    ofImage squareThumbnail;
    ofImage morseThumbnail;
    ofImage customThumbnail;
    ofImage resetSplinePointsButton;
}


#endif /* argon_internal_hpp */
