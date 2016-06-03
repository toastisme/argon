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
#include "utilities.hpp"

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
    
    struct rect
    {
        /*
            Struct defining a rectangle. Data is stored as the position of each side of the
            rectangle, and various methods are given to get the width, height, centre, etc.
         */
        
        double left, right, top, bottom;
        
        double width()   const;
        double height()  const;
        double centreX() const;   // x coordinate of centre
        double centreY() const;   // y coordinate of centre
        
        // set rect to location and size either by left/right/top/bottom or by x/y/width/height
        void setLRTB(double left, double right, double top, double bottom);
        void setXYWH(double x, double y, double width, double height);
        
        // get the coordinates of a point within the rect specified by the Position enum,
        // i.e. getPos(TOP_LEFT) == {left, top}
        coord getPos(Position position) const;
        
        // move the rect (preserving width and height) such that a specified position in the rectangle,
        // given by the enum position, is now at the point pos
        void movePos(Position position, coord pos);
        
        // move the rect (preserving width and height) by an offset
        void moveBy(coord offset);
        
        // true if the point x, y is inside (or on the boundary of) the rect
        bool inside(double x, double y) const;
    };
    
    class UIBase
    {
        /*
            Abstract class defining a base UI element. It is not inteded for direct use;
            instead use the UIAtom and UIContainer classes below which inherit from UIBase.
            It defines a few common features of all UI elements, which are:
         
            Has a bounding box, defined by rect bounds, and a getter for this rect.
         
            Has a pure virtual draw() method to implement drawing to the screen, which should
            be called whenever the main ofApp calls its draw method.
         
            Has pure virtual visibility methods to implement making it visible or not. The
            purpose of these is made clearer in UIAtom and UIContainer.
         
            Has methods to handle mouse events, implemented in UIBase as empty methods.
            These should be overloaded by UI elements which need to react to the mouse,
            using UIBase.getRect().inside(x, y) to check if mouse events occur within the
            bounding box.
         */
        
    protected:
        rect bounds;    // position and size
    
    public:
        UIBase();
        UIBase(double x, double y, double width = 0, double height = 0);
        virtual ~UIBase();
        
        // return bounding box rectangle (bounds)
        const rect getRect() const;
        
        // move UI element by offset
        virtual void moveBy(coord offset);
        
        // draw element; called from ofApp.draw()
        virtual void draw() = 0;
        
        // pure virtual methods to handle visibility
        virtual void makeVisible() = 0;
        virtual void makeInvisible() = 0;
        virtual void toggleVisible() = 0;
        
        // methods to handle mouse events
        virtual void mouseMoved(int x, int y);
        virtual void mousePressed(int x, int y, int button);
        virtual void mouseReleased(int x, int y, int button);
        
    };
    
    class UIAtom : public UIBase
    {
        /*
            Abstract class defining a single atomistic piece of UI which is drawn to the screen.
         
            Has a visibility flag, which defines whether the element is visible or not, and
            overrides the makeVisible() etc. methods so that they control this flag.
            
            Has a protected virtual method render(), which is the function that actually draws the
            UI element to the screen. draw() is overridden to call render() only if the visible
            flag is set to true.
         */
        
    protected:
        virtual void render() = 0;
        bool visible;
        
    public:
        UIAtom();
        UIAtom(double x, double y, double width = 0, double height = 0, bool visible = true);
        
        // if visible, calls render
        virtual void draw();
        
        // control the flag visible
        virtual void makeVisible();
        virtual void makeInvisible();
        virtual void toggleVisible();
    };
    
    class UIContainer : public UIBase
    {
        /*
            (Non-abstract) class defining a container for UI elements.
         
            Contains a vector of child UIBases (which could be UIAtoms or other UIContainers), and
            a method to add a child element. The virtual methods defined in UIBase are overidden to
            simply pass the call onto each of the children within this vector.
         
            This allows collective control over a group of related UI elements, such as all objects
            within a single menu.
         */
        
    protected:
        std::vector <UIBase *> children;
        
        // pass function call and arguments to all children
        template<typename T, typename ...Args>
        void passCallToChildren(T (UIBase::*func)(Args...), Args ... args);
        
    public:
        UIContainer();
        UIContainer(double x, double y, double width = 0, double height = 0);
        
        // destructor calls all childrens' destructors, and frees their memory
        virtual ~UIContainer();
        
        // move the container, which also moves all children by the same amount
        virtual void moveBy(coord offset);
        
        // add a child UIAtom or UIContainer
        void addChild(UIBase *child);
        
        // remaining methods passed through to children
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
