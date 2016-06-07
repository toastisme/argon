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
    
    class UIBase
    {
        /*
            Abstract class defining a base UI element. It is not inteded for direct use;
            instead use the UIAtom and UIContainer classes below which inherit from UIBase.
            It defines a few common features of all UI elements, which are:
         
            Has a bounding box, defined by rect bounds, and a getter for this rect.
         
            Has a pure virtual draw() method to implement drawing to the screen, which should
            be called whenever the main ofApp calls its draw method.
         
            Has a flag, visible, which says whether the element is visible or not. Also has a
            getter for this flag, and methods to set the element visible, invisible, or toggle
            the visibility, which just sets this flag accordingly.
         
            Has methods to handle mouse events, implemented in UIBase as empty methods.
            These should be overloaded by UI elements which need to react to the mouse,
            using UIBase.getRect().inside(x, y) to check if mouse events occur within the
            bounding box.
         */
        
    protected:
        rect bounds;    // position and size
        bool visible;
    
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
        bool getVisible() const;
        virtual void makeVisible();
        virtual void makeInvisible();
        virtual void toggleVisible();
        
        // methods to handle mouse events
        virtual void mouseMoved(int x, int y);
        virtual void mousePressed(int x, int y, int button);
        virtual void mouseReleased(int x, int y, int button);
        
    };
    
    class UIAtom : public UIBase
    {
        /*
            Abstract class defining a single atomistic piece of UI which is drawn to the screen.
         
            Has a protected virtual method render(), which is the function that actually draws the
            UI element to the screen. draw() is overridden to call render() only if the visible
            flag is set to true.
         */
        
    protected:
        virtual void render() = 0;
        
    public:
        UIAtom();
        UIAtom(double x, double y, double width = 0, double height = 0);
        
        // if visible, calls render
        virtual void draw();
    };
    
    class UIContainer : public UIBase
    {
        /*
            (Non-abstract) class defining a container for UI elements.
         
            Contains a vector of child UIBases (which could be UIAtoms or other UIContainers), and
            a method to add a child element.
         
            The passCallToChildren is a helper method to pass a method call to every child element.
         
            The draw, mouseMoved, mousePressed, and mouseReleased methods simply pass their call to
            the children.
         
            The makeVisible, makeInvisible, and toggleVisible methods set the container's visibility
            flag as required, and then passes the call to the children.
         
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
        
        // change visibility flag and pass call through to children
        virtual void makeVisible();
        virtual void makeInvisible();
        virtual void toggleVisible();
        
        // remaining methods just pass call through to children
        virtual void draw();
        virtual void mouseMoved(int x, int y);
        virtual void mousePressed(int x, int y, int button);
        virtual void mouseReleased(int x, int y, int button);
    };
    
}

#endif /* gui_base_hpp */
