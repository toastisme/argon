#pragma once

#include "ofMain.h"
#include "ljforces.hpp"
#include "gaussian.hpp"
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
    void drawData(string name, double value, int x, int y, int length);
    void drawData(string name, double value, int x, int y);
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
    
    // Routines to setup the system
    // Convert box coordinates to screen coordinates
    double box2screen_x(double x, double x0 = 0.0);
    double box2screen_y(double y, double y0 = 0.0);
    ofPoint box2screen(double x, double y, double x0 = 0.0, double y0 = 0.0);
    ofPoint box2screen(lj::coord point, lj::coord origin = {0.0, 0.0});
    // Assign random initial velocities from an appropriate Maxwell distribution
    void randomiseVelocity(vector<double> &vel, double T);
    // Set the parameters for the system
    void setupSystem(int numParticles, double temperature, double box_length, double box_width, double cutoff, double timestep);

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
    
    // Parameters for theSystem - REDUNDANT
    int N_PARTICLES;
    double BOX_WIDTH;
    double BOX_LENGTH;
    double CUTOFF;
    double TIMESTEP;
    double TEMPERATURE;
    
    // Keep track of very first kinetic/potential energies
    // for graph drawing purposes - it may be prudent to
    // instead keep minEKin and minEPot values in LJContainer
    // along with maxEKin and maxEPot
    double firstEKin, firstEPot;

    // Counters
    // Keep track of the number of timesteps, so that
    // we do not thermostat every integration step -
    // REDUNDANT due to enCounter in LJContainer
    int thermCounter;
    // Keep track of which Gaussian potential is selected
    // a value of -1 implies there are no Gaussians
    int selectedGaussian;
    // Keep track of which UI control slider is selected
    int selectedSlider;
    
    // Font and images needed for UI
    ofTrueTypeFont drawFont;
    ofImage playbutton;
    ofImage pausebutton;
    ofImage circGradient;
    
    // Audio data
    ofSoundStream soundStream; // Audio input stream
    // Left and right audio channel amplitudes
    vector <float> left;
    vector <float> right;
    float smoothedVol; // Average volume, smoothed out
    float scaledVol; // Volume rescaled between 0 and 1
    float sensitivity; // Sensitivity of Gaussians to changes in volume
    
    // Logical variables
    bool audioOn; // Is audio input turned on?
    bool helpOn; // Is the UI showing?
    bool loganOn; // Is secret-Logan-mode turned on?
    bool graphOn; // Are the energy graphs showing?
    bool playOn; // Is the simulation playing?
    
    // ~Trivial~ variables
    ofImage loganLeft, loganRight;
    int loganShiftx, loganShifty;

};
