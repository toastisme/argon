/*
 StarredMD
 
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