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

// implements TutorialContainer

/*
 TutorialContainer carries out all methods for displaying text boxes.
 Mouse events are controlled by storing an instance of the TutorialContainer in the UI container tutorialUI in ofApp.
 A third UIContainer, tutorialHighlightUI, is used to allow the user to interact with specific parts of the interface.
 TutorialContainer methods are passed to tutorialHighlightUI via a UIAtom, tutorialHighlightAtom, which contains a pointer to the TutorialContainer instance.
*/

namespace gui {
    
    TutorialContainer::TutorialContainer(int x, int y, int width, int height,ofTrueTypeFont &font, ofImage &nextButton, ofImage &previousButton, ofImage &closeButton, UIContainer &_tutorialUI, UIContainer &_tutorialHighlightUI, UIContainer &_tutorialBlockUI, md::MDContainer &system, UIContainer &_graphUI, UIContainer &_controlsUI, UIContainer &_potentialUI, UIContainer &_systemUI, int &_gaussianContainerIndex): aboutFont12(font), nextButton(nextButton), previousButton(previousButton), closeButton(closeButton), tutorialUI(_tutorialUI), tutorialHighlightUI(_tutorialHighlightUI), tutorialBlockUI(_tutorialBlockUI), theSystem(system), graphUI(_graphUI), controlsUI(_controlsUI), potentialUI(_potentialUI), systemUI(_systemUI), gaussianContainerIndex(_gaussianContainerIndex),UIContainer(x, y, width, height){
        
        textBoxx = 250;
        textBoxy = 0;
        textBoxWidth = 774;
        textBoxHeight = 150;
        tutorialCounter = 0;
        updateComponents(tutorialCounter);
    }
    
