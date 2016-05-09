//
//  gaussian.hpp
//  StarredMD
//
//  Defines a class for containing Gaussian external potentials
//  which take the form:
//       A * exp( -a * ( (x - x0)^2 + (y-y0)^2 ) )
//
//  Created by Robert Shaw on 05/05/2016.
//
//

#ifndef gaussian_hpp
#define gaussian_hpp

#include <vector>
#include <array>

class Gaussian
{
private:
    double gAmp; // Amplitude of Gaussian, A above
    double gAlpha; // Exponent, a above
    double gex0, gey0; // (x0, y0) centre of Gaussian
    double scale; // Stores the scaling value from the audio input
    
public:
    
    Gaussian(double _gAmp, double _gAlpha, double _gex0, double _gey0, double _scale); // Constructor
    Gaussian(const Gaussian& other); // Copy constructor
    
    // Get private variables
    double const getgAmp();
    double const getgAlpha();
    double const getgex0();
    double const getgey0();
    double const getScale();
    
    // Set the parameters
    void setParams(double _gAmp, double _gAlpha, double _gex0, double _gey0, double _scale);
    
    // Calculate the force vector at (x, y) due to the Gaussian
    std::vector<double> calcForceEnergy(double x, double y);
    void calcForceEnergy(double x, double y, std::array<double, 3> &forceEnergy);
    
};

#endif /* gaussian_hpp */
