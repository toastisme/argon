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

#include "gui_derived.hpp"

// implements PotentialContainer, PotentialAtom, SplineContainer and SplineControlPoint

namespace gui {
    
    /*
        PotentialContainer
     */
    
    // all locations stored as magic numbers for now
    // perhaps eventually store these as static variables in PotentialContainer? or work them out from the window dimensions?
    PotentialContainer::PotentialContainer(md::MDContainer &system, ofTrueTypeFont &uiFont12, ArgonImage &ljThumbnail, ArgonImage &squareThumbnail, ArgonImage &morseThumbnail, ArgonImage &customThumbnail, ArgonImage &resetSplinePointsButton) : UIContainer(0, 0, 924, 500), theSystem(system) {
        
        ofColor bgcolor = ofColor(80, 80, 80, 80);
        ofColor textcolor = ofColor(255, 255, 240);
        
        addChild(new RectAtom(bgcolor, 0, 0, 924, 500));
        addChild(new RectAtom(bgcolor, 150, 0, 774, 500));
        highlightAtomIndex = addIndexedChild(new RectAtom(bgcolor, 0, 0, 150, 125));
        
        // Setup potential atoms
        addChild(new PotentialAtom(theSystem, 300, 0.95, 3.0, -2, 2, 150, 0, 774, 500));
        splineContainerIndex = addIndexedChild(
            new SplineContainer(theSystem, 0.95, 3.0, -2, 2, 15, 150, 0, 774, 500)
        );
        
        addChild(new gui::SetColour(ofColor(255, 255, 240)));
        
        // add the first four buttons and text
        addChild(new ButtonAtom([&] () { setPotential(LENNARD_JONES); }, ljThumbnail, 25, 0, 100, 100));
        addChild(new TextAtom("Lennard-Jones", uiFont12, textcolor, POS_TOP, 0, 100, 150, 25));
        
        addChild(new ButtonAtom([&] () { setPotential(SQUARE_WELL); }, squareThumbnail, 25, 125, 100, 100));
        addChild(new TextAtom("Square Well", uiFont12, textcolor, POS_TOP, 0, 225, 150, 25));
        
        addChild(new ButtonAtom([&] () { setPotential(MORSE); }, morseThumbnail, 25, 250, 100, 100));
        addChild(new TextAtom("Morse", uiFont12, textcolor, POS_TOP, 0, 350, 150, 25));
        
        customPotentialIndex = addIndexedChild(new ButtonAtom([&] () { setPotential(CUSTOM); }, customThumbnail, 25, 375, 100, 100));
        addChild(new TextAtom("Custom", uiFont12, textcolor, POS_TOP, 0, 475, 150, 25));
        
        // reset potentials button + text
        UIContainer* resetContainer = new UIContainer(0, 375, 100, 100);
        resetContainer->addChild(new ButtonAtom([&] () { ((gui::SplineContainer *)getChild(splineContainerIndex))->destroyAllPoints(); },
                                                 resetSplinePointsButton, 45, 5, 60, 60));
        resetContainer->addChild(new TextAtom("Reset Points", uiFont12, textcolor, POS_TOP, 0, 65, 150, 65));
        resetContainer->makeInvisible();
        resetPotentialIndex = addIndexedChild(resetContainer);
        
        makeInvisible();
        getChild(splineContainerIndex)->makeVisible();
    }
    
