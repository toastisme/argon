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

// implements TutorialAtom



namespace gui {
    
    TutorialAtom::TutorialAtom(int x, int y, int width, int height) : UIAtom(x, y, width, height){}
    
    void TutorialAtom::render(){
        int section = 1;
        int slide = 1;
        ofColor textcolor = ofColor(255, 255, 240);
        
        //myFont = new ofTrueTypeFont();
        //myFont->loadFont("fonts/Montserrat-Bold.ttf", 12);
        
        ofSetColor(80, 80, 80, 150);
        ofDrawRectangle(0, 0, 1024 , 600);
        
        /*
        TextAtom("This tutorial takes you through the basics of Argon.",
                 "fonts/Tahoma.ttf", textcolor, POS_LEFT, 260, 0, 205, 120);
     
 
         tutorialUI.addChild(new gui::RectAtom(ofColor(80, 80, 80, 150), 0, 0, screenWidth, screenHeight));
         tutorialUI.addChild(new gui::RectAtom(ofColor(0, 0, 0, 80), 250, 0, 510, 160));
         tutorialUI.addChild(new gui::TextAtom("This tutorial takes you through the basics of Argon.", aboutFont12, textcolor, POS_LEFT, 260, 0, 205, 120));
         tutorialUI.addChild(new gui::TextAtom("You can leave at any point by left-clicking the x in this text box.", aboutFont12, textcolor, POS_LEFT, 260, 25, 205, 135));
         tutorialUI.addChild(new gui::ButtonAtom([&] () { tutorialUI.makeInvisible(); }, closeButton,725, 5, 30, 30));
         tutorialUI.addChild(new gui::ButtonAtom([&] () { tutorialUI.makeInvisible(); }, nextButton,725, 125, 30, 30));
         */
        
        
    }
    
}