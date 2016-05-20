//
//  gui.cpp
//  StarredMD
//
//  Created by Staszek Welsh on 20/05/2016.
//
//

#include "gui.hpp"

namespace gui {
    
    // Get position as left / right / top / bottom
    double Element::getLeft()    const { return origin_x + left; }
    double Element::getRight()   const { return origin_y + right; }
    double Element::getTop()     const { return origin_x + top; }
    double Element::getBottom()  const { return origin_y + bottom; }
    
    // Get position as x / y / width / height
    double Element::getX()       const { return getLeft(); }
    double Element::getY()       const { return getTop(); }
    double Element::getWidth()   const { return right - left; }
    double Element::getHeight()  const { return bottom - top; }
    
    // Get position of centre
    double Element::getCentreX() const { return origin_x + (right + left) / 2.0; }
    double Element::getCentreY() const { return origin_y + (bottom + top) / 2.0; }
    
    // get/set visibility
    bool Element::isVisible() const { return visible; }
    void Element::makeVisible()     { visible = true; }
    void Element::makeInvisible()   { visible = false; }
    void Element::toggleVisible()   { visible = not visible; }
    
    // draw calls render only if visible
    void Element::draw() const {
        if (visible) { render(); }
    }
    
    TextElement::TextElement() {
        font = NULL;
    }
    
    TextElement::TextElement(const std::string &_string, double x, double y, const ofTrueTypeFont &_font) {
        string = _string;
        font = &_font;
        
        origin_x = 0;
        origin_y = 0;
        left = x;
        top = y;
        right = left + _font.stringWidth(_string);
        bottom = top + _font.stringHeight(_string);
        
        visible = true;
    }
    
    void TextElement::render() const {
        if (font) { font->drawString(string, left, top); }
    }
}
