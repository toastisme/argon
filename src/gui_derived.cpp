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
    
    rect TextComponent::getStringBounds() const { return stringBounds; }
    
    void TextComponent::renderString(int left, int top) const {
        ofSetColor(colour);
        if (font) { font->drawString(string, left, top + stringBounds.height()); }
    }
    
    
    /*
        RectAtom
     */
    
    RectAtom::RectAtom() : UIAtom() {}
    RectAtom::RectAtom(double x, double y, double width, double height, ofColor _colour)
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
    
    TextAtom::TextAtom(const std::string &string, const ofTrueTypeFont &font, const ofColor &colour, double x, double y)
        : UIAtom(x, y), TextComponent(string, font, colour)
    {
        rect stringBounds = getStringBounds();
        bounds = stringBounds.offset({bounds.left, bounds.top});
    }
    
    void TextAtom::render() { renderString(bounds.left, bounds.top); }
    
    /*
        ValueAtom
     */
    
    ValueAtom::ValueAtom() : UIAtom(), value(NULL), TextComponent() {}
    
    ValueAtom::ValueAtom(const double (md::MDContainer::*_getValue)(), md::MDContainer *system, const std::string &_format, const ofTrueTypeFont &font, const ofColor &colour, double x, double y)
        : UIAtom(x, y), value(NULL), format(_format), TextComponent("", font, colour)
    {
        getValue = std::bind(_getValue, system);
        setString(getValue(), format);
        
        rect stringBounds = getStringBounds();
        bounds = stringBounds.offset({bounds.left, bounds.top});
    }
    
    ValueAtom::ValueAtom(double *_value, const std::string &_format, const ofTrueTypeFont &font, const ofColor &colour, double x, double y)
        : UIAtom(x, y), value(_value), format(_format), TextComponent("", font, colour)
    {
        getValue = [&] () { return *value; };
        setString(getValue(), format);
        
        rect stringBounds = getStringBounds();
        bounds = stringBounds.offset({bounds.left, bounds.top});
    }
    
    void ValueAtom::render() {
        setString(getValue(), format);
        renderString(bounds.left, bounds.top);
    }
    
    /*
        SliderAtom
     */
    
    SliderAtom::SliderAtom() : UIAtom() {}
    
    SliderAtom::SliderAtom(const double (md::MDContainer::*_getValue)(), void (md::MDContainer::*_setValue)(double), md::MDContainer *system, double _min, double _max, int _width, double x, double y)
        : UIAtom(x, y), value(NULL), min(_min), max(_max), width(_width)
    {
        getValue = std::bind(_getValue, system);
        setValue = std::bind(_setValue, system, std::placeholders::_1);
        
        bounds.right = x + _width;
        bounds.bottom = y + 20;
    }
    
    double SliderAtom::getSliderPos() { return ofMap(getValue(), min, max, bounds.left, bounds.right, true); }
    void SliderAtom::setFromSliderPos(double x) { setValue(ofMap(x, bounds.left, bounds.right, min, max, true)); }
    
    void SliderAtom::render() {
        ofSetColor(255, 255, 255);
        ofDrawRectangle(rect2of(bounds));
        ofSetColor(255, 0, 0);
        ofDrawRectangle(getSliderPos(), bounds.top, 1, 20);
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

}