    void TutorialContainer::updateComponents(int counter){
        
        // All children in the TutorialContainer are deleted and redrawn in the positions for a given tutorialCounter value
        
        ofColor textBoxColor = ofColor(0, 0, 0, 100);
        ofColor textColor = ofColor(255, 255, 240);
        ofColor notHighlightedColor = ofColor(50, 50, 50, 210);
        
        int size = children.size() - 1;
        for (int i = size; i >= 0; i--){
            delete children[i];
            children.erase(children.begin() + i);}
        
        /*
         Introduction and description of particles
         */
        
        if (counter == 0){
            
            textBoxx = 250;
            textBoxy = 0;
            textBoxWidth = 774;
            textBoxHeight = 150;
            
            
            
            // Faded area
            addChild(new RectAtom(notHighlightedColor, 0, 0, 250, 600));
            addChild(new RectAtom(notHighlightedColor, 250, 150, 774, 450));
            // Text box
            addChild(new RectAtom(textBoxColor, textBoxx, textBoxy, textBoxWidth, textBoxHeight));
            // Text box text
            addChild(new TextAtom("This tutorial takes you through the basics of Argon.", aboutFont12, textColor, POS_CENTRE, textBoxx + 80, textBoxy + 50, 600, 20));
            addChild(new TextAtom("You can leave the tutorial at any point by left-clicking on the x in this text box.", aboutFont12, textColor, POS_CENTRE, textBoxx + 80, textBoxy + 80, 600, 20));
            addChild(new TextAtom("1/5", aboutFont12, textColor, POS_LEFT, textBoxx + textBoxWidth - 70, textBoxy + textBoxHeight - 30, 600, 20));
            // Text box close button
            addChild(new ButtonAtom([&] () {tutorialCounter = 0; updateComponents(tutorialCounter); makeInvisible(); tutorialUI.makeInvisible(); tutorialBlockUI.makeInvisible();}, closeButton, textBoxx+textBoxWidth - 35, textBoxy + 5, 30, 30));
            // Text box next button
            addChild(new ButtonAtom([&] () {tutorialCounter++; updateComponents(tutorialCounter);}, nextButton, textBoxx + textBoxWidth - 35, textBoxy + textBoxHeight - 35, 30, 30));
            
            // The two containers used to control mouse events are initially set to offscreen
            // The tutorialHighlight must be set for each slide, as the user can move back and forth through the section
            tutorialHighlightUI.moveTo(-50, 50);
            tutorialHighlightUI.setSize(30, 30);
            tutorialBlockUI.moveTo(-50, 50);
            tutorialBlockUI.setSize(30,30);
            
        }
        
        
        else if (counter == 1){
            // Faded area
            addChild(new RectAtom(notHighlightedColor, 0, 0, 250, 410));
            // Text box
            addChild(new RectAtom(textBoxColor, textBoxx, textBoxy, textBoxWidth, textBoxHeight));
            // Text box text
            addChild(new TextAtom("The colours of the particles indicate their velocity, and changes in shape", aboutFont12, textColor, POS_CENTRE, textBoxx + 80, textBoxy + 50, 600, 20));
            
            addChild(new TextAtom("show forces acting on them.", aboutFont12, textColor, POS_CENTRE, textBoxx + 80, textBoxy + 80, 600, 20));
            
            
            addChild(new TextAtom("2/5", aboutFont12, textColor, POS_LEFT, textBoxx + textBoxWidth - 70, textBoxy + textBoxHeight - 30, 600, 20));
            // Text box close button
            addChild(new ButtonAtom([&] () {tutorialCounter = 0; updateComponents(tutorialCounter); makeInvisible(); tutorialUI.makeInvisible(); tutorialBlockUI.makeInvisible();}, closeButton, textBoxx+textBoxWidth - 35, textBoxy + 5, 30, 30));
            // Text box next button
            addChild(new ButtonAtom([&] () {tutorialCounter++; updateComponents(tutorialCounter);}, nextButton, textBoxx + textBoxWidth - 35, textBoxy + textBoxHeight - 35, 30, 30));
            // Text box previous button
            addChild(new ButtonAtom([&] () {tutorialCounter--; updateComponents(tutorialCounter);}, previousButton, textBoxx + 5, textBoxy + textBoxHeight - 35, 30, 30));
            tutorialHighlightUI.makeVisible();
            tutorialBlockUI.makeVisible();
            tutorialHighlightUI.moveTo(-50, 50);
            tutorialHighlightUI.setSize(30, 30);
            theSystem.setNAfterReset(60);
            theSystem.setTemp(0.4);
            theSystem.setStepsPerUpdate(5);
            ((gui::GaussianContainer *)systemUI.getChild(gaussianContainerIndex))->destroyAllGaussians();
            theSystem.resetSystem();
            theSystem.setRunning(true);

        }
        
        else if (counter == 2){
            // Faded area
            addChild(new RectAtom(notHighlightedColor, 0, 0, 250, 410));
            // Text box
            addChild(new RectAtom(textBoxColor, textBoxx, textBoxy, textBoxWidth, textBoxHeight));
            // Text box text
            addChild(new TextAtom("When particles collide, the colour change shows the transfer of momentum in real time.", aboutFont12, textColor, POS_CENTRE, textBoxx + 80, textBoxy + 50, 600, 20));
            addChild(new TextAtom("3/5", aboutFont12, textColor, POS_LEFT, textBoxx + textBoxWidth - 70, textBoxy + textBoxHeight - 30, 600, 20));
            // Text box close button
            addChild(new ButtonAtom([&] () {tutorialCounter = 0; updateComponents(tutorialCounter); makeInvisible(); tutorialUI.makeInvisible(); tutorialBlockUI.makeInvisible();}, closeButton, textBoxx+textBoxWidth - 35, textBoxy + 5, 30, 30));
            // Text box next button
            addChild(new ButtonAtom([&] () {tutorialCounter++; updateComponents(tutorialCounter);}, nextButton, textBoxx + textBoxWidth - 35, textBoxy + textBoxHeight - 35, 30, 30));
            // Text box previous button
            addChild(new ButtonAtom([&] () {tutorialCounter--; updateComponents(tutorialCounter);}, previousButton, textBoxx + 5, textBoxy + textBoxHeight - 35, 30, 30));
            theSystem.setRunning(true);
            tutorialHighlightUI.moveTo(-50, 50);
            tutorialHighlightUI.setSize(30, 30);

        } //330
        
        else if (counter == 3){
            // Faded area
            addChild(new RectAtom(notHighlightedColor, 0, 0, 250, 325));
            // Text box
            addChild(new RectAtom(textBoxColor, textBoxx, textBoxy, textBoxWidth, textBoxHeight));
            // Text box text
            addChild(new TextAtom("The Reset button restarts the simulation with particles placed on a grid,", aboutFont12, textColor, POS_CENTRE, textBoxx + 80, textBoxy + 50, 600, 20));
            addChild(new TextAtom("and the Play/pause button allows you to pause or resume the simulation.", aboutFont12, textColor, POS_CENTRE, textBoxx + 80, textBoxy + 80, 600, 20));
            addChild(new TextAtom("4/5", aboutFont12, textColor, POS_LEFT, textBoxx + textBoxWidth - 70, textBoxy + textBoxHeight - 30, 600, 20));
            
            // Text box close button
            addChild(new ButtonAtom([&] () {tutorialCounter = 0; updateComponents(tutorialCounter); makeInvisible(); tutorialUI.makeInvisible(); tutorialBlockUI.makeInvisible();}, closeButton, textBoxx+textBoxWidth - 35, textBoxy + 5, 30, 30));
            // Text box next button
            addChild(new ButtonAtom([&] () {tutorialCounter++; updateComponents(tutorialCounter);}, nextButton, textBoxx + textBoxWidth - 35, textBoxy + textBoxHeight - 35, 30, 30));
            // Previous button
            addChild(new ButtonAtom([&] () {tutorialCounter--; updateComponents(tutorialCounter);}, previousButton, textBoxx + 5, textBoxy + textBoxHeight - 35, 30, 30));
            tutorialHighlightUI.moveTo(200,330);
            tutorialHighlightUI.setSize(30, 70);
            
            
            
        }
        
        
        
        else if (counter == 4){
            // Faded area
            addChild(new RectAtom(notHighlightedColor, 0, 0, 250, 45));
            addChild(new RectAtom(notHighlightedColor, 0, 85, 250, 325));
            // Text box
            addChild(new RectAtom(textBoxColor, textBoxx, textBoxy, textBoxWidth, textBoxHeight));
            // Text box text
            addChild(new TextAtom("To interact with the system, left click on the Controls button.", aboutFont12, textColor, POS_CENTRE, textBoxx + 80, textBoxy + 50, 600, 20));
            addChild(new TextAtom("5/5", aboutFont12, textColor, POS_LEFT, textBoxx + textBoxWidth - 70, textBoxy + textBoxHeight - 30, 600, 20));
            
            // Text box close button
            addChild(new ButtonAtom([&] () {tutorialCounter = 0; updateComponents(tutorialCounter); makeInvisible(); tutorialUI.makeInvisible(); tutorialBlockUI.makeInvisible();}, closeButton, textBoxx+textBoxWidth - 35, textBoxy + 5, 30, 30));
            // Previous button
            addChild(new ButtonAtom([&] () {tutorialCounter--; updateComponents(tutorialCounter);}, previousButton, textBoxx + 5, textBoxy + textBoxHeight - 35, 30, 30));
            tutorialHighlightUI.moveTo(200,50);
            tutorialHighlightUI.setSize(30, 30);
            theSystem.setRunning(true); // Ensures the particles are moving for the rest of the tutorial
        }
        
        /*
         Controls
         */
        
        
        else if(counter == 5){
            
            textBoxy = 150;

            // Faded area
            addChild(new RectAtom(notHighlightedColor, 0, 0, 250, 410));
            // Text box
            addChild(new RectAtom(textBoxColor, textBoxx, textBoxy, textBoxWidth, textBoxHeight));
            // Text box text
            addChild(new TextAtom("Properties of the system are selected by left-clicking, and are controlled by the slider.", aboutFont12, textColor, POS_CENTRE, textBoxx + 80, textBoxy + 50, 600, 20));
            addChild(new TextAtom("1/7", aboutFont12, textColor, POS_LEFT, textBoxx + textBoxWidth - 70, textBoxy + textBoxHeight - 30, 600, 20));
            // Text box close button
            addChild(new ButtonAtom([&] () {tutorialCounter = 0; updateComponents(tutorialCounter); makeInvisible(); tutorialUI.makeInvisible(); tutorialBlockUI.makeInvisible();}, closeButton, textBoxx+textBoxWidth - 35, textBoxy + 5, 30, 30));
            // Text box next button
            addChild(new ButtonAtom([&] () {tutorialCounter++; updateComponents(tutorialCounter);}, nextButton, textBoxx + textBoxWidth - 35, textBoxy + textBoxHeight - 35, 30, 30));
            
            tutorialHighlightUI.moveTo(250,0);
            tutorialHighlightUI.setSize(774, 160);
            //controlsUI.makeVisible();
            
            

        }
        
        
        else if(counter == 6){
            // Faded area
            addChild(new RectAtom(notHighlightedColor, 0, 0, 250, 410));
            // Text box
            addChild(new RectAtom(textBoxColor, textBoxx, textBoxy, textBoxWidth, textBoxHeight));
            // Text box text
            addChild(new TextAtom("Changing the temperature alters the kinetic energy of the particles,", aboutFont12, textColor, POS_CENTRE, textBoxx + 80, textBoxy + 50, 600, 20));
            addChild(new TextAtom("with higher temperatures corresponding to higher velocities.", aboutFont12, textColor, POS_CENTRE, textBoxx + 80, textBoxy + 80, 600, 20));
            addChild(new TextAtom("2/7", aboutFont12, textColor, POS_LEFT, textBoxx + textBoxWidth - 70, textBoxy + textBoxHeight - 30, 600, 20));
            // Text box close button
            addChild(new ButtonAtom([&] () {tutorialCounter = 0; updateComponents(tutorialCounter); makeInvisible(); tutorialUI.makeInvisible(); tutorialBlockUI.makeInvisible();}, closeButton, textBoxx+textBoxWidth - 35, textBoxy + 5, 30, 30));
            // Text box next button
            addChild(new ButtonAtom([&] () {tutorialCounter++; updateComponents(tutorialCounter);}, nextButton, textBoxx + textBoxWidth - 35, textBoxy + textBoxHeight - 35, 30, 30));
            // Previous button
            addChild(new ButtonAtom([&] () {tutorialCounter--; updateComponents(tutorialCounter);}, previousButton, textBoxx + 5, textBoxy + textBoxHeight - 35, 30, 30));
            

            tutorialHighlightUI.moveTo(250,0);
            tutorialHighlightUI.setSize(774, 160);

        }
        
        else if(counter == 7){
            
            // Faded area
            addChild(new RectAtom(notHighlightedColor, 0, 0, 250, 410));
            // Text box
            addChild(new RectAtom(textBoxColor, textBoxx, textBoxy, textBoxWidth, textBoxHeight));
            // Text box text
            addChild(new TextAtom("At low temperatures the particles will eventually crystallise.", aboutFont12, textColor, POS_CENTRE, textBoxx + 80, textBoxy + 50, 600, 20));
            addChild(new TextAtom("3/7", aboutFont12, textColor, POS_LEFT, textBoxx + textBoxWidth - 70, textBoxy + textBoxHeight - 30, 600, 20));
            // Text box close button
            addChild(new ButtonAtom([&] () {tutorialCounter = 0; updateComponents(tutorialCounter); makeInvisible(); tutorialUI.makeInvisible(); tutorialBlockUI.makeInvisible();}, closeButton, textBoxx+textBoxWidth - 35, textBoxy + 5, 30, 30));
            // Text box next button
            addChild(new ButtonAtom([&] () {tutorialCounter++; updateComponents(tutorialCounter);}, nextButton, textBoxx + textBoxWidth - 35, textBoxy + textBoxHeight - 35, 30, 30));
            // Previous button
            addChild(new ButtonAtom([&] () {tutorialCounter--; updateComponents(tutorialCounter);}, previousButton, textBoxx + 5, textBoxy + textBoxHeight - 35, 30, 30));
            
            tutorialHighlightUI.moveTo(250,0);
            tutorialHighlightUI.setSize(774, 160);
            
        }
        
        else if(counter == 8){
            
            // Faded area
            addChild(new RectAtom(notHighlightedColor, 0, 0, 250, 410));
            // Text box
            addChild(new RectAtom(textBoxColor, textBoxx, textBoxy, textBoxWidth, textBoxHeight));
            // Text box text
            addChild(new TextAtom("Changing the number of particles resets the system, with the new number", aboutFont12, textColor, POS_CENTRE, textBoxx + 80, textBoxy + 50, 600, 20));
            addChild(new TextAtom("of particles arranged on a grid.", aboutFont12, textColor, POS_CENTRE, textBoxx + 80, textBoxy + 80, 600, 20));
            addChild(new TextAtom("4/7", aboutFont12, textColor, POS_LEFT, textBoxx + textBoxWidth - 70, textBoxy + textBoxHeight - 30, 600, 20));
            // Text box close button
            addChild(new ButtonAtom([&] () {tutorialCounter = 0; updateComponents(tutorialCounter); makeInvisible(); tutorialUI.makeInvisible(); tutorialBlockUI.makeInvisible();}, closeButton, textBoxx+textBoxWidth - 35, textBoxy + 5, 30, 30));
            // Text box next button
            addChild(new ButtonAtom([&] () {tutorialCounter++; updateComponents(tutorialCounter);}, nextButton, textBoxx + textBoxWidth - 35, textBoxy + textBoxHeight - 35, 30, 30));
            // Previous button
            addChild(new ButtonAtom([&] () {tutorialCounter--; updateComponents(tutorialCounter);}, previousButton, textBoxx + 5, textBoxy + textBoxHeight - 35, 30, 30));
            
            tutorialHighlightUI.moveTo(250,0);
            tutorialHighlightUI.setSize(774, 160);
            
        }
        
        else if(counter == 9){
            
            // Faded area
            addChild(new RectAtom(notHighlightedColor, 0, 0, 250, 410));
            // Text box
            addChild(new RectAtom(textBoxColor, textBoxx, textBoxy, textBoxWidth, textBoxHeight));
            // Text box text
            addChild(new TextAtom("The simulation speed controls how fast the simulation runs.", aboutFont12, textColor, POS_CENTRE, textBoxx + 80, textBoxy + 50, 600, 20));
            addChild(new TextAtom("5/7", aboutFont12, textColor, POS_LEFT, textBoxx + textBoxWidth - 70, textBoxy + textBoxHeight - 30, 600, 20));
            // Text box close button
            addChild(new ButtonAtom([&] () {tutorialCounter = 0; updateComponents(tutorialCounter); makeInvisible(); tutorialUI.makeInvisible(); tutorialBlockUI.makeInvisible();}, closeButton, textBoxx+textBoxWidth - 35, textBoxy + 5, 30, 30));
            // Text box next button
            addChild(new ButtonAtom([&] () {tutorialCounter++; updateComponents(tutorialCounter);}, nextButton, textBoxx + textBoxWidth - 35, textBoxy + textBoxHeight - 35, 30, 30));
            // Previous button
            addChild(new ButtonAtom([&] () {tutorialCounter--; updateComponents(tutorialCounter);}, previousButton, textBoxx + 5, textBoxy + textBoxHeight - 35, 30, 30));
            
            tutorialHighlightUI.moveTo(250,0);
            tutorialHighlightUI.setSize(774, 160);
            
        }
        
        else if(counter == 10){
            
            textBoxx = 250;
            textBoxy = 150;
            textBoxWidth = 774;
            textBoxHeight = 150;
            
            // Faded area
            addChild(new RectAtom(notHighlightedColor, 0, 0, 250, 410));
            // Text box
            addChild(new RectAtom(textBoxColor, textBoxx, textBoxy, textBoxWidth, textBoxHeight));
            // Text box text
            addChild(new TextAtom("The Gaussian button gives control of gaussians placed in the system.", aboutFont12, textColor, POS_CENTRE, textBoxx + 80, textBoxy + 50, 600, 20));
            addChild(new TextAtom("6/7", aboutFont12, textColor, POS_LEFT, textBoxx + textBoxWidth - 70, textBoxy + textBoxHeight - 30, 600, 20));
            // Text box close button
            addChild(new ButtonAtom([&] () {tutorialCounter = 0; updateComponents(tutorialCounter); makeInvisible(); tutorialUI.makeInvisible(); tutorialBlockUI.makeInvisible();}, closeButton, textBoxx+textBoxWidth - 35, textBoxy + 5, 30, 30));
            // Text box next button
            addChild(new ButtonAtom([&] () {tutorialCounter++; updateComponents(tutorialCounter);}, nextButton, textBoxx + textBoxWidth - 35, textBoxy + textBoxHeight - 35, 30, 30));
            // Previous button
            addChild(new ButtonAtom([&] () {tutorialCounter--; updateComponents(tutorialCounter);}, previousButton, textBoxx + 5, textBoxy + textBoxHeight - 35, 30, 30));
            
            tutorialHighlightUI.moveTo(250,0);
            tutorialHighlightUI.setSize(774, 160);
            
        }
        
        else if(counter == 11){
            
            textBoxx = 0;
            textBoxy = 450;
            textBoxWidth = 1024;
            textBoxHeight = 150;
            
            // Faded area
            addChild(new RectAtom(notHighlightedColor, 0, 0, 250, 410));
            // Text box
            addChild(new RectAtom(textBoxColor, textBoxx, textBoxy, textBoxWidth, textBoxHeight));
            // Text box text
            addChild(new TextAtom("These are added to the system by left-clicking on the screen.", aboutFont12, textColor, POS_CENTRE, textBoxx + 220, textBoxy + 50, 600, 20));
            addChild(new TextAtom("Try it!", aboutFont12, textColor, POS_CENTRE, textBoxx + 220, textBoxy + 80, 600, 20));
            addChild(new TextAtom("7/7", aboutFont12, textColor, POS_LEFT, textBoxx + textBoxWidth - 70, textBoxy + textBoxHeight - 30, 600, 20));
            // Text box close button
            addChild(new ButtonAtom([&] () {tutorialCounter = 0; updateComponents(tutorialCounter); makeInvisible(); tutorialUI.makeInvisible(); tutorialBlockUI.makeInvisible();}, closeButton, textBoxx+textBoxWidth - 35, textBoxy + 5, 30, 30));
            // Previous button
            addChild(new ButtonAtom([&] () {tutorialCounter--; updateComponents(tutorialCounter);}, previousButton, textBoxx + 5, textBoxy + textBoxHeight - 35, 30, 30));
            
            tutorialHighlightUI.moveTo(250, 150);
            tutorialHighlightUI.setSize(774, 300);
            tutorialBlockUI.moveTo(0, 0);
            tutorialBlockUI.setSize(250, 410);
            
        }
        
        else if(counter == 12){
            
            textBoxx = 0;
            textBoxy = 450;
            textBoxWidth = 1024;
            textBoxHeight = 150;
            
            // Faded area
            addChild(new RectAtom(notHighlightedColor, 0, 0, 250, 410));
            // Text box
            addChild(new RectAtom(textBoxColor, textBoxx, textBoxy, textBoxWidth, textBoxHeight));
            // Text box text
            addChild(new TextAtom("These interact with particles by attracting or repelling them.", aboutFont12, textColor, POS_CENTRE, textBoxx + 220, textBoxy + 50, 600, 20));
            addChild(new TextAtom("1/8", aboutFont12, textColor, POS_LEFT, textBoxx + textBoxWidth - 70, textBoxy + textBoxHeight - 30, 600, 20));
            // Text box close button
            addChild(new ButtonAtom([&] () {tutorialCounter = 0; updateComponents(tutorialCounter); makeInvisible(); tutorialUI.makeInvisible(); tutorialBlockUI.makeInvisible();}, closeButton, textBoxx+textBoxWidth - 35, textBoxy + 5, 30, 30));
            // Text box next button
            addChild(new ButtonAtom([&] () {tutorialCounter++; updateComponents(tutorialCounter);}, nextButton, textBoxx + textBoxWidth - 35, textBoxy + textBoxHeight - 35, 30, 30));
       
            tutorialHighlightUI.moveTo(0, 0);
            tutorialHighlightUI.setSize(1024, 450);
            tutorialBlockUI.moveTo(0, 0);
            tutorialBlockUI.setSize(250, 410);
            
        }
        
        else if(counter == 13){
            
            // Faded area
            addChild(new RectAtom(notHighlightedColor, 0, 0, 250, 410));
            // Text box
            addChild(new RectAtom(textBoxColor, textBoxx, textBoxy, textBoxWidth, textBoxHeight));
            // Text box text
            addChild(new TextAtom("The size of the gaussian is controlled by the sound through your microphone.", aboutFont12, textColor, POS_CENTRE, textBoxx + 220, textBoxy + 50, 600, 20));
            addChild(new TextAtom("2/8", aboutFont12, textColor, POS_LEFT, textBoxx + textBoxWidth - 70, textBoxy + textBoxHeight - 30, 600, 20));
            // Text box close button
            addChild(new ButtonAtom([&] () {tutorialCounter = 0; updateComponents(tutorialCounter); makeInvisible(); tutorialUI.makeInvisible(); tutorialBlockUI.makeInvisible();}, closeButton, textBoxx+textBoxWidth - 35, textBoxy + 5, 30, 30));
            // Text box next button
            addChild(new ButtonAtom([&] () {tutorialCounter++; updateComponents(tutorialCounter);}, nextButton, textBoxx + textBoxWidth - 35, textBoxy + textBoxHeight - 35, 30, 30));
            // Previous button
            addChild(new ButtonAtom([&] () {tutorialCounter--; updateComponents(tutorialCounter);}, previousButton, textBoxx + 5, textBoxy + textBoxHeight - 35, 30, 30));
            
            tutorialHighlightUI.moveTo(0, 0);
            tutorialHighlightUI.setSize(1024, 450);
            tutorialBlockUI.moveTo(0, 0);
            tutorialBlockUI.setSize(250, 410);
            
        }
        
        else if(counter == 14){
            
            // Faded area
            addChild(new RectAtom(notHighlightedColor, 0, 0, 250, 410));
            // Text box
            addChild(new RectAtom(textBoxColor, textBoxx, textBoxy, textBoxWidth, textBoxHeight));
            // Text box text
            addChild(new TextAtom("The louder the sound, the more the gaussian will repel particles.", aboutFont12, textColor, POS_CENTRE, textBoxx + 220, textBoxy + 50, 600, 20));
            addChild(new TextAtom("When no sound is registered, the gaussian will attract particles.", aboutFont12, textColor, POS_CENTRE, textBoxx + 220, textBoxy + 80, 600, 20));
            addChild(new TextAtom("3/8", aboutFont12, textColor, POS_LEFT, textBoxx + textBoxWidth - 70, textBoxy + textBoxHeight - 30, 600, 20));
            // Text box close button
            addChild(new ButtonAtom([&] () {tutorialCounter = 0; updateComponents(tutorialCounter); makeInvisible(); tutorialUI.makeInvisible(); tutorialBlockUI.makeInvisible();}, closeButton, textBoxx+textBoxWidth - 35, textBoxy + 5, 30, 30));
            // Text box next button
            addChild(new ButtonAtom([&] () {tutorialCounter++; updateComponents(tutorialCounter);}, nextButton, textBoxx + textBoxWidth - 35, textBoxy + textBoxHeight - 35, 30, 30));
            // Previous button
            addChild(new ButtonAtom([&] () {tutorialCounter--; updateComponents(tutorialCounter);}, previousButton, textBoxx + 5, textBoxy + textBoxHeight - 35, 30, 30));
            
            tutorialHighlightUI.moveTo(0, 0);
            tutorialHighlightUI.setSize(1024, 450);
            tutorialBlockUI.moveTo(0, 0);
            tutorialBlockUI.setSize(250, 410);
            theSystem.setRunning(true);
            

            
        }
        
        else if(counter == 15){
            
            // Faded area
            addChild(new RectAtom(notHighlightedColor, 0, 0, 250, 245));
            // Text box
            addChild(new RectAtom(textBoxColor, textBoxx, textBoxy, textBoxWidth, textBoxHeight));
            // Text box text
            addChild(new TextAtom("The Mic On/Off button controls if sound is heard through your microphone.", aboutFont12, textColor, POS_CENTRE, textBoxx + 220, textBoxy + 50, 600, 20));
            addChild(new TextAtom("4/8", aboutFont12, textColor, POS_LEFT, textBoxx + textBoxWidth - 70, textBoxy + textBoxHeight - 30, 600, 20));
            // Text box close button
            addChild(new ButtonAtom([&] () {tutorialCounter = 0; updateComponents(tutorialCounter); makeInvisible(); tutorialUI.makeInvisible(); tutorialBlockUI.makeInvisible();}, closeButton, textBoxx+textBoxWidth - 35, textBoxy + 5, 30, 30));
            // Text box next button
            addChild(new ButtonAtom([&] () {tutorialCounter++; updateComponents(tutorialCounter);}, nextButton, textBoxx + textBoxWidth - 35, textBoxy + textBoxHeight - 35, 30, 30));
            // Previous button
            addChild(new ButtonAtom([&] () {tutorialCounter--; updateComponents(tutorialCounter);}, previousButton, textBoxx + 5, textBoxy + textBoxHeight - 35, 30, 30));
            
            tutorialHighlightUI.moveTo(0, 0);
            tutorialHighlightUI.setSize(1024, 450);
            tutorialBlockUI.moveTo(0, 0);
            tutorialBlockUI.setSize(250, 245);
            
        }
        
        else if(counter == 16){
            
            // Faded area
            addChild(new RectAtom(notHighlightedColor, 0, 0, 250, 245));
            // Text box
            addChild(new RectAtom(textBoxColor, textBoxx, textBoxy, textBoxWidth, textBoxHeight));
            // Text box text
            addChild(new TextAtom("When this is selected, you can instead control the strength", aboutFont12, textColor, POS_CENTRE, textBoxx + 220, textBoxy + 50, 600, 20));
            addChild(new TextAtom("of the gaussians with the slider.", aboutFont12, textColor, POS_CENTRE, textBoxx + 220, textBoxy + 80, 600, 20));
            addChild(new TextAtom("5/8", aboutFont12, textColor, POS_LEFT, textBoxx + textBoxWidth - 70, textBoxy + textBoxHeight - 30, 600, 20));
            // Text box close button
            addChild(new ButtonAtom([&] () {tutorialCounter = 0; updateComponents(tutorialCounter); makeInvisible(); tutorialUI.makeInvisible(); tutorialBlockUI.makeInvisible();}, closeButton, textBoxx+textBoxWidth - 35, textBoxy + 5, 30, 30));
            // Text box next button
            addChild(new ButtonAtom([&] () {tutorialCounter++; updateComponents(tutorialCounter);}, nextButton, textBoxx + textBoxWidth - 35, textBoxy + textBoxHeight - 35, 30, 30));
            // Previous button
            addChild(new ButtonAtom([&] () {tutorialCounter--; updateComponents(tutorialCounter);}, previousButton, textBoxx + 5, textBoxy + textBoxHeight - 35, 30, 30));
            
            tutorialHighlightUI.moveTo(0, 0);
            tutorialHighlightUI.setSize(1024, 450);
            tutorialBlockUI.moveTo(0, 0);
            tutorialBlockUI.setSize(250, 245);
            
        }
        
        else if(counter == 17){
            
            // Faded area
            addChild(new RectAtom(notHighlightedColor, 0, 0, 250, 245));
            // Text box
            addChild(new RectAtom(textBoxColor, textBoxx, textBoxy, textBoxWidth, textBoxHeight));
            // Text box text
            addChild(new TextAtom("Gaussians are selected by left-clicking on them, and moved by", aboutFont12, textColor, POS_CENTRE, textBoxx + 220, textBoxy + 50, 600, 20));
            addChild(new TextAtom("holding down the left mouse button and dragging.", aboutFont12, textColor, POS_CENTRE, textBoxx + 220, textBoxy + 80, 600, 20));
            addChild(new TextAtom("6/8", aboutFont12, textColor, POS_LEFT, textBoxx + textBoxWidth - 70, textBoxy + textBoxHeight - 30, 600, 20));
            // Text box close button
            addChild(new ButtonAtom([&] () {tutorialCounter = 0; updateComponents(tutorialCounter); makeInvisible(); tutorialUI.makeInvisible(); tutorialBlockUI.makeInvisible();}, closeButton, textBoxx+textBoxWidth - 35, textBoxy + 5, 30, 30));
            // Text box next button
            addChild(new ButtonAtom([&] () {tutorialCounter++; updateComponents(tutorialCounter);}, nextButton, textBoxx + textBoxWidth - 35, textBoxy + textBoxHeight - 35, 30, 30));
            // Previous button
            addChild(new ButtonAtom([&] () {tutorialCounter--; updateComponents(tutorialCounter);}, previousButton, textBoxx + 5, textBoxy + textBoxHeight - 35, 30, 30));
            
            tutorialHighlightUI.moveTo(0, 0);
            tutorialHighlightUI.setSize(1024, 450);
            tutorialBlockUI.moveTo(0, 0);
            tutorialBlockUI.setSize(250, 245);
            
        }
        
        else if(counter == 18){
            
            // Faded area
            addChild(new RectAtom(notHighlightedColor, 0, 0, 250, 245));
            // Text box
            addChild(new RectAtom(textBoxColor, textBoxx, textBoxy, textBoxWidth, textBoxHeight));
            // Text box text
            addChild(new TextAtom("Right-click on a gaussian to remove it, or use the Reset gaussians", aboutFont12, textColor, POS_CENTRE, textBoxx + 220, textBoxy + 50, 600, 20));
            addChild(new TextAtom("button to remove all gaussians on the screen.", aboutFont12, textColor, POS_CENTRE, textBoxx + 220, textBoxy + 80, 600, 20));
            addChild(new TextAtom("7/8", aboutFont12, textColor, POS_LEFT, textBoxx + textBoxWidth - 70, textBoxy + textBoxHeight - 30, 600, 20));
            // Text box close button
            addChild(new ButtonAtom([&] () {tutorialCounter = 0; updateComponents(tutorialCounter); makeInvisible(); tutorialUI.makeInvisible(); tutorialBlockUI.makeInvisible();}, closeButton, textBoxx+textBoxWidth - 35, textBoxy + 5, 30, 30));
            // Text box next button
            addChild(new ButtonAtom([&] () {tutorialCounter++; updateComponents(tutorialCounter);}, nextButton, textBoxx + textBoxWidth - 35, textBoxy + textBoxHeight - 35, 30, 30));
            // Previous button
            addChild(new ButtonAtom([&] () {tutorialCounter--; updateComponents(tutorialCounter);}, previousButton, textBoxx + 5, textBoxy + textBoxHeight - 35, 30, 30));
            
            tutorialHighlightUI.moveTo(0, 0);
            tutorialHighlightUI.setSize(1024, 450);
            tutorialBlockUI.moveTo(0, 0);
            tutorialBlockUI.setSize(250, 245);
            
        }
        
        else if(counter == 19){
            
            // Faded area
            addChild(new RectAtom(notHighlightedColor, 0, 0, 250, 125));
            addChild(new RectAtom(notHighlightedColor, 0, 165, 250, 240));

            // Text box
            addChild(new RectAtom(textBoxColor, textBoxx, textBoxy, textBoxWidth, textBoxHeight));
            // Text box text
            addChild(new TextAtom("To see how these gaussians are changing the system, you can plot", aboutFont12, textColor, POS_CENTRE, textBoxx + 220, textBoxy + 50, 600, 20));
            addChild(new TextAtom("the properties of the particles by selecting the Graphs button.", aboutFont12, textColor, POS_CENTRE, textBoxx + 220, textBoxy + 80, 600, 20));
            addChild(new TextAtom("8/8", aboutFont12, textColor, POS_LEFT, textBoxx + textBoxWidth - 70, textBoxy + textBoxHeight - 30, 600, 20));
            // Text box close button
            addChild(new ButtonAtom([&] () {tutorialCounter = 0; updateComponents(tutorialCounter); makeInvisible(); tutorialUI.makeInvisible(); tutorialBlockUI.makeInvisible();}, closeButton, textBoxx+textBoxWidth - 35, textBoxy + 5, 30, 30));
            // Previous button
            addChild(new ButtonAtom([&] () {tutorialCounter--; updateComponents(tutorialCounter);}, previousButton, textBoxx + 5, textBoxy + textBoxHeight - 35, 30, 30));

            tutorialHighlightUI.moveTo(200, 130);
            tutorialHighlightUI.setSize(30, 30);
            tutorialBlockUI.moveTo(-50, 50);
            tutorialBlockUI.setSize(30,30);
            theSystem.setRunning(true);
            
        }
        
        else if(counter == 20){
            
            // Faded area
            addChild(new RectAtom(notHighlightedColor, 0, 0, 250, 245));
            
            // Text box
            addChild(new RectAtom(textBoxColor, textBoxx, textBoxy, textBoxWidth, textBoxHeight));
            // Text box text
            addChild(new TextAtom("The kinetic energy graph plots the velocity of all particles in the system over time.", aboutFont12, textColor, POS_CENTRE, textBoxx + 220, textBoxy + 50, 600, 20));
            addChild(new TextAtom("1/6", aboutFont12, textColor, POS_LEFT, textBoxx + textBoxWidth - 70, textBoxy + textBoxHeight - 30, 600, 20));
            // Text box close button
            addChild(new ButtonAtom([&] () {tutorialCounter = 0; updateComponents(tutorialCounter); makeInvisible(); tutorialUI.makeInvisible(); tutorialBlockUI.makeInvisible();}, closeButton, textBoxx+textBoxWidth - 35, textBoxy + 5, 30, 30));

            // Text box next button
            addChild(new ButtonAtom([&] () {tutorialCounter++; updateComponents(tutorialCounter);}, nextButton, textBoxx + textBoxWidth - 35, textBoxy + textBoxHeight - 35, 30, 30));
            
            tutorialHighlightUI.moveTo(0, 0);
            tutorialHighlightUI.setSize(1024, 450);
            tutorialBlockUI.moveTo(0, 0);
            tutorialBlockUI.setSize(250, 245);
            graphUI.makeVisible();
        }
        
        else if(counter == 21){
            
            // Faded area
            addChild(new RectAtom(notHighlightedColor, 0, 0, 250, 245));
            
            // Text box
            addChild(new RectAtom(textBoxColor, textBoxx, textBoxy, textBoxWidth, textBoxHeight));
            // Text box text
            addChild(new TextAtom("The potential energy graph sums the interactions of all particles with each other", aboutFont12, textColor, POS_CENTRE, textBoxx + 220, textBoxy + 50, 600, 20));
            addChild(new TextAtom("and with the gaussians over time.", aboutFont12, textColor, POS_CENTRE, textBoxx + 220, textBoxy + 80, 600, 20));
            addChild(new TextAtom("2/6", aboutFont12, textColor, POS_LEFT, textBoxx + textBoxWidth - 70, textBoxy + textBoxHeight - 30, 600, 20));
            // Text box close button
            addChild(new ButtonAtom([&] () {tutorialCounter = 0; updateComponents(tutorialCounter); makeInvisible(); tutorialUI.makeInvisible(); tutorialBlockUI.makeInvisible();}, closeButton, textBoxx+textBoxWidth - 35, textBoxy + 5, 30, 30));
            // Previous button
            addChild(new ButtonAtom([&] () {tutorialCounter--; updateComponents(tutorialCounter);}, previousButton, textBoxx + 5, textBoxy + textBoxHeight - 35, 30, 30));
            // Text box next button
            addChild(new ButtonAtom([&] () {tutorialCounter++; updateComponents(tutorialCounter);}, nextButton, textBoxx + textBoxWidth - 35, textBoxy + textBoxHeight - 35, 30, 30));
            

            tutorialHighlightUI.moveTo(0, 0);
            tutorialHighlightUI.setSize(1024, 450);
            tutorialBlockUI.moveTo(0, 0);
            tutorialBlockUI.setSize(250, 245);
            
        }
        
        else if(counter == 22){
            
            // Faded area
            addChild(new RectAtom(notHighlightedColor, 0, 0, 250, 245));
            
            // Text box
            addChild(new RectAtom(textBoxColor, textBoxx, textBoxy, textBoxWidth, textBoxHeight));
            // Text box text
            addChild(new TextAtom("Although kinetic energy is being converted into potential energy, the two graphs", aboutFont12, textColor, POS_CENTRE, textBoxx + 220, textBoxy + 50, 600, 20));
            addChild(new TextAtom("are not exact inverses of each other, as the system is being heated.", aboutFont12, textColor, POS_CENTRE, textBoxx + 220, textBoxy + 80, 600, 20));
            addChild(new TextAtom("3/6", aboutFont12, textColor, POS_LEFT, textBoxx + textBoxWidth - 70, textBoxy + textBoxHeight - 30, 600, 20));
            // Text box close button
            addChild(new ButtonAtom([&] () {tutorialCounter = 0; updateComponents(tutorialCounter); makeInvisible(); tutorialUI.makeInvisible(); tutorialBlockUI.makeInvisible();}, closeButton, textBoxx+textBoxWidth - 35, textBoxy + 5, 30, 30));
            // Previous button
            addChild(new ButtonAtom([&] () {tutorialCounter--; updateComponents(tutorialCounter);}, previousButton, textBoxx + 5, textBoxy + textBoxHeight - 35, 30, 30));
            // Text box next button
            addChild(new ButtonAtom([&] () {tutorialCounter++; updateComponents(tutorialCounter);}, nextButton, textBoxx + textBoxWidth - 35, textBoxy + textBoxHeight - 35, 30, 30));
            
            tutorialHighlightUI.moveTo(0, 0);
            tutorialHighlightUI.setSize(1024, 450);
            tutorialBlockUI.moveTo(0, 0);
            tutorialBlockUI.setSize(250, 245);
            
        }
        
        else if(counter == 23){
            
            // Faded area
            addChild(new RectAtom(notHighlightedColor, 0, 0, 250, 245));
            
            // Text box
            addChild(new RectAtom(textBoxColor, textBoxx, textBoxy, textBoxWidth, textBoxHeight));
            // Text box text
            addChild(new TextAtom("The Maxwell-Boltzmann plot shows the number of particles with different velocities.", aboutFont12, textColor, POS_CENTRE, textBoxx + 220, textBoxy + 50, 600, 20));
            addChild(new TextAtom("4/6", aboutFont12, textColor, POS_LEFT, textBoxx + textBoxWidth - 70, textBoxy + textBoxHeight - 30, 600, 20));
            // Text box close button
            addChild(new ButtonAtom([&] () {tutorialCounter = 0; updateComponents(tutorialCounter); makeInvisible(); tutorialUI.makeInvisible(); tutorialBlockUI.makeInvisible();}, closeButton, textBoxx+textBoxWidth - 35, textBoxy + 5, 30, 30));
            // Previous button
            addChild(new ButtonAtom([&] () {tutorialCounter--; updateComponents(tutorialCounter);}, previousButton, textBoxx + 5, textBoxy + textBoxHeight - 35, 30, 30));
            // Text box next button
            addChild(new ButtonAtom([&] () {tutorialCounter++; updateComponents(tutorialCounter);}, nextButton, textBoxx + textBoxWidth - 35, textBoxy + textBoxHeight - 35, 30, 30));
            
            tutorialHighlightUI.moveTo(0, 0);
            tutorialHighlightUI.setSize(1024, 450);
            tutorialBlockUI.moveTo(0, 0);
            tutorialBlockUI.setSize(250, 245);
            
        }
        
        else if(counter == 24){
            
            // Faded area
            addChild(new RectAtom(notHighlightedColor, 0, 0, 250, 245));
            
            // Text box
            addChild(new RectAtom(textBoxColor, textBoxx, textBoxy, textBoxWidth, textBoxHeight));
            // Text box text
            addChild(new TextAtom("For a given temperature, the system will always reach the same distribution of velocities,", aboutFont12, textColor, POS_CENTRE, textBoxx + 220, textBoxy + 50, 600, 20));
            addChild(new TextAtom("if the number of particles is large enough.", aboutFont12, textColor, POS_CENTRE, textBoxx + 220, textBoxy + 80, 600, 20));
            addChild(new TextAtom("5/6", aboutFont12, textColor, POS_LEFT, textBoxx + textBoxWidth - 70, textBoxy + textBoxHeight - 30, 600, 20));
            // Text box close button
            addChild(new ButtonAtom([&] () {tutorialCounter = 0; updateComponents(tutorialCounter); makeInvisible(); tutorialUI.makeInvisible(); tutorialBlockUI.makeInvisible();}, closeButton, textBoxx+textBoxWidth - 35, textBoxy + 5, 30, 30));
            // Previous button
            addChild(new ButtonAtom([&] () {tutorialCounter--; updateComponents(tutorialCounter);}, previousButton, textBoxx + 5, textBoxy + textBoxHeight - 35, 30, 30));
            // Text box next button
            addChild(new ButtonAtom([&] () {tutorialCounter++; updateComponents(tutorialCounter);}, nextButton, textBoxx + textBoxWidth - 35, textBoxy + textBoxHeight - 35, 30, 30));
            
            tutorialHighlightUI.moveTo(0, 0);
            tutorialHighlightUI.setSize(1024, 450);
            tutorialBlockUI.moveTo(0, 0);
            tutorialBlockUI.setSize(250, 245);
            
        }
        
        else if(counter == 25){
            
            // Faded area
            addChild(new RectAtom(notHighlightedColor, 0, 0, 250, 85));
            addChild(new RectAtom(notHighlightedColor, 0, 127, 250, 283));
            
            // Text box
            addChild(new RectAtom(textBoxColor, textBoxx, textBoxy, textBoxWidth, textBoxHeight));
            // Text box text
            addChild(new TextAtom("How the particles interact with each other depends on the potential used.", aboutFont12, textColor, POS_CENTRE, textBoxx + 220, textBoxy + 50, 600, 20));
            addChild(new TextAtom("You can modify this by left-clicking the Potentials button.", aboutFont12, textColor, POS_CENTRE, textBoxx + 220, textBoxy + 80, 600, 20));
            addChild(new TextAtom("6/6", aboutFont12, textColor, POS_LEFT, textBoxx + textBoxWidth - 70, textBoxy + textBoxHeight - 30, 600, 20));
            // Text box close button
            addChild(new ButtonAtom([&] () {tutorialCounter = 0; updateComponents(tutorialCounter); makeInvisible(); tutorialUI.makeInvisible(); tutorialBlockUI.makeInvisible();}, closeButton, textBoxx+textBoxWidth - 35, textBoxy + 5, 30, 30));
            // Previous button
            addChild(new ButtonAtom([&] () {tutorialCounter--; updateComponents(tutorialCounter);}, previousButton, textBoxx + 5, textBoxy + textBoxHeight - 35, 30, 30));
            
            
            tutorialHighlightUI.moveTo(200, 90);
            tutorialHighlightUI.setSize(30, 30);
            tutorialBlockUI.moveTo(-50, 50);
            tutorialBlockUI.setSize(30,30);
        }
        
        else if(counter == 26){
            
            textBoxx = 200;
            textBoxy = 480;
            textBoxWidth = 774;
            textBoxHeight = 100;
            
            addChild(new RectAtom(notHighlightedColor, 50, 175, 150, 400));
            
            // Text box
            addChild(new RectAtom(textBoxColor, textBoxx, textBoxy, textBoxWidth, textBoxHeight));
            // Text box text
            addChild(new TextAtom("Particles attract and repel each other based on the potential selected.", aboutFont12, textColor, POS_CENTRE, textBoxx + 80, textBoxy + 10, 600, 20));
            addChild(new TextAtom("(The control parameters have now been modified to show this more clearly.)", aboutFont12, textColor, POS_CENTRE, textBoxx + 80, textBoxy + 40, 600, 20));
            addChild(new TextAtom("1/10", aboutFont12, textColor, POS_LEFT, textBoxx + textBoxWidth - 75, textBoxy + textBoxHeight - 30, 600, 20));
            // Text box close button
            addChild(new ButtonAtom([&] () {tutorialCounter = 0; updateComponents(tutorialCounter); makeInvisible(); tutorialUI.makeInvisible(); tutorialBlockUI.makeInvisible();}, closeButton, textBoxx+textBoxWidth - 35, textBoxy + 5, 30, 30));
            // Text box next button
            addChild(new ButtonAtom([&] () {tutorialCounter++; updateComponents(tutorialCounter);}, nextButton, textBoxx + textBoxWidth - 35, textBoxy + textBoxHeight - 35, 30, 30));
            
            theSystem.setNAfterReset(20);
            theSystem.setTemp(0.04);
            theSystem.setStepsPerUpdate(20);
            ((gui::GaussianContainer *)systemUI.getChild(gaussianContainerIndex))->destroyAllGaussians();
            theSystem.resetSystem();
            theSystem.setRunning(true);

        }
        
        else if(counter == 27){
            
            textBoxx = 200;
            textBoxy = 480;
            textBoxWidth = 774;
            textBoxHeight = 100;
            
            addChild(new RectAtom(notHighlightedColor, 50, 175, 150, 400));
            
            // Text box
            addChild(new RectAtom(textBoxColor, textBoxx, textBoxy, textBoxWidth, textBoxHeight));
            // Text box text
            addChild(new TextAtom("The x-axis corresponds to the distance between particles, and the y-axis is energy.", aboutFont12, textColor, POS_CENTRE, textBoxx + 80, textBoxy + 10, 600, 20));
            addChild(new TextAtom("2/10", aboutFont12, textColor, POS_LEFT, textBoxx + textBoxWidth - 75, textBoxy + textBoxHeight - 30, 600, 20));
            // Text box close button
            addChild(new ButtonAtom([&] () {tutorialCounter = 0; updateComponents(tutorialCounter); makeInvisible(); tutorialUI.makeInvisible(); tutorialBlockUI.makeInvisible();}, closeButton, textBoxx+textBoxWidth - 35, textBoxy + 5, 30, 30));
            // Previous button
            addChild(new ButtonAtom([&] () {tutorialCounter--; updateComponents(tutorialCounter);}, previousButton, textBoxx + 5, textBoxy + textBoxHeight - 35, 30, 30));
            // Text box next button
            addChild(new ButtonAtom([&] () {tutorialCounter++; updateComponents(tutorialCounter);}, nextButton, textBoxx + textBoxWidth - 35, textBoxy + textBoxHeight - 35, 30, 30));
            
        }
        
        else if(counter == 28){
            
            textBoxx = 200;
            textBoxy = 480;
            textBoxWidth = 774;
            textBoxHeight = 100;
            
            addChild(new RectAtom(notHighlightedColor, 50, 175, 150, 400));
            
            // Text box
            addChild(new RectAtom(textBoxColor, textBoxx, textBoxy, textBoxWidth, textBoxHeight));
            // Text box text
            addChild(new TextAtom("Every particle moves towards a lower energy state, and hence are attracted to", aboutFont12, textColor, POS_CENTRE, textBoxx + 80, textBoxy + 10, 600, 20));
            addChild(new TextAtom("low energy areas of the potential, and repelled from areas with high energy.", aboutFont12, textColor, POS_CENTRE, textBoxx + 80, textBoxy + 40, 600, 20));
            addChild(new TextAtom("3/10", aboutFont12, textColor, POS_LEFT, textBoxx + textBoxWidth - 75, textBoxy + textBoxHeight - 30, 600, 20));
            // Text box close button
            addChild(new ButtonAtom([&] () {tutorialCounter = 0; updateComponents(tutorialCounter); makeInvisible(); tutorialUI.makeInvisible(); tutorialBlockUI.makeInvisible();}, closeButton, textBoxx+textBoxWidth - 35, textBoxy + 5, 30, 30));
            // Previous button
            addChild(new ButtonAtom([&] () {tutorialCounter--; updateComponents(tutorialCounter);}, previousButton, textBoxx + 5, textBoxy + textBoxHeight - 35, 30, 30));
            // Text box next button
            addChild(new ButtonAtom([&] () {tutorialCounter++; updateComponents(tutorialCounter);}, nextButton, textBoxx + textBoxWidth - 35, textBoxy + textBoxHeight - 35, 30, 30));
            
        }
        
        else if(counter == 29){
            
            textBoxx = 200;
            textBoxy = 480;
            textBoxWidth = 774;
            textBoxHeight = 100;
            
            addChild(new RectAtom(notHighlightedColor, 50, 175, 150, 400));
            
            // Text box
            addChild(new RectAtom(textBoxColor, textBoxx, textBoxy, textBoxWidth, textBoxHeight));
            // Text box text
            addChild(new TextAtom("The background graph shows the density of particles away from a given particle.", aboutFont12, textColor, POS_CENTRE, textBoxx + 80, textBoxy + 10, 600, 20));
            addChild(new TextAtom("4/10", aboutFont12, textColor, POS_LEFT, textBoxx + textBoxWidth - 75, textBoxy + textBoxHeight - 30, 600, 20));
            // Text box close button
            addChild(new ButtonAtom([&] () {tutorialCounter = 0; updateComponents(tutorialCounter); makeInvisible(); tutorialUI.makeInvisible(); tutorialBlockUI.makeInvisible();}, closeButton, textBoxx+textBoxWidth - 35, textBoxy + 5, 30, 30));
            // Previous button
            addChild(new ButtonAtom([&] () {tutorialCounter--; updateComponents(tutorialCounter);}, previousButton, textBoxx + 5, textBoxy + textBoxHeight - 35, 30, 30));
            // Text box next button
            addChild(new ButtonAtom([&] () {tutorialCounter++; updateComponents(tutorialCounter);}, nextButton, textBoxx + textBoxWidth - 35, textBoxy + textBoxHeight - 35, 30, 30));
            
        }
        
        
        
        
        
        
        else if(counter == 30){
            
            textBoxx = 200;
            textBoxy = 480;
            textBoxWidth = 774;
            textBoxHeight = 100;
            
            // Text box
            addChild(new RectAtom(textBoxColor, textBoxx, textBoxy, textBoxWidth, textBoxHeight));
            // Text box text
            addChild(new TextAtom("Different template potentials can be selected by left-clicking on the", aboutFont12, textColor, POS_CENTRE, textBoxx + 80, textBoxy + 10, 600, 20));
            addChild(new TextAtom("potential icons, or you can click on the Custom potential to design your own.", aboutFont12, textColor, POS_CENTRE, textBoxx + 80, textBoxy + 40, 600, 20));
            addChild(new TextAtom("5/10", aboutFont12, textColor, POS_LEFT, textBoxx + textBoxWidth - 75, textBoxy + textBoxHeight - 30, 600, 20));
            // Text box close button
            addChild(new ButtonAtom([&] () {tutorialCounter = 0; updateComponents(tutorialCounter); makeInvisible(); tutorialUI.makeInvisible(); tutorialBlockUI.makeInvisible();}, closeButton, textBoxx+textBoxWidth - 35, textBoxy + 5, 30, 30));
            // Previous button
            addChild(new ButtonAtom([&] () {tutorialCounter--; updateComponents(tutorialCounter);}, previousButton, textBoxx + 5, textBoxy + textBoxHeight - 35, 30, 30));
            // Text box next button
            addChild(new ButtonAtom([&] () {tutorialCounter++; updateComponents(tutorialCounter);}, nextButton, textBoxx + textBoxWidth - 35, textBoxy + textBoxHeight - 35, 30, 30));
            
            tutorialHighlightUI.moveTo(50, 50);
            tutorialHighlightUI.setSize(924, 500);
            
        }
        
        else if(counter == 31){
            
            textBoxx = 200;
            textBoxy = 480;
            textBoxWidth = 774;
            textBoxHeight = 100;
            
            // Text box
            addChild(new RectAtom(textBoxColor, textBoxx, textBoxy, textBoxWidth, textBoxHeight));
            // Text box text
            addChild(new TextAtom("Custom potentials can be designed by adding and moving control points.", aboutFont12, textColor, POS_CENTRE, textBoxx + 80, textBoxy + 10, 600, 20));
            addChild(new TextAtom("6/10", aboutFont12, textColor, POS_LEFT, textBoxx + textBoxWidth - 75, textBoxy + textBoxHeight - 30, 600, 20));
            // Text box close button
            addChild(new ButtonAtom([&] () {tutorialCounter = 0; updateComponents(tutorialCounter); makeInvisible(); tutorialUI.makeInvisible(); tutorialBlockUI.makeInvisible();}, closeButton, textBoxx+textBoxWidth - 35, textBoxy + 5, 30, 30));
            // Previous button
            addChild(new ButtonAtom([&] () {tutorialCounter--; updateComponents(tutorialCounter);}, previousButton, textBoxx + 5, textBoxy + textBoxHeight - 35, 30, 30));
            // Text box next button
            addChild(new ButtonAtom([&] () {tutorialCounter++; updateComponents(tutorialCounter);}, nextButton, textBoxx + textBoxWidth - 35, textBoxy + textBoxHeight - 35, 30, 30));
            
            tutorialHighlightUI.moveTo(50, 50);
            tutorialHighlightUI.setSize(924, 500);
            
        }
        
        else if(counter == 32){
            
            textBoxx = 200;
            textBoxy = 480;
            textBoxWidth = 774;
            textBoxHeight = 100;
            
            // Text box
            addChild(new RectAtom(textBoxColor, textBoxx, textBoxy, textBoxWidth, textBoxHeight));
            // Text box text
            addChild(new TextAtom("To add a control point, left-click anywhere within the potential window", aboutFont12, textColor, POS_CENTRE, textBoxx + 80, textBoxy + 10, 600, 20));
            addChild(new TextAtom("when the Custom potential is selected.", aboutFont12, textColor, POS_CENTRE, textBoxx + 80, textBoxy + 40, 600, 20));
            addChild(new TextAtom("7/10", aboutFont12, textColor, POS_LEFT, textBoxx + textBoxWidth - 75, textBoxy + textBoxHeight - 30, 600, 20));
            // Text box close button
            addChild(new ButtonAtom([&] () {tutorialCounter = 0; updateComponents(tutorialCounter); makeInvisible(); tutorialUI.makeInvisible(); tutorialBlockUI.makeInvisible();}, closeButton, textBoxx+textBoxWidth - 35, textBoxy + 5, 30, 30));
            // Previous button
            addChild(new ButtonAtom([&] () {tutorialCounter--; updateComponents(tutorialCounter);}, previousButton, textBoxx + 5, textBoxy + textBoxHeight - 35, 30, 30));
            // Text box next button
            addChild(new ButtonAtom([&] () {tutorialCounter++; updateComponents(tutorialCounter);}, nextButton, textBoxx + textBoxWidth - 35, textBoxy + textBoxHeight - 35, 30, 30));
            
            tutorialHighlightUI.moveTo(50, 50);
            tutorialHighlightUI.setSize(924, 500);

            
        }
        
        else if(counter == 33){
            
            textBoxx = 200;
            textBoxy = 480;
            textBoxWidth = 774;
            textBoxHeight = 100;
            
            // Text box
            addChild(new RectAtom(textBoxColor, textBoxx, textBoxy, textBoxWidth, textBoxHeight));
            // Text box text
            addChild(new TextAtom("Control points can be moved by holding down left-click on a point and dragging,", aboutFont12, textColor, POS_CENTRE, textBoxx + 80, textBoxy + 10, 600, 20));
            addChild(new TextAtom("and removed by right-clicking on a control point.", aboutFont12, textColor, POS_CENTRE, textBoxx + 80, textBoxy + 40, 600, 20));
            addChild(new TextAtom("8/10", aboutFont12, textColor, POS_LEFT, textBoxx + textBoxWidth - 75, textBoxy + textBoxHeight - 30, 600, 20));
            // Text box close button
            addChild(new ButtonAtom([&] () {tutorialCounter = 0; updateComponents(tutorialCounter); makeInvisible(); tutorialUI.makeInvisible(); tutorialBlockUI.makeInvisible();}, closeButton, textBoxx+textBoxWidth - 35, textBoxy + 5, 30, 30));
            // Previous button
            addChild(new ButtonAtom([&] () {tutorialCounter--; updateComponents(tutorialCounter);}, previousButton, textBoxx + 5, textBoxy + textBoxHeight - 35, 30, 30));
            // Text box next button
            addChild(new ButtonAtom([&] () {tutorialCounter++; updateComponents(tutorialCounter);}, nextButton, textBoxx + textBoxWidth - 35, textBoxy + textBoxHeight - 35, 30, 30));
            
            tutorialHighlightUI.moveTo(50, 50);
            tutorialHighlightUI.setSize(924, 500);
            
        }
        
        else if(counter == 34){
            
            textBoxx = 200;
            textBoxy = 480;
            textBoxWidth = 774;
            textBoxHeight = 100;
            
            // Text box
            addChild(new RectAtom(textBoxColor, textBoxx, textBoxy, textBoxWidth, textBoxHeight));
            // Text box text
            addChild(new TextAtom("All control points can also be removed by clicking the reset button in", aboutFont12, textColor, POS_CENTRE, textBoxx + 80, textBoxy + 10, 600, 20));
            addChild(new TextAtom("the top left corner of the Potentials window.", aboutFont12, textColor, POS_CENTRE, textBoxx + 80, textBoxy + 40, 600, 20));
            addChild(new TextAtom("9/10", aboutFont12, textColor, POS_LEFT, textBoxx + textBoxWidth - 75, textBoxy + textBoxHeight - 30, 600, 20));
            // Text box close button
            addChild(new ButtonAtom([&] () {tutorialCounter = 0; updateComponents(tutorialCounter); makeInvisible(); tutorialUI.makeInvisible(); tutorialBlockUI.makeInvisible();}, closeButton, textBoxx+textBoxWidth - 35, textBoxy + 5, 30, 30));
            // Previous button
            addChild(new ButtonAtom([&] () {tutorialCounter--; updateComponents(tutorialCounter);}, previousButton, textBoxx + 5, textBoxy + textBoxHeight - 35, 30, 30));
            // Text box next button
            addChild(new ButtonAtom([&] () {tutorialCounter++; updateComponents(tutorialCounter);}, nextButton, textBoxx + textBoxWidth - 35, textBoxy + textBoxHeight - 35, 30, 30));
            
            tutorialHighlightUI.moveTo(50, 50);
            tutorialHighlightUI.setSize(924, 500);
            
        }
        
        else if(counter == 35){
            
            textBoxx = 200;
            textBoxy = 480;
            textBoxWidth = 774;
            textBoxHeight = 100;
            
            // Text box
            addChild(new RectAtom(textBoxColor, textBoxx, textBoxy, textBoxWidth, textBoxHeight));
            // Text box text
            addChild(new TextAtom("That concludes the basics of Argon. More information about the program can be", aboutFont12, textColor, POS_CENTRE, textBoxx + 80, textBoxy + 10, 600, 20));
            addChild(new TextAtom("found by left-clicking on the About button on the main menu.", aboutFont12, textColor, POS_CENTRE, textBoxx + 80, textBoxy + 40, 600, 20));
            addChild(new TextAtom("10/10", aboutFont12, textColor, POS_LEFT, textBoxx + textBoxWidth - 75, textBoxy + textBoxHeight - 30, 600, 20));
            // Text box close button
            addChild(new ButtonAtom([&] () {tutorialCounter = 0; updateComponents(tutorialCounter); makeInvisible(); tutorialUI.makeInvisible(); tutorialBlockUI.makeInvisible();}, closeButton, textBoxx+textBoxWidth - 35, textBoxy + 5, 30, 30));
            // Previous button
            addChild(new ButtonAtom([&] () {tutorialCounter--; updateComponents(tutorialCounter);}, previousButton, textBoxx + 5, textBoxy + textBoxHeight - 35, 30, 30));

            tutorialHighlightUI.moveTo(50, 50);
            tutorialHighlightUI.setSize(924, 500);
            
        }

   
        
        
    }
    
