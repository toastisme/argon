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
    double gAmp, gAlpha, gex0, gey0;
    
public:
    
    Gaussian(double _gAmp, double _gAlpha, double _gex0, double _gey0);
    Gaussian(const Gaussian& other); // Copy constructor
    
    double const getgAmp();
    double const getgAlpha();
    double const getgex0();
    double const getgey0();
    
    void setParams(double _gAmp, double _gAlpha, double _gex0, double _gey0);
    
    std::vector<double> calcForceEnergy(double x, double y);
    
};

#endif /* gaussian_hpp */
