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

#include "gui_derived.hpp"
#include <sstream>
#include <math.h>

namespace gui {
    
    /*
        TextComponent
     */

    // default constructor nulls the font pointer (which is checked for nullness before drawing), effectively disabling the object
    TextComponent::TextComponent() : font(NULL) {}
    
    // set everything and make sure stringBounds is the bounding box for the string
    TextComponent::TextComponent(const std::string &_string, const ArgonFont &_font, RGB &_colour)
        : string(_string), font(&_font), colour(_colour)
    {}
    
    // set the string and make sure the bounds are correct
    void TextComponent::setString(const std::string &_string) {
        string = _string;
    }
    
    // set string using value and number of decimal places
    void TextComponent::setString(double value, int precision) {
        std::stringstream drawstr;
        drawstr.precision(precision);    // set precision
        drawstr.setf(std::ios::fixed);   // use precision based on number of decimal places (fixed point)
        drawstr << value;
        string = drawstr.str();
    }
    
    // changing the font changes the size, so recalculate string bounds
    void TextComponent::setFont(const ArgonFont &_font) {
        font = &_font;
    }
    
    void TextComponent::setColour(RGB &_colour) { colour = _colour; }
    
    // actually draw the string to the screen, aligned within a larger rectangle
    // typically, this larger rectangle is ofBase::bounds
    void TextComponent::renderString(rect bounds, Position align) const {
        if (font) {                                                 // font might be null, so check
            coord pos = bounds.getPos(align);                       // get position to draw string at
            font->drawText(pos, align, colour, string);                // draw aligned string
        }
    }
    
    
    /*
        RectAtom
     */
    
    RectAtom::RectAtom() : UIAtom(), colour() {}
    RectAtom::RectAtom(RGB _colour, double x, double y, double width, double height) : gui::UIAtom(x, y, width, height), colour(_colour) {}
    void RectAtom::render() { drawRect(bounds, colour); }
    
    /*
        TextAtom
     */
    
    TextAtom::TextAtom() : UIAtom(), TextComponent(), align(POS_TOP_LEFT) {}
    
    // pass things through to TextComponent, and store align
    TextAtom::TextAtom(const std::string &string, const ArgonFont &font, RGB &colour, Position _align, double x, double y, double width, double height)
        : UIAtom(x, y, width, height), TextComponent(string, font, colour), align(_align)
    {}
    
    // render the string aligned within bounds
    void TextAtom::render() { renderString(bounds, align); }
    
    // Set the text of the text atom
    void TextAtom::setText(const std::string &string) { setString(string); }
    /*
        ValueAtom
     */
    
    ValueAtom::ValueAtom() : UIAtom(), TextComponent() {}
    
    // pass stuff into TextComponent, set the function, precision and align, and set the string using getValue
    ValueAtom::ValueAtom(FuncGetter _getValue, int _precision, const ArgonFont &font, RGB &colour, Position _align, double x, double y, double width, double height)
        : UIAtom(x, y, width, height), TextComponent("", font, colour), getValue(_getValue), precision(_precision), align(_align)
    {
        setString(getValue(), precision);
    }
    
    // render the string by getting the value from getValue, setting the string and rendering
    // note that the call to setString here stops render from being const in general: do we want
    // to separate the draw call into separate update() and draw() const calls?
    void ValueAtom::render() {
        setString(getValue(), precision);
        renderString(bounds, align);
    }
    
    /*
     ImageAtom
     */
    
    
    ImageAtom::ImageAtom() : UIAtom(), image(NULL), colour() {}
    ImageAtom::ImageAtom(const ArgonImage &_image, double x, double y, double width, double height, RGB _colour)
    : UIAtom(x, y, width, height), image(&_image), colour(_colour) {}
    void ImageAtom::render() {
        if (image) { image->draw(bounds, colour); }
    }
    void ImageAtom::resize(float xScale, float yScale) {
        float scale = xScale < yScale ? xScale : yScale;
        bounds.setXYWH(bounds.left*xScale, bounds.top*yScale, bounds.width()*scale, bounds.height()*scale);
    }

    
    /*
        SliderAtom
     */
    
    SliderAtom::SliderAtom() : UIAtom() {}
    
