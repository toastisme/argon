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
        rect
     */
    
    double rect::width()   const { return right - left; }
    double rect::height()  const { return bottom - top; }
    double rect::centreX() const { return (left + right) / 2.0; }
    double rect::centreY() const { return (top + bottom) / 2.0; }
    
    point rect::getPos(Position position) const {
        point ret;
        
        switch (position) {
            case TOP_LEFT:     { ret.x = left;      ret.y = top;       } break;
            case TOP:          { ret.x = centreX(); ret.y = top;       } break;
            case TOP_RIGHT:    { ret.x = right;     ret.y = top;       } break;
            case LEFT:         { ret.x = left;      ret.y = centreY(); } break;
            case CENTRE:       { ret.x = centreX(); ret.y = centreY(); } break;
            case RIGHT:        { ret.x = right;     ret.y = centreY(); } break;
            case BOTTOM_LEFT:  { ret.x = left;      ret.y = bottom;    } break;
            case BOTTOM:       { ret.x = centreX(); ret.y = bottom;    } break;
            case BOTTOM_RIGHT: { ret.x = right;     ret.y = bottom;    } break;
            default:           { ret.x = 0;         ret.y = 0;         } break;
        }
        
        return ret;
    }
    
    void rect::setVertex(Position position, point pos) {
        switch (position) {
            case TOP_LEFT:     { left  = pos.x; top    = pos.y; } break;
            case TOP_RIGHT:    { right = pos.x; top    = pos.y; } break;
            case BOTTOM_LEFT:  { left  = pos.x; bottom = pos.y; } break;
            case BOTTOM_RIGHT: { right = pos.x; bottom = pos.y; } break;
            default:           { } break;
        }
    }
    
    void rect::setSide(Position position, double pos) {
        switch (position) {
            case TOP:    { top    = pos; } break;
            case LEFT:   { left   = pos; } break;
            case RIGHT:  { right  = pos; } break;
            case BOTTOM: { bottom = pos; } break;
            default:     { } break;
        }
    }
    
    bool rect::inside(double x, double y) const {
        if (x >= left && x <= right && y >= top && y <= bottom) {
            return true;
        }
        return false;
    }
    
    rect rect::offset(point origin) const {
        rect ret;
        
        ret.left   = origin.x + left;
        ret.right  = origin.x + right;
        ret.top    = origin.y + top;
        ret.bottom = origin.y + bottom;
        
        return ret;
    }
    
    /*
        UIBase
     */
    
    // Default constructor: initialise position and width to 0, 0
    UIBase::UIBase() { bounds = { 0, 0, 0, 0 }; }
    
    // Overloaded constructor: initialise position to x, y and width to 0, 0
    UIBase::UIBase(double x, double y) { bounds = { x, x, y, y }; }
    
    // Default destructor: no memory needs freeing, so do nothing
    UIBase::~UIBase() {}
    
    const rect UIBase::getRect() const { return bounds; }
    const rect UIBase::absoluteRect() const { return bounds.offset(getOrigin()); }
    
    const point UIBase::getOrigin() const { return {0, 0}; }
    
    
    /*
        UIChild
     */
    
    // Default constructor: intialise parent to NULL
    UIChild::UIChild() : parent(NULL) {}
    
    // Overloaded constructor: initialise parent
    UIChild::UIChild(UIBase *_parent) : parent(_parent) {}
    
    UIChild::~UIChild() {}
    
    // Set parent
    void UIChild::setParent(UIBase *_parent) { parent = _parent; }
    
    // Set origin to return parent's absolute position
    const point UIChild::getOrigin() const { return parent->absoluteRect().getPos(TOP_LEFT); }
    
    /*
        UIAtom
     */
    
    // Default constructor: start invisible since no position was given
    UIAtom::UIAtom() : visible(false) {}
    
    // Overloaded constructor: start visible, pass pos through to base
    UIAtom::UIAtom(bool _visible) : visible(_visible) {}
    
    UIAtom::~UIAtom() {}
    
    // visibility methods just alter the visibility flag
    void UIAtom::makeVisible()   { visible = true; }
    void UIAtom::makeInvisible() { visible = false; }
    void UIAtom::toggleVisible() { visible = not visible; }
    
    // draw checks if visible, and then calls render()
    void UIAtom::draw() {
        if (visible) { render(); }
    }
    
    /*
        Container
     */
    
    // Default constructor
    UIContainer::UIContainer() : UIBase() {}
    
    // Destructor: delete all children
    UIContainer::~UIContainer() {
        for (int i = 0; i < children.size(); ++i) {
            delete children[i];
        }
    }

    // pass through to children
    void UIContainer::draw() {
        for (int i = 0; i < children.size(); ++i) {
            children[i]->draw();
        }
    }
    
    // Add a child: set its parent to this object
    // Then make sure size matches the total size spanned by the children
    void UIContainer::addChild(UIChild *child) {
        child->setParent(this);
        children.push_back(child);
        
        // set bounds to match total size
        rect childRect = child->absoluteRect();
        if (childRect.left   < bounds.left  ) { bounds.left   = childRect.left; }
        if (childRect.right  > bounds.right ) { bounds.right  = childRect.right; }
        if (childRect.top    < bounds.top   ) { bounds.top    = childRect.top; }
        if (childRect.bottom > bounds.bottom) { bounds.bottom = childRect.bottom; }
    }
    
    // pass through to children
    void UIContainer::makeVisible() {
        for (int i = 0; i < children.size(); ++i) {
            children[i]->makeVisible();
        }
    }
    
    // pass through to children
    void UIContainer::makeInvisible() {
        for (int i = 0; i < children.size(); ++i) {
            children[i]->makeInvisible();
        }
    }
    
    // pass through to children
    void UIContainer::toggleVisible() {
        for (int i = 0; i < children.size(); ++i) {
            children[i]->toggleVisible();
        }
    }
    
    
    System::System() {}
    System::System(double x, double y) : UIBase(x, y) {}
    
    Container::Container(UIContainer *parent) : UIChild(parent) {}
    //Container::Container(double x, double y, UIContainer *parent) : UIBase(x, y), UIChild(parent) {}
    
    Atom::Atom(UIContainer *parent) : UIAtom(true), UIChild(parent) {}
}
