#pragma once

#include "ofMain.h"
#include "ljforces.hpp"
#include "gaussian.hpp"
#include "gui_base.hpp"
#include "gui_derived.hpp"
#include "cubicspline.hpp"
#include <sstream>
#include <string>


#define N_THREADS 1 // Number of threads to be used in the forces calculations

class ofApp : public ofBaseApp
{
public:
    
    void setup(); // Constructor for app
    
    // Infinite loop
    void update();
    void draw();

    // Custom drawing routines
    // Draw strings from floating point data
    void drawData(const ofTrueTypeFont &font, string name, double value, int x, int y, int length);
    void drawData(const ofTrueTypeFont &font, string name, double value, int x, int y);
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
    
    // Routines to setup the system
    // Convert box coordinates to screen coordinates
    double box2screen_x(double x, double x0 = 0.0);
    double box2screen_y(double y, double y0 = 0.0);
    ofPoint box2screen(double x, double y, double x0 = 0.0, double y0 = 0.0);
    ofPoint box2screen(lj::coord point, lj::coord origin = {0.0, 0.0});
    
    // Assign random initial velocities from an appropriate Maxwell distribution
    //void randomiseVelocity(vector<double> &vel, double T);
    
    // Set the parameters for the system
    void setupSystem(int numParticles, double temperature, double box_length, double box_width, double timestep, double cutoff);

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
   
    lj::LJContainer theSystem; // The MD simulation system
    
    // Store the number of particles locally until the system
    // is reset with 'r' and we can regrid everything
    int numParticles;
    
    // Thermostat frequency
    double thermFreq;
    
    // Keep track of very first kinetic/potential energies
    // for graph drawing purposes - it may be prudent to
    // instead keep minEKin and minEPot values in LJContainer
    // along with maxEKin and maxEPot
    double firstEKin, firstEPot;
    
    // For the potential UI
    float topHeight, sideWidth, buttonHeight;

    // Counters
    // Keep track of which Gaussian potential is selected
    // a value of -1 implies there are no Gaussians
    int selectedGaussian;
    // Keep track of which UI control slider is selected
    int selectedSlider;
    // Keep track of which pair potential is being used
    int selectedPotential;
    
    // Font and images needed for UI
    ofTrueTypeFont uiFont14;
    ofTrueTypeFont uiFont12;
    ofTrueTypeFont uiFont10;
    ofImage playbutton;
    ofImage pausebutton;
    ofImage circGradient;
    
    // Audio data
    ofSoundStream soundStream; // Audio input stream
    // Left and right audio channel amplitudes
    vector <float> left;
    vector <float> right;
    
    // note: changed these to doubles
    double smoothedVol; // Average volume, smoothed out
    double scaledVol; // Volume rescaled between 0 and 1
    double sensitivity; // Sensitivity of Gaussians to changes in volume
    
    // Logical variables
    bool audioOn; // Is audio input turned on?
    bool helpOn; // Is the UI showing?
    bool loganOn; // Is secret-Logan-mode turned on?
    bool graphOn; // Are the energy graphs showing?
    bool playOn; // Is the simulation playing?
    bool drawOn; // Is the drawing UI open?
    
    // ~Trivial~ variables
    ofImage loganLeft, loganRight;
    int loganShiftx, loganShifty;
    
    // UI Test stuff (to be removed)
    gui::UIContainer testUI;

};
