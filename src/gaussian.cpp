//
//  gaussian.cpp
//  StarredMD
//
//  Created by Robert Shaw on 05/05/2016.
//
//

#include "gaussian.hpp"
#include <cmath>

Gaussian::Gaussian(double _gAmp, double _gAlpha, double _gex0, double _gey0, double _scale)
{
    setParams(_gAmp, _gAlpha, _gex0, _gey0, _scale);
}

Gaussian::Gaussian(const Gaussian& other)
{
    setParams(other.gAmp, other.gAlpha, other.gex0, other.gey0, other.scale);
}

double const Gaussian::getgAlpha() { return gAlpha; }
double const Gaussian::getgAmp() { return gAmp; }
double const Gaussian::getgex0() { return gex0; }
double const Gaussian::getgey0() { return gey0; }
double const Gaussian::getScale() { return scale; }

void Gaussian::setParams(double _gAmp, double _gAlpha, double _gex0, double _gey0, double _scale){
    gAmp = _gAmp;
    gAlpha = _gAlpha;
    gex0 = _gex0;
    gey0 = _gey0;
    scale = _scale;
}

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
