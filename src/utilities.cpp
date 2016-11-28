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
    return BilinearMap(point, in, rect(0, 0, out.x, out.y), clamp);
}
coord BilinearMap(coord point, coord in, rect out, bool clamp) {
    return BilinearMap(point, rect(0, 0, in.x, in.y), out, clamp);
}
coord BilinearMap(coord point, coord in, coord out, bool clamp) {
    return BilinearMap(point, rect(0, 0, in.x, in.y), rect(0, 0, out.x, out.y), clamp);
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
