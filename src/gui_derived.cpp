//
//  gui_derived.cpp
//  StarredMD
//
//  Created by Staszek Welsh on 21/05/2016.
//
//

#include "gui_derived.hpp"

namespace gui {
    
    /*
        TextElement
     */
    
    TextElement::TextElement() : Atom(), font(NULL) {}
    
    TextElement::TextElement(const std::string &_string, const ofTrueTypeFont &_font, double x, double y)
        : UIBase(x, y), Atom(), string(_string), font(&_font)
    {
        ofRectangle rect = _font.getStringBoundingBox(_string, x, y);
        pos_r = rect.getRight();
        pos_b = rect.getBottom();
    }
    
    void TextElement::render() const {
        if (font) { font->drawString(string, x(), y()); }
    }

}
