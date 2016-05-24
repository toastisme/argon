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
    
    void TextComponent::renderString(rect absoluteBounds) const {
        ofSetColor(colour);
        if (font) {
            rect drawRect = stringBounds;
            drawRect.moveAnchor(align, absoluteBounds.getPos(align));
            font->drawString(string, drawRect.left, drawRect.bottom);
        }
    }
    
    
    
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
    
    void TextAtom::render() { renderString(absoluteRect()); }
    
    /*
        ValueAtom
     */
    
    ValueAtom::ValueAtom() : UIAtom(), value(NULL), TextComponent() {}
    
    ValueAtom::ValueAtom(double (md::MDContainer::*_getValue)() const, md::MDContainer *system, const std::string &_format, const ofTrueTypeFont &font, const ofColor &colour, Position anchor, double x, double y, double width, double height)
        : UIAtom(x, y, width, height), value(NULL), format(_format), TextComponent("", font, colour, anchor)
    {
        getValue = std::bind(_getValue, system);
        setString(getValue(), format);
    }
    
    ValueAtom::ValueAtom(double *_value, const std::string &_format, const ofTrueTypeFont &font, const ofColor &colour, Position anchor, double x, double y, double width, double height)
        : UIAtom(x, y, width, height), value(_value), format(_format), TextComponent("", font, colour, anchor)
    {
        getValue = [&] () { return *value; };
        setString(getValue(), format);
    }
    
    void ValueAtom::render() {
        setString(getValue(), format);
        renderString(absoluteRect());
    }
    
    /*
        SliderAtom
     */
    
    SliderAtom::SliderAtom() : UIAtom() {}
    
    SliderAtom::SliderAtom(double (md::MDContainer::*_getValue)() const, void (md::MDContainer::*_setValue)(double), md::MDContainer *system, double _min, double _max, double x, double y, double width, double height)
        : UIAtom(x, y, width, height), value(NULL), min(_min), max(_max)
    {
        getValue = std::bind(_getValue, system);
        setValue = std::bind(_setValue, system, std::placeholders::_1);
    }
    
    SliderAtom::SliderAtom(double *_value, double _min, double _max, double x, double y, double width, double height)
        : UIAtom(x, y, width, height), value(_value), min(_min), max(_max)
    {
        getValue = [&] () { return *value; };
        setValue = [&] (double set) { *value = set; };
    }
    
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
    
    ButtonAtom::ButtonAtom() : UIAtom(), toggle(NULL), imageOn(NULL), imageOff(NULL) {}
    
    ButtonAtom::ButtonAtom(bool &_toggle, const ofImage &_imageOn, const ofImage &_imageOff, double x, double y, double width, double height)
        : UIAtom(x, y, width, height), imageOn(&_imageOn), imageOff(&_imageOff), toggle(&_toggle) {}
    
    void ButtonAtom::render() {
        if (toggle) {
            if (*toggle) {
                if (imageOn)  {  imageOn->draw(bounds.left, bounds.top, bounds.width(), bounds.height()); }
            } else {
                if (imageOff) { imageOff->draw(bounds.left, bounds.top, bounds.width(), bounds.height()); }
            }
        }
    }
    
    void ButtonAtom::mousePressed(int x, int y, int button) {
        if (button == 0 && bounds.inside(x, y)) { *toggle = not *toggle; }
    }
    
    /*
        SliderContainer
     */
    
    SliderContainer::SliderContainer() {}
    
    SliderContainer::SliderContainer(const std::string &label, const ofTrueTypeFont &font, const ofColor &colour, double (md::MDContainer::*getValue)() const, void (md::MDContainer::*setValue)(double), md::MDContainer *system, double min, double max, const std::string &format, double x, double y, double labelWidth, double sliderWidth, double valueWidth, double height)
    {
        double labelLeft  = x;
        double sliderLeft = x + labelWidth + SliderContainer::PADDING;
        double valueLeft  = x + labelWidth + sliderWidth + 2 * SliderContainer::PADDING;
        
        addChild(new TextAtom(label, font, colour, RIGHT, labelLeft, y, labelWidth, height));
        addChild(new SliderAtom(getValue, setValue, system, min, max, sliderLeft, y, sliderWidth, height));
        addChild(new ValueAtom(getValue, system, format, font, colour, LEFT, valueLeft, y, valueWidth, height));
    }

    SliderContainer::SliderContainer(const std::string &label, const ofTrueTypeFont &font, const ofColor &colour, double *value, double min, double max, const std::string &format, double x, double y, double labelWidth, double sliderWidth, double valueWidth, double height)
    {
        double labelLeft  = x;
        double sliderLeft = x + labelWidth + SliderContainer::PADDING;
        double valueLeft  = x + labelWidth + sliderWidth + 2 * SliderContainer::PADDING;
        
        addChild(new TextAtom(label, font, colour, RIGHT, labelLeft, y, labelWidth, height));
        addChild(new SliderAtom(value, min, max, sliderLeft, y, sliderWidth, height));
        addChild(new ValueAtom(value, format, font, colour, LEFT, valueLeft, y, valueWidth, height));
    }
    
    int SliderContainer::PADDING = 5;
    
}