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
    
    void rect::setLRTB(double _left, double _right, double _top, double _bottom) {
        left = _left; right = _right; top = _top; bottom = _bottom;
    }
    
    void rect::setXYWH(double x, double y, double width, double height) {
        left = x; right = x + width; top = y; bottom = y + height;
    }
    
    coord rect::getPos(Position position) const {
        coord ret;
        
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
    
    //void rect::setVertex(Position position, coord pos) {
    //    switch (position) {
    //        case TOP_LEFT:     { left  = pos.x; top    = pos.y; } break;
    //        case TOP_RIGHT:    { right = pos.x; top    = pos.y; } break;
    //        case BOTTOM_LEFT:  { left  = pos.x; bottom = pos.y; } break;
    //        case BOTTOM_RIGHT: { right = pos.x; bottom = pos.y; } break;
    //        default:           { } break;
    //    }
    //}
    
    void rect::movePos(Position anchor, coord pos) {
        double x = pos.x;
        double y = pos.y;
        double W = width();
        double H = height();
        
        switch(anchor) {
            case TOP_LEFT:     { setXYWH(x,           y,           W, H); } break;
            case TOP:          { setXYWH(x - W / 2.0, y,           W, H); } break;
            case TOP_RIGHT:    { setXYWH(x - W,       y,           W, H); } break;
            case LEFT:         { setXYWH(x,           y - H / 2.0, W, H); } break;
            case CENTRE:       { setXYWH(x - W / 2.0, y - H / 2.0, W, H); } break;
            case RIGHT:        { setXYWH(x - W,       y - H / 2.0, W, H); } break;
            case BOTTOM_LEFT:  { setXYWH(x,           y - H,       W, H); } break;
            case BOTTOM:       { setXYWH(x - W / 2.0, y - H,       W, H); } break;
            case BOTTOM_RIGHT: { setXYWH(x - W,       y - H,       W, H); } break;
            default:           { } break;
        }
    }
    
    void rect::moveBy(coord offset) {
        left   += offset.x;
        right  += offset.x;
        top    += offset.y;
        bottom += offset.y;
    }
    
    bool rect::inside(double x, double y) const {
        if (x >= left && x <= right && y >= top && y <= bottom) {
            return true;
        }
        return false;
    }
    
    //rect rect::offset(coord origin) const { return offset(origin.x, origin.y); }
    //                                  
    //rect rect::offset(double x, double y) const {
    //rect ret;

    //    ret.left   = x + left;
    //    ret.right  = x + right;
    //    ret.top    = y + top;
    //    ret.bottom = y + bottom;
    //    
    //    return ret;
    //}
    
    
    /*
        UIBase
     */
    
    // Default constructor: initialise position and size to 0, 0 and parent to NULL
    UIBase::UIBase() { bounds.setXYWH(0, 0, 0, 0); }
    
    // Overloaded constructor: initialise position to x, y; size and origin to 0, 0
    UIBase::UIBase(double x, double y, double width, double height) { bounds.setXYWH(x, y, width, height); }
    
    // Default destructor: no memory needs freeing, so do nothing
    UIBase::~UIBase() {}
    
    // return bounds
    const rect UIBase::getRect() const { return bounds; }
    
    // move by offset
    void UIBase::moveBy(const coord &offset) {
        
    }
    
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
        children.push_back(child);
        
        // set bounds to match total size
        // TODO: this doesn't handle origins properly
        //rect childRect = child->getRect();
        //bounds.expandToFit(childRect);
    }

    // remainder of methods just pass the call through to its children
    // some trivial templating
    
    template<typename T, typename ...Args>
    void UIContainer::passCallToChildren(T (UIBase::*func)(Args...), Args ... args) {
        for (int i = 0; i < children.size(); ++i) {
            (children[i]->*func)(std::forward<Args>(args)...);
        }
    }
    
    void UIContainer::draw() { passCallToChildren(&UIBase::draw); }
    void UIContainer::makeVisible() { passCallToChildren(&UIBase::makeVisible); }
    void UIContainer::makeInvisible() { passCallToChildren(&UIBase::makeInvisible); }
    void UIContainer::toggleVisible() { passCallToChildren(&UIBase::toggleVisible); }
    void UIContainer::mouseMoved(int x, int y) { passCallToChildren(&UIBase::mouseMoved, x, y); }
    void UIContainer::mousePressed(int x, int y, int button) { passCallToChildren(&UIBase::mousePressed, x, y, button); }
    void UIContainer::mouseReleased(int x, int y, int button) { passCallToChildren(&UIBase::mouseReleased, x, y, button); }
    
}
