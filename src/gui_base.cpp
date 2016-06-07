//
//  gui_base.cpp
//  StarredMD
//
//  Created by Staszek Welsh on 20/05/2016.
//
//

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
    
    // Default handling of mouse events is to do nothing
    void UIBase::mouseMoved(int x, int y) {}
    void UIBase::mousePressed(int x, int y, int button) {}
    void UIBase::mouseReleased(int x, int y, int button) {}
    
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
    
    // move container and all children
    void UIContainer::moveBy(coord offset) {
        bounds.moveBy(offset);
        passCallToChildren(&UIBase::moveBy, offset);
    }
    
    // templated function to pass an arbitrary function call and its arguments to each child element
    // just some trivial templating, easy stuff really...
    template<typename T, typename ...Args>
    void UIContainer::passCallToChildren(T (UIBase::*func)(Args...), Args ... args) {
        for (int i = 0; i < children.size(); ++i) {
            (children[i]->*func)(std::forward<Args>(args)...);
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
    
    // set visibility flag and also pass call through to children
    
    void UIContainer::makeVisible() {
        visible = true;
        passCallToChildren(&UIBase::makeVisible);
    }
    
    void UIContainer::makeInvisible() {
        visible = false;
        passCallToChildren(&UIBase::makeInvisible);
    }
    
    void UIContainer::toggleVisible() {
        visible = not visible;
        passCallToChildren(&UIBase::toggleVisible);
    }

    // remainder of methods just pass the call through to its children
    
    void UIContainer::draw() { passCallToChildren(&UIBase::draw); }
    void UIContainer::mouseMoved(int x, int y) { passCallToChildren(&UIBase::mouseMoved, x, y); }
    void UIContainer::mousePressed(int x, int y, int button) { passCallToChildren(&UIBase::mousePressed, x, y, button); }
    void UIContainer::mouseReleased(int x, int y, int button) { passCallToChildren(&UIBase::mouseReleased, x, y, button); }
    
}
