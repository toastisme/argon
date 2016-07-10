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

#include "gui_base.hpp"

namespace gui {
    
    /*
        UIBase
     */
    
    // Default constructor: initialise position and size to 0, 0 and parent to NULL
    UIBase::UIBase() : visible(false) { bounds.setXYWH(0, 0, 0, 0); }
    
    // Overloaded constructor: initialise position to x, y; size and origin to 0, 0
    UIBase::UIBase(double x, double y, double width, double height) : visible(true) { bounds.setXYWH(x, y, width, height); }
    
    // Default destructor: no memory needs freeing, so do nothing
    UIBase::~UIBase() {}
    
    // return bounds
    const rect UIBase::getRect() const { return bounds; }
    
    // move by offset
    void UIBase::moveBy(coord offset) { bounds.moveBy(offset); }
    
    // getter and setters for visibility flag
    bool UIBase::getVisible() const { return visible; }
    void UIBase::makeVisible()   { visible = true; }
    void UIBase::makeInvisible() { visible = false; }
    void UIBase::toggleVisible() { visible = not visible; }
    
    // Default handling of mouse events is to return false (mouse event not handled)
    bool UIBase::mouseMoved(int x, int y) { return false; }
    bool UIBase::mousePressed(int x, int y, int button) { return false; }
    bool UIBase::mouseReleased(int x, int y, int button) { return false; }
    
    /*
        UIAtom
     */
    
    // Default constructor: start invisible since no position was given
    UIAtom::UIAtom() {}
    
    // Overloaded constructor: start visible, pass pos through to base
    UIAtom::UIAtom(double x, double y, double width, double height) : UIBase(x, y, width, height) {}
    
    // draw checks if visible, and then calls render()
    void UIAtom::draw() {
        if (visible) { render(); }
    }
    
    /*
        Container
     */
    
    // Default constructor
    UIContainer::UIContainer() : UIBase() {}
    
    // Overloaded constructor: set position
    UIContainer::UIContainer(double x, double y, double width, double height) : UIBase(x, y, width, height) {}
    
    // Destructor: delete all children
    UIContainer::~UIContainer() {
        for (int i = 0; i < children.size(); ++i) {
            delete children[i];
        }
    }
    
    // Add a child: move it so that its coordinates are given relative to the top-left corner of the container
    // then add to the vector of children
    // i.e. a child originally positioned at (100, 100) relative to the top-left corner of the screen becomes
    // positioned at (100, 100) relative to the top-left corner of bounds
    void UIContainer::addChild(UIBase *child) {
        child->moveBy(bounds.getPos(POS_TOP_LEFT));
        children.push_back(child);
    }
    
    // Add a child and index it, returning the index
    int UIContainer::addIndexedChild(gui::UIBase *child) {
        addChild(child);
        indexedChildren.push_back(child);
        return indexedChildren.size()-1;
    }
    
    // Return a pointer to the ith child
    UIBase* UIContainer::getChild(int i) {
        return indexedChildren.at(i);
    }
    
    // move container and all children
    void UIContainer::moveBy(coord offset) {
        bounds.moveBy(offset);
        for (int i = 0; i < children.size(); ++i) { children[i]->moveBy(offset); }
    }
    
    // set visibility flag and also pass call through to children
    void UIContainer::makeVisible() {
        visible = true;
        for (int i = 0; i < children.size(); ++i) { children[i]->makeVisible(); }
    }
    
    void UIContainer::makeInvisible() {
        visible = false;
        for (int i = 0; i < children.size(); ++i) { children[i]->makeInvisible(); }
    }
    
    void UIContainer::toggleVisible() {
        visible = not visible;
        for (int i = 0; i < children.size(); ++i) { children[i]->toggleVisible(); }
    }

    // draw just passes the call through to its children
    void UIContainer::draw() {
        for (int i = 0; i < children.size(); ++i) { children[i]->draw(); }
    }
    
    // mouse events by default only pass through the event to the first child to handle them
    bool UIContainer::mouseMoved(int x, int y) {
        bool handled = false;
        for (int i = 0; i < children.size(); ++i) {
            handled = children[i]->mouseMoved(x, y);
            if (handled) { break; }
        }
        return handled;
    }
    
    bool UIContainer::mousePressed(int x, int y, int button) {
        bool handled = false;
        for (int i = 0; i < children.size(); ++i) {
            handled = children[i]->mousePressed(x, y, button);
            if (handled) { break; }
        }
        return handled;
    }
    
    bool UIContainer::mouseReleased(int x, int y, int button) {
        bool handled = false;
        for (int i = 0; i < children.size(); ++i) {
            handled = children[i]->mouseReleased(x, y, button);
            if (handled) { break; }
        }
        return handled;
    }
}
