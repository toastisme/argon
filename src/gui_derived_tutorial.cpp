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
    
    TutorialContainer::TutorialContainer(int x, int y, int width, int height, ofTrueTypeFont &font, ofImage &nextButton, ofImage &previousButton, ofImage &closeButton, UIContainer &tutorialUI, UIContainer &tutorialHighlightUI) : aboutFont12(font), nextButton(nextButton), previousButton(previousButton), closeButton(closeButton), tutorialUI(tutorialUI), tutorialHighlightUI(tutorialHighlightUI), UIContainer(x, y, width, height){
        
        updateComponents(tutorialCounter);
    }
    
    void TutorialContainer::updateComponents(int counter){
        
        // All children in the TutorialContainer are deleted and redrawn in the positions for a given tutorialCounter value
        
        ofColor textBoxColor = ofColor(0, 0, 0, 100);
        ofColor textColor = ofColor(255, 255, 240);
        ofColor notHighlightedColor = ofColor(120, 120, 120, 200);
        
        int size = children.size() - 1;
        for (int i = size; i >= 0; i--){
            delete children[i];
            children.erase(children.begin() + i);}
        
        /*
         Introduction and description of particles
         */
        
        if (counter == 0){
            
            tutorialCounter = 0;
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
            addChild(new TextAtom("This tutorial takes you through the basics of Argon.", aboutFont12, textColor, POS_LEFT, textBoxx + 10, textBoxy + 50, 600, 20));
            addChild(new TextAtom("You can leave the tutorial at any point by left-clicking the x in this text box.", aboutFont12, textColor, POS_LEFT, textBoxx + 10, textBoxy + 80, 600, 20));
            addChild(new TextAtom("1/4", aboutFont12, textColor, POS_LEFT, textBoxx + textBoxWidth - 70, textBoxy + textBoxHeight - 30, 600, 20));
            // Text box close button
            addChild(new ButtonAtom([&] () {tutorialCounter = 0; updateComponents(tutorialCounter); makeInvisible(); tutorialUI.makeInvisible();}, closeButton, textBoxx+textBoxWidth - 35, textBoxy + 5, 30, 30));
            // Text box next button
            addChild(new ButtonAtom([&] () {tutorialCounter++; updateComponents(tutorialCounter);}, nextButton, textBoxx + textBoxWidth - 35, textBoxy + textBoxHeight - 35, 30, 30));
            
        }
        
        
        else if (counter == 1){
            // Faded area
            addChild(new RectAtom(notHighlightedColor, 0, 0, 250, 410));
            // Text box
            addChild(new RectAtom(textBoxColor, textBoxx, textBoxy, textBoxWidth, textBoxHeight));
            // Text box text
            addChild(new TextAtom("The Colours of the particles indicate their velocity, and changes in shape", aboutFont12, textColor, POS_LEFT, textBoxx + 10, textBoxy + 50, 600, 20));
            
            addChild(new TextAtom("show forces acting on them.", aboutFont12, textColor, POS_LEFT, textBoxx + 10, textBoxy + 80, 600, 20));
            
            
            addChild(new TextAtom("2/4", aboutFont12, textColor, POS_LEFT, textBoxx + textBoxWidth - 70, textBoxy + textBoxHeight - 30, 600, 20));
            // Text box close button
            addChild(new ButtonAtom([&] () {tutorialCounter = 0; updateComponents(tutorialCounter); makeInvisible(); tutorialUI.makeInvisible();}, closeButton, textBoxx+textBoxWidth - 35, textBoxy + 5, 30, 30));
            // Text box next button
            addChild(new ButtonAtom([&] () {tutorialCounter++; updateComponents(tutorialCounter);}, nextButton, textBoxx + textBoxWidth - 35, textBoxy + textBoxHeight - 35, 30, 30));
            // Text box previous button
            addChild(new ButtonAtom([&] () {tutorialCounter--; updateComponents(tutorialCounter);}, previousButton, textBoxx + 5, textBoxy + textBoxHeight - 35, 30, 30));
        }
        
        else if (counter == 2){
            // Faded area
            addChild(new RectAtom(notHighlightedColor, 0, 0, 250, 410));
            // Text box
            addChild(new RectAtom(textBoxColor, textBoxx, textBoxy, textBoxWidth, textBoxHeight));
            // Text box text
            addChild(new TextAtom("When particles collide, the colour change shows the transfer of momentum in real time.", aboutFont12, textColor, POS_LEFT, textBoxx + 10, textBoxy + 50, 600, 20));
            addChild(new TextAtom("3/4", aboutFont12, textColor, POS_LEFT, textBoxx + textBoxWidth - 70, textBoxy + textBoxHeight - 30, 600, 20));
            // Text box close button
            addChild(new ButtonAtom([&] () {tutorialCounter = 0; updateComponents(tutorialCounter); makeInvisible(); tutorialUI.makeInvisible();}, closeButton, textBoxx+textBoxWidth - 35, textBoxy + 5, 30, 30));
            // Text box next button
            addChild(new ButtonAtom([&] () {tutorialCounter++; updateComponents(tutorialCounter);}, nextButton, textBoxx + textBoxWidth - 35, textBoxy + textBoxHeight - 35, 30, 30));
            // Text box previous button
            addChild(new ButtonAtom([&] () {tutorialCounter--; updateComponents(tutorialCounter);}, previousButton, textBoxx + 5, textBoxy + textBoxHeight - 35, 30, 30));

        }
        
        else if (counter == 3){
            // Faded area
            addChild(new RectAtom(notHighlightedColor, 0, 0, 250, 45));
            addChild(new RectAtom(notHighlightedColor, 0, 85, 250, 325));
            // Text box
            addChild(new RectAtom(textBoxColor, textBoxx, textBoxy, textBoxWidth, textBoxHeight));
            // Text box text
            addChild(new TextAtom("To interact with the system, left click on the controls button.", aboutFont12, textColor, POS_LEFT, textBoxx + 10, textBoxy + 50, 600, 20));
            addChild(new TextAtom("4/4", aboutFont12, textColor, POS_LEFT, textBoxx + textBoxWidth - 70, textBoxy + textBoxHeight - 30, 600, 20));
            tutorialHighlightUI.moveTo(200,50);
            // Text box close button
            addChild(new ButtonAtom([&] () {tutorialCounter = 0; updateComponents(tutorialCounter); makeInvisible(); tutorialUI.makeInvisible();}, closeButton, textBoxx+textBoxWidth - 35, textBoxy + 5, 30, 30));
            // Previous button
            addChild(new ButtonAtom([&] () {tutorialCounter--; updateComponents(tutorialCounter);}, previousButton, textBoxx + 5, textBoxy + textBoxHeight - 35, 30, 30));
            
        }
        
        /*
         Controls
         */
        
        else if(counter == 4){
            
            textBoxy = 150;

            // Faded area
            addChild(new RectAtom(notHighlightedColor, 0, 0, 250, 410));
            // Text box
            addChild(new RectAtom(textBoxColor, textBoxx, textBoxy, textBoxWidth, textBoxHeight));
            // Text box text
            addChild(new TextAtom("Properties of the system are selected by left-clicking, and are controlled by the slider.", aboutFont12, textColor, POS_LEFT, textBoxx + 10, textBoxy + 50, 600, 20));
            addChild(new TextAtom("1/7", aboutFont12, textColor, POS_LEFT, textBoxx + textBoxWidth - 70, textBoxy + textBoxHeight - 30, 600, 20));
            // Text box close button
            addChild(new ButtonAtom([&] () {tutorialCounter = 0; updateComponents(tutorialCounter); makeInvisible(); tutorialUI.makeInvisible();}, closeButton, textBoxx+textBoxWidth - 35, textBoxy + 5, 30, 30));
            // Text box next button
            addChild(new ButtonAtom([&] () {tutorialCounter++; updateComponents(tutorialCounter);}, nextButton, textBoxx + textBoxWidth - 35, textBoxy + textBoxHeight - 35, 30, 30));
            

        }
        
        else if(counter == 5){
            // Faded area
            addChild(new RectAtom(notHighlightedColor, 0, 0, 250, 410));
            // Text box
            addChild(new RectAtom(textBoxColor, textBoxx, textBoxy, textBoxWidth, textBoxHeight));
            // Text box text
            addChild(new TextAtom("Changing the temperature alters the kinetic energy of the particles,", aboutFont12, textColor, POS_LEFT, textBoxx + 10, textBoxy + 50, 600, 20));
            addChild(new TextAtom("with higher temperatures corresponding to higher velocities.", aboutFont12, textColor, POS_LEFT, textBoxx + 10, textBoxy + 80, 600, 20));
            addChild(new TextAtom("2/7", aboutFont12, textColor, POS_LEFT, textBoxx + textBoxWidth - 70, textBoxy + textBoxHeight - 30, 600, 20));
            // Text box close button
            addChild(new ButtonAtom([&] () {tutorialCounter = 0; updateComponents(tutorialCounter); makeInvisible(); tutorialUI.makeInvisible();}, closeButton, textBoxx+textBoxWidth - 35, textBoxy + 5, 30, 30));
            // Text box next button
            addChild(new ButtonAtom([&] () {tutorialCounter++; updateComponents(tutorialCounter);}, nextButton, textBoxx + textBoxWidth - 35, textBoxy + textBoxHeight - 35, 30, 30));
            // Previous button
            addChild(new ButtonAtom([&] () {tutorialCounter--; updateComponents(tutorialCounter);}, previousButton, textBoxx + 5, textBoxy + textBoxHeight - 35, 30, 30));
            

            tutorialHighlightUI.moveTo(250,0);
            tutorialHighlightUI.setSize(774, 160);

        }
        
        else if(counter == 6){
            
            // Faded area
            addChild(new RectAtom(notHighlightedColor, 0, 0, 250, 410));
            // Text box
            addChild(new RectAtom(textBoxColor, textBoxx, textBoxy, textBoxWidth, textBoxHeight));
            // Text box text
            addChild(new TextAtom("At low temperatures, the particles will crystallise.", aboutFont12, textColor, POS_LEFT, textBoxx + 10, textBoxy + 50, 600, 20));
            addChild(new TextAtom("3/7", aboutFont12, textColor, POS_LEFT, textBoxx + textBoxWidth - 70, textBoxy + textBoxHeight - 30, 600, 20));
            // Text box close button
            addChild(new ButtonAtom([&] () {tutorialCounter = 0; updateComponents(tutorialCounter); makeInvisible(); tutorialUI.makeInvisible();}, closeButton, textBoxx+textBoxWidth - 35, textBoxy + 5, 30, 30));
            // Text box next button
            addChild(new ButtonAtom([&] () {tutorialCounter++; updateComponents(tutorialCounter);}, nextButton, textBoxx + textBoxWidth - 35, textBoxy + textBoxHeight - 35, 30, 30));
            // Previous button
            addChild(new ButtonAtom([&] () {tutorialCounter--; updateComponents(tutorialCounter);}, previousButton, textBoxx + 5, textBoxy + textBoxHeight - 35, 30, 30));
            
        }
        
        else if(counter == 7){
            
            // Faded area
            addChild(new RectAtom(notHighlightedColor, 0, 0, 250, 410));
            // Text box
            addChild(new RectAtom(textBoxColor, textBoxx, textBoxy, textBoxWidth, textBoxHeight));
            // Text box text
            addChild(new TextAtom("Changing the number of particles resets the system, with the new number", aboutFont12, textColor, POS_LEFT, textBoxx + 10, textBoxy + 50, 600, 20));
            addChild(new TextAtom("of particles arranged on a grid.", aboutFont12, textColor, POS_LEFT, textBoxx + 10, textBoxy + 80, 600, 20));
            addChild(new TextAtom("4/7", aboutFont12, textColor, POS_LEFT, textBoxx + textBoxWidth - 70, textBoxy + textBoxHeight - 30, 600, 20));
            // Text box close button
            addChild(new ButtonAtom([&] () {tutorialCounter = 0; updateComponents(tutorialCounter); makeInvisible(); tutorialUI.makeInvisible();}, closeButton, textBoxx+textBoxWidth - 35, textBoxy + 5, 30, 30));
            // Text box next button
            addChild(new ButtonAtom([&] () {tutorialCounter++; updateComponents(tutorialCounter);}, nextButton, textBoxx + textBoxWidth - 35, textBoxy + textBoxHeight - 35, 30, 30));
            // Previous button
            addChild(new ButtonAtom([&] () {tutorialCounter--; updateComponents(tutorialCounter);}, previousButton, textBoxx + 5, textBoxy + textBoxHeight - 35, 30, 30));
            
        }
        
        else if(counter == 8){
            
            // Faded area
            addChild(new RectAtom(notHighlightedColor, 0, 0, 250, 410));
            // Text box
            addChild(new RectAtom(textBoxColor, textBoxx, textBoxy, textBoxWidth, textBoxHeight));
            // Text box text
            addChild(new TextAtom("The simulation speed controls how fast the simulation runs.", aboutFont12, textColor, POS_LEFT, textBoxx + 10, textBoxy + 50, 600, 20));
            addChild(new TextAtom("5/7", aboutFont12, textColor, POS_LEFT, textBoxx + textBoxWidth - 70, textBoxy + textBoxHeight - 30, 600, 20));
            // Text box close button
            addChild(new ButtonAtom([&] () {tutorialCounter = 0; updateComponents(tutorialCounter); makeInvisible(); tutorialUI.makeInvisible();}, closeButton, textBoxx+textBoxWidth - 35, textBoxy + 5, 30, 30));
            // Text box next button
            addChild(new ButtonAtom([&] () {tutorialCounter++; updateComponents(tutorialCounter);}, nextButton, textBoxx + textBoxWidth - 35, textBoxy + textBoxHeight - 35, 30, 30));
            // Previous button
            addChild(new ButtonAtom([&] () {tutorialCounter--; updateComponents(tutorialCounter);}, previousButton, textBoxx + 5, textBoxy + textBoxHeight - 35, 30, 30));
            
        }
        
        else if(counter == 9){
            
            // Faded area
            addChild(new RectAtom(notHighlightedColor, 0, 0, 250, 410));
            // Text box
            addChild(new RectAtom(textBoxColor, textBoxx, textBoxy, textBoxWidth, textBoxHeight));
            // Text box text
            addChild(new TextAtom("The gaussians button gives control of gaussians placed in the system.", aboutFont12, textColor, POS_LEFT, textBoxx + 10, textBoxy + 50, 600, 20));
            addChild(new TextAtom("6/7", aboutFont12, textColor, POS_LEFT, textBoxx + textBoxWidth - 70, textBoxy + textBoxHeight - 30, 600, 20));
            // Text box close button
            addChild(new ButtonAtom([&] () {tutorialCounter = 0; updateComponents(tutorialCounter); makeInvisible(); tutorialUI.makeInvisible();}, closeButton, textBoxx+textBoxWidth - 35, textBoxy + 5, 30, 30));
            // Text box next button
            addChild(new ButtonAtom([&] () {tutorialCounter++; updateComponents(tutorialCounter);}, nextButton, textBoxx + textBoxWidth - 35, textBoxy + textBoxHeight - 35, 30, 30));
            // Previous button
            addChild(new ButtonAtom([&] () {tutorialCounter--; updateComponents(tutorialCounter);}, previousButton, textBoxx + 5, textBoxy + textBoxHeight - 35, 30, 30));
            
        }
        
        else if(counter == 10){
            
            // Faded area
            addChild(new RectAtom(notHighlightedColor, 0, 0, 250, 410));
            // Text box
            addChild(new RectAtom(textBoxColor, textBoxx, textBoxy, textBoxWidth, textBoxHeight));
            // Text box text
            addChild(new TextAtom("These are added to the system by left-clicking on the screen.", aboutFont12, textColor, POS_LEFT, textBoxx + 10, textBoxy + 50, 600, 20));
            addChild(new TextAtom("Try it!", aboutFont12, textColor, POS_LEFT, textBoxx + 10, textBoxy + 80, 600, 20));
            addChild(new TextAtom("7/7", aboutFont12, textColor, POS_LEFT, textBoxx + textBoxWidth - 70, textBoxy + textBoxHeight - 30, 600, 20));
            // Text box close button
            addChild(new ButtonAtom([&] () {tutorialCounter = 0; updateComponents(tutorialCounter); makeInvisible(); tutorialUI.makeInvisible();}, closeButton, textBoxx+textBoxWidth - 35, textBoxy + 5, 30, 30));
            // Previous button
            addChild(new ButtonAtom([&] () {tutorialCounter--; updateComponents(tutorialCounter);}, previousButton, textBoxx + 5, textBoxy + textBoxHeight - 35, 30, 30));
            
            tutorialHighlightUI.moveTo(250, 410);
            tutorialHighlightUI.setSize(774, 190);
            
        }
        
        else if(counter == 11){
            
            // Faded area
            addChild(new RectAtom(notHighlightedColor, 0, 0, 250, 410));
            // Text box
            addChild(new RectAtom(textBoxColor, textBoxx, textBoxy, textBoxWidth, textBoxHeight));
            // Text box text
            addChild(new TextAtom("These are added to the system by left-clicking on the screen.", aboutFont12, textColor, POS_LEFT, textBoxx + 10, textBoxy + 50, 600, 20));
            addChild(new TextAtom("1/7", aboutFont12, textColor, POS_LEFT, textBoxx + textBoxWidth - 70, textBoxy + textBoxHeight - 30, 600, 20));
            // Text box close button
            addChild(new ButtonAtom([&] () {tutorialCounter = 0; updateComponents(tutorialCounter); makeInvisible(); tutorialUI.makeInvisible();}, closeButton, textBoxx+textBoxWidth - 35, textBoxy + 5, 30, 30));
            // Text box next button
            addChild(new ButtonAtom([&] () {tutorialCounter++; updateComponents(tutorialCounter);}, nextButton, textBoxx + textBoxWidth - 35, textBoxy + textBoxHeight - 35, 30, 30));
       
            tutorialHighlightUI.moveTo(250, 0);
            tutorialHighlightUI.setSize(774, 600);
            
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
        if ((tutorialContainer->getTutorialCounter() == 3 || tutorialContainer->getTutorialCounter() == 10) && bounds.inside(x,y)){
            tutorialContainer->incrementCounter();
            tutorialContainer->updateComponents(tutorialContainer->getTutorialCounter());
            
        }
    }
    
    
    
    
}
