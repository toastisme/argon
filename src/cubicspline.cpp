//
//  cubicspline.cpp
//  testPaths
//
//  Created by Staszek Welsh on 08/05/2016.
//
//

#include "cubicspline.hpp"
#include <algorithm>

namespace cubic {
    
    // Segment

    // Create a segment
    // Left-hand control point at _x0, _y0 with slope _m0
    // Right-hand control point at _x1, _y1 with slope _m1
    Segment::Segment(double _x0, double _y0, double _m0, double _x1, double _y1, double _m1) {
        double dy = _y1 - _y0;
        
        x0 = _x0;
        dx = _x1 - _x0;
        y0 = _y0;
        y1 = _y1;
        a = _m0 * dx - dy;
        b = dy - _m1 * dx;
    }
    
    // Create a segment between the Points left and right
    Segment::Segment(const Point &left, const Point &right) {
        double dy = right.y - left.y;
        
        x0 = left.x;
        dx = right.x - left.x;
        y0 = left.y;
        y1 = right.y;
        a = left.m * dx - dy;
        b = dy - right.m * dx;
    }

    // Return the Point at the left-hand side of the Segment
    Point Segment::left() const {
        Point p;
        p.x = x0;
        p.y = y0;
        p.m = (y1 - y0 + a) / dx;
        return p;
    }
    
    // Return the Point at the right-hand side of the Segment
    Point Segment::right() const {
        Point p;
        p.x = x0 + dx;
        p.y = y1;
        p.m = (y1 - y0 - b) / dx;
        return p;
    }

    // Returns true if x is bewteen the left and right control points
    bool Segment::inside(double x) const {
        if (x < x0) { return false; }
        if (x > (x0 + dx)) { return false; }
        return true;
    }

    // Returns the value of the spline at position x, i.e. y(x)
    double Segment::value(double x) const {
        double t = (x - x0) / dx;
        double omt = 1 - t;
        
        double value = omt * y0 + t * y1;
        value += t * omt * (a * omt + b * t);
        
        return value;
    }

    // Returns the slope at position x, i.e. y'(x)
    double Segment::slope(double x) const {
        double t = (x - x0) / dx;
        double omt = 1 - t;
        
        double slope = y1 - y0;
        slope += (omt - t) * (a * omt + b * t);
        slope += t * omt * (b - a);
        slope /= dx;
        
        return slope;
    }

    // Moves the left-hand control point to a position/slope defined by target
    void Segment::moveLeft(const Point &target) {
        double m1 = (y1 - y0 - b) / dx;
        
        dx = x0 + dx - target.x;
        x0 = target.x;
        y0 = target.y;
        
        double dy = y1 - y0;
        a = target.m * dx - dy;
        b = dy - m1 * dx;
    }

    // Moves the right-hand control point to a position/slope defined by target
    void Segment::moveRight(const Point &target) {
        double m0 = (y1 - y0 + a) / dx;
        
        dx = target.x - x0;
        y1 = target.y;
        
        double dy = y1 - y0;
        a = m0 * dx - dy;
        b = dy - target.m * dx;
    }

    // Spline

    // Create a new Spline with one segment
    // Use addPoint() later to subdivide into further segments
    // Check if the points are in the correct order and reverse them if not
    Spline::Spline(double x0, double y0, double m0, double x1, double y1, double m1) {
        spline.clear();
        if (x0 <= x1) {
            spline.push_back(Segment(x0, y0, m0, x1, y1, m1));
        } else {
            spline.push_back(Segment(x1, y1, m1, x0, y0, m0));
        }
    }

    // Returns the index in the vector of Segments corresponding to the
    // segment containing x
    int Spline::getSegment(double x) const {
        int index;
        
        // loop through the spline segments
        // stop when we've gone past x
        
        for (index = 1; index < spline.size(); ++index) {
            if (spline[index].left().x > x) { break; }
        }
        
        // we've gone past x, so return the index containing x
        return index - 1;
    }

    int Spline::segments() const { return spline.size(); }
    int Spline::points()   const { return segments() + 1; }