    // Keeps track of where the user is in the tutorial
    int TutorialContainer::getTutorialCounter(){return tutorialCounter;}
    // Causes the next part of the tutorial to run
    void TutorialContainer::incrementCounter(){tutorialCounter++;}
    
    // Used to pass mouse events from tutorialHighlightUI to the TutorialContainer
    TutorialHighlightAtom::TutorialHighlightAtom(int x, int y, int width, int height, TutorialContainer* _tutorialContainer, UIContainer& tutorialHighlightUI) : UIAtom(x, y, width, height), tutorialContainer(_tutorialContainer), tutorialHighlightUI(tutorialHighlightUI){
    }
    
    void TutorialHighlightAtom::render(){
    }
    
    bool TutorialHighlightAtom::mousePressed(int x, int y, int button){
        bool inside = getRect().inside(x, y);
        if ((tutorialContainer->getTutorialCounter() == 4 || tutorialContainer->getTutorialCounter() == 11 ||
             tutorialContainer->getTutorialCounter()==19 || tutorialContainer->getTutorialCounter()==25) && bounds.inside(x,y)){
            tutorialContainer->incrementCounter();
            tutorialContainer->updateComponents(tutorialContainer->getTutorialCounter());
        }
        return inside;
    }
    

    
    
    
    
    
}
