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

#ifndef gui_derived_hpp
#define gui_derived_hpp

#include <ios>
#include <functional>
#include "ofMain.h"
#include "gui_base.hpp"
#include "mdforces.hpp"
#include "potentials.hpp"
#include "gaussian.hpp"
#include "cubicspline.hpp"
#include "utilities.hpp"


/*
    Note on order of arguments to constructors:
    The constuctors for some of these gui elements can be lengthy due to the number of variables, so the
    parameters are put in a standard order:
 
        1. What:  parameters describing what is actually drawn
        2. How:   parameters describing how it is drawn (colour, size, etc.)
        3. Where: parameters describing where it is drawn on the screen (usually x, y, width, height)
 
    For example, the parameters for SliderConstructor (label + slider + value) are in the following order:
 
        SliderContainer(const std::string &label, FuncGetter getValue, FuncSetter setValue, double min, double max, const ofTrueTypeFont &font, const ofColor &colour, int precision, double x, double y, double labelWidth, double sliderWidth, double valueWidth, double height);
 
        1. What:  label text, value to track (getter, setter, min, max)
        2. How:   font, colour, precision of value
        3. Where: x, y, width of label text, width of slider, width of value text, height
 */

namespace gui {
    
    // Some typedefs for the function arguments, allowing the gui classes to interact with other objects
    // in an arbirtary manner. When declaring functions of these types in constructors, it is easiest to
    // use a lambda function
    
    typedef std::function <double()>     FuncGetter;       // function void -> double
    typedef std::function <void(double)> FuncSetter;       // function double -> void
    typedef std::function <bool()>       FuncGetterBool;   // function void -> bool
    typedef std::function <void(bool)>   FuncSetterBool;   // function void -> bool
    typedef std::function <void()>       FuncAction;       // function void -> void
    
    /*
        Components
     */
    
    class TextComponent
    {
        /*
            Defines behaviour for an atom which draws text to the screen. Compensates for openFrameworks
            being dumb about the origin for drawing text, and allows easy alignment of text within a given
            rectangle.
         */
        
    private:
        std::string string;          // string to be drawn
        rect stringBounds;           // bounding box of the string itself (which is different to ofBase::bounds)
        double descenderHeight;      // size of descender (maximum size glyphs can go below baseline of text)
        
        const ofTrueTypeFont *font;  // pointer to font asset
        ofColor colour;              // text colour
        
        // reset the bounding box of the string and the descender height if the string is changed
        void resetBounds();
        
    protected:
        TextComponent();
        TextComponent(const std::string &string, const ofTrueTypeFont &font, const ofColor &colour);
        
        // set the string either directly or by formatting a double into a string with precision decimal places
        void setString(const std::string &string);
        void setString(double value, int precision);
        
        // setters for colour and font of the drawn string
        void setColour(const ofColor &colour);
        void setFont(const ofTrueTypeFont &font);
        
        // return the bounding box of the string
        rect getStringBounds() const;
        
        // render the string to the screen, align within a rectangle
        void renderString(rect bounds, Position align) const;
        
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
        virtual void render();   // call ofSetColor(colour);
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
        virtual void render();   // draw a rectangle of size bounds
        ofColor colour;          // colour of rect
        
    public:
        RectAtom();
        RectAtom(const ofColor &colour, double x, double y, double width, double height);
    };
    
    class ImageAtom : public UIAtom
    {
        /*
         UI Atom for an image
         */
        
    private:
        virtual void render();   // draws the image to the screen
        
        const ofImage *image;    // pointer to image asset
        
    public:
        ImageAtom();
        ImageAtom(const ofImage &image, double x, double y, double width, double height);
        
        // Override resize method so that button does not distort
        virtual void resize(float xScale, float yScale);
    };

    
    class TextAtom : public UIAtom, TextComponent
    {
        /* 
            UI Atom for a single line of text.
         */
        
    private:
        virtual void render();   // draws the text string within the rect bounds
        Position align;          // where the string is aligned within bounds
        
    public:
        TextAtom();
        TextAtom(const std::string &string, const ofTrueTypeFont &font, const ofColor &colour, Position align, double x, double y, double width, double height);
    };
    
    class ValueAtom : public UIAtom, TextComponent
    {
        /*
            UI Atom for drawing a double outputted from a member function of an MDContainer.
         */
        
    private:
        virtual void render();   // draws the value string within the rect bounds
        Position align;          // where the string is aligned within bounds
        
        FuncGetter getValue;     // getter function (void -> double) returning the value to be drawn
        int precision;           // number of decimal places to draw - set to zero to draw an integer
        
    public:
        ValueAtom();
        ValueAtom(FuncGetter getValue, int precision, const ofTrueTypeFont &font, const ofColor &colour, Position align, double x, double y, double width, double height);
    };
    
