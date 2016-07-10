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

#include "gui_derived.hpp"

// implements PotentialAtom, SplineContainer and SplineControlPoint

namespace gui {
    
    /*
        PotentialAtom
     */
    
    PotentialAtom::PotentialAtom(md::MDContainer &system, int _numPoints, double min_x, double max_x, double min_y, double max_y, int x, int y, int width, int height): theSystem(system), numPoints(_numPoints), UIAtom(x, y, width, height)
    {
        potBounds = {min_x, max_x, max_y, min_y};
    }
    
    void PotentialAtom::render() {
        PotentialFunctor &pot = theSystem.getPotential();
        std::vector<coord> potPoints, particlePoints;
        
        double x, y;
        double x_spacing = potBounds.width() / (numPoints - 1);
        
        // Set up particle separations, relative to particle N/2, which
        // is hopefully roughly in the centre of the system
        int posRelIndex = theSystem.getN() / 2;
        coord posRel = theSystem.getPos(posRelIndex);
        
        // put all particle positions into particlePoints
        coord pos;
        for (int i = 0; i < theSystem.getN(); i++){
            if (i == posRelIndex) { continue; }
            
            pos = theSystem.getPos(i);
            x = pos.x - posRel.x;
            y = pos.y - posRel.y;
            pos.x = sqrt(x*x + y*y);
            pos.y = pot.potential(pos.x);
            
            // map to dimensions of UI element
            pos = BilinearMap(pos, potBounds, bounds);
            particlePoints.push_back(pos);
        }
        
        // put all positions along potential curve into potPoints
        for (int i = 0; i < numPoints; i++){
            pos.x = potBounds.left + i * x_spacing;
            pos.y = pot.potential(pos.x);
            
            // map to dimensions of UI element
            pos = BilinearMap(pos, potBounds, bounds);
            potPoints.push_back(pos);
        }
        
        // Plot the potential
        ofSetLineWidth(3.5);
        ofSetColor(255,255,255, 220);
        for (int i = 0; i < potPoints.size() - 1; i++){
            coord lPoint = potPoints[i];
            coord rPoint = potPoints[i + 1];
            
            if (bounds.inside(lPoint) && bounds.inside(rPoint)) {
                ofDrawLine(lPoint.x, lPoint.y, rPoint.x, rPoint.y);
            }
        }
        
        // Plot the particles along the curve
        ofSetCircleResolution(10);
        ofSetColor(0, 100, 220, 220);
        for (int i = 0; i < particlePoints.size(); i++){
            pos = particlePoints[i];
            if (bounds.inside(pos)) { ofDrawCircle(pos.x, pos.y, 4); }
        }
        
    }
    
    
    
    /*
        SplineControlPoint
     */
    
    SplineControlPoint::SplineControlPoint(int x, int y, double _radius, rect _range) : m(0), radius(_radius), range(_range), mouseFocus(false), UIAtom(0, 0, 2 * _radius, 2 * _radius)
    {
        movePoint(x, y);
    }
    
    void SplineControlPoint::render() {
        ofSetCircleResolution(20);
        
        ofSetColor(0, 0, 0);
        ofDrawCircle(bounds.centreX(), bounds.centreY(), radius);
        
        ofSetColor(255, 255, 255);
        ofDrawCircle(bounds.centreX(), bounds.centreY(), radius - 3);
    }
    
    void SplineControlPoint::movePoint(double x, double y) {
        coord target = {x, y};
        target = BilinearClamp(target, range);
        bounds.movePos(POS_CENTRE, target);
    }
    
    bool SplineControlPoint::mousePressed(int x, int y, int button) {
        if (bounds.inside(x, y)) {
            switch (button) {
                case 0:   // left click
                    mouseFocus = true;
                    movePoint(x, y);
                    goto handled;
                case 2:   // right click
                    goto handled;
                case 3:   // left button
                    m -= 0.4;
                    if (m < -4) { m = -4; }
                    goto handled;
                case 4:   // right button
                    m += 0.4;
                    if (m > 4) { m = 4; }
                    goto handled;
                handled:
                    return true;
                    break;
                default:
                    return false;
            }
        } else { return false; }
    }
    
