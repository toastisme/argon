//
//  gaussian.cpp
//  StarredMD
//
//  Implementation of gaussian.hpp
//
//  Created by Robert Shaw on 05/05/2016.
//
//

#include "gaussian.hpp"
#include <cmath>

/*
    CONSTRUCTOR:
      Currently just sets the parameters
 */
Gaussian::Gaussian(double _gAmp, double _gAlpha, double _gex0, double _gey0, double _scale)
{
    setParams(_gAmp, _gAlpha, _gex0, _gey0, _scale);
}

/*
    COPY CONSTRUCTOR:
        Takes the parameters from the Gaussian other and sets the parameters for
        this to be the same. Necessary for std::vector to be able to store instances
        of this class.
 */
Gaussian::Gaussian(const Gaussian& other)
{
    setParams(other.gAmp, other.gAlpha, other.gex0, other.gey0, other.scale);
}


/*
    GETTERS:
        Return the values of the private variables; scope to alter how these values are
        accessed if necessary.
 */
double const Gaussian::getgAlpha() { return gAlpha; }
double const Gaussian::getgAmp() { return gAmp; }
double const Gaussian::getgex0() { return gex0; }
double const Gaussian::getgey0() { return gey0; }
double const Gaussian::getScale() { return scale; }

/*
    ROUTINE setParams:
        Sets each private variable, var, of the class, as:  var = _var
        Ideally, checks should be included as to the suitability of the values being set.
 */
void Gaussian::setParams(double _gAmp, double _gAlpha, double _gex0, double _gey0, double _scale){
    gAmp = _gAmp;
    gAlpha = _gAlpha;
    gex0 = _gex0;
    gey0 = _gey0;
    scale = _scale;
}


/*
    ROUTINE calcForceEnergy - x, y arguments:
        Calculates the force vector (and energy as the third element of the vector)
        at position (x, y) due to this Gaussian.
 
    OVERLOAD x, y, [array<double, 3> reference] arguments:
        Does the same but puts the results in the referenced array, with the last element
        the energy.
 */
 
std::vector<double> Gaussian::calcForceEnergy(double x, double y){
    std::vector<double> forceEnergy;
    double epot = gAmp*exp(-gAlpha*(pow(x - gex0,2)+pow(y - gey0,2)));
    double forcex = -2*(x-gex0)*gAlpha*epot;
    double forcey = -2*(y-gey0)*gAlpha*epot;
    forceEnergy.push_back(forcex);
    forceEnergy.push_back(forcey);
    forceEnergy.push_back(epot);
    return forceEnergy;
}

void Gaussian::calcForceEnergy(double x, double y, std::array<double, 3> &forceEnergy){
    //epot
    forceEnergy[2] = gAmp*exp(-gAlpha*(pow(x - gex0,2)+pow(y - gey0,2)));
    
    //force
    forceEnergy[0] = -2*(x-gex0)*gAlpha*forceEnergy[2];
    forceEnergy[1] = -2*(y-gey0)*gAlpha*forceEnergy[2];
}
