//
//  gui_derived.cpp
//  StarredMD
//
//  Created by Staszek Welsh on 21/05/2016.
//
//

#include "gui_derived.hpp"

namespace gui {
    
    ofRectangle rect2of(rect Rect) { return ofRectangle(Rect.left, Rect.top, Rect.width(), Rect.height()); }
    rect of2rect(ofRectangle Rect) { return { Rect.getLeft(), Rect.getRight(), Rect.getTop(), Rect.getBottom() }; }
    
    /*
        TextComponent
     */
    
    TextComponent::TextComponent() : font(NULL) {}
    
    TextComponent::TextComponent(const std::string &_string, const ofTrueTypeFont &_font, const ofColor &_colour, Position _align)
        : string(_string), font(&_font), colour(_colour), align(_align)
    {
        resetBounds();
    }
    
    void TextComponent::resetBounds() {
        ofRectangle bounds = font->getStringBoundingBox(string, 0, 0);
        stringBounds.left = 0;
        stringBounds.right = bounds.getRight();
        stringBounds.top = 0;
        stringBounds.bottom = -bounds.getTop();
    }
    
    void TextComponent::setString(const std::string &_string) {
        string = _string;
        resetBounds();
    }
    
    void TextComponent::setString(double value, const std::string &format) {
        char drawstr[31];
        sprintf(drawstr, format.c_str(), value);
        string = std::string(drawstr);
        resetBounds();
    }
    
    void TextComponent::setFont(const ofTrueTypeFont &_font) { font = &_font; }
    void TextComponent::setColour(const ofColor &_colour) { colour = _colour; }
    void TextComponent::setAlign(Position _align) { align = _align; }
    
    rect TextComponent::getStringBounds() const { return stringBounds; }
    
    void TextComponent::renderString(int left, int top) const {
        ofSetColor(colour);
        if (font) { font->drawString(string, left, top + stringBounds.height()); }
    }
    
    void TextComponent::renderString(rect bounds) const {
        ofSetColor(colour);
        if (font) {
            rect drawRect = stringBounds;
            drawRect.moveAnchor(align, bounds.getPos(align));
            font->drawString(string, drawRect.left, drawRect.bottom);
        }
    }
    
    
    
    /*
        SetColour
     */
    
    SetColour::SetColour() : UIAtom() {}
    SetColour::SetColour(const ofColor &_colour) : UIAtom(), colour(_colour) {}
    void SetColour::render() { ofSetColor(colour); }
    
    /*
        RectAtom
     */
    
    RectAtom::RectAtom() : UIAtom() {}
    RectAtom::RectAtom(const ofColor &_colour, double x, double y, double width, double height)
        : UIAtom(x, y, width, height)
    {
        colour = _colour;
    }
    
    void RectAtom::render() {
        ofSetColor(colour);
        ofDrawRectangle(rect2of(bounds));
    }
    
    /*
        TextAtom
     */
    
    TextAtom::TextAtom() : UIAtom(), TextComponent() {}
    
    TextAtom::TextAtom(const std::string &string, const ofTrueTypeFont &font, const ofColor &colour, Position _anchor, double x, double y, double width, double height)
        : UIAtom(x, y, width, height), TextComponent(string, font, colour, _anchor)
    {}
    
    void TextAtom::render() { renderString(bounds); }
    
    /*
        ValueAtom
     */
    
    ValueAtom::ValueAtom() : UIAtom(), value(NULL), TextComponent() {}
    
    ValueAtom::ValueAtom(FuncGetter _getValue, const std::string &_format, const ofTrueTypeFont &font, const ofColor &colour, Position anchor, double x, double y, double width, double height)
        : UIAtom(x, y, width, height), getValue(_getValue), format(_format), TextComponent("", font, colour, anchor)
    {
        setString(getValue(), format);
    }
    
    void ValueAtom::render() {
        setString(getValue(), format);
        renderString(bounds);
    }
    
    /*
        SliderAtom
     */
    
    SliderAtom::SliderAtom() : UIAtom() {}
    