    SliderAtom::SliderAtom(FuncGetter _getValue, FuncSetter _setValue, double _min, double _max, double x, double y, double width, double height)
        : UIAtom(x, y, width, height), min(_min), max(_max), getValue(_getValue), setValue(_setValue), mouseFocus(false) {}
    
    // get the value of getValue, and map it to the x-coordinate of the slider
    // subtract the handle-width so that both the left and right sides of the
    // slider handle stay within the slider body. Without subtracting, the right
    // side of the slider handle moves past the right side of the slider body
    double SliderAtom::getSliderPos() {
        return util::map(getValue(), min, max, bounds.left, bounds.right - SliderAtom::HANDLE_WIDTH, true);
    }
    
    // set the value using setValue, obtained by mapping the x-coordinate of the
    // slider to between min and max. Note that we make the same slider-handle-
    // within-slider-body correction, and subtract half the slider width from x so
    // that it corresponds to the centre of the slider handle, not the left side
    void SliderAtom::setFromSliderPos(double x, double y) {
        x -= SliderAtom::HANDLE_WIDTH / 2;
        
        setValue(util::map(x, bounds.left, bounds.right - SliderAtom::HANDLE_WIDTH, min, max, true));
    }
    
    void SliderAtom::render() {
        // slider body
        // set size of body using bounds and BODY_HEIGHT
        double left = bounds.left;
        double top = bounds.centreY() - SliderAtom::BODY_HEIGHT / 2;
        double width = bounds.width();
        double height = SliderAtom::BODY_HEIGHT;
        drawRect(left, top, width, height, BODY_COLOR);
        
        // slider height
        // set size of handle using its x-position, HANDLE_WIDTH and HANDLE_HEIGHT
        left = getSliderPos();
        top = bounds.centreY() - SliderAtom::HANDLE_HEIGHT / 2;
        width = SliderAtom::HANDLE_WIDTH;
        height = SliderAtom::HANDLE_HEIGHT;
        drawRect(int(left), top, width, height, HANDLE_COLOR);// int(left) stops the slider handle from being
                                                              // blurry due to being drawn off the pixel grid
    }
    
    // if the mouse is left-clicked inside the slider, take mouse focus and update the slider's position
    bool SliderAtom::mousePressed(int x, int y, int button) {
        if (visible && button == 0 && bounds.inside(x, y)) {
            mouseFocus = true;
            setFromSliderPos(x, y);
            return true;
        } else { return false; }
    }
    
    // if the mouse is moved, and we have mouse focus, update the slider's position
    bool SliderAtom::mouseMoved(int x, int y) {
        if (mouseFocus) { setFromSliderPos(x, y); return true; }
        else { return false; }
    }
    
    // if the mouse is released (anywhere on the screen), lose mouse focus
    bool SliderAtom::mouseReleased(int x, int y, int button) {
        if (button == 0) { mouseFocus = false; }
        return false;   // we do not want to capture the mouse release, so return false
    }
    
    // some static varaibles to ensure sliders are drawn the same
    // these could be customised on a per-slider basis, but the slider constructor is long
    // enough already and we probably want sliders to look the same anyway
    RGB SliderAtom::BODY_COLOR = RGB(255, 255, 255);
    RGB SliderAtom::HANDLE_COLOR = RGB(80, 80, 80);
    
    int SliderAtom::BODY_HEIGHT = 10;
    int SliderAtom::HANDLE_WIDTH = 7;
    int SliderAtom::HANDLE_HEIGHT = 20;
    
    /*
        CircularSliderAtom
     */
    
    CircularSliderAtom::CircularSliderAtom(FuncGetter getValue, FuncSetter setValue, double min, double max, double x, double y, double _radius) : SliderAtom(getValue, setValue, min, max, x, y, 2.1*_radius, 1.1*_radius), radius(_radius)
    { }
    
    // get the value of getValue, and map it to the angle (in degrees) on the semicircle
    double CircularSliderAtom::getSliderPos() {
        return util::map(getValue(), min, max, 0, 180, true);
    }
    
    // set the value using setValue, obtained by mapping the distance round the circle of the
    // slider to between min and max.
    void CircularSliderAtom::setFromSliderPos(double x, double y) {
        // Get distance from centre of circle to x and y positions
        x = bounds.centreX() - x;
        y = bounds.bottom - y;
        y = y > 0 ? y : 0;
        
        // Calculate angle of arc in degrees
        double angle = atan2(y, x) * 180 / M_PI;
        
        // Map this onto the value range
        setValue(util::map(angle, 0, 180, min, max, true));
    }

