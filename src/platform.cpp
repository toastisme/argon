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
#include <math.h>

/*
    coord
 */

coord::coord() : x(0), y(0) {}
coord::coord(double _x, double _y) : x(_x), y(_y) {}

void coord::setXY(double _x, double _y) { x = _x; y = _y; }

vector3::vector3() : x(0), y(0), z(0){}
vector3::vector3(float _x, float _y, float _z){
    x = _x; y = _y; z = _z;}


/*
    colour
 */

colour colour::fromName(COLOURNAME colourname) {
    switch (colourname) {
        case COL_WHITE:         return colour(255, 255, 255, 255);
        case COL_BLACK:         return colour(  0,   0,   0, 255);
        case COL_RED:           return colour(255,   0,   0, 255);
        case COL_GREEN:         return colour(  0, 255,   0, 255);
        case COL_BLUE:          return colour(  0,   0, 255, 255);
        case COL_BG:            return colour( 80,  80,  80,  80);
        case COL_TEXT:          return colour(255, 255, 240, 255);
        case COL_BLUEHL:        return colour( 82, 225, 247, 255);
        case COL_BLUEHL_DARK:   return colour( 10, 174, 199, 255);
        case COL_VIOLINGRAPH:   return colour(186, 255, 163,  80);
    }
}

colour::colour() : r(0), g(0), b(0), a(255) {}

colour::colour(unsigned char _r, unsigned char _g, unsigned char _b) : r(_r), g(_g), b(_b), a(255) {}
colour::colour(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a) : r(_r), g(_g), b(_b), a(_a) {}

void colour::setRGB(unsigned char _r, unsigned char _g, unsigned char _b) { r = _r; g = _g; b = _b; }
void colour::setRGB(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a) { r = _r; g = _g; b = _b; a = _a; }

void colour::setHSB(unsigned char hue, unsigned char saturation, unsigned char brightness) {
    // HSV algorithm from wikipedia: https://en.wikipedia.org/wiki/HSL_and_HSV#From_HSV
    double S = saturation / 255.0;  // S in [0, 1]
    double V = brightness / 255.0;  // L in [0, 1]
    double H = hue * 3 / 128.0;     // H in [0, 6), equivalent to H = (hue / 256) * 6
    
    double C = S * V;
    double X = C * (1 - fabs(fmod(H, 2) - 1));
    
    double R, G, B; // R, G, B components in [0, 1]
    switch (int(H)) {
        case 0:  R = C; G = X; B = 0; break;
        case 1:  R = X; G = C; B = 0; break;
        case 2:  R = 0; G = C; B = X; break;
        case 3:  R = 0; G = X; B = C; break;
        case 4:  R = X; G = 0; B = C; break;
        case 5:  R = C; G = 0; B = X; break;
        default: R = 0; G = 0; B = 0;
    }
    R += V - C;
    G += V - C;
    B += V - C;
    
    // round to unsigned char
    r = 255 * R + 0.5;
    g = 255 * G + 0.5;
    b = 255 * B + 0.5;
}

void colour::setHSB(unsigned char hue, unsigned char saturation, unsigned char brightness, unsigned char _alpha) {
    setHSB(hue, saturation, brightness);
    a = _alpha;
}

/*
    rect
 */

rect::rect() : left(0), top(0), right(0), bottom(0) {}
rect::rect(double l, double t, double r, double b) : left(l), top(t), right(r), bottom(b) {}
rect::rect(coord min, coord max) : left(min.x), top(min.y), right(max.x), bottom(max.y) {}

double rect::width()   const { return right - left; }
double rect::height()  const { return bottom - top; }
double rect::centreX() const { return (left + right) / 2.0; }
double rect::centreY() const { return (top + bottom) / 2.0; }

// set rect position by left, right, top, bottom
void rect::setLRTB(double _left, double _right, double _top, double _bottom) {
    left = _left; right = _right; top = _top; bottom = _bottom;
}

