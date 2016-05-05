#pragma once

#include "ofMain.h"
#include "ljforces.hpp"
#include "gaussian.hpp"

#define N_THREADS 1

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
    
        void drawData(string name, double value);
        void drawGaussian(Gaussian& g, double boxw, double boxl, float scale);
    
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
        bool audioOn;

		ofSoundStream soundStream;
    
    private:
        lj::LJContainer theSystem;
        int thermCounter;
        ofTrueTypeFont drawFont;
        float drawDataHeight;
    
};
