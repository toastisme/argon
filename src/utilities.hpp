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

#ifndef utilities_hpp
#define utilities_hpp

#include "ofMain.h"

enum Position
{
    /*
        Enum defining nine positions within a rect.
     */
    
    POS_TOP_LEFT, POS_TOP, POS_TOP_RIGHT,
    POS_LEFT, POS_CENTRE, POS_RIGHT,
    POS_BOTTOM_LEFT, POS_BOTTOM, POS_BOTTOM_RIGHT
};

struct coord
{
    double x, y;
    
    ofPoint ofPoint() const; // convert to openFrameworks ofPoint
};

struct rect
{
    /*
        Struct defining a rectangle. Data is stored as the position of each side of the
        rectangle, and various methods are given to get the width, height, centre, etc.
     */
    
    double left, right, top, bottom;
    
    double width()   const;
    double height()  const;
    double centreX() const;   // x coordinate of centre
    double centreY() const;   // y coordinate of centre
    
    // set rect to location and size either by left/right/top/bottom or by x/y/width/height
    void setLRTB(double left, double right, double top, double bottom);
    void setXYWH(double x, double y, double width, double height);
    
    // get the coordinates of a point within the rect specified by the Position enum,
    // i.e. getPos(TOP_LEFT) == {left, top}
    coord getPos(Position position) const;
    
    // move the rect (preserving width and height) such that a specified position in the rectangle,
    // given by the enum position, is now at the point pos
    void movePos(Position position, coord pos);
    
    // move the rect (preserving width and height) by an offset
    void moveBy(coord offset);
    
    // true if the point x, y is inside (or on the boundary of) the rect
    bool inside(double x, double y) const;
    bool inside(coord point) const;
    
    ofRectangle ofRect() const; // convert to openFrameworks ofRectangle
};

// bilinear clamp, returning a coord clamped inside a rect
// essentially just a normal linear clamp on both x and y
coord BilinearClamp(coord point, rect out);

// bilinear map from space defined by rect in to space defined by rect out
// e.g. maps from MD space to screen space
// if the rect is passed in as a coord, then use (0, 0) as the x, y coordinates and
// (coord.x, coord.y) as the width and height
coord BilinearMap(coord point, rect  in, rect  out, bool clamp = false);
coord BilinearMap(coord point, rect  in, coord out, bool clamp = false);
coord BilinearMap(coord point, coord in, rect  out, bool clamp = false);
coord BilinearMap(coord point, coord in, coord out, bool clamp = false);

// Convert box coordinates to screen coordinates
double box2screen_x(double x, double x0, double screenWidth, double systemWidth);
double box2screen_y(double y, double y0, double screenHeight, double systemHeight);
ofPoint box2screen(double x, double y, double x0, double y0, double screenWidth, double screenHeight, double systemWidth, double systemHeight);
ofPoint box2screen(coord point, coord origin, coord screen, coord system);

ofMesh makeThickLine(ofPolyline& line, float widthSmooth);

#endif /* utilities_hpp */
