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

double clamp(double value, double min, double max);
double lerp(double t, double min, double max, bool clamp = false);
double invlerp(double value, double min, double max, bool clamp = false);
double map(double value, double in_min, double in_max, double out_min, double out_max, bool clamp = false);

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
// THESE NEED TO BE CHANGED TO BILINEAR MAPS
double box2screen_x(double x, double x0, double screenWidth, double systemWidth);
double box2screen_y(double y, double y0, double screenHeight, double systemHeight);
ofPoint box2screen(double x, double y, double x0, double y0, double screenWidth, double screenHeight, double systemWidth, double systemHeight);
ofPoint box2screen(coord point, coord origin, coord screen, coord system);

ofMesh makeThickLine(ofPolyline& line, float widthSmooth);

std::vector <double> histogram(std::vector <double> &data, double min, double max, int bins);

#endif /* utilities_hpp */
