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

#ifndef potentials_hpp
#define potentials_hpp

#include "utilities.hpp"
#include "cubicspline.hpp"

// Base class - currently just an empty functor defining what every potential
// functor must have, which is a force/energy calculating method with arguments
// separation rij, force, and returning the potential energy.

class PotentialFunctor
{
public:
    // Pure virtual functions, must be overridden in child classes
    virtual double operator()(double rij, coord& force) = 0;
    virtual double potential(double rij) = 0;
};


// Lennard-Jones potential
// V(rij) = 4 * epsilon * [ (sigma/rij)^repelPow - (sigma/rij)^attractPow ]
class LennardJones : public PotentialFunctor
{
private:
    int repelPow, attractPow; // E.g. 12-6 potential would be repelPow = 12, attractPow = 6
    double epsilon, sigma; // The well-depth and zero-potential distance
public:
    // Constructor
    LennardJones(); // Defaults to a 12-6 potential with epsilon = sigma = 1
    
    // Force/energy calculation
    double operator()(double rij, coord& force);
    // Just energy
    double potential(double rij);
    
    // Set the repel/attract powers
    void setPowers(int _repel, int _attract);
    // Set epsilon and sigma
    void setEpsilon(double _epsilon);
    void setSigma(double _sigma);
};

// Square well potential
// V(rij) = V0 for  rMin < rij < rMax, and `infinity' otherwise
class SquareWell : public PotentialFunctor
{
private:
    double V0; // Value of potential in the well
    double rMin, rMax; // The potential is V0 for rMin < rij < rMax
    double steepness, intercept; // Hard wall approximation at left of well
public:
    // Constructor
    SquareWell(); // Defaults to V0 = -1, rMin = 1 and rMax = 2
    
    // Force/energy calculation
    double operator()(double rij, coord& force);
    // Just energy
    double potential(double rij);
    
    // Set well-depth, rMin and rMax
    void setV0(double _v0);
    void setBounds(double _rMin, double _rMax);
    void setIntercept(double _intercept);
    
    // Determine steepness
    void calcSteepness();
};

// Morse potential
// V(rij) = De( 1 - exp( -a( rij - req ) ) )^2
class Morse : public PotentialFunctor
{
private:
    double De; // The well-depth
    double a; // The `width' of the potential
    double req; // The equilibrium `bond' length
public:
    // Constructor
    Morse(); // Defaults to De = 1, a = 1, req = 2
    
    // Force/energy calculation
    double operator()(double rij, coord& force);
    // Just energy
    double potential(double rij);
    
    // Set well-depth, width and req
    void setDepth(double _de);
    void setWidth(double _a);
    void setREq(double _req);
    
};

// Custom potential, defined by spline
class CustomPotential : public PotentialFunctor
{
private:
    cubic::Spline spline;
    
public:
    // Constructor
    CustomPotential();
    
    // Force/energy calculation
    double operator()(double rij, coord& force);
    // Just energy
    double potential(double rij);
    
    // Get spline
    cubic::Spline& getSpline();
    
};


#endif /* potentials_hpp */
