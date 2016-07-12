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

// implements SystemAtom, EnergyGraphAtom, GaussianAtom, GaussianContainer

namespace gui {
    
    /*
        SystemAtom
     */
    
    SystemAtom::SystemAtom(md::MDContainer& _theSystem, ofImage& _loganLeft, ofImage& _loganRight, int x, int y, int width, int height) :
                            theSystem(_theSystem), loganLeft(_loganLeft), loganRight(_loganRight), inflictTorture(false),
                            UIAtom(x, y, width, height)
    { }
    
    void SystemAtom::render() {
        // Setup temporary placeholders
        ofColor particleColor;
        coord tempVel;
        coord tempAcc;
        
        double radius_x;
        double radius_y;
        double radius;
        double hue;
        
        double loganShiftx = loganLeft.getWidth() / 2;
        double loganShifty = loganLeft.getHeight() / 2;
        
        double v_avg = theSystem.getVAvg(); // Get average velocity for scaling purposes
        ofSetCircleResolution(20);
        
        // Draw all the particles and trails
        for (int i = 0; i < theSystem.getN(); ++i) {
            tempVel = theSystem.getVel(i);
            tempAcc = theSystem.getForce(i);
            
            hue = ofMap(abs(tempVel.x) + abs(tempVel.y), 0, 3 * v_avg, 170, 210, true);
            particleColor.setHsb(hue, 255, 255);
            
            radius_x = ofMap(log(1.0 + abs(tempAcc.x)), 0, 10, 10, 25);
            radius_y = ofMap(log(1.0 + abs(tempAcc.y)), 0, 10, 10, 25);
            radius = (radius_x + radius_y) / 2;
            
            if (inflictTorture) {
                ofSetColor(particleColor);
                coord pos = theSystem.getPos(i);
                
                if (tempVel.x >= 0)
                    loganRight.draw(box2screen(pos.x, pos.y, loganShiftx, loganShifty, ofGetWidth(), ofGetHeight(), theSystem.getWidth(), theSystem.getHeight()), radius_x * 4, radius_y * 4);
                else
                    loganLeft.draw( box2screen(pos.x, pos.y, loganShiftx, loganShifty, ofGetWidth(), ofGetHeight(), theSystem.getWidth(), theSystem.getHeight()), radius_x * 4, radius_y * 4);
            } else {
                //trail
                if (theSystem.getNPrevPos() >= 15) {
                    particleColor.a = 100;
                    drawParticle(i, radius * 0.25, particleColor, 14);
                }
                if (theSystem.getNPrevPos() >= 10) {
                    particleColor.a = 150;
                    drawParticle(i, radius * 0.5,  particleColor, 9);
                }
                if (theSystem.getNPrevPos() >= 5) {
                    particleColor.a = 200;
                    drawParticle(i, radius * 0.75, particleColor, 4);
                }
                
                //particle
                particleColor.a = 255;
                drawParticle(i, radius_x, radius_y, particleColor);
            }
        }

    }
    
    /*
     ROUTINE drawParticle:
     Draws a particle, specified by index and a size given either as x and y radii (ellipse)
     or by a single constant radius (circle).
     Optional: set the colour before drawing
     Optional: draws the particle with position nframes frames in the past
     */
    void SystemAtom::drawParticle(int index, double radius_x, double radius_y, ofColor color, int nframes) {
        ofSetColor(color);
        drawParticle(index, radius_x, radius_y, nframes);
    }
    
    void SystemAtom::drawParticle(int index, double radius, ofColor color, int nframes) {
        ofSetColor(color);
        drawParticle(index, radius, nframes);
    }
    
    void SystemAtom::drawParticle(int index, double radius_x, double radius_y, int nframes) {
        coord pos = theSystem.getPos(index, nframes);
        ofDrawEllipse(box2screen(pos.x, pos.y, 0.0, 0.0, ofGetWidth(), ofGetHeight(), theSystem.getWidth(), theSystem.getHeight()), radius_x * 2, radius_y * 2);
    }
    
    void SystemAtom::drawParticle(int index, double radius, int nframes) {
        coord pos = theSystem.getPos(index, nframes);
        ofDrawCircle(box2screen(pos.x, pos.y, 0.0, 0.0, ofGetWidth(), ofGetHeight(), theSystem.getWidth(), theSystem.getHeight()), radius);
    }

    void SystemAtom::toggleTheHorrors() {
        inflictTorture = !inflictTorture;
    }
    
    /*
        EnergyGraphAtom
     */
    
    EnergyGraphAtom::EnergyGraphAtom(md::MDContainer& _theSystem, int x, int y, int width, int height) : theSystem(_theSystem), UIAtom(x, y, width, height) {}
    
