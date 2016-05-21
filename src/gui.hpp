//
//  gui.hpp
//  StarredMD
//
//  Created by Staszek Welsh on 20/05/2016.
//
//

#ifndef gui_hpp
#define gui_hpp

#include <stdio.h>
#include <vector>
#include <string>
#include "ofMain.h"

namespace gui {
    enum Position
    {
        TOP_LEFT, TOP, TOP_RIGHT,
        MID_LEFT, MID_RIGHT,
        BOTTOM_LEFT, BOTTOM, BOTTOM_RIGHT
    };
    
    class Element
    {
        /*
            Abstract class defining an individual UI element.
         
            Has a position, width and height (all contained in ofRectangle),
            a visibility flag, and have a method to render itself to the screen.
            Derived elements override this render method to draw themselves.
            
            Public interface: draw(), which calls render if visible, and various methods
            to get/set the position, size and visibility
         */
        
    protected:
        const Element *parent;
        
        double left, top, right, bottom;
    
    public:
        Element() {};
        virtual ~Element() {};
        
        virtual void draw() const = 0;
        void setParent(Element *parent);
        
        double getLeft()    const;
        double getRight()   const;
        double getTop()     const;
        double getBottom()  const;
        
        double getX()       const;
        double getY()       const;
        double getWidth()   const;
        double getHeight()  const;
        
        double getCentreX() const;
        double getCentreY() const;
        
        virtual void makeVisible() = 0;
        virtual void makeInvisible() = 0;
        virtual void toggleVisible() = 0;
        
    };
    
    class Atom : public Element
    {
        /*
            Abstract class defining a single UI object drawn directly to screen.
         */
        
    protected:
        virtual void render() const = 0;
        bool visible;
        
    public:
        virtual void draw() const;
        
        virtual void makeVisible();
        virtual void makeInvisible();
        virtual void toggleVisible();
    };
    
    class Container : public Element
    {
        /*
            Abstract class defining a container for Elements.
            Aside from being an Element itself (which must have a size, visibility,
            and be drawable), it has a container for child Elements, which could be
            Elements or other Containers.
         */
        
    protected:
        std::vector <Element *> children;
        
    public:
        Container();
        Container(double x, double y);
        virtual ~Container();
        
        virtual void draw() const;
        void addChild(Element *child);
        
        virtual void makeVisible();
        virtual void makeInvisible();
        virtual void toggleVisible();
    };
    
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
        TextElement(const std::string &string, double x, double y, const ofTrueTypeFont &font);
        
    };
    
}

#endif /* gui_hpp */
