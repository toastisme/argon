/*
 StarredMD
 
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

namespace gui {
    
    /*
        TextComponent
     */

    // default constructor nulls the font pointer (which is checked for nullness before drawing), effectively disabling the object
    TextComponent::TextComponent() : font(NULL) {}
    
    // set everything and make sure stringBounds is the bounding box for the string
    TextComponent::TextComponent(const std::string &_string, const ofTrueTypeFont &_font, const ofColor &_colour)
        : string(_string), font(&_font), colour(_colour)
    {
        resetBounds();
    }
    
    // set the string bounds and descender height properly - stringBounds only needs the width and height of the string
    void TextComponent::resetBounds() {
        descenderHeight = font->getDescenderHeight();
        double width = font->stringWidth(string);
        double height = font->getLineHeight();
        stringBounds.setXYWH(0, 0, width, height);
    }
    
    // set the string and make sure the bounds are correct
    void TextComponent::setString(const std::string &_string) {
        string = _string;
        resetBounds();
    }
    
    // set string using value and number of decimal places
    void TextComponent::setString(double value, int precision) {
        stringstream drawstr;
        drawstr.precision(precision);    // set precision
        drawstr.setf(std::ios::fixed);   // use precision based on number of decimal places (fixed point)
        drawstr << value;
        string = drawstr.str();
        resetBounds();
    }
    
    // changing the font changes the size, so recalculate string bounds
    void TextComponent::setFont(const ofTrueTypeFont &_font) {
        font = &_font;
        resetBounds();
    }
    
    void TextComponent::setColour(const ofColor &_colour) { colour = _colour; }
    
    rect TextComponent::getStringBounds() const { return stringBounds; }
    
    // actually draw the string to the screen, aligned within a larger rectangle
    // typically, this larger rectangle is ofBase::bounds
    void TextComponent::renderString(rect bounds, Position align) const {
        if (font) {                                                                       // font might be null, so check
            ofSetColor(colour);
            rect drawRect = stringBounds;                                                 // make local copy of stringBounds
            drawRect.movePos(align, bounds.getPos(align));                                // align the string within the larger rectangle
            font->drawString(string, drawRect.left, drawRect.bottom + descenderHeight);   // drawString takes the left position and the baseline height
        }
    }
    
    
    
    /*
        SetColour
     */
    
    // all this class does is set the draw colour using ofSetColor
    SetColour::SetColour() : UIAtom(), colour(ofColor(0, 0, 0)) {}
    
    SetColour::SetColour(const ofColor &_colour) : UIAtom(), colour(_colour) {}
    
    void SetColour::render() { ofSetColor(colour); }
    
    /*
        RectAtom
     */
    
    // defaults to black rect at UIAtom default constructor values: left 0, top 0, width 0, height 0
    RectAtom::RectAtom() : UIAtom(), colour(ofColor(0, 0, 0)) {}
    
    // store the size in ofBase::bounds
    RectAtom::RectAtom(const ofColor &_colour, double x, double y, double width, double height)
        : UIAtom(x, y, width, height), colour(_colour) {}
    
    // draw the rect
    void RectAtom::render() {
        ofSetColor(colour);
        ofDrawRectangle(bounds.left, bounds.top, bounds.width(), bounds.height());
    }
    
    /*
        TextAtom
     */
    
    TextAtom::TextAtom() : UIAtom(), TextComponent(), align(POS_TOP_LEFT) {}
    
    // pass things through to TextComponent, and store align
    TextAtom::TextAtom(const std::string &string, const ofTrueTypeFont &font, const ofColor &colour, Position _align, double x, double y, double width, double height)
        : UIAtom(x, y, width, height), TextComponent(string, font, colour), align(_align)
    {}
    
    // render the string aligned within bounds
    void TextAtom::render() { renderString(bounds, align); }
    
    /*
        ValueAtom
     */
    
    ValueAtom::ValueAtom() : UIAtom(), TextComponent() {}
    
    // pass stuff into TextComponent, set the function, precision and align, and set the string using getValue
    ValueAtom::ValueAtom(FuncGetter _getValue, int _precision, const ofTrueTypeFont &font, const ofColor &colour, Position _align, double x, double y, double width, double height)
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
        return ofMap(getValue(), min, max, bounds.left, bounds.right - SliderAtom::HANDLE_WIDTH, true);
    }
    
    // set the value using setValue, obtained by mapping the x-coordinate of the
    // slider to between min and max. Note that we make the same slider-handle-
    // within-slider-body correction, and subtract half the slider width from x so
    // that it corresponds to the centre of the slider handle, not the left side
    void SliderAtom::setFromSliderPos(double x) {
        x -= SliderAtom::HANDLE_WIDTH / 2;
        
        setValue(ofMap(x, bounds.left, bounds.right - SliderAtom::HANDLE_WIDTH, min, max, true));
    }
    
    void SliderAtom::render() {
        // slider body
        ofSetColor(BODY_COLOR);
        // set size of body using bounds and BODY_HEIGHT
        double left = bounds.left;
        double top = bounds.centreY() - SliderAtom::BODY_HEIGHT / 2;
        double width = bounds.width();
        double height = SliderAtom::BODY_HEIGHT;
        ofDrawRectangle(left, top, width, height);
        
        // slider height
        ofSetColor(HANDLE_COLOR);
        // set size of handle using its x-position, HANDLE_WIDTH and HANDLE_HEIGHT
        left = getSliderPos();
        top = bounds.centreY() - SliderAtom::HANDLE_HEIGHT / 2;
        width = SliderAtom::HANDLE_WIDTH;
        height = SliderAtom::HANDLE_HEIGHT;
        ofDrawRectangle(int(left), top, width, height);   // int(left) stops the slider handle from being
                                                          // blurry due to being drawn off the pixel grid
    }
    
    // if the mouse is left-clicked inside the slider, take mouse focus and update the slider's position
    void SliderAtom::mousePressed(int x, int y, int button) {
        if (button == 0 && bounds.inside(x, y)) {
            mouseFocus = true;
            setFromSliderPos(x);
        }
    }
    
    // if the mouse is moved, and we have mouse focus, update the slider's position
    void SliderAtom::mouseMoved(int x, int y) {
        if (mouseFocus) { setFromSliderPos(x); }
    }
    
    // if the mouse is released (anywhere on the screen), lose mouse focus
    void SliderAtom::mouseReleased(int x, int y, int button) {
        mouseFocus = false;
    }
    
    // some static varaibles to ensure sliders are drawn the same
    // these could be customised on a per-slider basis, but the slider constructor is long
    // enough already and we probably want sliders to look the same anyway
    ofColor SliderAtom::BODY_COLOR = ofColor(255, 255, 255);
    ofColor SliderAtom::HANDLE_COLOR = ofColor(80, 80, 80);
    
    int SliderAtom::BODY_HEIGHT = 10;
    int SliderAtom::HANDLE_WIDTH = 7;
    int SliderAtom::HANDLE_HEIGHT = 20;
    
    /*
        ButtonAtom
     */
    
    ButtonAtom::ButtonAtom() : UIAtom(), image(NULL) {}
    
    ButtonAtom::ButtonAtom(FuncAction _doAction, const ofImage &_image, double x, double y, double width, double height)
        : UIAtom(x, y, width, height), doAction(_doAction), image(&_image)
    { }
    
    // render just draws the button image
    void ButtonAtom::render() {
        if (image) { image->draw(bounds.left, bounds.top, bounds.width(), bounds.height()); }
    }
    
    // if the mouse is left-clicked inside the button, do its action
    void ButtonAtom::mousePressed(int x, int y, int button) {
        if (button == 0 && bounds.inside(x, y)) { doAction(); }
    }
    
    /*
        ButtonToggleAtom
     */
    
    ButtonToggleAtom::ButtonToggleAtom() : UIAtom(), imageOn(NULL), imageOff(NULL) {}
    
    ButtonToggleAtom::ButtonToggleAtom(FuncGetterBool _getBool, FuncSetterBool _setBool, const ofImage &_imageOn, const ofImage &_imageOff, double x, double y, double width, double height)
        : UIAtom(x, y, width, height), getBool(_getBool), setBool(_setBool), imageOn(&_imageOn), imageOff(&_imageOff) {}
    
    // render button image based on the value of getBool
    void ButtonToggleAtom::render() {
        if (getBool()) {
            if (imageOn)  {  imageOn->draw(bounds.left, bounds.top, bounds.width(), bounds.height()); }
        } else {
            if (imageOff) { imageOff->draw(bounds.left, bounds.top, bounds.width(), bounds.height()); }
        }
    }
    
    // if the mouse is left-clicked inside the button, invert the value of the boolean
    void ButtonToggleAtom::mousePressed(int x, int y, int button) {
        if (button == 0 && bounds.inside(x, y)) { setBool(not getBool()); }
    }
    
    /*
     PotentialAtom
     */
    
    PotentialAtom::PotentialAtom(md::MDContainer &system, double minx, double maxx, int _numPoints, int x, int y, int width, int height): theSystem(system), min_x(minx), max_x(maxx), numPoints(_numPoints), UIAtom(x, y, width, height) {

    }
    void PotentialAtom::render(){

        DrawPotential(theSystem.getPotential());
        
    }
     
    void PotentialAtom::DrawPotential(PotentialFunctor& pot){
        std::vector<double> xpoints, ypoints, partx, party;
        
        double x, y;
        double min_y = 9999, max_y = 0;
        double x_spacing = (max_x - min_x) / (numPoints - 1);
        
        // Set up particle separations, relative to particle 0
        coord pos1 = theSystem.getPos(0);
        for (int i = 1; i < theSystem.getN(); i++){
            coord pos = theSystem.getPos(i);
            x = pos.x - pos1.x;
            y = pos.y - pos1.y;
            x = sqrt(x*x + y*y);
            partx.push_back(x);
        }
        
        // Obtain x and y values for potential calculations
        for (int i = 0; i < numPoints; i++){
            x = min_x + i * x_spacing;
            xpoints.push_back(x);
            y = pot.potential(x);
            max_y = ( y > max_y ? y : max_y);
            min_y = ( y < min_y ? y : min_y);
            ypoints.push_back(y);
        }
        
        // TODO: do we need to truncate min_y as well?
        max_y = ( max_y > 2.0 ? 2.0 : max_y );
        
        // Calculate y values for the particles and add to party
        for (int i = 0; i < theSystem.getN() - 1; i++){
            x = partx[i];
            y = pot.potential(x);
            party.push_back(y);
        }
        // Map the potential values to the UI
        for (int i = 0; i < xpoints.size(); i++){
            xpoints[i] = ofMap(xpoints[i], min_x, max_x, getRect().left, getRect().right, true);
            ypoints[i] = ofMap(ypoints[i], min_y, max_y, getRect().top, getRect().bottom, true);
        }
        
        // Map the particle values to the UI
        for (int i = 0; i < theSystem.getN()-1; i++){
            partx[i] = ofMap(partx[i], min_x, max_x, getRect().left, getRect().right, true);
            party[i] = ofMap(party[i], min_y, max_y, getRect().top, getRect().bottom, true);
        }
        
        // Plot the potential
        
        ofSetLineWidth(3.5);
        ofSetColor(255,255,255, 220);
        for (int i = 0; i < xpoints.size() - 1; i++){
            ofDrawLine(xpoints[i], ofGetHeight()- ypoints[i], xpoints[i+1], ofGetHeight() - ypoints[i+1]);
        }
        
        // Plot the particles along the curve
       
        ofSetCircleResolution(10);
        ofSetColor(0, 100, 220, 220);
        for (int i = 0; i < theSystem.getN(); i++){
            ofDrawCircle(partx[i], ofGetHeight() - party[i], 4);
        }
        
    }
    
    
    /*
      CustomPotentialAtom
     */
    
    CustomPotentialAtom::CustomPotentialAtom(md::MDContainer &system, int minx, int maxx, int numPoints, int sideWidth, int x, int y, int width, int height) : PotentialAtom(system, minx, maxx, numPoints, x, y, width, height) {
    
        // Rescale the default position of the four-point spline
        // Only occurs the first time the custom potential is selected
        double x0 = ofMap(min_x*1.025, min_x, max_x, sideWidth + 40, ofGetWidth() - 40, true);
        double x_end = ofMap(max_x, min_x, max_x, sideWidth + 40, ofGetWidth() - 40, true);
        double y0 = ofGetHeight()/5.5;
        double y_end = ofGetHeight()/1.8;
        
        // Move the two points to the scaled position;
        CustomPotential& customPotential = theSystem.getCustomPotential();
        customPotential.getSpline().movePoint(1, x_end, y_end, 0);
        customPotential.getSpline().movePoint(0, x0, y0, 0);

    
    }
    
    void CustomPotentialAtom::DrawPotential(PotentialFunctor &pot) {
        
        CustomPotential& customPotential = theSystem.getCustomPotential();
        
        // Once the initial two-point spline has been rescaled, update the spline each iteration
 
        // Set color, position of each point in the spline
        for (int i = 1; (i < customPotential.getSpline().points()+1); i++){
            ofSetColor(0, 200, 240,200);
            ofFill();
            ofDrawCircle(customPotential.getSpline().getPoint(i-1).x, customPotential.getSpline().getPoint(i-1).y, 10);
            ofSetColor(255, 255, 255,90);
            ofNoFill();
            ofDrawCircle(customPotential.getSpline().getPoint(i-1).x, customPotential.getSpline().getPoint(i-1).y, 10);
        }
            
        // Map every x value to the UI and obtain y, add each value to ypoints
        PotentialAtom::DrawPotential(pot);
    }
    
    void CustomPotentialAtom::render() {
        DrawPotential(theSystem.getCustomPotential());
    }
    
    void CustomPotentialAtom::mouseMoved(int x, int y) {}
    void CustomPotentialAtom::mousePressed(int x, int y, int button) {}
    void CustomPotentialAtom::mouseReleased(int x, int y, int button) {}
    
    /*
        SliderContainer
     */
    
    SliderContainer::SliderContainer() {}
    
    // big constructor which sets everything up, the behaviour is all in the individual components
    SliderContainer::SliderContainer(const std::string &label, FuncGetter getValue, FuncSetter setValue, double min, double max, const ofTrueTypeFont &font, const ofColor &colour, int precision, double x, double y, double labelWidth, double sliderWidth, double valueWidth, double padding, double height)
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
}