    class SliderAtom : public UIAtom
    {
        /*
            UI Atom for drawing a slider based on member functions from an MDContainer.
         */
        
    private:
        virtual void render();             // draws slider to screen with width given by bounds and in the
                                           // centre of the rect vertically - the actual height of the slider
                                           // is given by the static variables BODY_HEIGHT and HANDLE_HEIGHT
    protected:
        FuncGetter getValue;               // getter function (void -> double) for the value represented by the slider
        FuncSetter setValue;               // setter function (double -> void) for the value represented by the slider
        
        double min, max;                   // minimum and maxiumum values for the slider
        
        bool mouseFocus;                   // whether the slider is being clicked + dragged
        
        virtual double getSliderPos();             // get position of slider by calling getValue()
        virtual void setFromSliderPos(double x, double y);   // set value from position of slider by calling setValue()
        
    public:
        SliderAtom();
        SliderAtom(FuncGetter getValue, FuncSetter setValue, double min, double max, double x, double y, double width, double height);
        
        // handle mouse events
        bool mouseMoved(int x, int y);
        bool mousePressed(int x, int y, int button);
        bool mouseReleased(int x, int y, int button);
        
        static int BODY_HEIGHT;            // height of slider body (the background rectangle) - set to 10
        static ofColor BODY_COLOR;         // colour of slider body - set to white (255, 255, 255)
        
        static int HANDLE_WIDTH;           // width of slider handle (the rectangle representing the position of the slider) - set to 7
        static int HANDLE_HEIGHT;          // height of slider handle - set to 20
        static ofColor HANDLE_COLOR;       // colour of slider handle - set to grey (80, 80, 80)
    };
    
    class CircularSliderAtom : public SliderAtom
    {
        /*
            UI Atom for a slider, as per SliderAtom, but drawn on a circle rather than a straight line.
         */
        
    private:
        virtual void render();
        
        double radius; // Radius of semicircle slider is drawn on
    
        double getSliderPos(); // Override method to get the slider position
        void setFromSliderPos(double x, double y); // And set the value from slider position
        
    public:
        CircularSliderAtom(FuncGetter getValue, FuncSetter setValue, double min, double max, double x, double y, double radius);
        
        static ofColor DEFAULT_COLOR;   // Color of `unslid' part of circle
        static ofColor HIGHLIGHT_COLOR; // Color of `slid' part of circle
        static float LINE_WIDTH; // Width of circle line
        
        void resize(float xScale, float yScale);
        
    };
    
    class ButtonAtom : public UIAtom
    {
        /*
            UI Atom for a button which calls a void(void) function.
         */
        
    private:
        virtual void render();   // draws the button image to the screen
        
        FuncAction doAction;     // function (void -> void) called when the button is pressed
        const ofImage *image;    // pointer to image asset
        
    public:
        ButtonAtom();
        ButtonAtom(FuncAction doAction, const ofImage &image, double x, double y, double width, double height);
        
        // handle mouse events
        bool mousePressed(int x, int y, int button);
        
        // Override resize method so that button does not distort
        virtual void resize(float xScale, float yScale);
    };
    
    class ButtonToggleAtom : public UIAtom
    {
        /*
            UI Atom for a button which toggles a boolean value, stored as a getter / setter pair
         */
        
    private:
        virtual void render();     // draws the correct button image to the screen
        
        FuncGetterBool getBool;    // getter function (void -> bool) for the boolean represented by the button
        FuncSetterBool setBool;    // setter function (bool -> void) for the boolean represented by the button
        
        const ofImage *imageOn;    // pointer to 'on' image asset
        const ofImage *imageOff;   // pointer to 'off' image asset
        
    public:
        ButtonToggleAtom();
        ButtonToggleAtom(FuncGetterBool getBool, FuncSetterBool setBool, const ofImage &imageOn, const ofImage &imageOff, double x, double y, double width, double height);
        
        // handle mouse events
        bool mousePressed(int x, int y, int button);
        
        // Override resize method so that button does not distort
        virtual void resize(float xScale, float yScale);
    };
    
    class ButtonPairAtom : public UIAtom
    {
        /*
            UI Atom for a button which has two states, 'on' and 'off' (starting on) and toggles between them
         */
        
    private:
        virtual void render();     // draws the correct button image to the screen
        bool status;               // boolean for whether in the 'on' (true) or 'off' (false) state
        
        FuncAction doActionOn;     // function to call when clicked in 'on' state
        const ofImage *imageOn;    // pointer to 'on' image asset
        
        FuncAction doActionOff;    // function to call when clicked in 'off' state
        const ofImage *imageOff;   // pointer to 'off' image asset
        
    public:
        ButtonPairAtom();
        ButtonPairAtom(FuncAction doActionOn, const ofImage &imageOn, FuncAction doActionOff, const ofImage &imageOff, double x, double y, double width, double height);
        
