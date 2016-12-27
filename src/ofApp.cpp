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

#include "ofApp.h"
#include "argon.hpp"

//--------------------------------------------------------------
// SETUP
//--------------------------------------------------------------

void ofApp::setup(){
    // set target framerate to 60
    ofSetFrameRate(60);
    
    // set background colour to black
    ofBackground(0, 0, 0);
    
    // Set root directory for assets
#ifdef WIN32
    ofSetDataPathRoot("data");
#else
    ofSetDataPathRoot("../Resources/data/");
#endif
    
    // call generic argon initialisation
    argon::Initialise();
    
#ifndef WIN32
    // setup Audio stream
    ofSoundStreamSetup(0, 2, this, 44100, 256, 4);
#endif
}

//--------------------------------------------------------------
// UPDATE
//--------------------------------------------------------------

void ofApp::update(){

}

//--------------------------------------------------------------
// DRAWING
//--------------------------------------------------------------

void ofApp::draw(){
    argon::Run();
}

//--------------------------------------------------------------------
// INPUT & EVENT HANDLING
//--------------------------------------------------------------------

void ofApp::audioIn(ofSoundBuffer &buffer){
    argon::AudioIn(buffer.getRMSAmplitude());
}


//--------------------------------------------------------------

void ofApp::keyPressed(int key){
    argon::KeyPress(key);
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    argon::KeyRelease(key);
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    argon::MouseMove(x, y);
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    argon::MouseMove(x, y);
}

//--------------------------------------------------------------

void ofApp::mousePressed(int x, int y, int button) {
    argon::MousePress(x, y, button);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    argon::MouseRelease(x, y, button);
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}