    void CircularSliderAtom::render() {
        // centre point of circle
        coord centre(bounds.centreX(), bounds.bottom);
        float sliderPos = getSliderPos();
        
        int precision = 50; // number of segments to draw in arc
        int pos = roundf(util::map(sliderPos, 0, 180, 0, precision));
        
        // create meshes for the filled and empty regions of the slider
        ArgonMesh filledMesh, emptyMesh;
        
        for (int i = 0; i <= pos; ++i) {
            float theta = 3.14159265f * (i / float(precision) - 1);
            
            float x = centre.x + (radius - LINE_WIDTH) * cosf(theta);
            float y = centre.y + (radius - LINE_WIDTH) * sinf(theta);
            filledMesh.addVertex(x, y);
            
            x = centre.x + (radius + LINE_WIDTH) * cosf(theta);
            y = centre.y + (radius + LINE_WIDTH) * sinf(theta);
            filledMesh.addVertex(x, y);
        }
        
        for (int i = pos; i <= precision; ++i) {
            float theta = 3.14159265f * (i / float(precision) - 1);
            
            float x = centre.x + (radius - LINE_WIDTH) * cosf(theta);
            float y = centre.y + (radius - LINE_WIDTH) * sinf(theta);
            emptyMesh.addVertex(x, y);
            
            x = centre.x + (radius + LINE_WIDTH) * cosf(theta);
            y = centre.y + (radius + LINE_WIDTH) * sinf(theta);
            emptyMesh.addVertex(x, y);
        }
        
        filledMesh.draw(HIGHLIGHT_COLOR, PRIMITIVE_TRIANGLE_STRIP);
        emptyMesh.draw(DEFAULT_COLOR, PRIMITIVE_TRIANGLE_STRIP);
    }
    
    void CircularSliderAtom::resize(float xScale, float yScale) {
        radius *= xScale;
        UIAtom::resize(xScale, yScale);
    }
    
    RGB CircularSliderAtom::DEFAULT_COLOR = RGB(80, 80, 80);
    RGB CircularSliderAtom::HIGHLIGHT_COLOR = RGB(10, 174, 199);
    float CircularSliderAtom::LINE_WIDTH = 12.0;
    
    /*
        ButtonAtom
     */
    
    ButtonAtom::ButtonAtom() : UIAtom(), image(NULL), colour() {}
    
    ButtonAtom::ButtonAtom(FuncAction _doAction, const ArgonImage &_image, RGB _colour, double x, double y, double width, double height)
        : UIAtom(x, y, width, height), doAction(_doAction), image(&_image), colour(_colour)
    { }
    
    // render just draws the button image
    void ButtonAtom::render() {
        if (image) { image->draw(bounds.left, bounds.top, bounds.width(), bounds.height(), colour); }
    }
    
    // if the mouse is left-clicked inside the button, do its action
    bool ButtonAtom::mousePressed(int x, int y, int button) {
        if (visible && button == 0 && bounds.inside(x, y)) { doAction(); return true; }
        else { return false; }
    }
    
    // resize with the smaller of xScale and yScale
    void ButtonAtom::resize(float xScale, float yScale) {
        float scale = xScale < yScale ? xScale : yScale;
        bounds.setXYWH(bounds.left*xScale, bounds.top*yScale, bounds.width()*scale, bounds.height()*scale);
    }
    
    /*
        ButtonToggleAtom
     */
    
    ButtonToggleAtom::ButtonToggleAtom() : UIAtom(), imageOn(NULL), imageOff(NULL), colour() {}
    
    ButtonToggleAtom::ButtonToggleAtom(FuncGetterBool _getBool, FuncSetterBool _setBool, const ArgonImage &_imageOn, const ArgonImage &_imageOff, RGB _colour, double x, double y, double width, double height)
        : UIAtom(x, y, width, height), getBool(_getBool), setBool(_setBool), imageOn(&_imageOn), imageOff(&_imageOff), colour(_colour) {}
    
