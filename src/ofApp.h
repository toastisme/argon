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
    void drawPotentialUI();
    
    // Draw potential functions
    void drawCustomPotential(float min_x, float min_y, float max_x, float max_y, std::vector<float> xpoints, std::vector<float> ypoints,std::vector<float> partx,std::vector<float> party);
    void drawPotential(float min_x, float min_y, float max_x, float max_y, std::vector<float> xpoints, std::vector<float> ypoints,std::vector<float> partx,std::vector<float> party, PotentialFunctor& pot);
    
    // Scale potential functions to UI
    void scalePotential(float min_x, float min_y, float max_x, float max_y, std::vector<float> xpoints, std::vector<float> ypoints,std::vector<float> partx,std::vector<float> party);
    
    
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
    
    // Variable potentials
    LennardJones ljPotential;
    SquareWell squarePotential;
    Morse morsePotential;
    CustomPotential customPotential;

    // Counters
    // Keep track of which Gaussian potential is selected
    // a value of -1 implies there are no Gaussians
    int selectedGaussian;
    // Keep track of which UI control slider is selected
    int selectedSlider;
    // Keep track of which pair potential is being used
    int selectedPotential;
    // Keep track of which customPotential button is being used
    int customPotentialButton;
    
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

};
