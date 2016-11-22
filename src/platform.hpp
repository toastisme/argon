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

/*
    Basic structs of two, three and four doubles. Used for coordinates, rectangles, and colours.
 */

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
 vec2 contains 2 doubles, which can be referred to in the following ways:
    vec2.x, vec2.y
    vec2.u, vec2.v
    vec2.left, vec2.top
    vec2.elem[0], vec2.elem[1]
 */
union vec2
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
};
typedef vec2 coord;

/*
 vec3 contains 3 doubles, which can be referred to in the following ways:
    vec3.x, vec3.y, vec3.z   - xyz components
    vec3.u, vec3.v, vec3.w   - uvw components
    vec3.r, vec3.b, vec3.g   - red, blue, green colour components
    vec3.h, vec3.s, vec3.b   - hue, saturation, brightness colour components
    vec3.elem[0], vec3.elem[1], vec3.elem[2]   - array of size 3
 */
union vec3
{
    struct {
        double x, y, z;
    };
    struct {
        double u, v, w;
    };
    struct {
        union {
            struct {
                double r, g;
            };
            struct {
                double h, s;
            };
        };
        double b;
    };
    double elem[3];
};
typedef vec3 colour3;

union vec4
{
    struct {
        double w, x, y, z;
    };
    struct {
        union {
            colour3 rgb;
            colour3 hsb;
            struct {
                union {
                    struct {
                        double r, g;
                    };
                    struct {
                        double h, s;
                    };
                };
                double b;
            };
        };
        double a;
    };
    double elem[4];
};
typedef vec4 colour4;

union rect
{
    /*
        Struct defining a rectangle. Data is stored as the position of each side of the
        rectangle, and various methods are given to get the width, height, centre, etc.
     */
    
    /*
     TODO: reorder this more sensibly as left, top, right, bottom instead of left, right, top, bottom
     This means fixing initialisation via C-style initialiser lists (e.g. rect R = {0, 0, 100, 100};) in
     the rest of the codebase and replacing it with a proper constructor or static factory method
     */
    
    struct {
        union {
            double left;
            double x;
        };
        double right;
        union {
            double top;
            double y;
        };
        double bottom;
    };
    double elem[4];
    
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
    // given by the enum position, is now at the point pos
    void movePos(Position position, coord pos);
    
    // move the rect (preserving width and height) by an offset
    void moveBy(coord offset);
    
    // true if the point x, y is inside (or on the boundary of) the rect
    bool inside(double x, double y) const;
    bool inside(coord point) const;
};



/*
    Wrapper classes for images, fonts and audio
 */

class BaseImage {
public:
    BaseImage();
    
    // Images must wrap the following functionality
    virtual void loadPNG(const std::string &filename) = 0;
    virtual double getWidth() const = 0;
    virtual double getHeight() const = 0;
    virtual void draw(double x, double y, double width, double height) const = 0;
   
    // the rest is defined as calls to the above four functions
    coord getSize() const;
    void draw(double x, double y, coord size) const;
    void draw(coord size, double width, double height) const;
    void draw(coord pos, coord size) const;
    void draw(rect pos) const;
};

/*
    Drawing functions
 */

// only need to implement the first of these in the platform-specific file
void drawLine(double x0, double y0, double x1, double y1, double width, colour4 colour);
void drawLine(double x0, double y0, double x1, double y1, double width, colour3 colour);
void drawLine(coord start, coord end, double width, colour4 colour);
// ...and more overloads for convenience

// again, only need to implement the first of these in platform_OF.hpp
void drawRect(double x0, double y0, double x1, double y1, colour4 colour);
void drawRect(rect r, colour4 colour);
// ...and more overloads for convenience

/*
    Other functions
 */

void setFramerate(int rate);

#endif /* platform_hpp */
