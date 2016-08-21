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

#include "utilities.hpp"

/*
    coord
 */

// convert to an ofPoint, which is actually a typedefed ofVec3d
ofPoint coord::ofPoint() const {
    return ofVec3f(x, y);
}

/*
    rect
 */

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
void rect::movePos(Position position, coord pos) {
    double x = pos.x;
    double y = pos.y;
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

// move rect by an offset
void rect::moveBy(coord offset) {
    left   += offset.x;
    right  += offset.x;
    top    += offset.y;
    bottom += offset.y;
}

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

// convert to openFrameworks ofRect
ofRectangle rect::ofRect() const {
    return ofRectangle(left, top, width(), height());
}



// clamp point inside rect
coord BilinearClamp(coord point, rect region) {
    point.x = point.x < region.left   ? region.left   : point.x;
    point.x = point.x > region.right  ? region.right  : point.x;
    point.y = point.y < region.top    ? region.top    : point.y;
    point.y = point.y > region.bottom ? region.bottom : point.y;
    
    return point;
}

// map point from one rect to another
coord BilinearMap(coord point, rect in, rect out, bool clamp) {
    coord ret;
    ret.x = out.left + (point.x - in.left) / in.width()  * out.width();
    ret.y = out.top  + (point.y - in.top ) / in.height() * out.height();
    
    if (clamp) {
        return BilinearClamp(ret, out);
    } else {
        return ret;
    }
}

// overloads assume that x and y of rects are zero
coord BilinearMap(coord point, rect in, coord out, bool clamp) {
    return BilinearMap(point, in, {0, out.x, 0, out.y}, clamp);
}
coord BilinearMap(coord point, coord in, rect out, bool clamp) {
    return BilinearMap(point, {0, in.x, 0, in.y}, out, clamp);
}
coord BilinearMap(coord point, coord in, coord out, bool clamp) {
    return BilinearMap(point, {0, in.x, 0, in.y}, {0, out.x, 0, out.y}, clamp);
}

/*
 ROUTINE box2screen_x:
 ROUTINE box2screen_y:
 ROUTINE box2screen:
 Scales coordinates from the box dimensions to the window size, measuring
 positions from a specified (on-screen) origin (defaulting to 0, 0)
 */
double box2screen_x(double x, double x0, double screenWidth, double systemWidth) {
    return x * screenWidth / systemWidth - x0;
}

double box2screen_y(double y, double y0, double screenHeight, double systemHeight) {
    return y * screenHeight / systemHeight - y0;
}

ofPoint box2screen(double x, double y, double x0, double y0, double screenWidth, double screenHeight, double systemWidth, double systemHeight) {
    return ofPoint(box2screen_x(x, x0, screenWidth, systemWidth), box2screen_y(y, y0, screenHeight, systemHeight));
}

ofPoint box2screen(coord point, coord origin, coord screen, coord system) {
    return box2screen(point.x, point.y, origin.x, origin.y, screen.x, screen.y, system.x, system.y);
}

// Make a thick line because openFrameworks is stupid and doesn't have a default method to do this
ofMesh makeThickLine(ofPolyline& line, float widthSmooth)
{
    ofMesh meshy;
    meshy.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    
    float angleSmooth;
    
    for (int i = 0;  i < line.getVertices().size(); i++){
        
        int me_m_one = i-1;
        int me_p_one = i+1;
        if (me_m_one < 0) me_m_one = 0;
        if (me_p_one ==  line.getVertices().size()) me_p_one =  line.getVertices().size()-1;
        
        ofPoint diff = line.getVertices()[me_p_one] - line.getVertices()[me_m_one];
        float angle = atan2(diff.y, diff.x);
        
        if (i == 0) angleSmooth = angle;
        else {
            angleSmooth = ofLerpDegrees(angleSmooth, angle, 1.0);
        }
        
        ofPoint offset;
        offset.x = cos(angleSmooth + PI/2) * widthSmooth;
        offset.y = sin(angleSmooth + PI/2) * widthSmooth;
        
        meshy.addVertex(  line.getVertices()[i] +  offset );
        meshy.addVertex(  line.getVertices()[i] -  offset );
        
    }
    
    return meshy;
}

// Takes in an input vector and returns a binned histogram of the numbers
// NOTE: this will sort the vector passed in
// make a copy of data and pass that if you don't want the order of its elements changed
std::vector <double> histogram(std::vector <double> &data, double min, double max, int bins) {
    int N = data.size();
    std::sort(data.begin(), data.end());
    
    std::vector <double> hist(bins, 0);
    
    double bin_width = (max - min) / bins;
    int curr_bin = 0;
    
    for (int i = 0; i < N; ++i) {
        if (data[i] < min) { continue; }
        if (data[i] > max) { break; }
        double bin_max = min + bin_width * (curr_bin + 1);
        
        while (data[i] > bin_max) {
            ++curr_bin;
            bin_max = min + bin_width * (curr_bin + 1);
        }
        
        hist[curr_bin] += 1.0;
    }
    
    double sum = 0.0;
    for (int i = 0; i < bins; ++i) { sum += hist[i]; }
    if (sum > 0) {
        for (int i = 0; i < bins; ++i) { hist[i] /= sum; }
    }
    
    return hist;
}
