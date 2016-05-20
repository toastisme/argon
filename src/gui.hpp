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
    
    struct coord { double x, y; };
    
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
        double left, top, right, bottom, origin_x, origin_y;
        bool visible;
        virtual void render() const = 0;
    
    public:
        void draw() const;
        
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
        
        bool isVisible() const;
        void makeVisible();
        void makeInvisible();
        void toggleVisible();
        
    };
    
    class TextElement: public Element {
        /* 
            UI Element for a single line of text.
         */
        
    private:
        void render() const;
        std::string string;
        const ofTrueTypeFont *font;
        
    public:
        TextElement();
        TextElement(const std::string &string, double x, double y, const ofTrueTypeFont &font);
        
    };
    
    class Container: public Element {
        /*
            Abstract class defining a container for Elements.
            Aside from being an Element itself (which must have a size, visibility,
            and be drawable), it has a container for child Elements, which could be
            Elements or other Containers.
         */
        
    protected:
        std::vector <Element> children;
        
    public:
        void draw() {
            for (int i = 0; i < children.size(); ++i) {
                children[i].draw();
            }
        }
        
    };
}

#endif /* gui_hpp */
