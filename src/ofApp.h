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

#pragma once

#include <sstream>
#include <string>

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
    void audioIn(float * input, int bufferSize, int nChannels);

private:
   
    md::MDContainer theSystem; // The MD simulation system
    
    // Font and images needed for UI
    ofTrueTypeFont uiFont14;
    ofTrueTypeFont uiFont12;
    ofTrueTypeFont uiFont10;
    
    ofImage playButton;
    ofImage pauseButton;
    ofImage resetButton;
    ofImage audioOnButton;
    ofImage audioOffButton;
    ofImage menuButton;
    ofImage circGradient;
    ofImage ljThumbnail;
    ofImage squareThumbnail;
    ofImage morseThumbnail;
    ofImage addPointsButton;
    ofImage removePointsButton;
    ofImage movePointsButton;
    ofImage changeSlopeButton;
    
    // Audio data
    AudioStream micInput;
    
    // ~Trivial~ variables
    ofImage loganLeft, loganRight;
    
    // UI Containers
    gui::UIContainer menuUI;
    gui::UIContainer potentialUI;
    gui::UIContainer presshUI;
    gui::UIContainer testUI;
    gui::UIContainer systemUI;
    gui::UIContainer graphUI;
    
    int splineContainerIndex; // Index of spline container in potentialUI
    int gaussianContainerIndex; // Index of gaussian container in systemUI
    int systemAtomIndex; // Index of the system atom in systemUI

};
