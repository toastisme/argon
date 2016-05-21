//
//  gui_derived.hpp
//  StarredMD
//
//  Created by Staszek Welsh on 21/05/2016.
//
//

#ifndef gui_derived_hpp
#define gui_derived_hpp

#include <stdio.h>
#include "ofMain.h"
#include "gui_base.hpp"

namespace gui {
    
    /*
        Atoms
     */
    
    class TextElement : public Atom
    {
        /* 
            UI Element for a single line of text.
         */
        
    protected:
        virtual void render() const;
        
        std::string string;
        const ofTrueTypeFont *font;
        
    public:
        TextElement();
        TextElement(const std::string &string, const ofTrueTypeFont &font, double x, double y);
    };
    
    
}

#endif /* gui_derived_hpp */
