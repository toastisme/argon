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



namespace gui {
    
    TutorialContainer::TutorialContainer(int x, int y, int width, int height, ofTrueTypeFont &font, ofImage &nextButton, ofImage &previousButton, ofImage &closeButton, UIContainer &tutorialUI, UIContainer &tutorialHighlightUI) : aboutFont12(font), nextButton(nextButton), previousButton(previousButton), closeButton(closeButton), tutorialUI(tutorialUI), tutorialHighlightUI(tutorialHighlightUI), UIContainer(x, y, width, height){
        
        tutorialCounter = 0;
        textBoxx = 250;
        textBoxy = 0;
        textBoxWidth = 774;
        textBoxHeight = 150;
        
        ofColor textBoxColor = ofColor(0, 255, 0, 100);
        ofColor textColor = ofColor(255, 255, 240);
        ofColor notHighlightedColor = ofColor(150, 150, 150, 80);
        
        // Text box close button
        addChild(new ButtonAtom([&] () {tutorialCounter = 0; updateComponents(tutorialCounter); makeInvisible(); tutorialUI.makeInvisible();}, closeButton, textBoxx+textBoxWidth - 35, textBoxy + 5, 30, 30));
        // Text box next button
        addChild(new ButtonAtom([&] () {tutorialCounter++; updateComponents(tutorialCounter);}, nextButton, textBoxx + textBoxWidth - 35, textBoxy + textBoxHeight - 35, 30, 30));
        // Faded area
        addChild(new RectAtom(notHighlightedColor, x, y, width, height));
        // Text box
        addChild(new RectAtom(textBoxColor, textBoxx, textBoxy, textBoxWidth, textBoxHeight));
        // Text box text
        addChild(new TextAtom("This tutorial takes you through the basics of Argon.", aboutFont12, textColor, POS_LEFT, textBoxx + 10, textBoxy + 50, 600, 20));
        addChild(new TextAtom("You can leave the tutorial at any point by left-clicking the x in this text box.", aboutFont12, textColor, POS_LEFT, textBoxx + 10, textBoxy + 80, 600, 20));
        addChild(new TextAtom("1/3", aboutFont12, textColor, POS_LEFT, textBoxx + textBoxWidth - 65, textBoxy + textBoxHeight - 30, 600, 20));
    }
    
    void TutorialContainer::updateComponents(int counter){
        
        ofColor textBoxColor = ofColor(0, 255, 0, 100);
        ofColor textColor = ofColor(255, 255, 240);
        ofColor notHighlightedColor = ofColor(150, 150, 150, 80);
        
        int size = children.size() - 1;
        for (int i = size; i > 0; i--){
            delete children[i];
            children.erase(children.begin() + i);}
        
        
        if (counter == 0){
            // Text box close button
            addChild(new ButtonAtom([&] () {tutorialCounter = 0; updateComponents(tutorialCounter); makeInvisible(); tutorialUI.makeInvisible();}, closeButton, textBoxx+textBoxWidth - 35, textBoxy + 5, 30, 30));
            // Text box next button
            addChild(new ButtonAtom([&] () {tutorialCounter++; updateComponents(tutorialCounter);}, nextButton, textBoxx + textBoxWidth - 35, textBoxy + textBoxHeight - 35, 30, 30));
            // Faded area
            addChild(new RectAtom(notHighlightedColor, 0, 0, 1024, 600));
            // Text box
            addChild(new RectAtom(textBoxColor, textBoxx, textBoxy, textBoxWidth, textBoxHeight));
            // Text box text
            addChild(new TextAtom("This tutorial takes you through the basics of Argon.", aboutFont12, textColor, POS_LEFT, textBoxx + 10, textBoxy + 50, 600, 20));
            addChild(new TextAtom("You can leave the tutorial at any point by left-clicking the x in this text box.", aboutFont12, textColor, POS_LEFT, textBoxx + 10, textBoxy + 80, 600, 20));
            addChild(new TextAtom("1/3", aboutFont12, textColor, POS_LEFT, textBoxx + textBoxWidth - 65, textBoxy + textBoxHeight - 30, 600, 20));
            
        }
        
        
        else if (counter == 1){
            // Text box close button
            addChild(new ButtonAtom([&] () {tutorialCounter = 0; updateComponents(tutorialCounter); makeInvisible(); tutorialUI.makeInvisible();}, closeButton, textBoxx+textBoxWidth - 35, textBoxy + 5, 30, 30));
            // Text box next button
            addChild(new ButtonAtom([&] () {tutorialCounter++; updateComponents(tutorialCounter);}, nextButton, textBoxx + textBoxWidth - 35, textBoxy + textBoxHeight - 35, 30, 30));
            addChild(new ButtonAtom([&] () {tutorialCounter--; updateComponents(tutorialCounter);}, previousButton, textBoxx + 5, textBoxy + textBoxHeight - 35, 30, 30));
            // Faded area
            addChild(new RectAtom(notHighlightedColor, 0, 0, 250, 410));
            // Text box
            addChild(new RectAtom(textBoxColor, textBoxx, textBoxy, textBoxWidth, textBoxHeight));
            // Text box text
            addChild(new TextAtom("Colours of the particles indicate their speed and changes in shape show forces acting on them.", aboutFont12, textColor, POS_LEFT, textBoxx + 10, textBoxy + 50, 600, 20));
            addChild(new TextAtom("When particles collide, the colour change shows the transfer of momentum in real time.", aboutFont12, textColor, POS_LEFT, textBoxx + 10, textBoxy + 80, 600, 20));
            addChild(new TextAtom("2/3", aboutFont12, textColor, POS_LEFT, textBoxx + textBoxWidth - 65, textBoxy + textBoxHeight - 30, 600, 20));
        }
        
        else if (counter == 2){
            // Text box close button
            addChild(new ButtonAtom([&] () {tutorialCounter = 0; updateComponents(tutorialCounter); makeInvisible(); tutorialUI.makeInvisible();}, closeButton, textBoxx+textBoxWidth - 35, textBoxy + 5, 30, 30));
            addChild(new ButtonAtom([&] () {tutorialCounter--; updateComponents(tutorialCounter);}, previousButton, textBoxx + 5, textBoxy + textBoxHeight - 35, 30, 30));
            // Faded area
            addChild(new RectAtom(notHighlightedColor, 0, 0, 250, 410));
            // Text box
            addChild(new RectAtom(textBoxColor, textBoxx, textBoxy, textBoxWidth, textBoxHeight));
            // Text box text
            addChild(new TextAtom("To interact with the system, left click on the controls button.", aboutFont12, textColor, POS_LEFT, textBoxx + 10, textBoxy + 50, 600, 20));
            addChild(new TextAtom("3/3", aboutFont12, textColor, POS_LEFT, textBoxx + textBoxWidth - 65, textBoxy + textBoxHeight - 30, 600, 20));
            tutorialHighlightUI.moveTo(200,50);
            
        }
        
        
    }
    
    int TutorialContainer::getTutorialCounter(){return tutorialCounter;}
    void TutorialContainer::incrementCounter(){tutorialCounter++;}
    
    
    TutorialHighlightAtom::TutorialHighlightAtom(int x, int y, int width, int height, UIContainer* TutorialContainer) : TutorialContainer(TutorialContainer), UIAtom(x, y, width, height){
    }
    
    bool TutorialHighlightAtom::mousePressed(int x, int y, int button){
        TutorialContainer->incrementcounter();
    }
    
    
    
    
}