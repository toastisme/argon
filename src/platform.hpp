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

#ifndef platform_hpp
#define platform_hpp

#include <string>
#include <vector>

enum Position
{
    /*
        Enum defining nine positions within a rect.
     */
    
    POS_TOP_LEFT, POS_TOP, POS_TOP_RIGHT,
    POS_LEFT, POS_CENTRE, POS_RIGHT,
    POS_BOTTOM_LEFT, POS_BOTTOM, POS_BOTTOM_RIGHT
};

/*
 coord contains 2 doubles, which can be referred to in the following ways:
    coord.x, coord.y
    coord.u, coord.v
    coord.left, coord.top
    coord.elem[0], coord.elem[1]
 */
union coord
{
    struct {
        double x, y;
    };
    struct {
        double u, v;
    };
    struct {
        double left, top;
    };
    double elem[2];
    
    coord();
    coord(double x, double y);
    
    void setXY(double x, double y);
};

/*
 RGB contains 4 unsigned chars, which are the red, green, blue and alpha components
 of the colour and can be referred to in the following ways:
    RGB.r, RGB.g, RGB.b, RGB.a
    RGB.red, RGB.green, RGB.blue, RGB.alpha
    RGB.rgb[0], RGB.rgb[1], RGB.rgb[2], RGB.a
    RGB.rgba[0], RGB.rgba[1], RGB.rgba[2], RGB.rgba[3]
 */

union RGB
{
    struct {
        unsigned char r, g, b, a, hue, saturation, brightness;
    };
    struct {
        union {
            struct {
                unsigned char red, green, blue;
            };
            unsigned char rgb[3];
        };
        unsigned char alpha;
    };
    unsigned char rgba[4];
    
    RGB();
    RGB(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha);
    RGB(unsigned char red, unsigned char green, unsigned char blue);
    
    void setRGB(unsigned char red, unsigned char green, unsigned char blue);
    void setRGB(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha);
    
    void setHSB(unsigned char hue, unsigned char saturation, unsigned char brightness);
    void setHSB(unsigned char hue, unsigned char saturation, unsigned char brightness, unsigned char alpha);
};

// Some standard colours to avoid having to redefine these every time
static const RGB RGB_WHITE         = RGB(255, 255, 255, 255);
static const RGB RGB_BLACK         = RGB(  0,   0,   0, 255);
static const RGB RGB_RED           = RGB(255,   0,   0, 255);
static const RGB RGB_GREEN         = RGB(  0, 255,   0, 255);
static const RGB RGB_BLUE          = RGB(  0,   0, 255, 255);

static const RGB RGB_BACKGROUND    = RGB( 80,  80,  80,  80);
static const RGB RGB_TEXT          = RGB(255, 255, 240, 255);
static const RGB RGB_HIGHLIGHT     = RGB( 82, 225, 247, 255);
static const RGB RGB_HIGHLIGHT_ALT = RGB( 10, 174, 199, 255);
static const RGB RGB_VIOLINGRAPH   = RGB(186, 255, 163,  80);

union rect
{
    /*
        Struct defining a rectangle. Data is stored as the position of each side of the
        rectangle, and various methods are given to get the width, height, centre, etc.
     */
    
    struct {
        union {
            coord min;
            struct {
                union {
                    double left;
                    double x;
                };
                union {
                    double top;
                    double y;
                };
            };
        };
        union {
            coord max;
            struct {
                double right, bottom;
            };
        };
    };
    double elem[4];
    
    rect();
    rect(double left, double top, double right, double bottom);
    rect(coord min, coord max);
    
    double width()   const;
    double height()  const;
    double centreX() const;   // x coordinate of centre
    double centreY() const;   // y coordinate of centre
    
    // set rect to location and size either by left/right/top/bottom or by x/y/width/height
    void setLRTB(double left, double right, double top, double bottom);
    void setXYWH(double x, double y, double width, double height);
    
    // get the coordinates of a point within the rect specified by the Position enum,
    // i.e. getPos(TOP_LEFT) == {left, top}
    coord getPos(Position position) const;
    
    // move the rect (preserving width and height) such that a specified position in the rectangle,
    // given by the enum position, is now at the given coordinates
    void movePos(Position position, double x, double y);
    void movePos(Position position, coord pos);
    
    // move the rect (preserving width and height) by an offset
    void moveBy(double x, double y);
    void moveBy(coord offset);
    
    // true if the point x, y is inside (or on the boundary of) the rect
    bool inside(double x, double y) const;
    bool inside(coord point) const;
};

/*
    Classes for images and fonts
 */

