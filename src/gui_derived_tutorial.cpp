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
    
    TutorialContainer::TutorialContainer(int x, int y, int width, int height, int textBoxx, int textBoxy, int textBoxWidth, int textBoxHeight, ofTrueTypeFont &font, ofImage &nextButton, ofImage &closeButton, UIContainer &tutorialUI) : aboutFont12(font), nextButton(nextButton), closeButton(closeButton), tutorialUI(tutorialUI), UIContainer(x, y, width, height){
        
        bool highlighted = false;
        int tutorialCounter = 0;
        
        ofColor textBoxColor = ofColor(0, 0, 0, 100);
        ofColor textColor = ofColor(255, 255, 240);
        ofColor notHighlightedColor = ofColor(150, 150, 150, 80);
        ofColor highlightedColor = ofColor(255,0,0);
        
        // Text box next button
        addIndexedChild(new ButtonAtom([&] () {tutorialCounter++;}, nextButton, textBoxx + textBoxWidth - 35, textBoxy + textBoxHeight - 35, 30, 30));
        // Text box close button
        addIndexedChild(new ButtonAtom([&] () {tutorialCounter = 1; makeInvisible(); tutorialUI.makeInvisible();}, closeButton, textBoxx+textBoxWidth - 35, textBoxy + 5, 30, 30));
        // Faded area
        addIndexedChild(new RectAtom(notHighlightedColor, x, y, width, height));
        // Text box
        addIndexedChild(new RectAtom(textBoxColor, textBoxx, textBoxy, textBoxWidth, textBoxHeight));
        // Text box text
        // Highlight button
        // Highlight area
        addIndexedChild(new RectAtom(highlightedColor, 500, 500, 50, 50));
        //addIndexedChild(new ButtonAtom([&] () {tutorialCounter++;}, nextButton, width - 35, height - 35, 30, 30));
        

    }
    
    bool TutorialContainer::CheckHighlight(){return highlighted;}
    void TutorialContainer::SetHighlight(bool highlight){highlighted = highlight;}


}