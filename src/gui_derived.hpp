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
#include <functional>
#include "ofApp.h"
#include "ofMain.h"
#include "gui_base.hpp"
#include "ljforces.hpp"

// template<typename MyFunc>
// void updateSlider(MyFunc func, double amount){
//     func(amount);
// }
// 
// updateSlider(system::getT, 0.5);

namespace gui {
    
    typedef std::function<const double()> getter;
    typedef std::function<void(double)>   setter;
    
    /*
        Components
     */
    
    class TextComponent
    {
        /*
            Defines behaviour for an atom which draws text to the screen. Compensates for openFrameworks
            being dumb about the origin for drawing text.
         */
        
    private:
        std::string string;
        rect stringBounds;
        
        const ofTrueTypeFont *font;
        ofColor colour;
        
        void resetBounds();
        
    public:
        TextComponent();
        TextComponent(const std::string &string, const ofTrueTypeFont &font, const ofColor &colour);
        
        void setString(const std::string &string);
        void setColour(const ofColor &colour);
        void setFont(const ofTrueTypeFont &font);
        
        void renderString(int top, int right) const;
    };
    
    
    /*
        Atoms
     */
    
    class RectAtom : public UIAtom
    {
        /*
            UI Atom for a plain rectangle.
         */
        
    private:
        virtual void render();
        ofColor colour;
        
    public:
        RectAtom();
        RectAtom(double x, double y, double width, double height, ofColor colour);
    };
    
    class TextAtom : public UIAtom
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
    
    class ValueAtom : public UIAtom
    {
        /*
            UI Atom for drawing a double outputted from a member function of an LJContainer.
         */
        
    private:
        virtual void render();
        
        double *value;
        getter getValue;
        
        std::string format;
        const ofTrueTypeFont *font;
    
    public:
        ValueAtom();
        ValueAtom(const double (lj::LJContainer::*getValue)(), lj::LJContainer *system, const std::string &format, const ofTrueTypeFont &font, double x, double y);
        ValueAtom(double *value, const std::string &format, const ofTrueTypeFont &font, double x, double y);
    };
    
    class SliderAtom : public UIAtom
    {
        /*
            UI Atom for drawing a slider based on member functions from an LJContainer.
         */
        
    private:
        virtual void render();
        
        double *value;
        getter getValue;
        setter setValue;
        
        double min, max;
        int width;
        
        bool mouseFocus;
        
    public:
        SliderAtom();
        SliderAtom(const double (lj::LJContainer::*getValue)(), void (lj::LJContainer::*setValue)(double), lj::LJContainer *system, double min, double max, int width, double x, double y);
        
        double getSliderPos();
        void setFromSliderPos(double x);
        
        // handle mouse events
        void mouseMoved(int x, int y);
        void mousePressed(int x, int y, int button);
        void mouseReleased(int x, int y, int button);
    };
    
}

#endif /* gui_derived_hpp */