        // handle mouse events
        bool mousePressed(int x, int y, int button);
        
        // Override resize method so that button does not distort
        virtual void resize(float xScale, float yScale);
    };
    
    class OptionsListAtom : public UIAtom
    {
        /*
            UI Atom for a list of selectable options
         */
    private:
        const ofTrueTypeFont *font;
        ofColor textcolor;
        double buttonWidth, buttonHeight;
     
    protected:
        virtual void render();
        
        int selectedOption;
        std::vector<TextAtom *> options;
        std::vector<FuncAction> actions;
        
    public:
        OptionsListAtom(const ofTrueTypeFont &font, const ofColor &textColour, double buttonWidth, double x, double y, double width, double height);
        ~OptionsListAtom();
        
        virtual void addOption(const std::string &label, FuncAction onSelect);
        
        virtual bool mousePressed(int x, int y, int button);
        
        virtual void resize(float xScale, float yScale);
        
        static ofColor DEFAULT_COLOR;
        static ofColor HIGHLIGHT_COLOR;
        static double OPTION_HEIGHT;
        
    };
    
    class AtomsListAtom : public OptionsListAtom
    {
        /*
         An options list that allows selection between atoms
         */
        
    private:
        virtual void render();
        
        std::vector<UIBase *> atoms;
        
        double widgetX, widgetWidth;
        
        void deselect();
        
    public:
        
        AtomsListAtom(const ofTrueTypeFont &font, const ofColor &textcolor, double x, double y, double optionsWidth, double widgetWidth, double height, double padding);
        ~AtomsListAtom();
        
        void addOption(const std::string &label, FuncAction doAction, UIBase *widget);
        
        bool mousePressed(int x, int y, int button);
        bool mouseReleased(int x, int y, int button);
        bool mouseMoved(int x, int y);
        
        void resize(float xScale, float yScale);
        
    };
    
    class PotentialAtom : public UIAtom
    {
    /*
        UI atom to plot a function
     */
    
    public:
        PotentialAtom(md::MDContainer &system, int numPoints, double min_x, double max_x, double min_y, double max_y, int x, int y, int width, int height);
        
    private:
        virtual void render();
        
    protected:
        md::MDContainer& theSystem;   // reference to the MD system
        rect potBounds;               // region of the potential curve to plot
        double numPoints;             // resolution of plot
        
    };
    
    class SplineControlPoint : public UIAtom
    {
        /*
            UI Atom corresponding to a point on a spline
         */
    private:
        virtual void render();
        
        double radius;      // radius of drawn point
        rect pointBounds;   // boundary of allowed points
        bool mouseFocus;    // whether the point is being dragged
        
        // move the point to position (x, y) with appropriate bounds checking
        void movePoint(double x, double y);
    
    public:
        SplineControlPoint(int x, int y, double radius, rect range);
        
        // x and y stored in screen space (in bounds)
        // m stored here in spline space
        double m;
        
        // handle mouse events
        bool mousePressed(int x, int y, int button);
        bool mouseReleased(int x, int y, int button);
        bool mouseMoved(int x, int y);
        
    };
    
    class GaussianAtom : public UIAtom {
    
        /*
            UI Atom corresponding to a Gaussian external potential
         */
        
    private:
        
        md::MDContainer& theSystem;
        ofImage& circGradient;
        
        int gaussianID;
        double radius;
        bool selected;
        bool audioOn;
        bool mouseFocus;
        
        virtual void render();
        
        // Move the potential to (x, y)
        void moveGaussian(double x, double y);
        
    public:
        GaussianAtom( md::MDContainer& theSystem, ofImage& circGradient, int gaussianID, ofTrueTypeFont* uiFont10, ofImage* closeButton, ofImage* audioOnButton, ofImage* audioOffButton, int x, int y, double radius);
        
        // handle mouse events
        bool mousePressed(int x, int y, int button);
        bool mouseReleased(int x, int y, int button);
        bool mouseMoved(int x, int y);
        
        void deselect();
        void select();
        void updateID(int _id);
        
        void audioIn(double volume);
        virtual void resize(float xScale, float yScale);
        
    };
    
    class SystemAtom : public UIAtom
    {
        /* 
            UI Atom of the system simulation, i.e. the drawing of the particles
         */
        
    private:
        md::MDContainer& theSystem;
        
        // Super secret variables
        ofImage& loganLeft;
        ofImage& loganRight;
        ofImage& boatLeft;
        ofImage& boatRight;
        bool inflictTorture;
        bool setSail;
        
        virtual void render();
        
        // Helper functions for drawing particles
        void drawParticle(int index, double radius_x, double radius_y, ofColor color, int nframes = 0);
        void drawParticle(int index, double radius, ofColor color, int nframes = 0);
        void drawParticle(int index, double radius_x, double radius_y, int nframes = 0);
        void drawParticle(int index, double radius, int nframes = 0);
        
