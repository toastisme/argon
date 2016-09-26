/*
 Argon
 
 Copyright (c) 2016 David McDonagh, Robert Shaw, Staszek Welsh
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */

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
            bounding box. These methods return true if the mouse event is properly handled, or
            false if it is not; the default behaviour is to return false as the mouse event is
            not handled.
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
        
        // move UI element to specific position
        virtual void moveTo(float xNew, float yNew);
        
        // draw element; called from ofApp.draw()
        virtual void draw() = 0;
        
        // resize element; called from update when screen size changes
        virtual void resize(float xScale, float yScale);
        
        // Set a specific size of an element
        virtual void setSize(float widthNew, float heightNew);
        
        // virtual methods to handle visibility
        bool getVisible() const;
        virtual void setVisible(bool visible);
        virtual void makeVisible();
        virtual void makeInvisible();
        virtual void toggleVisible();
        
        // methods to handle mouse events
        virtual bool mouseMoved(int x, int y);
        virtual bool mousePressed(int x, int y, int button);
        virtual bool mouseReleased(int x, int y, int button);
        
        // method for audio input
        virtual void audioIn(double volume);
        
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
        
        virtual void resize(float xScale, float yScale);
        virtual void setSize(float widthNew, float heightNew);
        virtual void moveBy(coord offset);
        virtual void moveTo(float xNew, float yNew);
    };
    
    class UIContainer : public UIBase
    {
        /*
            (Non-abstract) class defining a container for UI elements.
         
            Contains a vector of child UIBases (which could be UIAtoms or other UIContainers), and
            a method to add a child element.
         
            The passCallToChildren is a helper method to pass a method call to every child element.
         
            The draw, mouseMoved, mousePressed, and mouseReleased methods simply pass their call to
            the children. The mouse events pass calls to the first child to handle the event; if one
            of its children handles the event, the container returns true, and if none of them handle
            it (i.e. all the children return false) the container returns false.
         
            The makeVisible, makeInvisible, and toggleVisible methods set the container's visibility
            flag as required, and then passes the call to the children.
         
            This allows collective control over a group of related UI elements, such as all objects
            within a single menu.
         */
        
    protected:
        std::vector <UIBase *> children;
        std::vector <UIBase *> indexedChildren;
        
    public:
        UIContainer();
        UIContainer(double x, double y, double width = 0, double height = 0);
        
        // destructor calls all childrens' destructors, and frees their memory
        virtual ~UIContainer();
        
        // add a child, or add a child and return an index to get it back later
        void addChild(UIBase *child);
        int addIndexedChild(UIBase *child);
        
        UIBase* getChild(int i);
        
        // move the container, which also moves all children by the same amount
        virtual void moveBy(coord offset);
        // Move the container and all children to a specific position
        virtual void moveTo(float xNew, float yNew);
        
        // change visibility flag and pass call through to children
        virtual void setVisible(bool visible);
        virtual void makeVisible();
        virtual void makeInvisible();
        virtual void toggleVisible();
        
        // draw is just passed to all children
        virtual void draw();
        
        // resize is passed to all children
        virtual void resize(float xScale, float yScale);
        
        // setSize passed to all children
        virtual void setSize(float widthNew, float heightNew);
        
        // only pass mouse event to the first child to handle it
        virtual bool mouseMoved(int x, int y);
        virtual bool mousePressed(int x, int y, int button);
        virtual bool mouseReleased(int x, int y, int button);
        
        // audio events
        virtual void audioIn(double volume);
    };
    
}

#endif /* gui_base_hpp */
