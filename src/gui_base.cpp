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
    
    rect rect::alignAnchor(rect other, Position anchor_this, Position anchor_other) const {
        rect ret;
        
        // First align anchor_this to other's top-left:
        switch (anchor_this) {
            case TOP_LEFT:     { ret = rect::fromXYWH(other.left, other.top, width(), height()); } break;
            case TOP:          { ret = rect::fromXYWH(other.left - width() / 2.0, other.top, width(), height()); } break;
            case TOP_RIGHT:    { ret = rect::fromXYWH(other.left - width(), other.top, width(), height()); } break;
            case LEFT:         { ret = rect::fromXYWH(other.left, other.top - height() / 2.0, width(), height()); } break;
            case CENTRE:       { ret = rect::fromXYWH(other.left - width() / 2.0, other.top - height() / 2.0, width(), height()); } break;
            case RIGHT:        { ret = rect::fromXYWH(other.left - width(), other.top - height() / 2.0, width(), height()); } break;
            case BOTTOM_LEFT:  { ret = rect::fromXYWH(other.left, other.top - height(), width(), height()); } break;
            case BOTTOM:       { ret = rect::fromXYWH(other.left - width() / 2.0, other.top - height(), width(), height()); } break;
            case BOTTOM_RIGHT: { ret = rect::fromXYWH(other.left - width(), other.top - height(), width(), height()); } break;
        }
        
        // now shift to match anchor_other:
        switch(anchor_other) {
            case TOP_LEFT:     {} break;
            case TOP:          { ret = ret.offset(other.width() / 2.0, 0); } break;
            case TOP_RIGHT:    { ret = ret.offset(other.width(), 0); } break;
            case LEFT:         { ret = ret.offset(0, other.height() / 2.0); } break;
            case CENTRE:       { ret = ret.offset(other.width() / 2.0, other.height() / 2.0); } break;
            case RIGHT:        { ret = ret.offset(other.width(), other.height() / 2.0); } break;
            case BOTTOM_LEFT:  { ret = ret.offset(0, other.height()); } break;
            case BOTTOM:       { ret = ret.offset(other.width() / 2.0, other.height()); } break;
            case BOTTOM_RIGHT: { ret = ret.offset(other.width(), other.height()); } break;
        }
        
        return ret;
    }
    
    
    bool rect::inside(double x, double y) const {
        if (x >= left && x <= right && y >= top && y <= bottom) {
            return true;
        }
        return false;
    }
    
    rect rect::offset(point origin) const { return offset(origin.x, origin.y); }
                                      
    rect rect::offset(double x, double y) const {
    rect ret;

        ret.left   = x + left;
        ret.right  = x + right;
        ret.top    = y + top;
        ret.bottom = y + bottom;
        
        return ret;
    }
    
    rect rect::fromLRTB(double left, double right, double top, double bottom) {
        return { left, right, top, bottom };
    }
    
    rect rect::fromXYWH(double x, double y, double width, double height) {
        return { x, x + width, y, y + height };
    }
    
    
    /*
        UIBase
     */
    
    // Default constructor: initialise position, size and origin to 0, 0
    UIBase::UIBase() { bounds = { 0, 0, 0, 0 }; origin = {0, 0}; }
    
    // Overloaded constructor: initialise position to x, y; size and origin to 0, 0
    UIBase::UIBase(double x, double y, double width, double height) { bounds = { x, x + width, y, y + height }; origin = {0, 0}; }
    
    // Default destructor: no memory needs freeing, so do nothing
    UIBase::~UIBase() {}
    
    // return bounds either relative to origin or in absoulute screen coordinates
    const rect UIBase::getRect() const { return bounds; }
    const rect UIBase::absoluteRect() const { return bounds.offset(getOrigin()); }
    
    // getter and setter for origin
    const point UIBase::getOrigin() const { return origin; }
    void UIBase::setOrigin(point _origin) { origin = _origin; }
    
    // Default handling of mouse events is to do nothing
    void UIBase::mouseMoved(int x, int y) {}
    void UIBase::mousePressed(int x, int y, int button) {}
    void UIBase::mouseReleased(int x, int y, int button) {}
    
    /*
        UIAtom
     */
    
    // Default constructor: start invisible since no position was given
    UIAtom::UIAtom() : visible(false) {}
    
    // Overloaded constructor: start visible, pass pos through to base
    UIAtom::UIAtom(double x, double y, double width, double height, bool _visible) : UIBase(x, y, width, height), visible(_visible) {}
    
    // draw checks if visible, and then calls render()
    void UIAtom::draw() {
        if (visible) { render(); }
    }
    
    // visibility methods just alter the visibility flag
    void UIAtom::makeVisible()   { visible = true; }
    void UIAtom::makeInvisible() { visible = false; }
    void UIAtom::toggleVisible() { visible = not visible; }
    
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
    
    // Add a child: set its parent to this object
    // Then make sure size matches the total size spanned by the children
    void UIContainer::addChild(UIBase *child) {
        child->setOrigin(bounds.getPos(TOP_LEFT));
        children.push_back(child);
        
        // set bounds to match total size
        rect childRect = child->absoluteRect();
        if (childRect.left   < bounds.left  ) { bounds.left   = childRect.left; }
        if (childRect.right  > bounds.right ) { bounds.right  = childRect.right; }
        if (childRect.top    < bounds.top   ) { bounds.top    = childRect.top; }
        if (childRect.bottom > bounds.bottom) { bounds.bottom = childRect.bottom; }
    }

    // remainder of methods just pass the call through to its children
    
    void UIContainer::draw() {
        for (int i = 0; i < children.size(); ++i) {
            children[i]->draw();
        }
    }
    
    void UIContainer::makeVisible() {
        for (int i = 0; i < children.size(); ++i) {
            children[i]->makeVisible();
        }
    }
    
    void UIContainer::makeInvisible() {
        for (int i = 0; i < children.size(); ++i) {
            children[i]->makeInvisible();
        }
    }
    
    void UIContainer::toggleVisible() {
        for (int i = 0; i < children.size(); ++i) {
            children[i]->toggleVisible();
        }
    }
    
    void UIContainer::mouseMoved(int x, int y) {
        for (int i = 0; i < children.size(); ++i) {
            children[i]->mouseMoved(x, y);
        }
    }
    
    void UIContainer::mousePressed(int x, int y, int button) {
        for (int i = 0; i < children.size(); ++i) {
            children[i]->mousePressed(x, y, button);
        }
    }
    
    void UIContainer::mouseReleased(int x, int y, int button) {
        for (int i = 0; i < children.size(); ++i) {
            children[i]->mouseReleased(x, y, button);
        }
    }
    
}
