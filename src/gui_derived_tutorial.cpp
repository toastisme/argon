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
    
    TutorialContainer::TutorialContainer(int x, int y, int width, int height, ofTrueTypeFont &font, ofImage &nextButton, ofImage &closeButton, UIContainer &tutorialUI) : aboutFont12(font), nextButton(nextButton), closeButton(closeButton), tutorialUI(tutorialUI), UIContainer(x, y, width, height){
        
        tutorialCounter = 0;
        textBoxx = 250;
        textBoxy = 0;
        textBoxWidth = 774;
        textBoxHeight = 100;
        
        ofColor textBoxColor = ofColor(0, 255, 0, 100);
        ofColor textColor = ofColor(255, 255, 240);
        ofColor notHighlightedColor = ofColor(150, 150, 150, 80);
        
        // Text box close button
        addChild(new ButtonAtom([&] () {tutorialCounter = 0; makeInvisible(); tutorialUI.makeInvisible();}, closeButton, textBoxx+textBoxWidth - 35, textBoxy + 5, 30, 30));
        // Text box next button
        addChild(new ButtonAtom([&] () {tutorialCounter++; moveComponents(tutorialCounter);}, nextButton, textBoxx + textBoxWidth - 35, textBoxy + textBoxHeight - 35, 30, 30));
        // Faded area
        addChild(new RectAtom(notHighlightedColor, x, y, width, height));
        // Text box
        addChild(new RectAtom(textBoxColor, textBoxx, textBoxy, textBoxWidth, textBoxHeight));
        // Text box text
        addChild(new TextAtom("This tutorial takes you through the basics of Argon.", aboutFont12, textColor, POS_LEFT, textBoxx + 10, textBoxy + 10, 600, 20));
        addChild(new TextAtom("You can leave the tutorial at any point by left-clicking the x in this text box.", aboutFont12, textColor, POS_LEFT, textBoxx + 10, textBoxy + 40, 600, 20));
        
    }
    
    void TutorialContainer::moveComponents(int counter){
        int size = children.size() - 1;
        for (int i = size; i > 0; i--){
            delete children[i];
            children.erase(children.begin() + i);}
        
        if (counter == 1){


        }
    }
    
    
    
}