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

#include "platform.hpp"
#include "ofMain.h"

static int glColorRGB(RGB colour) {
    glColor4ub(colour.r, colour.g, colour.b, colour.a);     // set the openGL draw colour
}

/*
    ArgonMesh
 */

void ArgonMesh::draw(RGB colour, MeshPrimitive primitive, double linewidth) const {
    glColorRGB(colour);                                 // set the colour
    glLineWidth(linewidth);                             // set the linewidth
    
    glEnableClientState(GL_VERTEX_ARRAY);               // ensure we can send OpenGL a vertex array
    glVertexPointer(2, GL_DOUBLE, 0, points.data());    // set a pointer to the array with two doubles per point and 0 padding
    
    int glPrim;
    switch (primitive) {                                // convert MeshPrimitive into the actual openGL primitive index
        case PRIMITIVE_POINTS:         { glPrim = GL_POINTS;         } break;
        case PRIMITIVE_LINES:          { glPrim = GL_LINES;          } break;
        case PRIMITIVE_LINE_STRIP:     { glPrim = GL_LINE_STRIP;     } break;
        case PRIMITIVE_LINE_LOOP:      { glPrim = GL_LINE_LOOP;      } break;
        case PRIMITIVE_TRIANGLES:      { glPrim = GL_TRIANGLES;      } break;
        case PRIMITIVE_TRIANGLE_STRIP: { glPrim = GL_TRIANGLE_STRIP; } break;
        case PRIMITIVE_TRIANGLE_FAN:   { glPrim = GL_TRIANGLE_FAN;   } break;
        default:                       { glPrim = GL_POINTS;         } break;
    }
    
    glDrawArrays(glPrim, 0, points.size());             // pass the coordinates as a direct array to openGL
}

/*
    Drawing functions
 */

void drawLine(double x0, double y0, double x1, double y1, double width, RGB colour) {
    glColorRGB(colour);     // set the draw colour
    glLineWidth(width);     // set the line width
    
    glBegin(GL_LINES);      // draw a line from (x0, y0) to (x1, y1)
    glVertex2d(x0, y0);
    glVertex2d(x1, y1);
    glEnd();
}

void drawRect(double x0, double y0, double width, double height, RGB colour) {
    glColorRGB(colour);                         // set the draw colour
    glRectd(x0, y0, x0 + width, y0 + height);   // draw the rectangle
}


void drawEllipse(double x, double y, double rx, double ry, RGB colour, int resolution) {
    glColorRGB(colour);
    
    // draw a polygon with (resolution) number of sides
    float theta = 2.0f * 3.14159265f / float(resolution);
    float cos_t = cosf(theta);
    float sin_t = sinf(theta);
    
    float dx = 1;                               // (dx, dy) is a point on the outside of a unit circle
    float dy = 0;
    
    glBegin(GL_TRIANGLE_FAN);
    glVertex2d(x, y);                           // add point at centre of circle for triangles to fan out from
    for (int i = 0; i < resolution; ++i) {
        glVertex2f(x + rx * dx, y + ry * dy);   // scale dx and dy to ellipse size and add vertex on outside of circle
        
        float temp = dx;                        // rotate dx about (0, 0) by angle theta
        dx = cos_t * dx   - sin_t * dy;
        dy = sin_t * temp + cos_t * dy;
    }
    glVertex2d(x + rx, y);                      // add final point to complete circle
    glEnd();
}

void setScissorClip(double x, double y, double width, double height) {
    glEnable(GL_SCISSOR_TEST);          // enable scissor clipping
    glScissor(x, y, width, height);     // set scissor region to given rectangle
}

void setScissorClip() {
    glDisable(GL_SCISSOR_TEST);         // if called with no arguments, disable scissor clipping
}