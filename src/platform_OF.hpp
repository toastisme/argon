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

inline ofPoint ofConvertCoord(coord pos) {
    return ofVec3f(pos.x, pos.y);
}

inline ofRectangle ofConvertRect(rect r) {
    return ofRectangle(r.x, r.y, r.width(), r.height());
}

inline ofColor ofConvertColour(colour3 colour) {
    return ofColor(colour.r, colour.g, colour.b);
}

inline ofColor ofConvertColour(colour4 colour) {
    return ofColor(colour.r, colour.g, colour.b, colour.a);
}

class ofWrappedImage : BaseImage {
private:
    ofImage image;
    
public:
    void loadPNG(const string &filename);
    
    double getWidth() const;
    double getHeight() const;
    
    void draw(double x, double y, double width, double height) const;
};

inline void ofWrappedImage::loadPNG(const string &filename) { image.load(filename); }
inline double ofWrappedImage::getWidth()  const { return image.getWidth();  }
inline double ofWrappedImage::getHeight() const { return image.getHeight(); }
inline void ofWrappedImage::draw(double x, double y, double width, double height) const {
    image.draw(x, y, width, height);
}


inline void drawLine(double x0, double y0, double x1, double y1, double width, colour4 colour) {
    ofSetColor(ofConvertColour(colour));
    ofSetLineWidth(width);
    ofDrawLine(x0, y0, x1, y1);
}

inline void drawRect(double x0, double y0, double x1, double y1, colour4 colour) {
    ofSetColor(ofConvertColour(colour));
    ofDrawRectangle(x0, y0, x1, y1);
}

inline void setFramerate(int rate) { ofSetFrameRate(rate); }
