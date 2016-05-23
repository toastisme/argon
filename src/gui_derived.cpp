//
//  gui_derived.cpp
//  StarredMD
//
//  Created by Staszek Welsh on 21/05/2016.
//
//

#include "gui_derived.hpp"
#include <sstream>
#include <iomanip>

namespace gui {
    
    /*
        TextAtom
     */
    
    TextAtom::TextAtom() : Atom(), font(NULL) {}
    
    TextAtom::TextAtom(const std::string &_string, const ofTrueTypeFont &_font, double x, double y)
        : UIBase(x, y), Atom(), string(_string), font(&_font)
    {
        ofRectangle rect = _font.getStringBoundingBox(_string, x, y);
        bounds.right  = rect.getRight();
        bounds.bottom = rect.getBottom();
    }
    
    void TextAtom::render() {
        rect absBounds = absoluteRect();
        if (font) { font->drawString(string, absBounds.left, absBounds.top); }
    }
    
    /*
        SystemValueAtom
     */
    
    SystemValueAtom::SystemValueAtom() : Atom(), font(NULL) {}
    
    SystemValueAtom::SystemValueAtom(const double (lj::LJContainer::*valueFunction)(), lj::LJContainer &_system, const std::string &_format, const ofTrueTypeFont &_font, double x, double y)
        : UIBase(x, y), Atom(), getfunc(valueFunction), system(&_system), format(_format), font(&_font)
    {
        char drawstr[31];
        sprintf(drawstr, format.c_str(), (system->*getfunc)());
        ofRectangle rect = _font.getStringBoundingBox(std::string(drawstr), x, y);
        bounds.right  = rect.getRight();
        bounds.bottom = rect.getBottom();
    }
    
    void SystemValueAtom::render() {
        if (font) {
            char drawstr[31];
            sprintf(drawstr, format.c_str(), (system->*getfunc)());
            
            rect absBounds = absoluteRect();
            font->drawString(drawstr, absBounds.left, absBounds.top);
        }
    }
    
    /*
        SliderAtom
     */
    
    

}
