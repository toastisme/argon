//
//  gui_base.hpp
//  StarredMD
//
//  Created by Staszek Welsh on 20/05/2016.
//
//

#ifndef gui_base_hpp
#define gui_base_hpp

#include <stdio.h>
#include <vector>

namespace gui {
    enum Position
    {
        TOP_LEFT, TOP, TOP_RIGHT,
        LEFT, CENTRE, RIGHT,
        BOTTOM_LEFT, BOTTOM, BOTTOM_RIGHT
    };
    
    struct coord
    {
        double x, y;
    };
    
    class UIBase
    {
        /*
            Abstract class defining a base UI element.
         
            Has a position (coord pos), a width and height (coord size), and various methods
            to get this position and size in both left/right/top/bottom and x/y/width/height
            formats.
         
            Has a pure virtual draw() method to implement drawing to the screen.
         
            Has pure virtual visibility methods to implement making it visible or not.
         */
        
    protected:
        double pos_l, pos_r, pos_t, pos_b; // left, right, top, bottom coordinates
    
    public:
        UIBase();
        UIBase(double x, double y);
        virtual ~UIBase();
        
        virtual void draw() const = 0;
        
        virtual double left()    const;
        virtual double right()   const;
        virtual double top()     const;
        virtual double bottom()  const;
        
        virtual double x()       const;
        virtual double y()       const;
        double width()  const;
        double height() const;           // not virtual as not defined relative to parent
        
        virtual double centreX() const;
        virtual double centreY() const;
        
        coord getCoord(Position position) const;
        
        virtual void makeVisible() = 0;
        virtual void makeInvisible() = 0;
        virtual void toggleVisible() = 0;
        
    };
    
    class UIChild : public virtual UIBase
    {
        /*
            Class containing a child element's behaviour.
         
            Has a parent, which can be set through setParent, and its position is defined
            relative to its parent's position.
         */
    
    protected:
        UIBase *parent;
        
    public:
        UIChild();
        //UIChild(double x, double y, UIBase *parent = NULL);
        UIChild(UIBase *parent);
        virtual ~UIChild();
        
        void setParent(UIBase *parent);
        
        virtual double left()    const;
        virtual double right()   const;
        virtual double top()     const;
        virtual double bottom()  const;
        
        virtual double x()       const;
        virtual double y()       const;
        
        virtual double centreX() const;
        virtual double centreY() const;
        
    };
    
    class UIAtom : public virtual UIBase
    {
        /*
            Class containing a single, basic drawable UI element's behaviour.
         
            Has a visibility flag, which defines whether the element is visible or not, and
            overrides the makeVisible() etc. methods so that they control this flag.
            
            Has a protected virtual method render(), which controls actually drawing the
            element to the screen. draw() is overridden so that it calls render() if visibile
            is set to true.
         */
        
    protected:
        virtual void render() const = 0;
        bool visible;
        
    public:
        UIAtom();
        UIAtom(double x, double y);
        UIAtom(bool visible);
        virtual ~UIAtom();
        
        virtual void draw() const;
        
        virtual void makeVisible();
        virtual void makeInvisible();
        virtual void toggleVisible();
    };
    
    class UIContainer : public virtual UIBase
    {
        /*
            Abstract class defining a container for Elements.
            Aside from being an Element itself (which must have a size, visibility,
            and be drawable), it has a container for child Elements, which could be
            Elements or other Containers.
         */
        
    protected:
        std::vector <UIChild *> children;
        
    public:
        UIContainer();
        UIContainer(double x, double y);
        virtual ~UIContainer();
        
        virtual void draw() const;
        void addChild(UIChild *child);
        
        virtual void makeVisible();
        virtual void makeInvisible();
        virtual void toggleVisible();
    };
    
    class System : public UIContainer {
        // A container which has no parent; drawn to a position on screen.
        
    public:
        System();
        System(double x, double y);
    };
    
    class Container : public UIContainer, public UIChild {
        // A container with a parent; drawn relative to that parent.
        
    public:
        Container(UIContainer *parent = NULL);
        Container(double x, double y, UIContainer *parent = NULL);
    };
    
    class Atom : public UIAtom, public UIChild {
        // A single UI element rendered to the screen, relative to its parent.
        
    public:
        Atom(UIContainer *parent = NULL);
    };
    
}

#endif /* gui_base_hpp */
