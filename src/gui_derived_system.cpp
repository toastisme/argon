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

// implements GaussianAtom, GaussianContainer

namespace gui {
    
    /* 
        GaussianAtom
     */
    GaussianAtom::GaussianAtom(Gaussian& _g, md::MDContainer& _theSystem, ofImage& _circGradient, int x, int y, double _radius) : g(_g), theSystem(_theSystem), circGradient(_circGradient), radius(_radius), mouseFocus(false),
        UIAtom(x - _radius, y - _radius, 2*_radius, 2*_radius)
    {}

    void GaussianAtom::render() {
        /*
         Takes a reference to a Gaussian object and whether this Gaussian is the one
         currently in focus (selected), and draws the Gaussian as a circular gradient
         texture, coloured based on the amplitude of the Gaussian
         */
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
        if (mouseFocus) {
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
        g.setParams(g.getgAmp(), g.getgAlpha(), scaled_x, scaled_y);
    }
    
    bool GaussianAtom::mousePressed(int x, int y, int button) {
        bool retVal = false;
        if (bounds.inside(x, y)) {
            switch(button) {
                    
                case 0: // Left click, get focus
                    mouseFocus = true;
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
            else if( (x - children[i]->getRect().centreX() < 1.5*radius) && (y - children[i]->getRect().centreY() < 1.5*radius) ) {
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

                        hitChild = children[i]->mousePressed(x, y, 0);
                        if (hitChild) { selectedGaussian = i; }
                    
                    }
                    
                    // If we haven't clicked on a child, then make a new Gaussian, if it won't overlap with another
                    if (!hitChild) {
                        if (!gaussianNear(x, y)) {
                            // Make a new Gaussian
                            if (system.getNGaussians() == 4) {
                                // Delete the first child to make room for new one
                                delete children[0];
                                children.erase(children.begin());
                            }
                            // Rescale the (x, y) coordinates of the mouse input so that they
                            // are within the dimensions of the box
                            double scaled_x = x * system.getWidth()/ofGetWidth();
                            double scaled_y = y * system.getHeight()/ofGetHeight();
                            // Add the new Gaussian to the system
                            system.addGaussian(scaled_x, scaled_y);
                            
                            // use push_back, not addChild, so that we make the child directly at (x, y),
                            // instead of at (x, y) relative to the top-left corner of the container
                            children.push_back(new GaussianAtom(system.getGaussian(system.getNGaussians() - 1), system, circGradient, x, y, radius));
                            children.back()->mousePressed(x, y, 0);
                            
                            selectedGaussian = system.getNGaussians() - 1;
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
                            
                            // remove the Gaussian from the system
                            system.removeGaussian(i);
                            
                            if (selectedGaussian > 0) { selectedGaussian--; }
                            else if (system.getNGaussians() == 0) { selectedGaussian = -1; }
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

    

}