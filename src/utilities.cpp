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

namespace util {
    double clamp(double value, double min, double max) {
        value = value < min ? min : value;
        value = value > max ? max : value;
        return value;
    }

    double lerp(double t, double min, double max, bool _clamp) {
        double value = (1 - t) * min + t * max;
        if (_clamp) { value = clamp(value, min, max); }
        return value;
    }

    double invlerp(double value, double min, double max, bool _clamp) {
        double t = (value - min) / (max - min);
        if (_clamp) { t = clamp(t, 0, 1); }
        return t;
    }

    double map(double value, double in_min, double in_max, double out_min, double out_max, bool _clamp) {
        double t = invlerp(value, in_min, in_max, _clamp);
        return lerp(t, out_min, out_max);
    }
    
    coord biclamp(coord point, coord min, coord max) {
        point.x = clamp(point.x, min.x, max.x);
        point.y = clamp(point.y, min.y, max.y);
        return point;
    }
    
    coord biclamp(coord point, rect limits) {
        return biclamp(point, limits.min, limits.max);
    }
    
    coord bilerp(double t, coord min, coord max, bool clamp) {
        coord point;
        point.x = lerp(t, min.x, max.x, clamp);
        point.y = lerp(t, min.y, max.y, clamp);
        return point;
    }
    
    coord bimap(coord point, rect in, rect out, bool clamp) {
        point.x = map(point.x, in.left, in.right, out.left, out.right, clamp);
        point.y = map(point.y, in.top, in.bottom, out.top, out.bottom, clamp);
        return point;
    }
    
    coord bimap(coord point, coord in, coord out, bool clamp) {
        return bimap(point, rect(coord(0, 0), in), rect(coord(0, 0), out), clamp);
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
};

// Make a thick line because openFrameworks is stupid and doesn't have a default method to do this
/*
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
 */
