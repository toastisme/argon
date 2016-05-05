//
//  gaussian.hpp
//  StarredMD
//
//  Created by Robert Shaw on 05/05/2016.
//
//

#ifndef gaussian_hpp
#define gaussian_hpp

#include <vector>

class Gaussian
{
private:
    double gAmp, gAlpha, gex0, gey0, scale;
    
public:
    
    Gaussian(double _gAmp, double _gAlpha, double _gex0, double _gey0, double _scale);
    Gaussian(const Gaussian& other); // Copy constructor
    
    double const getgAmp();
    double const getgAlpha();
    double const getgex0();
    double const getgey0();
    double const getScale();
    
    void setParams(double _gAmp, double _gAlpha, double _gex0, double _gey0, double _scale);
    
    std::vector<double> calcForceEnergy(double x, double y);
    
};

#endif /* gaussian_hpp */
