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
#include <cmath>

// implements SystemAtom, EnergyGraphAtom, GaussianAtom, GaussianContainer

namespace gui {
    
    /*
        SystemAtom
     */
    
    SystemAtom::SystemAtom(md::MDContainer& _theSystem, ArgonImage& _loganLeft, ArgonImage& _loganRight, ArgonImage& _boatLeft, ArgonImage& _boatRight, int x, int y, int width, int height) :
                            theSystem(_theSystem), loganLeft(_loganLeft), loganRight(_loganRight), boatLeft(_boatLeft), boatRight(_boatRight), inflictTorture(false), setSail(false),
                            UIAtom(x, y, width, height)
    { }
    
    void SystemAtom::render() {
        // Setup temporary placeholders
        colour particleColor;
        coord tempVel;
        coord tempAcc;
        
        double radius_x;
        double radius_y;
        double radius;
        double hue;
        
        double loganShiftx = loganLeft.getWidth() / 2;
        double loganShifty = loganLeft.getHeight() / 2;
                
        ArgonImage* leftImage = inflictTorture ? &loganLeft : &boatLeft;
        ArgonImage* rightImage = inflictTorture ? &loganRight : &boatRight;
        
        double v_avg = theSystem.getVAvg(); // Get average velocity for scaling purposes
        ofSetCircleResolution(20);
        
        // Draw all the particles and trails
        for (int i = 0; i < theSystem.getN(); ++i) {
            tempVel = theSystem.getVel(i);
            tempAcc = theSystem.getForce(i);
            
            hue = ofMap(abs(tempVel.x) + abs(tempVel.y), 0, 3 * v_avg, 170, 210, true);
            particleColor.setHSB(hue, 255, 255);
            
            radius_x = ofMap(log(1.0 + abs(tempAcc.x)), 0, 10, 10, 25);
            radius_y = ofMap(log(1.0 + abs(tempAcc.y)), 0, 10, 10, 25);
            radius = (radius_x + radius_y) / 2;
            
            if (inflictTorture || setSail) {
                //ofSetColor(particleColor);
                coord screenpos = util::bimap(theSystem.getPos(i), theSystem.getBox(), windowSize());
                rect drawpos;
                drawpos.setXYWH(screenpos.x - loganShiftx, screenpos.y - loganShifty, radius_x * 4, radius_y * 4);
                if (tempVel.x >= 0)
                    //rightImage->draw(box2screen(pos.x, pos.y, loganShiftx, loganShifty, ofGetWidth(), ofGetHeight(), theSystem.getWidth(), theSystem.getHeight()), radius_x * 4, radius_y * 4);
                    rightImage->draw(drawpos);
                else
                    //leftImage->draw( box2screen(pos.x, pos.y, loganShiftx, loganShifty, ofGetWidth(), ofGetHeight(), theSystem.getWidth(), theSystem.getHeight()), radius_x * 4, radius_y * 4);
                    leftImage->draw(drawpos);
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
    void SystemAtom::drawParticle(int index, double radius_x, double radius_y, colour colour, int nframes, int resolution) {
        coord screenpos = util::bimap(theSystem.getPos(index, nframes), theSystem.getBox(), windowSize());
        drawEllipse(screenpos.x, screenpos.y, radius_x * 2, radius_y * 2, colour, resolution);

    }
    
    void SystemAtom::drawParticle(int index, double radius, colour colour, int nframes, int resolution) {
        coord screenpos = util::bimap(theSystem.getPos(index, nframes), theSystem.getBox(), windowSize());
        drawCircle(screenpos.x, screenpos.y, radius, colour, resolution);
    }
    /*
    void SystemAtom::drawParticle(int index, double radius_x, double radius_y, int nframes) {
        coord screenpos = util::bimap(theSystem.getPos(index, nframes), theSystem.getBox(), windowSize());
        ofDrawEllipse(screenpos.x, screenpos.y, radius_x * 2, radius_y * 2);
        //ofDrawEllipse(box2screen(pos.x, pos.y, 0.0, 0.0, ofGetWidth(), ofGetHeight(), theSystem.getWidth(), theSystem.getHeight()), radius_x * 2, radius_y * 2);
    }
    
    void SystemAtom::drawParticle(int index, double radius, int nframes) {
        coord screenpos = util::bimap(theSystem.getPos(index, nframes), theSystem.getBox(), windowSize());
        ofDrawCircle(screenpos.x, screenpos.y, radius);
        //ofDrawCircle(box2screen(pos.x, pos.y, 0.0, 0.0, ofGetWidth(), ofGetHeight(), theSystem.getWidth(), theSystem.getHeight()), radius);
    }
    */
    void SystemAtom::toggleTheHorrors() {
        inflictTorture = !inflictTorture;
        setSail = false;
    }
    
    void SystemAtom::sailTheHighSeas() {
        setSail = !setSail;
        inflictTorture = false;
    }
    
    /*
        EnergyGraphAtom
     */
    
    EnergyGraphAtom::EnergyGraphAtom(md::MDContainer& _theSystem, int x, int y, int width, int height) : theSystem(_theSystem), UIAtom(x, y, width, height) {}
    
    void EnergyGraphAtom::render() {
        /*
         Draws the kinetic/potential energy graphs.
         
         This rescales the values for the graphs using minEKin/Pot and maxEKin/Pot
         as the minimum/maximum values respectively.
         */
        
        int numPoints = theSystem.getNEnergies();
        
        // max and min of potential and kinetic energies
        double top    = std::max(theSystem.getMaxEkin(), theSystem.getMaxEpot());
        double bottom = std::min(theSystem.getMinEkin(), theSystem.getMinEpot());
        
        // ensure that zero is drawn
        top    = top    > 0 ? top : 0;
        bottom = bottom < 0 ? bottom : 0;
        
        // make sure height is not zero
        if (top - bottom < 1e-5) {
            top = bottom + 1e-5;
        }
        
        rect energySpace;
        energySpace.setLRTB(0, 119, top, bottom); // 119 is max number of energy points - 1
        
        coord point;
        ofPolyline Ekin, Epot;
        for (int i = 0; i < numPoints; ++i) {
            point = {(double)i, theSystem.getPreviousEkin(i)};
            point = util::bimap(point, energySpace, bounds);
            Ekin.addVertex(point.x, point.y);
            
            ofSetColor(255, 255, 255);
            point = {(double)i, theSystem.getPreviousEpot(i)};
            point = util::bimap(point, energySpace, bounds);
            Epot.addVertex(point.x, point.y);
        }
       
        // draw tick lines
        // the log_2 scaling means that, if too many are drawn, it removes every second line
        // dividing by 7 means it draws between 4 and 8 lines
        double tickSpacing = pow(2, ceil(log(top - bottom) / log(2))) / 7.0;
        double yEnergy = ceil(bottom / tickSpacing) * tickSpacing;
        double yScreen;
        
        ofSetLineWidth(1);
        ofSetColor(60, 60, 60);
        while (true) {
            yScreen = round(ofMap(yEnergy, energySpace.bottom, energySpace.top, bounds.bottom, bounds.top));
            ofDrawLine(bounds.left, yScreen, bounds.right, yScreen);
            yEnergy += tickSpacing;
            if (yEnergy > energySpace.top) { break; }
        }
        
        // plot energies
        ofSetLineWidth(2);
        ofSetColor(200, 0, 0);
        Ekin.draw();
        ofSetColor(255, 255, 255);
        Epot.draw();
    }
    
    /*
        MaxwellGraphAtom
     */
    
    MaxwellGraphAtom::MaxwellGraphAtom(md::MDContainer& _theSystem, int x, int y, int width, int height) : theSystem(_theSystem), UIAtom(x, y, width, height), numBins(100), numPrevMB(40), maxHeight(0.1), prevMB() {}
    
    void MaxwellGraphAtom::render() {
        /*
         Draws the Maxwell-Boltzmann distribution.
         */
        
        double maxSpeed = 10;
        
        rect maxwellSpace;
        maxwellSpace.setLRTB(0, numBins, maxHeight, 0);
        
        prevMB.push_back(theSystem.maxwell(0, maxSpeed, numBins));
        while (prevMB.size() > numPrevMB) {
            prevMB.pop_front();
        }
        
        coord point;
        ofPolyline MBcurve;
        MBcurve.addVertex(bounds.left, bounds.bottom);
        double currMaxHeight = 0.0;
        
        for (int i = 0; i < numBins; ++i) {
            double sum = 0.0;
            for (int j = 0; j < prevMB.size(); ++j) {
                sum += prevMB[j][i];
            }
            sum /= prevMB.size();
            
            point = {(double)i + 0.5, sum};
            point = util::bimap(point, maxwellSpace, bounds);
            MBcurve.addVertex(point.x, point.y);
            
            currMaxHeight = sum > currMaxHeight ? sum : currMaxHeight;
        }
        
        maxHeight = currMaxHeight > 0.1 ? currMaxHeight : 0.1;
        
        glScissor(bounds.left, ofGetHeight() - 1 - bounds.bottom, bounds.width(), bounds.height() + 2);
        glEnable(GL_SCISSOR_TEST);
        
        // draw tick lines
        ofSetLineWidth(1);
        ofSetColor(60, 60, 60);
        for (int i = 0; i < 5; ++i) {
            int height = bounds.top + bounds.height() * i / 4;
            ofDrawLine(bounds.left, height, bounds.right, height);
        }
        
        // plot energies
        ofSetLineWidth(2);
        ofSetColor(255, 255, 255);
        MBcurve.draw();
        
        glDisable(GL_SCISSOR_TEST);
    }
    
    
    /*
        GaussianAtom
     */
    GaussianAtom::GaussianAtom(md::MDContainer& _theSystem, ArgonImage& _circGradient, int _gaussianID, ArgonFont* uiFont10, ArgonImage* closeButton, ArgonImage* audioOnButton, ArgonImage* audioOffButton, int x, int y, double _radius) : theSystem(_theSystem), circGradient(_circGradient), gaussianID(_gaussianID), selected(false), radius(_radius), mouseFocus(false), audioOn(true),
        UIAtom(x - _radius, y - _radius, 2*_radius, 2*_radius)
    { }

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
        unsigned char hue = 200;
        unsigned char saturation = gA > 0 ? 255 : 0;
        unsigned char brightness = 180;
        
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
        double height = width;
        
        double x = gx * xscale - width / 2;
        double y = gy * yscale - height / 2;
        
        // Draw as circGradient
        circGradient.draw(x, y, width, height);
        
    }
    
    void GaussianAtom::moveGaussian(double x, double y) {
        
        // Rescale the (x, y) coordinates of the mouse input so that they
        // are within the dimensions of the box
        double scaled_x = x * theSystem.getWidth()/ofGetWidth();
        double scaled_y = y * theSystem.getHeight()/ofGetHeight();
        
        // Update position of Gaussian
        Gaussian& g = theSystem.getGaussian(gaussianID);
        g.setParams(g.getgAmp(), g.getgAlpha(), scaled_x, scaled_y);
        
        bounds.movePos(POS_CENTRE, coord(x, y));
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
                    select();
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
            // if trying to move it off-screen, move it to the screen edge
            x = x < 0 ? 0 : x;
            x = x > ofGetWidth() ? ofGetWidth() : x;
            y = y < 0 ? 0 : y;
            y = y > ofGetHeight() ? ofGetHeight() : y;
            
            moveGaussian(x, y);
            retVal = true;
        }
        return retVal;
    }
    
    // If audio control is turned on, rescale Gaussian with audio input
    void GaussianAtom::audioIn(double volume) {
        if (audioOn) {
            theSystem.updateGaussian(gaussianID, 50 - volume*100, 0.8 - 0.5*volume,
                                  theSystem.getGaussianX0(gaussianID),
                                  theSystem.getGaussianY0(gaussianID));
        }
    }
    
    // Resize control panel when GaussianAtom resizes
    void GaussianAtom::resize(float xScale, float yScale) {
        // Make sure Gaussians stay circular
        float scale = xScale < yScale ? xScale : yScale;
        bounds.setXYWH(bounds.left*xScale, bounds.top*yScale, bounds.width()*scale, bounds.height()*scale);
    }
    
    /* 
        GaussianContainer 
     */
    
    GaussianContainer::GaussianContainer(md::MDContainer& _system, ArgonImage& _circGradient, ArgonFont* _uiFont10, ArgonImage* _closeButton, ArgonImage* _audioOnButton, ArgonImage* _audioOffButton, double _radius, double x, double y, double width, double height) : system(_system), circGradient(_circGradient), uiFont10(_uiFont10), closeButton(_closeButton), audioOnButton(_audioOnButton), audioOffButton(_audioOffButton), radius(_radius), selectedGaussian(-1), UIContainer(x, y, width, height)
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
                    
                    // Loop through children
                    for (int i = 0; i < children.size(); ++i) {
                        
                        GaussianAtom* g = (GaussianAtom*) children[i];
                        hitChild = g->mousePressed(x, y, 0);
                        if (hitChild) {
                            selectedGaussian = i;
                            deselectGaussians(i);
                            break;
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
                            children.push_back(new GaussianAtom(system, circGradient, system.getNGaussians() - 1, uiFont10, closeButton, audioOnButton, audioOffButton, x, y, radius));
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
                            
                            break;
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
            // but still react to control panel
            if (!gaussianNear(x, y, i) || children[i]->getRect().inside(x, y) ) {
                handled = children[i]->mouseMoved(x, y);
            }
        }
        
        return handled;
    }
    
    // Send an audioIn event to all children
    void GaussianContainer::audioIn(double volume) {
        for (int i = 0; i < children.size(); ++i) {
            children[i]->audioIn(volume);
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
    
    int GaussianContainer::getSelectedID() const {
        return selectedGaussian;
    }
    
    void GaussianContainer::destroyAllGaussians() {
        // free all the memory and clear the vector
        for (int i = 0; i < children.size(); ++i) {
            delete children[i];
        }
        
        // Has to be done in reverse order
        for (int i = system.getNGaussians()-1; i > -1; --i) {
            system.removeGaussian(i);
        }
       
        children.clear();
        selectedGaussian = -1;
    }

}