    public:
        SystemAtom(md::MDContainer& theSystem, ofImage& loganLeft, ofImage& loganRight, ofImage& boatLeft, ofImage& boatRight, int x, int y, int width, int height);
        
        void toggleTheHorrors();
        void sailTheHighSeas();
    };
    
    class EnergyGraphAtom : public UIAtom
    {
        /*
            UI Atom for graph of kinetic and potential energies
         */
        
    private:
        md::MDContainer& theSystem;
        virtual void render();
        
    public:
        EnergyGraphAtom(md::MDContainer& theSystem, int x, int y, int width, int height);
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
        // a whole bunch of stuff to pass through to the individual elements
        SliderContainer(const std::string &label, FuncGetter getValue, FuncSetter setValue, double min, double max, const ofTrueTypeFont &font, const ofColor &textColour, int precision, double x, double y, double labelWidth, double sliderWidth, double valueWidth, double padding, double height);

        static int PADDING;
    };
    
    class CircularSliderContainer : public UIContainer
    {
        /*
            UI Container for a value and circular slider combination
         */
        
    public:
        CircularSliderContainer(FuncGetter getValue, FuncSetter setValue, double min, double max, const ofTrueTypeFont &font, const ofColor &textColor, int precision, double x, double y, double sliderRadius, double valueWidth, double valueHeight, double padding);
    };
    
    
    class SplineContainer : public UIContainer
    {
        /*
            UI Container to control a custom potential's spline with the mouse
         */
    private:
        md::MDContainer &system; // Get custom potential from the system
        
        double radius;       // visual size of control points
        rect splineRegion;   // region of spline to control
        rect pointRegion;    // region in which the centres of the control points are allowed to be, which
                             // is slightly smaller than the region of the controller to avoid control points
                             // sticking out of the sides
        
        // update the spline with the contained control points
        void updateSpline();
        
        // return true if a control point is horizontally close to the given x-coordinate
        // optionally exclude the point with index except
        bool controlPointNear(double x, int except = -1);
        
    public:
        SplineContainer(md::MDContainer &system, double x_min, double x_max, double y_min, double y_max, double controlPointRadius, double x, double y, double width, double height);
        
        // override moveBy to adjust pointRegion simultaneously
        virtual void moveBy(coord offset);
        
        // returns true if the mouse event caused the spline to be updated
        bool mousePressed(int x, int y, int button);
        
        // returns true if a point was moved
        bool mouseMoved(int x, int y);
        
        // Override draw method
        virtual void draw();
        
        // override resize method
        virtual void resize(float xScale, float yScale);
        
        // reset the entire spline
        void destroyAllPoints();
    };
    
    class GaussianContainer : public UIContainer
    {
        /*
            UI Container to control the positions of Gaussian external potentials with the mouse
         */
        
    private:
        md::MDContainer &system;
        ofImage& circGradient;
        
        // For Gaussian control panel
        ofTrueTypeFont* uiFont10;
        ofImage* closeButton;
        ofImage* audioOnButton;
        ofImage* audioOffButton;
        
        double radius;
        int selectedGaussian;
        
        // Return true if there is another Gaussian nearby, ignoring the gaussian referred to by index (if specified)
        bool gaussianNear(double x, double y, int index = -1);
        
        // Update the IDs of the GaussianAtoms after one is deleted
        void updateGaussianIDs(int deletedID);
        
        // Deselect all Gaussians except the one with the given id
        void deselectGaussians(int id);
        
        // Select a particular Gaussian
        void selectGaussian(int id);
        
    public:
        GaussianContainer(md::MDContainer& system, ofImage& circGradient, ofTrueTypeFont* uiFont10, ofImage* closeButton, ofImage* audioOnButton, ofImage* audioOffButton, double radius, double x, double y, double width, double height);
        
        // Returns true if mouse event caused Gaussians to be updated
        bool mousePressed(int x, int y, int button);
        
        // Returns true if Gaussian was moved
        bool mouseMoved(int x, int y);
        
        void audioIn(double volume);
        
        int getSelectedID() const;
        
        // remove all Gaussians
        void destroyAllGaussians();
    };

    class TutorialContainer : public UIContainer
    {
    public:
        TutorialContainer(int x, int y, int width, int height,ofTrueTypeFont &font, ofImage &nextButton, ofImage &closeButton, UIContainer &tutorialUI);
        
        int tutorialCounter;
        int textBoxx;
        int textBoxy;
        int textBoxWidth;
        int textboxHeight;
    
    protected:
        ofTrueTypeFont& aboutFont12;
        ofImage& nextButton;
        ofImage& closeButton;
        UIContainer& tutorialUI;
        
    };
    
    
}

#endif /* gui_derived_hpp */
