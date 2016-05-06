#pragma once

#include "ofMain.h"
#include "ljforces.hpp"
#include "gaussian.hpp"
//#include "ofxGui.h"

#define N_THREADS 1

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
    
        void drawData(string name, double value);
        void drawGaussian(Gaussian& g, double boxw, double boxl, bool selected);
        void drawGraph();
        void randomiseVelocity(vector<double> &vel, double T);
        void setupSystem(lj::LJContainer system, int numParticles, double temperature, double box_length, double box_width, double cutoff, double timestep);
    
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
    
        vector <float> left;
        vector <float> right;
    
        float smoothedVol;
        float scaledVol;

        bool audioOn, helpOn, loganOn, graphOn;

        ofSoundStream soundStream;
//        ofxFloatSlider temperature2;
        ofImage playbutton;

        ofImage loganLeft, loganRight;
        int loganShiftx, loganShifty;
    
    private:
        lj::LJContainer theSystem;
        int thermCounter;
        int selectedGaussian;
        ofTrueTypeFont drawFont;
        float drawDataHeight;
        double firstEKin, firstEPot;
    
};