class ArgonImage
{
private:
    // a generic pointer to the base image class for whatever backend we use
    // cast this to whatever type we need it to be, then call its methods
    void *base;
    
public:
    // The platform-specific layer must implement the constructor, destructor, and following four methods:
    ArgonImage();
    ~ArgonImage();
    
    void loadPNG(const std::string &filename);                          // load a PNG file
    double getWidth() const;                                            // return image width
    double getHeight() const;                                           // return image height
    void draw(double x, double y, double width, double height, RGB colour) const;
    
    // the rest is implemented in platform.cpp as calls to the above five functions
    coord getSize() const;
    void draw(double x, double y, double width, double height) const;   // draw image to screen
    void draw(double x, double y, coord size) const;
    void draw(double x, double y, coord size, RGB colour) const;
    void draw(coord size, double width, double height) const;
    void draw(coord size, double width, double height, RGB colour) const;
    void draw(coord pos, coord size) const;
    void draw(coord pos, coord size, RGB colour) const;
    void draw(rect pos) const;
    void draw(rect pos, RGB colour) const;
};

class ArgonFont
{
private:
    // pointer to backend class if needed
    void *base;
    
public:
    // The platform-specifc layer should implement:
    ArgonFont();
    ~ArgonFont();
    
    void loadTTF(const std::string &filename, int fontsize);            // load a TTF file with given fontsize
    double getAscenderHeight() const;                                   // distance from bottom of "o" to top of "d" in "dog"    (positive value)
    double getDescenderHeight() const;                                  // distance from bottom of "o" to bottom of "g" in "dog" (negative value)
    double getTextWidth(const std::string &text) const;                 // width of given text string
    // draw text string to screen, with given colour, such that left end of baseline is at (x, y)
    void drawText(double x, double y, RGB colour, const std::string &text) const;
    
    // the rest are implemented in platform.cpp:
    double getLineHeight() const;                                           // distance from bottom of "g" to top of "d" in "dog" (positive value)
    coord getTextSize(const std::string &text) const;                       // width and height of a given text string
    rect getTextBounds(const std::string &text) const;                      // bounding box of a given text string, with origin (0, 0)
    void drawText(coord pos, RGB colour, const std::string &text) const;    // use coord for position
    
    // draw text string to screen, with a specified anchor point at a position
    // e.g. drawText(100, 100, TOP_RIGHT, "Hello World!") draws the string such that its top right corner is at (100, 100)
    void drawText(double x, double y, Position anchor, RGB colour, const std::string &text) const;
    void drawText(coord pos, Position anchor, RGB colour, const std::string &text) const;
};

// Enum defining the possible primitives for an ArgonMesh
enum MeshPrimitive
{
    PRIMITIVE_POINTS,
    PRIMITIVE_LINES,
    PRIMITIVE_LINE_STRIP,
    PRIMITIVE_LINE_LOOP,
    PRIMITIVE_TRIANGLES,
    PRIMITIVE_TRIANGLE_STRIP,
    PRIMITIVE_TRIANGLE_FAN
};

class ArgonMesh
{
    // A class to emulate an OpenGL mesh
    // Create a series of points using addVertex
    // Then draw them to the screen as an OpenGL primitive
private:
    std::vector<coord> points;
    
public:
    void draw(RGB colour, MeshPrimitive primitive, double linewidth = 1) const;
    
    void addVertex(double x, double y);
    void addVertex(coord pos);
};

/*
    Mic input functions
 */

void setMicVolume(double input);
double getMicVolume();

void setMicActive(bool active);
bool getMicActive();
void toggleMicActive();

/*
    Drawing functions
 */

// only need to implement the first of these in the platform-specific file
void drawLine(double x0, double y0, double x1, double y1, double width, RGB colour);
void drawLine(coord start, coord end, double width, RGB colour);

// again, only need to implement the first of these in platform_OF.hpp
void drawRect(double x, double y, double width, double height, RGB colour);
void drawRect(rect r, RGB colour);

void drawEllipse(double x, double y, double rx, double ry, RGB colour, int resolution);
void drawCircle(double x, double y, double r, RGB colour, int resolution);

// set a region that restricts where the draw functions can draw
// don't forget to call setScissorClip() with no arguments at the end to reset it!
void setScissorClip(double x, double y, double width, double height);
void setScissorClip(rect r);
void setScissorClip(); // overload with no arguments disables the scissor clip

/*
    Other functions
 */

// Implemented in platform-specific layer
int windowWidth();      // width of window
int windowHeight();     // height of window
double timeElapsed();   // time since program began in seconds

// Implemented in platform.cpp
coord windowSize();     // width and height of window
rect  windowBounds();   // rect from (0, 0) to (window width, window height)

#endif /* platform_hpp */