    SliderAtom::SliderAtom(FuncGetter _getValue, FuncSetter _setValue, double _min, double _max, double x, double y, double width, double height)
        : UIAtom(x, y, width, height), min(_min), max(_max), getValue(_getValue), setValue(_setValue) {}
    
    double SliderAtom::getSliderPos() {
        return ofMap(getValue(), min, max, bounds.left, bounds.right - SliderAtom::HANDLE_WIDTH, true);
    }
    
    void SliderAtom::setFromSliderPos(double x) {
        x -= SliderAtom::HANDLE_WIDTH / 2;
        
        setValue(ofMap(x, bounds.left, bounds.right - SliderAtom::HANDLE_WIDTH, min, max, true));
    }
    
    void SliderAtom::render() {
        // slider body
        ofSetColor(255, 255, 255);
        double left = bounds.left;
        double top = bounds.centreY() - SliderAtom::BODY_HEIGHT / 2;
        double width = bounds.width();
        double height = SliderAtom::BODY_HEIGHT;
        ofDrawRectangle(left, top, width, height);
        
        // slider height
        ofSetColor(80, 80, 80);
        left = getSliderPos();
        top = bounds.centreY() - SliderAtom::HANDLE_HEIGHT / 2;
        width = SliderAtom::HANDLE_WIDTH;
        height = SliderAtom::HANDLE_HEIGHT;
        ofDrawRectangle(int(left), int(top), width, height);
    }
    
    void SliderAtom::mouseMoved(int x, int y) {
        if (mouseFocus) { setFromSliderPos(x); }
    }
    
    void SliderAtom::mousePressed(int x, int y, int button) {
        if (bounds.inside(x, y)) {
            mouseFocus = true;
            setFromSliderPos(x);
        }
    }
    
    void SliderAtom::mouseReleased(int x, int y, int button) {
        mouseFocus = false;
    }
    
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
    
    void ButtonAtom::render() {
        if (image) { image->draw(bounds.left, bounds.top, bounds.width(), bounds.height()); }
    }
    
    void ButtonAtom::mousePressed(int x, int y, int button) {
        if (button == 0 && bounds.inside(x, y)) { doAction(); }
    }
    
    /*
        ButtonToggleAtom
     */
    
    ButtonToggleAtom::ButtonToggleAtom() : UIAtom(), toggle(NULL), imageOn(NULL), imageOff(NULL) {}
    
    ButtonToggleAtom::ButtonToggleAtom(bool &_toggle, const ofImage &_imageOn, const ofImage &_imageOff, double x, double y, double width, double height)
        : UIAtom(x, y, width, height), imageOn(&_imageOn), imageOff(&_imageOff), toggle(&_toggle) {}
    
    void ButtonToggleAtom::render() {
        if (toggle) {
            if (*toggle) {
                if (imageOn)  {  imageOn->draw(bounds.left, bounds.top, bounds.width(), bounds.height()); }
            } else {
                if (imageOff) { imageOff->draw(bounds.left, bounds.top, bounds.width(), bounds.height()); }
            }
        }
    }
    
    void ButtonToggleAtom::mousePressed(int x, int y, int button) {
        if (button == 0 && bounds.inside(x, y)) { *toggle = not *toggle; }
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
    
    SliderContainer::SliderContainer(const std::string &label, const ofTrueTypeFont &font, const ofColor &colour, FuncGetter getValue, FuncSetter setValue, double min, double max, const std::string &format, double x, double y, double labelWidth, double sliderWidth, double valueWidth, double height)
    {
        double sliderLeft = labelWidth + SliderContainer::PADDING;
        double valueLeft  = labelWidth + sliderWidth + 2 * SliderContainer::PADDING;
        
        addChild(new TextAtom(label, font, colour, RIGHT, 0, y, labelWidth, height));
        addChild(new SliderAtom(getValue, setValue, min, max, sliderLeft, y, sliderWidth, height));
        addChild(new ValueAtom(getValue, format, font, colour, LEFT, valueLeft, y, valueWidth, height));
    }
    
    int SliderContainer::PADDING = 5;
    
    
    
    
}
