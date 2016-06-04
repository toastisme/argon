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
    
    TextComponent::TextComponent(const std::string &_string, const ofTrueTypeFont &_font, const ofColor &_colour)
        : string(_string), font(&_font), colour(_colour)
    {
        resetBounds();
    }
    
    void TextComponent::resetBounds() {
        descenderHeight = font->getDescenderHeight();
        double width = font->stringWidth(string);
        double height = font->getLineHeight();
        stringBounds.setXYWH(0, 0, width, height);
    }
    
    void TextComponent::setString(const std::string &_string) {
        string = _string;
        resetBounds();
    }
    
    // set string using value and number of decimal places
    void TextComponent::setString(double value, int precision) {
        stringstream drawstr;
        drawstr.precision(precision);
        drawstr.setf(std::ios::fixed);
        drawstr << value;
        string = drawstr.str();
        resetBounds();
    }
    
    void TextComponent::setFont(const ofTrueTypeFont &_font) {
        font = &_font;
        resetBounds();
    }
    
    void TextComponent::setColour(const ofColor &_colour) { colour = _colour; }
    
    rect TextComponent::getStringBounds() const { return stringBounds; }
    
    void TextComponent::renderString(rect bounds, Position align) const {
        ofSetColor(colour);
        if (font) {
            rect drawRect = stringBounds;
            drawRect.movePos(align, bounds.getPos(align));
            font->drawString(string, drawRect.left, drawRect.bottom + descenderHeight);
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
        : UIAtom(x, y, width, height), colour(_colour) {}
    
    void RectAtom::render() {
        ofSetColor(colour);
        ofDrawRectangle(rect2of(bounds));
    }
    
    /*
        TextAtom
     */
    
    TextAtom::TextAtom() : UIAtom(), TextComponent() {}
    
    TextAtom::TextAtom(const std::string &string, const ofTrueTypeFont &font, const ofColor &colour, Position _align, double x, double y, double width, double height)
        : UIAtom(x, y, width, height), TextComponent(string, font, colour), align(_align)
    {}
    
    void TextAtom::render() { renderString(bounds, align); }
    
    /*
        ValueAtom
     */
    
    ValueAtom::ValueAtom() : UIAtom(), TextComponent() {}
    
    ValueAtom::ValueAtom(FuncGetter _getValue, int _precision, const ofTrueTypeFont &font, const ofColor &colour, Position _align, double x, double y, double width, double height)
        : UIAtom(x, y, width, height), TextComponent("", font, colour), getValue(_getValue), precision(_precision), align(_align)
    {
        setString(getValue(), precision);
    }
    
    void ValueAtom::render() {
        setString(getValue(), precision);
        renderString(bounds, align);
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
        ButtonPairAtom
     */
    
    ButtonPairAtom::ButtonPairAtom() : UIAtom(), status(true), imageOn(NULL), imageOff(NULL) {}
    
    ButtonPairAtom::ButtonPairAtom(FuncAction _doActionOn, const ofImage &_imageOn, FuncAction _doActionOff, const ofImage &_imageOff, double x, double y, double width, double height)
        : UIAtom(x, y, width, height), doActionOn(_doActionOn), imageOn(&_imageOn), doActionOff(_doActionOff), imageOff(&_imageOff), status(true) {}
    
    void ButtonPairAtom::render() {
        if (status) {
            imageOn->draw(bounds.left, bounds.top, bounds.width(), bounds.height());
        } else {
            imageOff->draw(bounds.left, bounds.top, bounds.width(), bounds.height());
        }
    }
    
    void ButtonPairAtom::mousePressed(int x, int y, int button) {
        if (button == 0 && bounds.inside(x, y)) {
            if (status) {
                doActionOn();  status = false;
            } else {
                doActionOff(); status = true;
            }
        }
    }
    
    /*
        SliderContainer
     */
    
    SliderContainer::SliderContainer() {}
    
    SliderContainer::SliderContainer(const std::string &label, FuncGetter getValue, FuncSetter setValue, double min, double max, const ofTrueTypeFont &font, const ofColor &colour, int precision, double x, double y, double labelWidth, double sliderWidth, double valueWidth, double height)
    {
        double sliderLeft = labelWidth + SliderContainer::PADDING;
        double valueLeft  = labelWidth + sliderWidth + 2 * SliderContainer::PADDING;
        
        addChild(new TextAtom(label, font, colour, RIGHT, 0, y, labelWidth, height));
        addChild(new SliderAtom(getValue, setValue, min, max, sliderLeft, y, sliderWidth, height));
        addChild(new ValueAtom(getValue, precision, font, colour, LEFT, valueLeft, y, valueWidth, height));
    }
    
    int SliderContainer::PADDING = 5;
    
}
