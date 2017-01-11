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
 vector3 contains three floats. Used as a replacement for ofPoint
 */

struct vector3{
    float x, y, z;
    vector3();
    vector3(float x, float y, float z);
};

enum COLOURNAME {
    /*
        Enum defining named colours. Their specific colour values are defined by the fromName
        static method of colour, which converts this enum to a usable colour
     */
    
    // standard pure colours
    COL_WHITE, COL_BLACK,
    COL_RED, COL_BLUE, COL_GREEN,
    
    COL_BG,             // transparent window backgrounds
    COL_TEXT,           // off-white text colour
    COL_BLUEHL,         // light blue highlight
    COL_BLUEHL_DARK,    // darker blue highlight
    COL_VIOLINGRAPH     // green violin graph colour
};

/*
 colour contains 4 unsigned chars, which are the red, green, blue and alpha components
 of the colour and can be referred to in the following ways:
    colour.r, colour.g, colour.b, colour.a
    colour.red, colour.green, colour.blue, colour.alpha
    colour.rgb[0], colour.rgb[1], colour.rgb[2], colour.a
    colour.rgba[0], colour.rgba[1], colour.rgba[2], colour.rgba[3]
 */

union colour
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
    
    static colour fromName(COLOURNAME colourname);
    
    colour();
    colour(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha);
    colour(unsigned char red, unsigned char green, unsigned char blue);
    
    void setRGB(unsigned char red, unsigned char green, unsigned char blue);
    void setRGB(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha);
    
    void setHSB(unsigned char hue, unsigned char saturation, unsigned char brightness);
    void setHSB(unsigned char hue, unsigned char saturation, unsigned char brightness, unsigned char alpha);
};

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

//enums for the openGL primitive modes used by mesh objects

enum primitiveMode{
    GL_PRIMITIVE_TRIANGLES,
    GL_PRIMITIVE_TRIANGLE_STRIP,
    GL_PRIMITIVE_TRIANGLE_FAN,
    GL_PRIMITIVE_LINES,
    GL_PRIMITIVE_LINE_STRIP,
    GL_PRIMITIVE_LINE_LOOP,
    GL_PRIMITIVE_POINTS,
#ifndef TARGET_OPENGLES
    GL_PRIMITIVE_LINES_ADJACENCY,
    GL_PRIMITIVE_LINE_STRIP_ADJACENCY,
    GL_PRIMITIVE_TRIANGLES_ADJACENCY,
    GL_PRIMITIVE_TRIANGLE_STRIP_ADJACENCY,
    GL_PRIMITIVE_PATCHES
#endif
};

/*
    Classes for images and fonts
 */

class ArgonImage {
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
    void draw(double x, double y, double width, double height, colour _colour) const;
    
    // the rest is implemented in platform.cpp as calls to the above five functions
    coord getSize() const;
    void draw(double x, double y, double width, double height) const;   // draw image to screen
    void draw(double x, double y, coord size) const;
    void draw(double x, double y, coord size, colour _colour) const;
    void draw(coord size, double width, double height) const;
    void draw(coord size, double width, double height, colour _colour) const;
    void draw(coord pos, coord size) const;
    void draw(coord pos, coord size, colour _colour) const;
    void draw(rect pos) const;
    void draw(rect pos, colour _colour) const;
};

class ArgonFont {
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
    void drawText(double x, double y, colour colour, const std::string &text) const;
    
    // the rest are implemented in platform.cpp:
    double getLineHeight() const;                                           // distance from bottom of "g" to top of "d" in "dog" (positive value)
    coord getTextSize(const std::string &text) const;                       // width and height of a given text string
    rect getTextBounds(const std::string &text) const;                      // bounding box of a given text string, with origin (0, 0)
    void drawText(coord pos, colour colour, const std::string &text) const; // use coord for position
    
    // draw text string to screen, with a specified anchor point at a position
    // e.g. drawText(100, 100, TOP_RIGHT, "Hello World!") draws the string such that its top right corner is at (100, 100)
    void drawText(double x, double y, Position anchor, colour colour, const std::string &text) const;
    void drawText(coord pos, Position anchor, colour colour, const std::string &text) const;
};

class polyline{
private:
    void *base;
public:
    polyline();
    ~polyline();
    void addVertex(float x, float y);
    std::vector<vector3> getVertices();
    void lineTo(float x, float y);
    void arc(const coord &point, float rx, float ry, float angleBegin, float angleEnd, int circleResolution = 20);
    void arc(const coord &point, float rx, float ry, float angleBegin, float angleEnd, bool blockwise, int circleResolution = 20);
    void arc(float x, float y, float rx, float ry, float angleBegin, float angleEnd, int circleResolution = 20);
    void draw();
    void draw(colour _colour, float _lineWidth);
};

class mesh{

public:
    void *base;
    mesh();
    ~mesh();
    void setMode(primitiveMode _primitiveMode) const;
    void addVertex(float x, float y, float z);
    void draw(colour _colour);
    void makeThickLine(polyline &_line, float widthSmooth);
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
void drawLine(double x0, double y0, double x1, double y1, double width, colour _colour);
void drawLine(coord start, coord end, double width, colour colour);
// ...and more overloads for convenience

// again, only need to implement the first of these in platform_OF.hpp
void drawRect(double x, double y, double width, double height, colour _colour);
void drawRect(rect r, colour colour);
// ...and more overloads for convenience
void drawCircle(double x, double y, double r, colour _colour, int resolution);
void drawEllipse(double x, double y, double rx, double ry, colour _colour, int resolution);

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
