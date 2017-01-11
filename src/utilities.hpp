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
#include "platform.hpp"

namespace util {
    // if value < min, return min
    // if value > max, return max
    // else return value
    double clamp(double value, double min, double max);
    
    // return the value partway between min and max, with its position defined by t, e.g.
    // e.g. lerp(0.25, 1, 5) == 2.0 as 2 is a quarter of the way from 1 to 5
    // use clamp to clamp the result between min and max
    double lerp(double t, double min, double max, bool clamp = false);
    
    // returns the inverse of the above lerp function
    // e.g. invlerp(2, 1, 5) == 0.25 as 2 is a quarter of the way from 1 to 5
    // use clamp to clamp the result between 0 and 1
    double invlerp(double value, double min, double max, bool clamp = false);
    
    // map value from the range (in_min, in_max) to the range (out_min, out_max)
    // i.e. an invlerp follwed by a lerp
    // use clamp to clamp the result between out_min and out_max
    double map(double value, double in_min, double in_max, double out_min, double out_max, bool clamp = false);
    
    // clamp a point between coords min and max
    // essentially just a normal linear clamp on both x and y
    coord biclamp(coord point, coord min, coord max);
    // as above, but clamp inside a rect
    coord biclamp(coord point, rect limits);
    
    // return the coord partway between min and max, with its position defined by t
    coord bilerp(double t, coord min, coord max, bool clamp = false);
    
    // map a point from a space defined by rect in to a space defined by rect out
    // e.g. map from MD space to screen space
    coord bimap(coord point, rect in, rect out, bool clamp = false);
    // overloads assume that a coord is equivalent to a rect with top-left of (0, 0)
    coord bimap(coord point, coord in, coord out, bool clamp = false);
    
    // create a histogram from a data set
    std::vector <double> histogram(std::vector <double> &data, double min, double max, int bins);
};

//ofMesh makeThickLine(ofPolyline& line, float widthSmooth);


#endif /* utilities_hpp */
