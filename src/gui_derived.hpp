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
#include "ljforces.hpp"

namespace gui {
    
    /*
        Atoms
     */
    
    class TextAtom : public Atom
    {
        /* 
            UI Atom for a single line of text.
         */
        
    private:
        virtual void render();
        
        std::string string;
        const ofTrueTypeFont *font;
        
    public:
        TextAtom();
        TextAtom(const std::string &string, const ofTrueTypeFont &font, double x, double y);
    };
    
    class SystemValueAtom : public Atom
    {
        /*
            UI Atom for drawing a double outputted from a member function of an LJContainer.
         */
        
    private:
        virtual void render();
        
        lj::LJContainer *system;
        const double (lj::LJContainer::*getfunc)();
        
        std::string format;
        const ofTrueTypeFont *font;
    
    public:
        SystemValueAtom();
        SystemValueAtom(const double (lj::LJContainer::*valueFunction)(), lj::LJContainer &system, const std::string &format, const ofTrueTypeFont &font, double x, double y);
    };
    
    class SliderAtom : public Atom
    {
        /*
            UI Atom for drawing a slider based on member functions from an LJContainer.
         */
        
    private:
        virtual void render();
        
        lj::LJContainer *system;
        const double (lj::LJContainer::*getfunc)();
        const double (lj::LJContainer::*setfunc)();
        
        double min, max, step;
        int width;
        bool selected;
        
    public:
        SliderAtom();
        SliderAtom(const double (lj::LJContainer::*getterFunction)(), void (lj::LJContainer::*setterFunction)(double), lj::LJContainer &system, double min, double max, int steps, int width, double x, double y);
        
        void setPosition(double x);
        
    };
    
}

#endif /* gui_derived_hpp */
