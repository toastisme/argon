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

#include "potentials.hpp"
#include "ofApp.h"
#include "gui_derived.hpp"


//------ POTENTIALFUNCTOR -----

// constructor
PotentialFunctor::PotentialFunctor(Potential _type) : type(_type) {}

// return the potential
// if within the wall, use the LJ potential
// else if past the cutoff, use 0
// else use the actual potential
double PotentialFunctor::potential(double r) {
    if (r < LJ_AT_3) { return calcEnergyLJ(r); }
    else if (r > 3.0) { return 0; }
    else { return calcEnergy(r); }
}

// return the force
// if within the wall, use the LJ force
// else if past the cutoff, use 0
// else use the actual force
double PotentialFunctor::force(double r) {
    if (r < LJ_AT_3) { return calcForceLJ(r); }
    else if (r > 3.0) { return 0; }
    else { return calcForce(r); }
}

// LJ potential
double PotentialFunctor::calcEnergyLJ(double r) {
    double rm6 = 1.0 / pow(r, 6); // r^(-6)
    return 4 * (rm6 * rm6 - rm6);
}

// LJ force
double PotentialFunctor::calcForceLJ(double r) {
    double rm6 = 1.0 / pow(r, 6);
    return 24 * r * (rm6 - 2 * rm6 * rm6);
}

// Get type
Potential PotentialFunctor::getType() const {
    return type;
}


//------ LENNARD-JONES POTENTIAL -----

// Constructor, sets parameters to default values
LennardJones::LennardJones() : PotentialFunctor(LENNARD_JONES) {}

// Just energy calculation
double LennardJones::calcEnergy(double r) { return calcEnergyLJ(r); }

// Force calculation
double LennardJones::calcForce(double r) { return calcForceLJ(r); }



//------ MORSE POTENTIAL ------

// Constructor, set default parameter values
Morse::Morse() : a(5.85), r_eq(pow(2, 1.0 / 6)), PotentialFunctor(MORSE) {}

// Force and energy calculation


// Morse potential, lerped to an LJ repulsive wall
double Morse::calcEnergy(double r)
{
    double omExp = 1.0 - exp(-a * (r - r_eq));
    double E_Morse = omExp * omExp - 1;
    
    if (r < LJ_AT_2) {
        double E_LJ = calcEnergyLJ(r);
        double t = (r - LJ_AT_3) / (LJ_AT_2 - LJ_AT_3);
        return t * E_Morse + (1 - t) * E_LJ;
    } else {
        return E_Morse;
    }
}

// Morse force, lerped to an LJ repulsive wall
double Morse::calcForce(double r)
{
    double exponential = exp(-a * (r - r_eq));
    double omExp = 1.0 - exponential;
    double F_Morse = 2 * a * omExp * exponential;
    
    if (r < LJ_AT_2) {
        double F_LJ = calcForceLJ(r);
        double t = (r - LJ_AT_3) / (LJ_AT_2 - LJ_AT_3);
        return t * F_Morse + (1 - t) * F_LJ;
    } else {
        return F_Morse;
    }
}



//------ SQUARE WELL POTENTIAL -----

// Constructor, sets default values of parameters
SquareWell::SquareWell() : lambda(1.85), PotentialFunctor(SQUARE_WELL) {}

// Square well potential
double SquareWell::calcEnergy(double r)
{
    double epot = 0;
    if ( r < 1.0 ) { epot = -1 + (r - 1.0) * LJ_F_3; }
    else if ( r < lambda ) { epot = -1; }
    else if ( r < lambda + 0.015 ) { epot = (r - lambda) / 0.015 - 1; }
    return epot;
}

// Square well force
double SquareWell::calcForce(double r)
{
    double force = 0;
    if ( r < 1.0 ) { // Use a steep wall approximation to a hard wall
        force = LJ_F_3;
    } else if ( lambda < r && r < lambda + 0.015 ) {
        force = 1 / 0.015;
    }
    
    return force;
}



//------ CUSTOM POTENTIAL ------

// Constructor
CustomPotential::CustomPotential() : PotentialFunctor(CUSTOM) {
    // define fixed points on the spline, corresponding to the repulsive wall and the cutoff of 3.0
    pointWallL = {LJ_AT_3, 3, LJ_F_3}; // LJ wall
    pointWallR = {LJ_AT_2, 2, -10};
    pointCutoff = {3.0, 0, 0};
    
    spline = cubic::Spline(pointWallL.x, pointWallL.y, pointWallL.m, pointCutoff.x, pointCutoff.y, pointCutoff.m);
    spline.addPoint(pointWallR.x, pointWallR.y, pointWallR.m);
}

// Get a reference to the spline
cubic::Spline& CustomPotential::getSpline() { return spline; }

// return the potential
double CustomPotential::calcEnergy(double r) { return spline.value(r); }
double CustomPotential::calcForce(double r)  { return spline.slope(r); }

// Update the spline
void CustomPotential::updatePoints(std::vector <cubic::Point> &points) {
    std::vector <cubic::Point> splinePoints;
    
    // add fixed points onto start and end
    splinePoints.push_back(pointWallL);
    splinePoints.push_back(pointWallR);
    splinePoints.insert(splinePoints.end(), points.begin(), points.end());
    splinePoints.push_back(pointCutoff);
    
    // update spline
    spline.setPoints(splinePoints);
    spline.reconstruct();
}