// set rect position by x, y, width, height
void rect::setXYWH(double x, double y, double width, double height) {
    left = x; right = x + width; top = y; bottom = y + height;
}

// get x and y coordinates of a Position within the rect
coord rect::getPos(Position position) const {
    coord ret;
    
    switch (position) {
        case POS_TOP_LEFT:     { ret.x = left;      ret.y = top;       } break;
        case POS_TOP:          { ret.x = centreX(); ret.y = top;       } break;
        case POS_TOP_RIGHT:    { ret.x = right;     ret.y = top;       } break;
        case POS_LEFT:         { ret.x = left;      ret.y = centreY(); } break;
        case POS_CENTRE:       { ret.x = centreX(); ret.y = centreY(); } break;
        case POS_RIGHT:        { ret.x = right;     ret.y = centreY(); } break;
        case POS_BOTTOM_LEFT:  { ret.x = left;      ret.y = bottom;    } break;
        case POS_BOTTOM:       { ret.x = centreX(); ret.y = bottom;    } break;
        case POS_BOTTOM_RIGHT: { ret.x = right;     ret.y = bottom;    } break;
        default:               { ret.x = 0;         ret.y = 0;         } break;
    }
    
    return ret;
}

// move the rect so that a Position within the rect is now at pos
void rect::movePos(Position position, double x, double y) {
    double W = width();
    double H = height();
    
    // set location by x and y of top-left corner, preserving width and height
    switch(position) {
        case POS_TOP_LEFT:     { setXYWH(x,           y,           W, H); } break;
        case POS_TOP:          { setXYWH(x - W / 2.0, y,           W, H); } break;
        case POS_TOP_RIGHT:    { setXYWH(x - W,       y,           W, H); } break;
        case POS_LEFT:         { setXYWH(x,           y - H / 2.0, W, H); } break;
        case POS_CENTRE:       { setXYWH(x - W / 2.0, y - H / 2.0, W, H); } break;
        case POS_RIGHT:        { setXYWH(x - W,       y - H / 2.0, W, H); } break;
        case POS_BOTTOM_LEFT:  { setXYWH(x,           y - H,       W, H); } break;
        case POS_BOTTOM:       { setXYWH(x - W / 2.0, y - H,       W, H); } break;
        case POS_BOTTOM_RIGHT: { setXYWH(x - W,       y - H,       W, H); } break;
        default:               { } break;
    }
}
void rect::movePos(Position position, coord pos) { movePos(position, pos.x, pos.y); }

// move rect by an offset
void rect::moveBy(double x, double y) {
    left   += x;
    right  += x;
    top    += y;
    bottom += y;
}
void rect::moveBy(coord offset) { moveBy(offset.x, offset.y); }

// return true if the given coordinates lie inside or on the boundary of the rectangle
bool rect::inside(double x, double y) const {
    if (x >= left && x <= right && y >= top && y <= bottom) {
        return true;
    }
    return false;
}

bool rect::inside(coord point) const {
    return inside(point.x, point.y);
}


/*
    ArgonImage
 */

coord ArgonImage::getSize() const { return coord(getWidth(), getHeight()); }

void ArgonImage::draw(double x, double y, double width, double height) const { draw(x, y, width, height, colour(255, 255, 255)); }
void ArgonImage::draw(double x, double y, coord size) const { draw(x, y, size.x, size.y); }
void ArgonImage::draw(double x, double y, coord size, colour _colour) const { draw(x, y, size.x, size.y, _colour); }
void ArgonImage::draw(coord pos, double width, double height) const { draw(pos.x, pos.y, width, height); }
void ArgonImage::draw(coord pos, double width, double height, colour _colour) const { draw(pos.x, pos.y, width, height, _colour); }
void ArgonImage::draw(coord pos, coord size) const { draw(pos.x, pos.y, size.x, size.y); }
void ArgonImage::draw(coord pos, coord size, colour _colour) const { draw(pos.x, pos.y, size.x, size.y, _colour); }
void ArgonImage::draw(rect pos) const { draw(pos.left, pos.top, pos.width(), pos.height()); }
void ArgonImage::draw(rect pos, colour _colour) const { draw(pos.left, pos.top, pos.width(), pos.height(), _colour); }

