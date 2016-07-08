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

    // Custom drawing routines
    // Draw a particle
    void drawParticle(int index, double radius_x, double radius_y, ofColor color, int nframes = 0);
    void drawParticle(int index, double radius, ofColor color, int nframes = 0);
    void drawParticle(int index, double radius_x, double radius_y, int nframes = 0);
    void drawParticle(int index, double radius, int nframes = 0);
    // Draw the Gaussian external potentials
    void drawGaussian(Gaussian& g, bool selected);
    // Draw the kinetic/potential energy graphs
    void drawGraph();
    // Draw the user interface components
    void drawUI();
    
    // Convert box coordinates to screen coordinates
    double box2screen_x(double x, double x0 = 0.0);
    double box2screen_y(double y, double y0 = 0.0);
    ofPoint box2screen(double x, double y, double x0 = 0.0, double y0 = 0.0);
    ofPoint box2screen(coord point, coord origin = {0.0, 0.0});

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
    
    // For the potential UI
    float topHeight, sideWidth, buttonHeight;

    // Counters
    // Keep track of which Gaussian potential is selected
    // a value of -1 implies there are no Gaussians
    int selectedGaussian;
    
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
    
    // Logical variables
    bool loganOn; // Is secret-Logan-mode turned on?
    bool graphOn; // Are the energy graphs showing?
    bool drawOn; // Is the drawing UI open?
    bool customPotentialOn; // Has the custom potential been selected?
    
    // ~Trivial~ variables
    ofImage loganLeft, loganRight;
    int loganShiftx, loganShifty;
    
    // UI Containers
    gui::UIContainer menuUI;
    gui::UIContainer potentialUI;
    int potentialIndex, customPotentialIndex; // Indices of potential atoms in potentialUI
    gui::UIContainer presshUI;
    gui::UIContainer testUI;

};
