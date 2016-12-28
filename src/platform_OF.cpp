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

ArgonImage::ArgonImage()  { base = new ofImage(); }
ArgonImage::~ArgonImage() { delete (ofImage *)base; }

void ArgonImage::loadPNG(const string &filename) { ((ofImage *)base)->load(filename); }
double ArgonImage::getWidth()  const { return ((ofImage *)base)->getWidth();  }
double ArgonImage::getHeight() const { return ((ofImage *)base)->getHeight(); }
void ArgonImage::draw(double x, double y, double width, double height) const {
    ofSetColor(255, 255, 255);
    ((ofImage *)base)->draw(x, y, width, height);
}

/*
    ArgonFont
 */

ArgonFont::ArgonFont()  { base = new ofTrueTypeFont(); }
ArgonFont::~ArgonFont() { delete (ofTrueTypeFont *)base; }

void ArgonFont::loadTTF(const string &filename, int size) { ((ofTrueTypeFont *)base)->load(filename, size); }
double ArgonFont::getAscenderHeight()  const { return ((ofTrueTypeFont *)base)->getAscenderHeight();  }
double ArgonFont::getDescenderHeight() const { return ((ofTrueTypeFont *)base)->getDescenderHeight(); }
double ArgonFont::getTextWidth(const std::string &text) const { return ((ofTrueTypeFont *)base)->stringWidth(text); }
void ArgonFont::drawText(double x, double y, colour colour, const std::string &text) const {
    ofSetColor(ofConvertColour(colour));
    ((ofTrueTypeFont *)base)->drawString(text, x, y);
}


/*
    Drawing functions
 */

void drawLine(double x0, double y0, double x1, double y1, double width, colour colour) {
    ofSetColor(ofConvertColour(colour));
    ofSetLineWidth(width);
    ofDrawLine(x0, y0, x1, y1);
}

void drawRect(double x0, double y0, double x1, double y1, colour colour) {
    ofSetColor(ofConvertColour(colour));
    ofDrawRectangle(x0, y0, x1 - x0, y1 - y0);
}

int windowWidth()  { return ofGetWidth();  }
int windowHeight() { return ofGetHeight(); }