    // render button image based on the value of getBool
    void ButtonToggleAtom::render() {
        if (getBool()) {
            if (imageOn)  {  imageOn->draw(bounds.left, bounds.top, bounds.width(), bounds.height(), colour); }
        } else {
            if (imageOff) { imageOff->draw(bounds.left, bounds.top, bounds.width(), bounds.height(), colour); }
        }
    }
    
    // if the mouse is left-clicked inside the button, invert the value of the boolean
    bool ButtonToggleAtom::mousePressed(int x, int y, int button) {
        if (visible && button == 0 && bounds.inside(x, y)) { setBool(!getBool()); return true; }
        else { return false; }
    }
    
    // resize with the smaller of xScale and yScale
    void ButtonToggleAtom::resize(float xScale, float yScale) {
        float scale = xScale < yScale ? xScale : yScale;
        bounds.setXYWH(bounds.left*xScale, bounds.top*yScale, bounds.width()*scale, bounds.height()*scale);
    }
    
    /*
        OptionsListAtom
     */

    OptionsListAtom::OptionsListAtom(const ArgonFont &_font, RGB &textColour, double _buttonWidth, double x, double y, double width, double height) : UIAtom(x, y, width, height), font(&_font), textcolor(textColour), selectedOption(-1), buttonWidth(_buttonWidth), buttonHeight(OPTION_HEIGHT) {}
    
    OptionsListAtom::~OptionsListAtom() {
        for (int i = 0; i < options.size(); i++) {
            delete options[i];
        }
    }
    
    void OptionsListAtom::render() {
    
        for (int i = 0; i < options.size(); i++) {
            
            if (i != selectedOption) {
                drawRect(bounds.left, options[i]->getRect().top, buttonWidth, buttonHeight, DEFAULT_COLOR);
                options[i]->draw();
            }
            
        }
        
        if (selectedOption > -1) {
            drawRect(bounds.left, options[selectedOption]->getRect().top, buttonWidth, buttonHeight, HIGHLIGHT_COLOR);
            options[selectedOption]->draw();
        }
        
    }
    
    void OptionsListAtom::addOption(const std::string &label, FuncAction onSelect){
        
        TextAtom* newOption = new TextAtom(label, *font, textcolor, POS_CENTRE, bounds.left, bounds.top + options.size()*buttonHeight, buttonWidth, buttonHeight);
        options.push_back(newOption);
        
        actions.push_back(onSelect);
        
        selectedOption = options.size() == 1 ? 0 : selectedOption;
        
    }
    
    bool OptionsListAtom::mousePressed(int x, int y, int button)
    {
        bool ret = false;
        // On left click, select the correct option
        if (visible && button == 0) {
            for (int i = 0; i < options.size(); i++) {
                
                if ( options[i]->getRect().inside(x, y) ) {
                    selectedOption = i;
                    actions[i]();
                    ret = true;
                    break;
                }
                
            }
        }
        
        return ret;
    }
    
    void OptionsListAtom::resize(float xScale, float yScale)
    {
        UIAtom::resize(xScale, yScale);
        buttonWidth *= xScale;
        buttonHeight *= yScale;
        for (int i = 0; i < options.size(); i++){
            options[i]->resize(xScale, yScale);
        }
    }
    
    RGB OptionsListAtom::DEFAULT_COLOR = RGB(80, 80, 80);
    RGB OptionsListAtom::HIGHLIGHT_COLOR = RGB(10, 174, 199);
    double OptionsListAtom::OPTION_HEIGHT = 28;
    
    /*
     AtomsListAtom
     */
    
    AtomsListAtom::AtomsListAtom(const ArgonFont &font, RGB &textcolor, double x, double y, double optionsWidth, double _widgetWidth, double height, double padding) : OptionsListAtom(font, textcolor, optionsWidth, x, y, optionsWidth+_widgetWidth+3*padding, height), widgetWidth(_widgetWidth)
    {
        // Work out x position of the widget bit
        widgetX = x - 2*padding - optionsWidth;
    }
    
    AtomsListAtom::~AtomsListAtom()
    {
        for (int i = 0; i < atoms.size(); i++){
            delete atoms[i];
        }
    }
    
    void AtomsListAtom::render()
    {
        OptionsListAtom::render();
        for (int i = 0; i < atoms.size(); i++){
            atoms[i]->draw();
        }
    }
    
