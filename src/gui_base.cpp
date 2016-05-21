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
    
    // Default constructor: initialise position and size to 0
    UIBase::UIBase() {
        pos_l = 0;
        pos_r = 0;
        pos_t = 0;
        pos_b = 0;
    }
    
    // Overloaded constructor: initialise position to x, y and size to 0
    UIBase::UIBase(double x, double y) {
        pos_l = x;
        pos_r = x;
        pos_t = y;
        pos_b = y;
    }
    
    // Default destructor: no memory needs freeing, so do nothing
    UIBase::~UIBase() {}
    
    // Get position as left / right / top / bottom
    double UIBase::left()    const { return pos_l; }
    double UIBase::right()   const { return pos_r; }
    double UIBase::top()     const { return pos_t; }
    double UIBase::bottom()  const { return pos_b; }
    
    // Get position as x / y / width / height
    double UIBase::x()       const { return pos_l; }
    double UIBase::y()       const { return pos_t; }
    double UIBase::width()   const { return pos_r - pos_l; }
    double UIBase::height()  const { return pos_b - pos_t; }
    
    // Get position of centre
    double UIBase::centreX() const { return (pos_r + pos_l) / 2.0; }
    double UIBase::centreY() const { return (pos_b + pos_t) / 2.0; }
    
    // Get position via Position enum
    coord UIBase::getCoord(Position position) const {
        coord point;
        
        switch (position) {
            case TOP_LEFT: {
                point.x = left(); point.y = top();
            } break;
            case TOP: {
                point.x = centreX(); point.y = top();
            } break;
            case TOP_RIGHT: {
                point.x = right(); point.y = top();
            } break;
            case LEFT: {
                point.x = left(); point.y = centreY();
            } break;
            case CENTRE: {
                point.x = centreX(); point.y = centreY();
            } break;
            case RIGHT: {
                point.x = right(); point.y = centreY();
            } break;
            case BOTTOM_LEFT: {
                point.x = left(); point.y = bottom();
            } break;
            case BOTTOM: {
                point.x = centreX(); point.y = bottom();
            } break;
            case BOTTOM_RIGHT: {
                point.x = right(); point.y = bottom();
            } break;
            default: {
                point.x = 0; point.y = 0;
            } break;
        }
    }
    
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
    
    // Get position as left / right / top / bottom relative to parent
    double UIChild::left()    const { return parent->x() + pos_l; }
    double UIChild::right()   const { return parent->x() + pos_r; }
    double UIChild::top()     const { return parent->y() + pos_t; }
    double UIChild::bottom()  const { return parent->y() + pos_b; }
    
    // Get position as x / y / width / height relative to parent
    double UIChild::x()       const { return parent->x() + pos_l; }
    double UIChild::y()       const { return parent->y() + pos_t; }
    
    // Get position of centre relative to parent
    double UIChild::centreX() const { return parent->x() + (pos_l + pos_r) / 2.0; }
    double UIChild::centreY() const { return parent->y() + (pos_t + pos_b) / 2.0; }
    
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
    void UIAtom::draw() const {
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
    void UIContainer::draw() const {
        for (int i = 0; i < children.size(); ++i) {
            children[i]->draw();
        }
    }
    
    // Add a child: set its parent to this object
    // Then make sure size matches the total size spanned by the children
    void UIContainer::addChild(UIChild *child) {
        child->setParent(this);
        children.push_back(child);
        
        //TODO: set size to child size
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
