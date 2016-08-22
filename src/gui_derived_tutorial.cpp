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

// implements TextBoxContainer



namespace gui {
    
    // Default constructor for the text box container
    TextBoxContainer::TextBoxContainer(int x, int y, int width, int height, int tutorialCounter, ofTrueTypeFont &font, ofImage &nextButton, ofImage &closeButton) : aboutFont12(font), nextButton(nextButton), closeButton(closeButton), UIContainer(x, y, width, height){
        
        addIndexedChild(new ButtonAtom([&] () {tutorialCounter++;}, nextButton, width - 50, height - 50, 30, 30));
        addIndexedChild(new ButtonAtom([&] () {tutorialCounter = 1;}, closeButton, width - 50, 50, 30, 30));
        addIndexedChild(new RectAtom(ofColor(0, 0, width, height), 250, 0, 774, 160));

        
    }
    /*

     
    void TextBoxContainer::MoveTextBox(tutorialCounter){
         if (tutorialCounter == 1){
            moveby/resize
         }
     
     
     }
*/

}