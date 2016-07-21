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

#include "gaussian.hpp"
#include <cmath>

/*
    CONSTRUCTOR:
      Currently just sets the parameters
 */
Gaussian::Gaussian(double _gAmp, double _gAlpha, double _gex0, double _gey0)
{
    setParams(_gAmp, _gAlpha, _gex0, _gey0);
}

/*
    COPY CONSTRUCTOR:
        Takes the parameters from the Gaussian other and sets the parameters for
        this to be the same. Necessary for std::vector to be able to store instances
        of this class.
 */
Gaussian::Gaussian(const Gaussian& other)
{
    setParams(other.gAmp, other.gAlpha, other.gex0, other.gey0);
}


/*
    GETTERS:
        Return the values of the private variables; scope to alter how these values are
        accessed if necessary.
 */
double Gaussian::getgAlpha() const { return gAlpha; }
double Gaussian::getgAmp()   const { return gAmp; }
double Gaussian::getgex0()   const { return gex0; }
double Gaussian::getgey0()   const { return gey0; }

/*
    ROUTINE setParams:
        Sets each private variable, var, of the class, as:  var = _var
        Ideally, checks should be included as to the suitability of the values being set.
 */
void Gaussian::setParams(double _gAmp, double _gAlpha, double _gex0, double _gey0){
    gAmp = _gAmp;
    gAlpha = _gAlpha;
    gex0 = _gex0;
    gey0 = _gey0;
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
