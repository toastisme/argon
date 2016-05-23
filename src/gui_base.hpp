//
//  gui_base.hpp
//  StarredMD
//
//  Created by Staszek Welsh on 20/05/2016.
//
//

#ifndef gui_base_hpp
#define gui_base_hpp

#include <vector>

namespace gui {
    enum Position
    {
        /*
            Enum defining nine positions within a UI element.
         */
        
        TOP_LEFT, TOP, TOP_RIGHT,
        LEFT, CENTRE, RIGHT,
        BOTTOM_LEFT, BOTTOM, BOTTOM_RIGHT
    };
    
    struct point
    {
        /*
            Struct defining a 2-D point.
         */
        
        double x, y;
    };
    
    struct rect
    {
        /*
            Struct defining a rectangle. Data is stored as the position of each side of the
            rectangle, and various methods are given to get the width, height, centre, etc.
         */
        
        double left, right, top, bottom;
        
        double width()   const;
        double height()  const;
        double centreX() const;
        double centreY() const;
        
        point getPos(Position position) const;
        
        void setVertex(Position position, point pos);
        void setSide(Position position, double pos);
        
        bool inside(double x, double y) const;
        
        rect offset(point origin) const;
    };
    
    class UIBase
    {
        /*
            Abstract class defining a base UI element.
         
            Has a bounding box, defined by rect bounds, a point origin, and various methods
            to get this position and size in both left/right/top/bottom and x/y/width/height
            formats.
         
            Has a pure virtual draw() method to implement drawing to the screen.
         
            Has pure virtual visibility methods to implement making it visible or not.
         */
        
    protected:
        rect bounds;  // position and size
        point origin; // either {0, 0} or top-left corner of parent
    
    public:
        UIBase();
        UIBase(double x, double y, double width = 0, double height = 0);
        virtual ~UIBase();
        
        // return bounding box rectangle (bounds)
        const rect getRect() const;      // relative to parent
        const rect absoluteRect() const; // absolute position on screen
        
        // getter and setter for origin (i.e. top-left corner of parent)
        const point getOrigin() const;
        void setOrigin(point origin);
        
        virtual void draw() = 0;
        
        virtual void makeVisible() = 0;
        virtual void makeInvisible() = 0;
        virtual void toggleVisible() = 0;
        
        virtual void mouseMoved(int x, int y);
        virtual void mousePressed(int x, int y, int button);
        virtual void mouseReleased(int x, int y, int button);
        
    };
    
    class UIAtom : public UIBase
    {
        /*
            Abstract class containing a single, basic drawable UI element's behaviour.
         
            Has a visibility flag, which defines whether the element is visible or not, and
            overrides the makeVisible() etc. methods so that they control this flag.
            
            Has a protected virtual method render(), which controls actually drawing the
            element to the screen. draw() is overridden so that it calls render() if visibile
            is set to true.
         */
        
    protected:
        virtual void render() = 0;
        bool visible;
        
    public:
        UIAtom();
        UIAtom(double x, double y, double width = 0, double height = 0, bool visible = true);
        
        virtual void draw();
        
        virtual void makeVisible();
        virtual void makeInvisible();
        virtual void toggleVisible();
    };
    
    class UIContainer : public UIBase
    {
        /*
            (Non-abstract) class defining a container for UI elements.
         
            Aside from being a UIBase itself (which must have a size, be drawable, etc.), it has a
            container for child UIBases, which could be UIAtoms or other UIContainers. Methods such
            as draw(), toggleVisible(), mouseMoved(), etc. pass the call through to its children.
         */
        
    protected:
        std::vector <UIBase *> children;
        
    public:
        UIContainer();
        UIContainer(double x, double y, double width = 0, double height = 0);
        virtual ~UIContainer();
        
        void addChild(UIBase *child);
        
        virtual void draw();
        
        virtual void makeVisible();
        virtual void makeInvisible();
        virtual void toggleVisible();
        
        virtual void mouseMoved(int x, int y);
        virtual void mousePressed(int x, int y, int button);
        virtual void mouseReleased(int x, int y, int button);
    };
    
}

#endif /* gui_base_hpp */
