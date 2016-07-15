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

#include <cmath>
#include "utilities.hpp"
#include "cubicspline.hpp"


// Enumerate different potential types
enum Potential {
    LENNARD_JONES, SQUARE_WELL, MORSE, CUSTOM
};

// Base class - currently just an empty functor defining what every potential
// functor must have, which is a force/energy calculating method with arguments
// separation rij, force, and returning the potential energy.

class PotentialFunctor
{
protected:
    // pure virtual functions, must be overridden in child classes
    // this is where the potential is defined
    // this should connect cleanly to an LJ potential at r <= LJ_AT_3 (the potential does
    // not need to be defined below this value)
    virtual double calcEnergy(double r) = 0;
    virtual double calcForce(double r)  = 0;
    
    // 12-6 Lennard-Jones potential to lerp to, so that the repulsive wall is always LJ-like
    double calcEnergyLJ(double r);
    double calcForceLJ(double r);
    
    // Store type so can safely check type of potential being used
    Potential type;
    
public:
    
    PotentialFunctor(Potential type);
    
    // magic numbers indicating particular values of the LJ potential
    // value of r such that V_LJ(r) = 2
    constexpr static const double LJ_AT_2 = 0.949343841329228; // ((1 + sqrt(3) / 2)^(-1/6)
    // value of r such that V_LJ(r) = 3
    constexpr static const double LJ_AT_3 = 0.934655265184067; // (3/2)^(-1/6)
    // gradient of V_LJ(r) at r = LJ_AT_3
    constexpr static const double LJ_F_3  = -72 * LJ_AT_3;
    
    // how the potential is surfaced to the MD system
    //double operator()(double rij, coord& force);
    double potential(double r);
    double force(double r);
    
    // Return the type
    Potential getType() const;

};


// Lennard-Jones potential
// V(r) = 4 * [ r^(-12) - r^(-6) ]
class LennardJones : public PotentialFunctor
{
private:
    // return an LJ potential
    double calcEnergy(double r);
    double calcForce(double r);
    
public:
    // Constructor
    LennardJones(); // A 12-6 potential with epsilon = sigma = 1
};


// Morse potential
// V(r) = (1 - exp[-a(r - r_eq)])^2 - 1
// r_eq = 2^(1/6), a = 5.85 to match LJ potential
class Morse : public PotentialFunctor
{
private:
    double a;    // The `width' of the potential, set to 5.85
    double r_eq; // The equilibrium `bond' length, set to 2^(1/6)
    
    // return a Morse potential, connected to an LJ repulsive wall
    double calcEnergy(double r);
    double calcForce(double r);
    
public:
    // Constructor
    Morse();
};


// Square well potential
// V(r) = -1 for 1.0 < r < 1.85, 0 for r > 1.85, and 'infinity' for r < 1.0
// in practice, 'infinity' is a steep slope which connects to an LJ repulsion
class SquareWell : public PotentialFunctor
{
private:
    double lambda; // well goes from r = 1 to r = lambda (= 1.85)
public:
    // Constructor
    SquareWell();
    
    // return the potential
    double calcEnergy(double r);
    double calcForce(double r);
};


// Custom potential, defined by spline
class CustomPotential : public PotentialFunctor
{
private:
    cubic::Spline spline;
    cubic::Point pointWallL, pointWallR, pointCutoff;
    
    // return the potential
    double calcEnergy(double r);
    double calcForce(double r);
    
public:
    // Constructor
    CustomPotential();
    
    // Get spline
    cubic::Spline& getSpline();
    
    // Rebuild spline from points
    void updatePoints(std::vector <cubic::Point> &points);
};


#endif /* potentials_hpp */
