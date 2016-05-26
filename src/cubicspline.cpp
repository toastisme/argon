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

    Segment::Segment(double _x0, double _y0, double _m0, double _x1, double _y1, double _m1) {
        double dy = _y1 - _y0;
        
        x0 = _x0;
        dx = _x1 - _x0;
        y0 = _y0;
        y1 = _y1;
        a = _m0 * dx - dy;
        b = dy - _m1 * dx;
    }
    
    Segment::Segment(const Point &left, const Point &right) {
        double dy = right.y - left.y;
        
        x0 = left.x;
        dx = right.x - left.x;
        y0 = left.y;
        y1 = right.y;
        a = left.m * dx - dy;
        b = dy - right.m * dx;
    }

    Point const Segment::left() {
        Point p;
        p.x = x0;
        p.y = y0;
        p.m = (y1 - y0 + a) / dx;
        return p;
    }
    
    Point const Segment::right() {
        Point p;
        p.x = x0 + dx;
        p.y = y1;
        p.m = (y1 - y0 - b) / dx;
        return p;
    }

    bool const Segment::inside(double x) {
        if (x < x0) { return false; }
        if (x > (x0 + dx)) { return false; }
        return true;
    }

    double const Segment::value(double x) {
        double t = (x - x0) / dx;
        double omt = 1 - t;
        
        double value = omt * y0 + t * y1;
        value += t * omt * (a * omt + b * t);
        
        return value;
    }

    double const Segment::slope(double x) {
        double t = (x - x0) / dx;
        double omt = 1 - t;
        
        double slope = y1 - y0;
        slope += (omt - t) * (a * omt + b * t);
        slope += t * omt * (b - a);
        slope /= dx;
        
        return slope;
    }

    void Segment::moveLeft(Point target) {
        double m1 = (y1 - y0 - b) / dx;
        
        dx = x0 + dx - target.x;
        x0 = target.x;
        y0 = target.y;
        
        double dy = y1 - y0;
        a = target.m * dx - dy;
        b = dy - m1 * dx;
    }

    void Segment::moveRight(Point target) {
        double m0 = (y1 - y0 + a) / dx;
        
        dx = target.x - x0;
        y1 = target.y;
        
        double dy = y1 - y0;
        a = m0 * dx - dy;
        b = dy - target.m * dx;
    }

    // Spline



    // Default constructor
    Spline::Spline(){
        spline.clear();
        spline.push_back(Segment(0,1,-5,1,0,0));
        //addPoint(3, 1, 1);
        //addPoint(4,1,1);
    }
    
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

    int Spline::getSegment(double x) {
        int index;
        for (index = 1; index < spline.size(); ++index) {
            if (spline[index].left().x > x) { break; }
        }
        return index - 1;
    }

    int Spline::segments() { return spline.size(); }
    int Spline::points() { return segments() + 1; }

    Point const Spline::left()  { return spline.front().left(); }
    Point const Spline::right() { return spline.back().right(); }

    Point const Spline::getPoint(int index) {
        if (index <= 0) {
            return left();
        } else if (index >= spline.size()) {
            return right();
        } else {
            return spline[index].left();
        }
    }
    
    std::vector<Point> const Spline::getPoints() {
        std::vector <Point> vec;
        vec.reserve(points());
        for (int i = 0; i < segments(); ++i) {
            vec.push_back(spline[i].left());
        }
        vec.push_back(spline.back().right());
        return vec;
    }
    
    void Spline::setPoints(std::vector <Point> vec) {
        spline.clear();
        spline.reserve(vec.size() - 1);
        for (int i = 0; i < vec.size() - 1; ++i) {
            spline.push_back(Segment(vec[i], vec[i+1]));
        }
    }
    
    void Spline::reconstruct() {
        std::vector <Point> points = getPoints();
        std::sort(points.begin(), points.end(), Point::comp);
        setPoints(points);
    }

    void Spline::addPoint(double x, double y, double m) {
        if (x < left().x) { // append to start
            Point l_point  = {x, y, m};
            Point r_point = left();
            
            Segment seg = Segment(l_point, r_point);
            spline.insert(spline.begin(), seg);
            
        } else if (x > right().x) { // append to end
            Point l_point  = right();
            Point r_point = {x, y, m};
            
            Segment seg = Segment(l_point, r_point);
            spline.push_back(seg);
            
        } else { // insert in middle
            int index = getSegment(x);
            Point l_point = spline[index].left();
            Point r_point = {x, y, m};
            
            spline[index].moveLeft(r_point);
            Segment seg = Segment(l_point, r_point);
            spline.insert(spline.begin() + index, seg);
        }
    }

    void Spline::removePoint(int index) {
        if (index <= 0) {
            spline.erase(spline.begin());
        } else if (index >= spline.size()) {
            spline.pop_back();
        } else {
            spline[index - 1].moveRight(spline[index].right());
            spline.erase(spline.begin() + index);
        }
    }

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
        
        // TODO: only recalculate everything if two control points cross
        reconstruct();
    }

    bool const Spline::inside(double x) {
        if (x < spline.front().left().x) { return false; }
        if (x > spline.back().right().x) { return false; }
        return true;
    }

    double const Spline::value(double x) {
        int index = getSegment(x);
        return spline[index].value(x);
    }

    double const Spline::slope(double x) {
        int index = getSegment(x);
        return spline[index].slope(x);
    }
}