    bool SplineControlPoint::mouseReleased(int x, int y, int button) {
        if (button == 0) { mouseFocus = false; }
        return false;
    }
    
    bool SplineControlPoint::mouseMoved(int x, int y) {
        if (mouseFocus) {
            movePoint(x, y);
            return true;
        } else { return false; }
    }
    
    
    
    /*
        SplineContainer
     */
    
    SplineContainer::SplineContainer(CustomPotential &_potential, double min_x, double max_x, double min_y, double max_y, double _radius, double x, double y, double width, double height) : potential(_potential), radius(_radius), UIContainer(x, y, width, height)
    {
        splineRegion = {min_x, max_x, max_y, min_y};
        pointRegion  = {bounds.left + radius, bounds.right - radius, bounds.top + radius, bounds.bottom - radius};
        
    }
    
    void SplineContainer::updateSpline() {
        std::vector <cubic::Point> points;
        
        coord pos;
        double m;
        for (int i = 0; i < children.size(); ++i) {
            pos.x = children[i]->getRect().getPos(POS_CENTRE).x;
            pos.y = children[i]->getRect().getPos(POS_CENTRE).y;
            m     = ((SplineControlPoint*)children[i])->m;
            
            pos = BilinearMap(pos, bounds, splineRegion);
            points.push_back({pos.x, pos.y, m});
        }
        
        potential.updatePoints(points);
    }
    
    bool SplineContainer::controlPointNear(double x, int except) {
        for (int i = 0; i < children.size(); ++i) {
            if (i == except) { continue; }
            if (abs(x - children[i]->getRect().centreX()) < 5) { return true; }
        }
        return false;
    }
    
    bool SplineContainer::mousePressed(int x, int y, int button) {
        if (visible && bounds.inside(x, y)) {
            switch (button) {
                    
                case 0: {
                    bool handled = false;
                    for (int i = 0; i < children.size(); ++i) {
                        handled = children[i]->mousePressed(x, y, 0);
                        
                        if (handled) {
                            // move child to back of list to draw this child on top
                            UIBase *child = children[i];
                            children.erase(children.begin() + i);
                            children.push_back(child);
                            break;
                        }
                    }
                    
                    if (!handled && !controlPointNear(x)) {
                        addChild(new SplineControlPoint(x - bounds.left, y - bounds.top, radius, pointRegion));
                    }
                } goto handled;
                    
                case 2: {
                    bool handled = false;
                    for (int i = 0; i < children.size(); ++i) {
                        handled = children[i]->mousePressed(x, y, 2);
                        
                        if (handled) {
                            delete children[i];
                            children.erase(children.begin() + i);
                            break;
                        }
                    }
                } goto handled;
                    
                case 3: {
                    bool handled = false;
                    for (int i = 0; i < children.size(); ++i) {
                        handled = children[i]->mousePressed(x, y, 3);
                        
                        if (handled) { break; }
                    }
                } goto handled;
                    
                case 4: {
                    bool handled = false;
                    for (int i = 0; i < children.size(); ++i) {
                        handled = children[i]->mousePressed(x, y, 4);
                        
                        if (handled) { break; }
                    }
                } goto handled;
                    
                handled:
                    updateSpline();
                    return true;
                    break;
                default:
                    return false;
            }
        } else { return false; }
    }
    
    bool SplineContainer::mouseMoved(int x, int y) {
        if (!controlPointNear(x)) {
            bool handled = false;
            for (int i = 0; i < children.size(); ++i) {
                handled = children[i]->mouseMoved(x, y);
            }
            if (handled) { updateSpline(); }
        }
        return false;
    }
}