    void EnergyGraphAtom::render() {
        /*
         Draws the kinetic/potential energy graphs as small red/blue circles
         in the background.
         
         This rescales the values for the graphs using minEKin/Pot and maxEKin/Pot
         as the minimum/maximum values respectively.
         */
        
        float winLeft = ofGetWidth()/6;
        float winTop = ofGetHeight()/6;
        float winWidth = 2*ofGetWidth()/3;
        float winHeight = 2*ofGetHeight()/3;
        float xOffset = 1.1*winLeft;
        float yOffset = 7*winHeight/6;
        float ekinMaxScale = theSystem.getMaxEkin();
        float ekinMinScale = theSystem.getMinEkin();
        float epotMaxScale = theSystem.getMaxEpot();
        float epotMinScale = theSystem.getMinEpot();
        
        // Draw graph
        float radius = 3;
        float ekin, epot;
        
        // Set fill and resolution
        ofFill();
        ofSetCircleResolution(10);
        
        // Loop over all data points stored in the previous energy arrays in theSystem
        // and draw them as small circles.
        for (int i = 0; i < theSystem.getNEnergies(); i++){
            ofSetColor(200, 0, 0);
            ekin = ofMap(theSystem.getPreviousEkin(i), ekinMinScale, ekinMaxScale, 0, 0.9*winHeight);
            ofDrawCircle(xOffset + 5*i, yOffset - ekin, radius);
            
            ofSetColor(255, 255, 255);
            epot = ofMap(fabs(theSystem.getPreviousEpot(i)), epotMinScale, epotMaxScale, 0, 0.9*winHeight);
            ofDrawCircle(xOffset + 5*i, yOffset - epot, radius);
        }
    }
    
    /*
        GaussianAtom
     */
    GaussianAtom::GaussianAtom(md::MDContainer& _theSystem, ofImage& _circGradient, int _gaussianID, int x, int y, double _radius) : theSystem(_theSystem), circGradient(_circGradient), gaussianID(_gaussianID), selected(false), radius(_radius), mouseFocus(false),
        UIAtom(x - _radius, y - _radius, 2*_radius, 2*_radius)
    {}

    void GaussianAtom::render() {
        /*
         Takes a reference to a Gaussian object and whether this Gaussian is the one
         currently in focus (selected), and draws the Gaussian as a circular gradient
         texture, coloured based on the amplitude of the Gaussian
         */
        
        Gaussian& g = theSystem.getGaussian(gaussianID);
        
        double gA = g.getgAmp();       // amplitude
        double galpha = g.getgAlpha(); // inverse width
        // Centre
        double gx = g.getgex0();
        double gy = g.getgey0();
        
        // Rescale between box size and window size
        double xscale = ofGetWidth() / theSystem.getWidth();
        double yscale = ofGetHeight() / theSystem.getHeight();
        
        // Determine the colour of the Gaussian, based on the amplitude
        ofColor color;
        u_char hue = 200;
        u_char saturation = gA > 0 ? 255 : 0;
        u_char brightness = 180;
        
        // Brighten if selected
        if (selected) {
            brightness = 255;
        }
        
        color.setHsb(hue, saturation, brightness);
        color.a = ofMap(abs(gA), 0, 50, 100, 255);
        ofSetColor(color);
        
        // Rescale the width and height of the Gaussian
        double scaleFactor = 2.5;
        double width  = scaleFactor * xscale / galpha;
        double height = scaleFactor * yscale / galpha;
        
        double x = gx * xscale - width / 2;
        double y = gy * yscale - height / 2;
        
        // Draw as circGradient
        circGradient.draw(x, y, width, height);
    }
    
    void GaussianAtom::moveGaussian(double x, double y) {
        bounds.movePos(POS_CENTRE, {x, y});
        
        // Rescale the (x, y) coordinates of the mouse input so that they
        // are within the dimensions of the box
        double scaled_x = x * theSystem.getWidth()/ofGetWidth();
        double scaled_y = y * theSystem.getHeight()/ofGetHeight();
        
        // Update position of Gaussian
        Gaussian& g = theSystem.getGaussian(gaussianID);
        g.setParams(g.getgAmp(), g.getgAlpha(), scaled_x, scaled_y);
    }
    
    void GaussianAtom::deselect() {
        selected = false;
    }
    
    void GaussianAtom::select() {
        selected = true;
    }
    
    void GaussianAtom::updateID(int _id) {
        gaussianID = _id < gaussianID ? (gaussianID - 1) : gaussianID;
    }
    
    bool GaussianAtom::mousePressed(int x, int y, int button) {
        bool retVal = false;
        if (bounds.inside(x, y)) {
            switch(button) {
                    
                case 0: // Left click, get focus
                    mouseFocus = true;
                    selected = true;
                    retVal = true;
                    break;
                    
                case 2: // Right click, delete point
                    retVal = true;
                    break;
                    
                default:
                    retVal = false;
                    
            }
        }
        return retVal;
    }
    
    // Lose focus if left mouse button released
    bool GaussianAtom::mouseReleased(int x, int y, int button) {
        if ( button == 0 ) {
            mouseFocus = false;
        }
        return false;
    }
    
    // Move the Gaussian if it has focus
    bool GaussianAtom::mouseMoved(int x, int y) {
        bool retVal = false;
        if ( mouseFocus ) {
            moveGaussian(x, y);
            retVal = true;
        }
        return retVal;
    }
    
    /* 
        GaussianContainer 
     */
    
