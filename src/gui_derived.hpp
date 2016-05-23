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
#include "mdforces.hpp"

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
        void setString(double value, const std::string &format);
        void setColour(const ofColor &colour);
        void setFont(const ofTrueTypeFont &font);
        
        rect getStringBounds() const;
        
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
    
    class TextAtom : public UIAtom, TextComponent
    {
        /* 
            UI Atom for a single line of text.
         */
        
    private:
        virtual void render();
        
    public:
        TextAtom();
        TextAtom(const std::string &string, const ofTrueTypeFont &font, const ofColor &colour, double x, double y);
    };
    
    class ValueAtom : public UIAtom, TextComponent
    {
        /*
            UI Atom for drawing a double outputted from a member function of an LJContainer.
         */
        
    private:
        virtual void render();
        
        double *value;
        getter getValue;
        
        std::string format;
        
    public:
        ValueAtom();
        ValueAtom(const double (md::MDContainer::*getValue)(), md::MDContainer *system, const std::string &format, const ofTrueTypeFont &font, const ofColor &colour, double x, double y);
        ValueAtom(double *value, const std::string &format, const ofTrueTypeFont &font, const ofColor &color, double x, double y);
        
        std::string getString() const;
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
        SliderAtom(const double (md::MDContainer::*getValue)(), void (md::MDContainer::*setValue)(double), md::MDContainer *system, double min, double max, int width, double x, double y);
        
        double getSliderPos();
        void setFromSliderPos(double x);
        
        // handle mouse events
        void mouseMoved(int x, int y);
        void mousePressed(int x, int y, int button);
        void mouseReleased(int x, int y, int button);
    };
    
    class ButtonAtom : public UIAtom
    {
        /*
            UI Atom for a button which toggles a boolean value, stored by reference
         */
        
    private:
        virtual void render();
        
        bool *toggle;
        const ofImage *imageOn;
        const ofImage *imageOff;
        
    public:
        ButtonAtom();
        ButtonAtom(bool &toggle, const ofImage &imageOn, const ofImage &imageOff, double x, double y, double width, double height);
        
        void mousePressed(int x, int y, int button);
    };
    
    /*
        Containers
     */
    
    class SliderContainer : public UIContainer
    {
        /*
            UI Container for a text label, slider and value horizontal combination
         */
        
    public:
        SliderContainer();
        SliderContainer(const std::string &label, const ofTrueTypeFont &font, const ofColor &colour, const double (md::MDContainer::*getValue)(), void (md::MDContainer::*setValue)(double), md::MDContainer *system, double min, double max, const std::string &format, double x, double y, double labelWidth, double sliderWidth, double valueWidth, double height);
    };
    
}

#endif /* gui_derived_hpp */
