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

#ifndef cubicspline_hpp
#define cubicspline_hpp

#include <stdio.h>
#include <vector>
#include <array>
#include "utilities.hpp"

namespace cubic {
    
    struct Point {
        // A control point for a spline, with a position (x, y) and slope m
        double x;
        double y;
        double m;
        
        static bool comp(const Point &a, const Point &b) { return a.x < b.x; }
    };
    
    class Segment {
        /*
            Defines a cubic interpolation connecting two points with specified start and end gradients.
            
            The cubic spline between two endpoints (x0, y0) and (x1, y1), with gradients at these endpoints
            of m0 and m1 respectively, is given by:
         
                y(t) = (1-t) * y0  +  t * y1  +  t * (1-t) * [ (1-t) * a + t * b ]
         
            where:
        
                t =  (x - x0) / (x1 - x0)       = (x - x0) / dx
                a =  m0 * (x1 - x0) - (y1 - y0) = m0 * dx - dy
                b = -m1 * (x1 - x0) + (y1 - y0) = dy - m1 * dx
               dx =  x1 - x0
               dy =  y1 - y0
         
            To store the six degrees of freedom in a spline segment, I've chosen to store a and b directly, as
            well as dx rather than x1. This should make the slowest part of the calculation (calculating the
            potential and gradient for each pair of particles) as fast as possible.
         */
        
    private:
        double x0;
        double dx;  // is storing as 1/dx faster?
        double y0;
        double y1;
        double a;   // m0 * dx - dy
        double b;   // dy - m1 * dx
    
    public:
        Segment(double x0, double y0, double m0, double x1, double y1, double m1);
        Segment(const Point &left, const Point &right);
        
        // getters for the two endpoints
        Point left()  const;
        Point right() const;
        
        bool  inside(double x) const;  // true if x is between the left and right endpoints
        double value(double x) const;   // value of segment at position x, i.e. returns y(x)
        double slope(double x) const;   // slope of segment at position x, i.e. returns y'(x)
        
        void moveLeft( const Point &target); // move left endpoint
        void moveRight(const Point &target); // move right endpoint
    };
    
    class Spline {
        /*
            Defines a cubic spline containing multiple cubic segments.
         
            This is effectively just a container for multiple Segments. The intended use is to intialise the class
            by defining the start and end points, and then use addPoint to add extra control points.
         
            The segments are stored within the vector spline in order of increasing x.
         */
        
    private:
        // returns the index of the segment which x lies in (i.e. which element of the spline vector to look at)
        // private function since the segments in the spline aren't exposed to the user
        int getSegment(double x) const;
        
        std::vector <Segment> spline;
        
    public:
        // Default constructor
        Spline();
        // initialise spline as a single segment to add futher points later
        Spline(double x0, double y0, double m0, double x1, double y1, double m1);
        
        int segments() const; // number of segments in spline
        int points()   const; // number of control points (== segments() + 1)
        
        // getters for the two endpoints
        Point left()  const;
        Point right() const;
        
        // getter for a particular control point on the spline
        Point getPoint(int index) const;
        
        // getters for using the spline as a function
        bool   inside(double x) const; // true if x is between the left and right endpoints
        double value(double x)  const; // value of the spline at position x, i.e. returns y(x)
        double slope(double x)  const; // slope of the spline at position x, i.e. returns y'(x)
        
        // get vector of control points
        std::vector <Point> getPoints() const;
        
        // set segments from vector of control points
        void setPoints(const std::vector <Point> &vec);
        
        // reconstruct segments from control points
        void reconstruct();
        
        // add, remove or move a control point to/from/in the spline
        void addPoint(double x, double y, double m);
        void removePoint(int index);
        void movePoint(int index, double x, double y, double m);
    };
}

#endif /* cubicspline_hpp */