/*
    ArgonFont
 */

double ArgonFont::getLineHeight() const { return getAscenderHeight() - getDescenderHeight(); }
coord ArgonFont::getTextSize(const std::string &text) const { return coord(getTextWidth(text), getLineHeight()); }
rect ArgonFont::getTextBounds(const std::string &text) const { return rect(coord(0, 0), getTextSize(text)); }

void ArgonFont::drawText(coord pos, colour colour, const std::string &text) const {
    drawText(pos.x, pos.y, colour, text);
}
void ArgonFont::drawText(double x, double y, Position anchor, colour colour, const std::string &text) const {
    rect bounds = getTextBounds(text);
    bounds.movePos(anchor, x, y);
    drawText(bounds.left, bounds.top + getAscenderHeight(), colour, text);
}
void ArgonFont::drawText(coord pos, Position anchor, colour colour, const std::string &text) const {
    drawText(pos.x, pos.y, anchor, colour, text);
}

/*
    Audio
 */

bool micActive = true;
double micVolume = 0;

void setMicVolume(double input) {
    // first increase input amplitude by factor of 20
    // then smooth by mixing current with new scaled input
    // clamp value between 0 and 1
    micVolume = 0.07 * 20 * input + 0.93 * micVolume;
    if (micVolume < 0) micVolume = 0;
    if (micVolume > 1) micVolume = 1;
}

#ifdef WIN32

// if on Windows, the microphone is setup with stub methods -- STILL NEED TO TEST ON WINDOWS
double getMicVolume() { return 0; }

void setMicActive(bool active) { }
bool getMicActive() { return false; }
void toggleMicActive() { }

#else

// otherwise actually return the mic input properly
double getMicVolume() { return micActive ? micVolume : 0; }

void setMicActive(bool active) { micActive = active; }
bool getMicActive() { return micActive; }
void toggleMicActive() { micActive = !micActive; }

#endif /* WIN32 */

/*
    drawLine
 */

void drawLine(coord start, coord end, double width, colour colour) {
    drawLine(start.x, start.y, end.x, end.y, width, colour);
}

/*
    drawRect
 */

void drawRect(rect r, colour colour) {
    drawRect(r.left, r.top, r.width(), r.height(), colour);
}

/*
    Other functions
 */

coord windowSize()   { return coord(windowWidth(), windowHeight()); }
rect  windowBounds() { return rect(coord(0, 0), windowSize()); }

/*
mesh makeThickLine(polyline& _line, float widthSmooth){
    mesh meshy;
    meshy.setMode(GL_PRIMITIVE_TRIANGLE_STRIP);
    
    float angleSmooth;
    
    for (int i = 0;  i < _line.getVertices().size(); i++){
        
        int me_m_one = i-1;
        int me_p_one = i+1;
        if (me_m_one < 0) me_m_one = 0;
        if (me_p_one ==  _line.getVertices().size()) me_p_one =  _line.getVertices().size()-1;
        
        vector3 diff = _line.getVertices()[me_p_one] - _line.getVertices()[me_m_one];
        float angle = atan2(diff.y, diff.x);
        
        if (i == 0) angleSmooth = angle;
        else {
            angleSmooth = ofLerpDegrees(angleSmooth, angle, 1.0);
        }
        
        vector3 offset;
        offset.x = cos(angleSmooth + PI/2) * widthSmooth;
        offset.y = sin(angleSmooth + PI/2) * widthSmooth;
        
        meshy.addVertex(  _line.getVertices()[i] +  offset );
        meshy.addVertex(  _line.getVertices()[i] -  offset );
        
    }
    return meshy;
}

*/