    void PotentialContainer::setVisible(bool vis) {
        visible = vis;
        for (int i = 0; i < children.size(); ++i) { children[i]->setVisible(vis); }
        
        if (vis && theSystem.getPotential().getType() == CUSTOM) {
            getChild(customPotentialIndex)->makeInvisible();
        } else {
            getChild(resetPotentialIndex)->makeInvisible();
        }
    }
    
    
    void PotentialContainer::setPotential(Potential potential) {
        theSystem.setPotential(potential);
        
        switch (potential) {
        default:
        case LENNARD_JONES:
            getChild(highlightAtomIndex)->moveTo(bounds.left, bounds.top);
            getChild(splineContainerIndex)->makeInvisible();
            getChild(customPotentialIndex)->makeVisible();
            getChild(resetPotentialIndex)->makeInvisible();
            break;
        case SQUARE_WELL:
            getChild(highlightAtomIndex)->moveTo(bounds.left, bounds.top + bounds.height() / 4);
            getChild(splineContainerIndex)->makeInvisible();
            getChild(customPotentialIndex)->makeVisible();
            getChild(resetPotentialIndex)->makeInvisible();
            break;
        case MORSE:
            getChild(highlightAtomIndex)->moveTo(bounds.left, bounds.top + bounds.height() / 2);
            getChild(splineContainerIndex)->makeInvisible();
            getChild(customPotentialIndex)->makeVisible();
            getChild(resetPotentialIndex)->makeInvisible();
            break;
        case CUSTOM:
            getChild(highlightAtomIndex)->moveTo(bounds.left, bounds.top + 3 * bounds.height() / 4);
            getChild(splineContainerIndex)->makeVisible();
            getChild(customPotentialIndex)->makeInvisible();
            getChild(resetPotentialIndex)->makeVisible();
            break;
        }
    }
    
    /*
        PotentialAtom
     */
    
    PotentialAtom::PotentialAtom(md::MDContainer &system, int _numPoints, double min_x, double max_x, double min_y, double max_y, int x, int y, int width, int height): theSystem(system), numPoints(_numPoints), UIAtom(x, y, width, height)
    {
        potBounds.setLRTB(min_x, max_x, max_y, min_y);
        numBins = 200;
        numPrevRDF = numBins / 10;
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
            pos = util::bimap(pos, potBounds, bounds);
            particlePoints.push_back(pos);
        }
        
        // put all positions along potential curve into potPoints
        for (int i = 0; i < numPoints; i++){
            pos.x = potBounds.left + i * x_spacing;
            pos.y = pot.potential(pos.x);
            
            // map to dimensions of UI element
            pos = util::bimap(pos, potBounds, bounds);
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
        
        //// Plot the particles along the curve
        //ofSetCircleResolution(10);
        //ofSetColor(186, 255, 163, 220);
        //for (int i = 0; i < particlePoints.size(); i++){
        //    pos = particlePoints[i];
        //    if (bounds.inside(pos)) { ofDrawCircle(pos.x, pos.y, 6); }
        //}
        
        // Plot the RDF
        rect RDFspace;
        RDFspace.setLRTB(0, numBins, 5.0 / numBins, 0);
        prevRDF.push_back(theSystem.rdf(potBounds.left, potBounds.right, numBins));
        while (prevRDF.size() > numPrevRDF) {
            prevRDF.pop_front();
        }
        
        ofSetColor(186, 255, 163, 80);
        
        ofMesh violin;
        violin.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
        
        rect violinSpaceUpper, violinSpaceLower;
        violinSpaceUpper.setLRTB(bounds.left, bounds.right, bounds.top,    bounds.centreY());
        violinSpaceLower.setLRTB(bounds.left, bounds.right, bounds.bottom, bounds.centreY());
        
        for (int i = 0; i < numBins; ++i) {
            double sum = 0.0;
            for (int j = 0; j < prevRDF.size(); ++j) {
                sum += prevRDF[j][i];
            }
            coord point = {(double)i, sum / prevRDF.size()};
            coord pointUpper = util::bimap(point, RDFspace, violinSpaceUpper);
            coord pointLower = util::bimap(point, RDFspace, violinSpaceLower);
            
            //ofDrawCircle(pointUpper.x, pointUpper.y, 2);
            //ofDrawCircle(pointLower.x, pointLower.y, 2);
            
            violin.addVertex(ofPoint(pointUpper.x, pointUpper.y, 0));
            violin.addVertex(ofPoint(pointLower.x, pointLower.y, 0));
            //violin.addIndex(2 * i);
            //violin.addIndex(2 * i + 1);
        }
        //violin.addIndex(2 * numBins);
        //violin.addIndex(2 * numBins + 1);
        violin.draw();
        
        glDisable(GL_SCISSOR_TEST);
    }
    