    void AtomsListAtom::addOption(const std::string &label, FuncAction doAction, UIBase *atom)
    {
        // Add an option to the OptionsListAtom, with action to select the given atom
        OptionsListAtom::addOption(label, doAction);
        
        // Place new atom and add it
        atom->moveBy({widgetX, bounds.top - atom->getRect().top});
        atoms.push_back(atom);
        
        if (atoms.size() == 1) atom->makeVisible();
        else atom->makeInvisible();
    }
    
    int AtomsListAtom::getSelectedOption()
    {
        return selectedOption;
    }
    
    bool AtomsListAtom::mousePressed(int x, int y, int button)
    {
        bool ret;
        
        if (selectedOption > -1) {
            ret = atoms[selectedOption]->mousePressed(x, y, button);
        }
        
        // On left click, select the correct option
        if (button == 0) {
            for (int i = 0; i < options.size(); i++) {
                
                if ( options[i]->getRect().inside(x, y) ) {
                    selectedOption = i;
                    actions[i]();
                    deselect();
                    atoms[i]->makeVisible();
                    ret = true;
                    break;
                }
                
            }
        }
        
        return ret;
    }
    
    bool AtomsListAtom::mouseReleased(int x, int y, int button)
    {
        if (selectedOption > -1) {
            atoms[selectedOption]->mouseReleased(x, y, button);
        }
        return false;
    }
    
    bool AtomsListAtom::mouseMoved(int x, int y)
    {
        if (selectedOption > -1) {
            atoms[selectedOption]->mouseMoved(x, y);
        }
    }

    void AtomsListAtom::deselect()
    {
        for (int i = 0; i < atoms.size(); i++){
            atoms[i]->makeInvisible();
        }
    }
    
    void AtomsListAtom::resize(float xScale, float yScale)
    {
        OptionsListAtom::resize(xScale, yScale);
        for (int i = 0; i < atoms.size(); i++){
            atoms[i]->resize(xScale, yScale);
        }
    }
    
    /*
        SliderContainer
     */
    
    SliderContainer::SliderContainer() {}
    
    // big constructor which sets everything up, the behaviour is all in the individual components
    SliderContainer::SliderContainer(const std::string &label, FuncGetter getValue, FuncSetter setValue, double min, double max, const ArgonFont &font, RGB &colour, int precision, double x, double y, double labelWidth, double sliderWidth, double valueWidth, double padding, double height)
    {
        // set the left position of each element from x + the widths of the previous elements, plus any padding
        double sliderLeft = x + labelWidth + padding;
        double valueLeft  = x + labelWidth + sliderWidth + 2 * padding;
        
        // pass the label text, font, text colour, and size of text bounds to TextAtom. Set align to right (i.e. against the slider).
        addChild(new TextAtom(label, font, colour, POS_RIGHT, x, y, labelWidth, height));
        
        // pass the functions, min, max and size of slider bounds to SliderAtom.
        addChild(new SliderAtom(getValue, setValue, min, max, sliderLeft, y, sliderWidth, height));
        
        // pass the font, text colour, getter function, precision, and size of value text bounds to ValueAtom. Set align to left (i.e. against the slider).
        addChild(new ValueAtom(getValue, precision, font, colour, POS_LEFT, valueLeft, y, valueWidth, height));
    }
    
    int SliderContainer::PADDING = 5;
    
    /*
        CircularSliderContainer
     */
    
    // big constructor which sets everything up, the behaviour is all in the individual components
    CircularSliderContainer::CircularSliderContainer(FuncGetter getValue, FuncSetter setValue, double min, double max, const ArgonFont &font, RGB &textcolour, int precision, double x, double y, double sliderRadius, double valueWidth, double valueHeight, double padding)
    {
        
        
        // pass the functions, min, max and radius to CircularSliderAtom.
        addChild(new CircularSliderAtom(getValue, setValue, min, max, x + padding, y, sliderRadius));
        
        // pass the font, text colour, getter function, precision, and size of value text bounds to ValueAtom. Set align to centre (middle of circle)
        addChild(new ValueAtom(getValue, precision, font, textcolour, POS_CENTRE, x + padding + sliderRadius - valueWidth/2, y + sliderRadius - valueHeight, valueWidth, valueHeight));
    }
    
}
