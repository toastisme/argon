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

#ifndef MDFORCES_HEADER_DEF
#define MDFORCES_HEADER_DEF

#include <vector>
#include <deque>
#include "gaussian.hpp"
#include "utilities.hpp"
#include "potentials.hpp"

namespace md{

    class MDContainer
    {
    private:
        bool running;         // is the system running?
        int stepsPerUpdate;   // number of time steps to take when run() is called
        
        int N;           // Current number of particles in system
        int NAfterReset; // Number of particles to be used when the system is reset
        
        // Matrices of dynamical variables
        std::vector <coord> positions, velocities, forces;
        
        // Store the last twenty position matrices for animating trails
        std::deque <std::vector <coord>> prevPositions;
        
        // Deques of the potential and kinetic energies for drawing graphs
        std::deque <double> prevEPot, prevEKin;
        
        // Vector of the simulation box dimensions: width, height
        coord box_dimensions;
        
        // Array of external Gaussian potentials
        std::vector<Gaussian> gaussians;
        
        double epot, ekin; // Potential and kinetic energies
        double rcutoff;    // Cutoff radius for pair potential
        double dt, T;      // MD timestep and desired temperature
        double freq;       // thermostat frequency
        
        double maxEKin, maxEPot, minEKin, minEPot; // Maximum/minimum kinetic and potential energies in prevEPot, prevEKin
        double v_avg; // Current average speed of particles
        
        // Default potential is Lennard-Jones
        LennardJones lj;
        
        // Reference to the potential functor to be used to calculate the forces
        PotentialFunctor* potential;
        
    public:
        MDContainer(); // Default constructor
        
        bool getRunning() const;         // getter for running
        void setRunning(bool running);   // setter for running
        void toggleRunning();            // toggle whether the system is running
        void resetSystem();              // Clears the system & regrids everything
        
        // Accessors
        // Getters
        
        // Return values of private variables
        int    getStepsPerUpdate() const;
        int    getN() const;
        int    getNAfterReset() const;
        double getTemp() const;
        double getVAvg() const;
        
        double getTimestep() const;
        double getCutoff() const;
        double getFreq() const;
        
        coord  getBox() const;
        double getWidth() const;
        double getHeight() const;
        
        double getEPot() const;
        double getEKin() const;
        double getMaxEpot() const;
        double getMaxEkin() const;
        double getMinEpot() const;
        double getMinEkin() const;
        
        // Return sizes of vectors
        int getNGaussians() const;
        int getNEnergies() const;
        int getNPrevPos() const;
        
        // Return struct of dynamical variables of particle i
        coord getPos(int i) const;
        coord getVel(int i) const;
        coord getForce(int i) const;
        
        // Return position struct of particle i nstep frames ago
        coord getPos(int i, int nstep) const;
        
        // Return energy nstep frames ago
        double getPreviousEpot(int nstep) const;
        double getPreviousEkin(int nstep) const;
        
        // Get a reference to or details of the ith Gaussian
        Gaussian& getGaussian(int i);
        double getGaussianAlpha(int i) const;
        double getGaussianAmp(int i) const;
        double getGaussianX0(int i) const;
        double getGaussianY0(int i) const;
        
        //Setters
        void setStepsPerUpdate(int steps);
        void setNAfterReset(int N);
        
        void setPos(int i, double x, double y);
        void setVel(int i, double vx, double vy);
        
        // Set basic system constants
        void setBox(double box_width, double box_height);
        void setCutoff(double cutoff);
        void setTimestep(double timestep);
        void setTemp(double temperature);
        void setFreq(double frequency);
        
        // Set the potential
        void setPotential(PotentialFunctor* _potential);
        
        // Add and remove particles
        void addParticle(double x, double y, double vx, double vy);
        void addParticle(coord pos, coord vel);
        void addParticlesGrid(int numParticles);
        void removeParticle();
        
        // Add a Gaussian, or remove or update the ith Gaussian
        void addGaussian(double gAmp, double gAlpha, double gex0, double gey0);
        void removeGaussian(int i = 0);
        void updateGaussian(int i, double gAmp, double gAlpha, double gex0, double gey0);

        // Calculate forces and energies
        void forcesEnergies(int nthreads);
        void externalForce();
        void forcesThread(int start, int end, std::vector<coord>& ftemp,
                double& etemp, std::vector<coord> postemp);
        
        // Main MD integration step
        void integrate(int nthreads);
        
        // save positions and energies in prevPos, prevEPot, prevEKin
        void savePreviousValues();
        
        // run for stepsPerUpdate time steps on nthreads threads; default to 1 thread
        void run(int nthreads = 1);

        // Thermostats
        coord randomVel();
        void andersen(double freq);
        void berendsen(double freq);
    };
}

#endif