    /*
        SplineControlPoint
     */
    
    SplineControlPoint::SplineControlPoint(int x, int y, double _radius, rect _pointBounds) : m(0), radius(_radius), pointBounds(_pointBounds), mouseFocus(false), UIAtom(x - 2*_radius, y - 2*_radius, 4 * _radius, 4 * _radius)
    {}
    
    void SplineControlPoint::render() {
        ofSetCircleResolution(20);
        
        ofSetColor(10, 174, 199);
        ofDrawCircle(bounds.centreX(), bounds.centreY(), radius);
        
        ofSetColor(82, 225, 247);
        ofDrawCircle(bounds.centreX(), bounds.centreY(), radius - 3);
    }
    
    void SplineControlPoint::movePoint(double x, double y) {
        coord target = coord(x, y);
        target = util::biclamp(target, pointBounds);
        bounds.movePos(POS_CENTRE, target);
    }
    
    bool SplineControlPoint::mousePressed(int x, int y, int button) {
        bool handled = false;
        if (bounds.inside(x, y)) {
            switch (button) {
                    
                case 0:   // left click, start being dragged
                    mouseFocus = true;
                    handled = true;
                    break;
                    
                case 2:   // right click, delete point
                    // do nothing (other than return true)
                    handled = true;
                    break;
                    
                case 3:   // left button, decrease slope
                    m -= 0.4;
                    if (m < -4) { m = -4; }
                    handled = true;
                    break;
                    
                case 4:   // right button, increase slope
                    m += 0.4;
                    if (m > 4) { m = 4; }
                    handled = true;
                    break;
                    
                default:
                    handled = false;
            }
        }
        
        return handled;
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
        }
        return mouseFocus; // Nothing done if false, something done if true
    }
    
    
    
    /*
        SplineContainer
     */
    
    SplineContainer::SplineContainer(md::MDContainer &_system, double min_x, double max_x, double min_y, double max_y, double _radius, double x, double y, double width, double height) : system(_system), radius(_radius), UIContainer(x, y, width, height)
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
            
            pos = util::bimap(pos, bounds, splineRegion);
            points.push_back({pos.x, pos.y, m});
        }
        
        system.getCustomPotential().updatePoints(points);
    }
    
    // return true if there is a control point with x-coordinate close to the given x
    // optionally can exclude the point with index except from the check
    bool SplineContainer::controlPointNear(double x, int except) {
        for (int i = 0; i < children.size(); ++i) {
            if (i == except) { continue; }
            if (abs(x - children[i]->getRect().centreX()) < 2*radius) { return true; }
        }
        return false;
    }
    
    // handle mouse click events
    bool SplineContainer::mousePressed(int x, int y, int button) {
        
        bool handled = false;
        
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
                            handled = true;
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
                            handled = true;
                        } else {
                            // only return here if we haven't clicked on a child, and if we couldn't
                            // create a new point at the mouse position
                            handled = false;
                        }
                    }
                
                    break;
                }
                    
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
                    if ( hitChild ) { handled = true; }
                    break;
                }
                    
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
                    if (hitChild) { handled = true; }
                    break;
                }
                    
                default:
                    handled = false;
            }
        }
        
        if (handled) { updateSpline(); }
        
        return handled;
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
    
    // Override draw so that it only draws if the custom potential is selected
    void SplineContainer::draw() {
        if ( system.getPotential().getType() == CUSTOM ) {
            UIContainer::draw();
        }
    }
    
    // Override resize so that the pointRegion is updated
    void SplineContainer::resize(float xScale, float yScale) {
        UIContainer::resize(xScale, yScale);
        pointRegion.setXYWH(pointRegion.left*xScale, pointRegion.top*yScale, pointRegion.width()*xScale, pointRegion.height()*yScale);
    }
    
    void SplineContainer::destroyAllPoints() {
        for (int i = 0; i < children.size(); ++i) {
            delete children[i];
        }
        children.clear();
        updateSpline();
    }
}
