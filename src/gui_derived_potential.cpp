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
        ofSetColor(255,255,255,220);
        ofPolyline line;
        for (int i = 0; i < potPoints.size(); i++){
            line.lineTo(potPoints[i].x, potPoints[i].y);
        }
        
        // draw potential, clipping to rectangle of size bounds
        glScissor(bounds.left, bounds.top, bounds.width(), bounds.height());
        glEnable(GL_SCISSOR_TEST);
        line.draw();
        glDisable(GL_SCISSOR_TEST);
        
        // Plot the particles along the curve
        ofSetCircleResolution(10);
        ofSetColor(0, 100, 220, 220);
        for (int i = 0; i < particlePoints.size(); i++){
            pos = particlePoints[i];
            if (bounds.inside(pos)) { ofDrawCircle(pos.x, pos.y, 6); }
        }
        
    }
    
    
    
    /*
        SplineControlPoint
     */
    
    SplineControlPoint::SplineControlPoint(int x, int y, double _radius, rect _pointBounds) : m(0), radius(_radius), pointBounds(_pointBounds), mouseFocus(false), UIAtom(x - _radius, y - _radius, 2 * _radius, 2 * _radius)
    {}
    
    void SplineControlPoint::render() {
        ofSetCircleResolution(20);
        
        ofSetColor(10, 174, 199);
        ofDrawCircle(bounds.centreX(), bounds.centreY(), radius);
        
        ofSetColor(82, 225, 247);
        ofDrawCircle(bounds.centreX(), bounds.centreY(), radius - 3);
    }
    
    void SplineControlPoint::movePoint(double x, double y) {
        coord target = {x, y};
        target = BilinearClamp(target, pointBounds);
        bounds.movePos(POS_CENTRE, target);
    }
    
    bool SplineControlPoint::mousePressed(int x, int y, int button) {
        if (bounds.inside(x, y)) {
            switch (button) {
                    
                case 0:   // left click, start being dragged
                    mouseFocus = true;
                    goto handled;
                    
                case 2:   // right click, delete point
                    // do nothing (other than return true)
                    goto handled;
                    
                case 3:   // left button, decrease slope
                    m -= 0.4;
                    if (m < -4) { m = -4; }
                    goto handled;
                    
                case 4:   // right button, increase slope
                    m += 0.4;
                    if (m > 4) { m = 4; }
                    goto handled;
                    
                handled:
                    return true;
                    break;
                    
                default:
                    return false;
            }
        } else {
            return false;
        }
    }
    
    // if left-click was released, lose mouse focus
    // return false so that we don't capture the mouse release event
    bool SplineControlPoint::mouseReleased(int x, int y, int button) {
        if (button == 0) {
            mouseFocus = false;
        }
        return false;
    }
    
    // if we have mouse focus, move the point
    bool SplineControlPoint::mouseMoved(int x, int y) {
        if (mouseFocus) {
            movePoint(x, y);
            return true;
        } else {
            return false;
        }
    }
    
    
    
    /*
        SplineContainer
     */
    
    SplineContainer::SplineContainer(CustomPotential &_potential, double min_x, double max_x, double min_y, double max_y, double _radius, double x, double y, double width, double height) : potential(_potential), radius(_radius), UIContainer(x, y, width, height)
    {
        splineRegion.setLRTB(min_x, max_x, max_y, min_y);
        pointRegion.setLRTB(bounds.left + radius, bounds.right - radius, bounds.top + radius, bounds.bottom - radius);
    }
    
    void SplineContainer::moveBy(coord offset) {
        bounds.moveBy(offset);
        pointRegion.moveBy(offset);
    }
    
    // map the spline points and pass to the potential
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
    
    // return true if there is a control point with x-coordinate close to the given x
    // optionally can exclude the point with index except from the check
    bool SplineContainer::controlPointNear(double x, int except) {
        for (int i = 0; i < children.size(); ++i) {
            if (i == except) { continue; }
            if (abs(x - children[i]->getRect().centreX()) < 8) { return true; }
        }
        return false;
    }
    
    // handle mouse click events
    bool SplineContainer::mousePressed(int x, int y, int button) {
        if (visible && bounds.inside(x, y)) {
            switch (button) {
                // left click: set mouseFocus to true or create new control point
                case 0: {
                    bool hitChild = false;   // slightly unfortunate variable name...
                    
                    // loop through backwards so that the point drawn on top is clicked first (which,
                    // since they are drawn in forward-order, is the last child)
                    for (int i = children.size() - 1; i >= 0; --i) {
                        
                        // handled is true if children[i] was just clicked on
                        hitChild = children[i]->mousePressed(x, y, 0);
                        
                        if (hitChild) {
                            // move child to back of list to draw this child on top
                            UIBase *child = children[i];
                            children.erase(children.begin() + i);
                            children.push_back(child);
                            break;
                        }
                    }
                    
                    // if we haven't clicked on a child, then make a new control point if the x-
                    // coordinate doesn't overlap another point
                    if (!hitChild) {
                        if (pointRegion.inside(x, y) && !controlPointNear(x)) {
                            // use push_back, not addChild, so that we make the child directly at (x, y),
                            // instead of at (x, y) relative to the top-left corner of the container
                            children.push_back(new SplineControlPoint(x, y, radius, pointRegion));
                            children.back()->mousePressed(x, y, 0);
                        } else {
                            // only return here if we haven't clicked on a child, and if we couldn't
                            // create a new point at the mouse position
                            return false;
                        }
                    }
                } goto handled;
                    
                // right click: remove control point
                case 2: {
                    bool hitChild = false;
                    for (int i = 0; i < children.size(); ++i) {
                        hitChild = children[i]->mousePressed(x, y, 2);
                        
                        if (hitChild) {
                            // free the memory, then delete the pointer
                            delete children[i];
                            children.erase(children.begin() + i);
                            break;
                        }
                    }
                    if (!hitChild) { return false; }
                } goto handled;
                    
                // left button: decrease gradient
                // right button: increase gradient
                // both behave exactly the same, so can use the same case
                case 3:
                case 4: {
                    bool hitChild = false;
                    for (int i = 0; i < children.size(); ++i) {
                        hitChild = children[i]->mousePressed(x, y, button);
                        
                        if (hitChild) { break; }
                    }
                    if (!hitChild) { return false; }
                } goto handled;
                    
                // we hit the goto if we need to update the spline
                handled:
                    updateSpline();
                    return true;
                    break;
                    
                default:
                    return false;
            }
        } else {
            return false;
        }
    }
    
    bool SplineContainer::mouseMoved(int x, int y) {
        bool handled = false;
        
            for (int i = 0; i < children.size(); ++i) {
                
                // test to avoid moving one control point on top of another
                if (!controlPointNear(x, i)) {
                    
                    // if mouse is outside the spline controls window, we need to be
                    // careful because controlPointNear won't test properly. The solution
                    // is to only move vertically if the mouse is too far left or right
                    if (x < pointRegion.left || x > pointRegion.right) {
                        handled = children[i]->mouseMoved(children[i]->getRect().centreX(), y);
                    } else {
                        handled = children[i]->mouseMoved(x, y);
                    }
                }
            }
        
        if (handled) {
            updateSpline();
        }
        
        return handled;
    }
}
