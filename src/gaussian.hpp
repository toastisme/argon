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

//  Defines a class for containing Gaussian external potentials
//  which take the form:
//       A * exp( -a * ( (x - x0)^2 + (y-y0)^2 ) )

#ifndef gaussian_hpp
#define gaussian_hpp

#include <vector>
#include <array>
#include "utilities.hpp"

class Gaussian
{
private:
    double gAmp; // Amplitude of Gaussian, A above
    double gAlpha; // Exponent, a above
    double gex0, gey0; // (x0, y0) centre of Gaussian
    
public:
    
    Gaussian(double _gAmp, double _gAlpha, double _gex0, double _gey0); // Constructor
    Gaussian(const Gaussian& other); // Copy constructor
    
    // Get private variables
    double getgAmp() const;
    double getgAlpha() const;
    double getgex0() const;
    double getgey0() const;
    
    // Set the parameters
    void setParams(double _gAmp, double _gAlpha, double _gex0, double _gey0);
    
    // Calculate the force vector at (x, y) due to the Gaussian
    std::vector<double> calcForceEnergy(double x, double y);
    void calcForceEnergy(double x, double y, std::array<double, 3> &forceEnergy);
    
};

#endif /* gaussian_hpp */