    GaussianContainer::GaussianContainer(md::MDContainer& _system, ofImage& _circGradient, double _radius, double x, double y, double width, double height) : system(_system), circGradient(_circGradient), radius(_radius), selectedGaussian(-1), UIContainer(x, y, width, height)
    { }
    
    // Work out if there is already a Gaussian near (x, y) to avoid putting them on top of one another
    bool GaussianContainer::gaussianNear(double x, double y, int index) {
        bool retVal = false;
        for (int i = 0; i < children.size(); i++) {
            if ( i == index ) { continue; }
            
            double xdist = fabs(x - children[i]->getRect().centreX());
            double ydist = fabs(y - children[i]->getRect().centreY());
            
            if( (xdist < 1.2*radius) && (ydist < 1.2*radius) ) {
                retVal = true;
            }
        }
        return retVal;
    }
    
    // Handle mouse click events
    bool GaussianContainer::mousePressed(int x, int y, int button) {
        bool retVal = true;
        
        if (visible && bounds.inside(x, y)) {
            switch (button) {
                    // left click: set mouseFocus to true or create new Gaussian
                case 0: {
                    
                    bool hitChild = false;
                    
                    // loop through backwards so that the point drawn on top is clicked first (which,
                    // since they are drawn in forward-order, is the last child)
                    for (int i = children.size() - 1; i >= 0; --i) {
                        
                        GaussianAtom* g = (GaussianAtom*) children[i];
                        hitChild = g->mousePressed(x, y, 0);
                        if (hitChild) {
                            selectedGaussian = i;
                            deselectGaussians(i);
                        }
                    
                    }
                    
                    // If we haven't clicked on a child, then make a new Gaussian, if it won't overlap with another
                    if (!hitChild) {
                        if (!gaussianNear(x, y)) {
                            // Make a new Gaussian
                            if (system.getNGaussians() == 6) {
                                // Delete the first child to make room for new one
                                delete children[0];
                                children.erase(children.begin());
                                updateGaussianIDs(0);
                            }
                            // Rescale the (x, y) coordinates of the mouse input so that they
                            // are within the dimensions of the box
                            double scaled_x = x * system.getWidth()/ofGetWidth();
                            double scaled_y = y * system.getHeight()/ofGetHeight();
                            // Add the new Gaussian to the system
                            system.addGaussian(scaled_x, scaled_y);
                            
                            // use push_back, not addChild, so that we make the child directly at (x, y),
                            // instead of at (x, y) relative to the top-left corner of the container
                            children.push_back(new GaussianAtom(system, circGradient, system.getNGaussians() - 1, x, y, radius));
                            children.back()->mousePressed(x, y, 0);
                            
                            selectedGaussian = system.getNGaussians() - 1;
                            deselectGaussians(system.getNGaussians() - 1);
                        } else {
                            retVal = false;
                        }
    
                    }
                } break;
                    
                // Right click to remove Gaussian
                case 2: {
                    bool hitChild = false;
                    for (int i = 0; i < children.size(); ++i) {
                        hitChild = children[i]->mousePressed(x, y, 2);
                        
                        if (hitChild) {
                            // free the memory, then delete the pointer
                            delete children[i];
                            children.erase(children.begin() + i);
                            updateGaussianIDs(i);
                            
                            // remove the Gaussian from the system
                            system.removeGaussian(i);
                            
                            if (selectedGaussian > 0) { selectedGaussian--; }
                            else if (system.getNGaussians() == 0) { selectedGaussian = -1; }
                            selectGaussian(selectedGaussian);
                        }
                    }
                    if (!hitChild) { retVal = false; }
                    break;
                }
                    
                default:
                    retVal = false;
            }
        } else {
            retVal = false;
        }
        
        return retVal;
    }
    
    bool GaussianContainer::mouseMoved(int x, int y) {
        bool handled = false;
        
        for (int i = 0; i < children.size(); ++i) {
            
            // test to avoid moving one Gaussian atop another
            if (!gaussianNear(x, y, i)) {
                handled = children[i]->mouseMoved(x, y);
            }
        }
        
        return handled;
    }
    
    void GaussianContainer::audioIn(double volume) {
        if ( selectedGaussian > -1) {
            system.updateGaussian(selectedGaussian, 50 - volume*100, 0.8 - 0.5*volume,
                                     system.getGaussianX0(selectedGaussian),
                                     system.getGaussianY0(selectedGaussian));
        }
    }
    
    void GaussianContainer::updateGaussianIDs(int deletedID) {
        for (int i = 0; i < children.size(); i++) {
            
            GaussianAtom* g = (GaussianAtom*) children[i];
            g->updateID(deletedID);
            
        }
    }
    
    void GaussianContainer::deselectGaussians(int id) {
        for (int i = 0; i < children.size(); i++) {
            
            if ( i != id ) {
                GaussianAtom* g = (GaussianAtom*) children[i];
                g->deselect();
            }
            
        }
    }
    
    void GaussianContainer::selectGaussian(int id) {
        if (id > -1) {
            GaussianAtom* g = (GaussianAtom*) children[id];
            g->select();
        }
    }

    

}