//
//  gui_derived.hpp
//  StarredMD
//
//  Created by Staszek Welsh on 21/05/2016.
//
//

#ifndef gui_derived_hpp
#define gui_derived_hpp

//#include <stdio.h>
#include <ios>
#include <functional>
#include "ofApp.h"
#include "ofMain.h"
#include "gui_base.hpp"
#include "mdforces.hpp"

namespace gui {
    
    typedef std::function <double()>     FuncGetter;   // function void -> double
    typedef std::function <void(double)> FuncSetter;   // function double -> void
    typedef std::function <void()>       FuncAction;   // function void -> void
    
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
        std::string string;  // string to be drawn
        rect stringBounds;   // bounding box of the string
        Position align;      // alignment of the string inside the rectangle
        
        const ofTrueTypeFont *font;
        ofColor colour;
        
        void resetBounds();
        
    public:
        TextComponent();
        TextComponent(const std::string &string, const ofTrueTypeFont &font, const ofColor &colour, Position align);
        
        void setString(const std::string &string);
        void setString(double value, const std::string &format);
        void setString(double value, int precision);
        void setColour(const ofColor &colour);
        void setFont(const ofTrueTypeFont &font);
        void setAlign(Position align);
        
        rect getStringBounds() const;
        
        void renderString(int left, int top) const;
        void renderString(rect bounds) const;
    };
    
    /*
        Atoms
     */
    
    class SetColour : public UIAtom
    {
        /*
            UIAtom which doesn't draw anything, but sets the draw colour
         */
        
    private:
        virtual void render();
        ofColor colour;
        
    public:
        SetColour();
        SetColour(const ofColor &colour);
    };
    
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
        RectAtom(const ofColor &colour, double x, double y, double width, double height);
    };
    
    class TextAtom : public UIAtom, TextComponent
    {
        /* 
            UI Atom for a single line of text.
         */
        
    private:
        virtual void render();
        Position anchor;
        
    public:
        TextAtom();
        TextAtom(const std::string &string, const ofTrueTypeFont &font, const ofColor &colour, Position anchor, double x, double y, double width, double height);
    };
    
    class ValueAtom : public UIAtom, TextComponent
    {
        /*
            UI Atom for drawing a double outputted from a member function of an LJContainer.
         */
        
    private:
        virtual void render();
        
        double *value;
        FuncGetter getValue;
        
        int precision;
        
    public:
        ValueAtom();
        ValueAtom(FuncGetter getValue, int precision, const ofTrueTypeFont &font, const ofColor &colour, Position anchor, double x, double y, double width, double height);
        
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
        FuncGetter getValue;
        FuncSetter setValue;
        
        double min, max;
        
        bool mouseFocus;
        
    public:
        SliderAtom();
        SliderAtom(FuncGetter getValue, FuncSetter setValue, double min, double max, double x, double y, double width, double height);
        
        double getSliderPos();
        void setFromSliderPos(double x);
        
        // handle mouse events
        void mouseMoved(int x, int y);
        void mousePressed(int x, int y, int button);
        void mouseReleased(int x, int y, int button);
        
        static int BODY_HEIGHT;
        static int HANDLE_WIDTH;
        static int HANDLE_HEIGHT;
    };
    
    class ButtonAtom : public UIAtom
    {
        /*
            UI Atom for a button which calls a void(void) function.
         */
        
    private:
        virtual void render();
        
        FuncAction doAction;
        const ofImage *image;
        
    public:
        ButtonAtom();
        ButtonAtom(FuncAction doAction, const ofImage &image, double x, double y, double width, double height);
        
        void mousePressed(int x, int y, int button);
    };
    
    class ButtonToggleAtom : public UIAtom
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
        ButtonToggleAtom();
        ButtonToggleAtom(bool &toggle, const ofImage &imageOn, const ofImage &imageOff, double x, double y, double width, double height);
        
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
        SliderContainer(const std::string &label, const ofTrueTypeFont &font, const ofColor &colour, FuncGetter getValue, FuncSetter setValue, double min, double max, int precision, double x, double y, double labelWidth, double sliderWidth, double valueWidth, double height);
        
        static int PADDING;
    };
    
}

#endif /* gui_derived_hpp */