    Point Spline::left()  const { return spline.front().left(); }
    Point Spline::right() const { return spline.back().right(); }

    // Get a control point by its index
    Point Spline::getPoint(int index) const {
        if (index <= 0) { // 
            return left();
        } else if (index >= spline.size()) {
            return right();
        } else {
            return spline[index].left();
        }
    }
    
    // create a vector of all the control points
    std::vector<Point> Spline::getPoints() const {
        std::vector <Point> vec;
        vec.reserve(points());
        for (int i = 0; i < segments(); ++i) {
            vec.push_back(spline[i].left());
        }
        vec.push_back(spline.back().right());
        return vec;
    }
    
    // sets all the segments so that they match a given vector of control points
    void Spline::setPoints(const std::vector <Point> &vec) {
        spline.clear();
        spline.reserve(vec.size() - 1);
        for (int i = 0; i < vec.size() - 1; ++i) {
            spline.push_back(Segment(vec[i], vec[i+1]));
        }
    }
    
    // reconstructs the internal vector so that all the segments are in left-to-right order
    void Spline::reconstruct() {
        std::vector <Point> points = getPoints();
        std::sort(points.begin(), points.end(), Point::comp);   // sort by Point.x 
        setPoints(points);
    }

    // add a new control point
    void Spline::addPoint(double x, double y, double m) {
        if (x < left().x) {                                     // append to start
            Point l_point  = {x, y, m};                         // Left point from parameters 
            Point r_point = left();                             // Right point is left end of spline 
            
            // create the new segment and add to the front
            Segment seg = Segment(l_point, r_point);
            spline.insert(spline.begin(), seg);
            
        } else if (x > right().x) {                             // append to end
            Point l_point  = right();                           // Left point is right end of spline 
            Point r_point = {x, y, m};                          // Right point from parameters 
            
            // create the new segment and add to the end
            Segment seg = Segment(l_point, r_point);
            spline.push_back(seg);
            
        } else {                                                // insert in middle
            int index = getSegment(x);                          // get the index of the segment to be inserted into 
            Point l_point = spline[index].left();               // Left point of segment to be made at left side of segment to be inserted into 
            Point r_point = {x, y, m};                          // Right point of segment to be made from parameters 
            
            // move the existing segment to start after the new one
            spline[index].moveLeft(r_point);
            // create the new segment and add just before the adjusted one
            Segment seg = Segment(l_point, r_point);
            spline.insert(spline.begin() + index, seg);
        }
    }

    // remove a specified control point
    void Spline::removePoint(int index) {
        if (index <= 0) {
            // Remove first segment
            spline.erase(spline.begin());
        } else if (index >= spline.size()) {
            // Remove last segment
            spline.pop_back();
        } else {
            // Move right side of previous segment to join up spline
            spline[index - 1].moveRight(spline[index].right());
            // Then remove segment
            spline.erase(spline.begin() + index);
        }
    }

    // move a specified point to a new position
    void Spline::movePoint(int index, double x, double y, double m) {
        Point target = {x, y, m};
        
        if (index <= 0) {
            spline.front().moveLeft(target);
        } else if (index > spline.size()) {
            spline.back().moveRight(target);
        } else {
            spline[index-1].moveRight(target);
            spline[index].moveLeft(target);
        }
        
        // This might change the order of the control points, so we need to reconstruct
        // the vector of segments so that everything is well-ordered
        // TODO: only recalculate everything if two control points change order
        reconstruct();
    }

    // return whether x is between left- and right-sides of spline
    bool Spline::inside(double x) const {
        if (x < spline.front().left().x) { return false; }
        if (x > spline.back().right().x) { return false; }
        return true;
    }

    // return value of spline at position x, i.e. y(x)
    double Spline::value(double x) const {
        int index = getSegment(x);
        return spline[index].value(x);
    }

    // return slope of spline at poisition x, i.e. y'(x)
    double Spline::slope(double x) const {
        int index = getSegment(x);
        return spline[index].slope(x);
    }
}
