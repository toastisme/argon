//
//  gui.cpp
//  StarredMD
//
//  Created by Staszek Welsh on 20/05/2016.
//
//

#include "gui.hpp"

namespace gui {
    
    /*
        Element
     */
    
    // Get position as left / right / top / bottom
    double Element::getLeft()    const { return parent->left + left; }
    double Element::getRight()   const { return parent->left + right; }
    double Element::getTop()     const { return parent->top  + top; }
    double Element::getBottom()  const { return parent->top  + bottom; }
    
    // Get position as x / y / width / height
    double Element::getX()       const { return getLeft(); }
    double Element::getY()       const { return getTop(); }
    double Element::getWidth()   const { return right - left; }
    double Element::getHeight()  const { return bottom - top; }
    
    // Get position of centre
    double Element::getCentreX() const { return parent->left + (right + left) / 2.0; }
    double Element::getCentreY() const { return parent->top  + (bottom + top) / 2.0; }
    
    void Element::setParent(Element *_parent) { parent = _parent; }
    
    /*
        Atom
     */
    
    void Atom::makeVisible()   { visible = true; }
    void Atom::makeInvisible() { visible = false; }
    void Atom::toggleVisible() { visible = not visible; }
    
    void Atom::draw() const {
        if (visible) { render(); }
    }
    
    /*
        Container
     */
    
    Container::Container() {
        left = 0;
        top = 0;
        parent = NULL;
    }
    
    Container::Container(double x, double y) {
        left = x;
        right = x;
        top = y;
        bottom = y;
        parent = NULL;
    }
    
    Container::~Container() {
        for (int i = 0; i < children.size(); ++i) {
            delete children[i];
        }
    }

    void Container::draw() const {
        for (int i = 0; i < children.size(); ++i) {
            children[i]->draw();
        }
    }
    
    void Container::addChild(Element *child) {
        child->setParent(this);
        children.push_back(child);
    }
    
    void Container::makeVisible() {
        for (int i = 0; i < children.size(); ++i) {
            children[i]->makeVisible();
        }
    }
    
    void Container::makeInvisible() {
        for (int i = 0; i < children.size(); ++i) {
            children[i]->makeInvisible();
        }
    }
    
    void Container::toggleVisible() {
        for (int i = 0; i < children.size(); ++i) {
            children[i]->toggleVisible();
        }
    }
    
    /*
        TextElement
     */
    
    TextElement::TextElement() {
        font = NULL;
    }
    
    TextElement::TextElement(const std::string &_string, double x, double y, const ofTrueTypeFont &_font) {
        string = _string;
        font = &_font;
        
        left = x;
        top = y;
        right = left + _font.stringWidth(_string);
        bottom = top + _font.stringHeight(_string);
        
        visible = true;
    }
    
    void TextElement::render() const {
        if (font) { font->drawString(string, getLeft(), getTop()); }
    }
    
}
