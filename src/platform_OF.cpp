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

#include "platform.hpp"
#include "ofMain.h"

ofPoint ofConvertCoord(coord pos) {
    return ofVec3f(pos.x, pos.y);
}

ofRectangle ofConvertRect(rect r) {
    return ofRectangle(r.x, r.y, r.width(), r.height());
}

ofColor ofConvertColour(colour colour) {
    return ofColor(colour.r, colour.g, colour.b, colour.a);
}

/*
    ArgonImage
 */

ArgonImage::ArgonImage() { base = new ofImage(); }
ArgonImage::~ArgonImage() { delete (ofImage *)base; }

void ArgonImage::loadPNG(const string &filename) { ((ofImage *)base)->load(filename); }
double ArgonImage::getWidth()  const { return ((ofImage *)base)->getWidth();  }
double ArgonImage::getHeight() const { return ((ofImage *)base)->getHeight(); }
void ArgonImage::draw(double x, double y, double width, double height) const {
    ((ofImage *)base)->draw(x, y, width, height);
}

/*
    ArgonFont
 */

ArgonFont::ArgonFont() {base = new ofTrueTypeFont(); }
ArgonFont::~ArgonFont(){delete (ofTrueTypeFont *)base; }

void ArgonFont::loadText(const string &_filename, int _size) {
    ((ofTrueTypeFont *)base)->load(_filename, _size);
}

double ArgonFont::getDescenderHeight() const{return ((ofTrueTypeFont *)base)->getDescenderHeight();}

double ArgonFont::stringWidth(std::string &_string) const{return ((ofTrueTypeFont *)base)->stringWidth(_string);}

double ArgonFont::getLineHeight() const{return ((ofTrueTypeFont *)base)->getLineHeight();}


void ArgonFont::drawString(const std::string &_string, double x0, double y0, colour _colour) const {
    ofSetColor(ofConvertColour(_colour));
    ((ofTrueTypeFont *)base)->drawString(_string, x0, y0);}

/*
 polyline
 */

polyline::polyline(){base = new ofPolyline(); }
polyline::~polyline(){delete (ofPolyline *)base; }

void polyline::addVertex(float x, float y){((ofPolyline *)base)->addVertex(x, y);}
void polyline::lineTo(float x, float y){((ofPolyline *)base)->lineTo(x, y);}
void polyline::draw(){((ofPolyline *)base)->draw();}
void polyline::draw(colour _colour, float _lineWdith){ofSetColor(ofConvertColour(_colour)); ofSetLineWidth(_lineWdith); ((ofPolyline *)base)->draw();}

void polyline::arc(const coord &point, float rx, float ry, float angleBegin, float angleEnd, int circleResolution){
    ofPoint _ofPoint(point.x, point.y);
    ((ofPolyline *)base)->arc(_ofPoint, rx, ry, angleBegin, angleEnd, circleResolution);
}
void polyline::arc(const coord &point, float rx, float ry, float angleBegin, float angleEnd, bool blockwise, int circleResolution){
    ofPoint _ofPoint(point.x, point.y);
    ((ofPolyline *)base)->arc(_ofPoint, rx, ry, angleBegin, angleEnd, blockwise, circleResolution);
}
void polyline::arc(float x, float y, float rx, float ry, float angleBegin, float angleEnd, int circleResolution){
    ((ofPolyline *)base)->arc(x, y, rx, ry, angleBegin, angleEnd, circleResolution);
}


/*
    Drawing functions
 */

void drawLine(double x0, double y0, double x1, double y1, double width, colour _colour) {
    ofSetColor(ofConvertColour(_colour));
    ofSetLineWidth(width);
    ofDrawLine(x0, y0, x1, y1);
}

void drawRect(double x0, double y0, double width, double height, colour _colour) {
    ofSetColor(ofConvertColour(_colour));
    ofDrawRectangle(x0, y0, width, height);
}

void drawCircle(double x, double y, double r, colour _colour, int resolution){
    ofSetColor(ofConvertColour(_colour));
    ofSetCircleResolution(resolution);
    ofDrawCircle(x, y, r);
}

void drawEllipse(double x, double y, double rx, double ry, colour _colour, int resolution){
    ofSetColor(ofConvertColour(_colour));
    ofSetCircleResolution(resolution);
    ofDrawEllipse(x, y, rx, ry);
}

int windowWidth()  { return ofGetWidth();  }
int windowHeight() { return ofGetHeight(); }
