//
//  potentials.cpp
//  StarredMD
//
//  Created by Robert Shaw on 26/05/2016.
//
//

#include "potentials.hpp"
#include <cmath>

//------ LENNARD-JONES POTENTIAL -----

// Constructor, sets parameters to default values
LennardJones::LennardJones() : repelPow(12), attractPow(6), epsilon(1.0), sigma(1.0)
{ }

// Setters

void LennardJones::setPowers(int _repel, int _attract)
{
    repelPow = ( _repel > 0 ? _repel : repelPow );
    attractPow = ( _attract > 0 ? _attract : attractPow );
}

void LennardJones::setEpsilon(double _epsilon)
{
    epsilon = _epsilon;
}

void LennardJones::setSigma(double _sigma)
{
    sigma = ( _sigma > 0 ? _sigma : sigma);
}

// Force and energy calculation

double LennardJones::operator()(double rij, coord& force)
{
    double epot = 0; // Potential energy
    
    double od = sigma/rij;
    
    double odRepel, odAttract; // (sigma/rij)^repelPow and (sigma/rij)^attractPow
    odRepel = pow(od, repelPow);
    odAttract = pow(od, attractPow);
    
    epot = 4 * epsilon * ( odRepel - odAttract );
    
    double forceCoeff = 4 * epsilon * ( double(attractPow) * odAttract - double(repelPow) * odRepel) / pow(rij, 2);
    
    force.x = forceCoeff;
    force.y = forceCoeff;
    
    return epot;
}

// Just energy calculation

double LennardJones::potential(double rij)
{
    double epot = 0;
    
    double od = sigma/rij;
    
    double odRepel, odAttract; // (sigma/rij)^repelPow and (sigma/rij)^attractPow
    odRepel = pow(od, repelPow);
    odAttract = pow(od, attractPow);
    
    epot = 4 * epsilon * ( odRepel - odAttract );
    
    return epot;
}


//------ SQUARE WELL POTENTIAL -----

// Constructor, sets default values of parameters
SquareWell::SquareWell() : V0(-1.0), rMin(1.0), rMax(2.0), intercept(1e6)
{
    calcSteepness();
}

// Setters

void SquareWell::setBounds(double _rMin, double _rMax)
{
    rMin = ( _rMin > 0 ? _rMin : rMin);
    rMax = ( _rMax > 0 ? _rMax : rMax);
}

void SquareWell::setV0(double _v0)
{
    V0 = _v0;
}

void SquareWell::calcSteepness()
{
    steepness = (V0 - intercept) / rMin;
}

// Force and energy calculation
double SquareWell::operator()(double rij, coord& force)
{
    double epot = 0; // Potential zero to right of well
    double forceCoeff = 0; // Forces are zero inside or to right of well
    
    if ( rij < rMin ) { // Use a steep wall approximation to a hard wall
        epot = steepness * rij + intercept;
        forceCoeff = steepness;
    } else if ( rij < rMax ) { // Inside well
        epot = V0;
    } else if ( rij <  1.05 * rMax ) {
        forceCoeff = steepness;
    }
    
    force.x = forceCoeff;
    force.y = forceCoeff;
    
    return epot;
    
}

// Just the energy
double SquareWell::potential(double rij)
{
    double epot = 0;
    if ( rij < rMin ) epot = steepness * rij + intercept;
    else if ( rij < rMax ) epot = V0;
    
    return epot;
}


//------ MORSE POTENTIAL ------

// Constructor, set default parameter values
Morse::Morse() : De(1.0), a(1.0), req(2.0)
{}

// Setters

void Morse::setDepth(double _de) { De = _de; }
void Morse::setWidth(double _a) { a = ( _a > 0 ? _a : a); }
void Morse::setREq(double _req) { req = ( _req > 0 ? _req : req); }

// Force and energy calculation

double Morse::operator()(double rij, coord& force)
{
    double epot = 0;
    
    double exponential = exp(-a * (rij - req));
    double omExp = 1.0 - exponential;
    
    epot = De * omExp * omExp;
    double forceCoeff = 2 * a * De * omExp * exponential;
    
    force.x = forceCoeff;
    force.y = forceCoeff;
    
    return epot;
}

// Just energy
double Morse::potential(double rij)
{
    double epot = 0;
    double omExp = 1.0 - exp(-a * (rij - req));
    epot = De * omExp * omExp;
    return epot;
}


//------ CUSTOM POTENTIAL ------

// Constructor (does nothing currently)

CustomPotential::CustomPotential() {}

// Get a reference to the spline

cubic::Spline& CustomPotential::getSpline() { return spline; }

// Force and energy calculation

double CustomPotential::operator()(double rij, coord& force)
{
    force.x = spline.slope(rij);
    force.y = force.x;
    return spline.value(rij);
}

// Just the energy
double CustomPotential::potential(double rij)
{
    return spline.value(rij);
}
