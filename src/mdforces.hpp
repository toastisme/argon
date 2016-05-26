/***************************************************
 *                                                 *
 * MDForces library                                *
 * Stores and calculates positions, velocities,    *
 * forces, temperature, pressure, and energies     *
 * for MD simulation.                              *
 * Robert A Shaw (2016)                            *
 *                                                 *
 ***************************************************/


#ifndef MDFORCES_HEADER_DEF
#define MDFORCES_HEADER_DEF

#include <vector>
#include <deque>
#include "gaussian.hpp"
#include "utilities.hpp"

namespace md{

    class MDContainer
    {
    private:
        int N; // Number of particles
        
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
        double rcutoff; // Cutoff radius for pair potential
        double dt, T; // MD timestep and desired temperature
        double maxEKin, maxEPot; // Maximum kinetic and potential energies in prevEPot, prevEKin
        double v_avg; // Current average speed of particles
        int enCounter; // Counter so that only every nth energy is stored
        
    public:
        MDContainer(); // Default constructor
        
        void clearSystem(); // Reset object
        
        // Accessors
        // Getters
        
        // Return values of private variables
        int getN() const;
        int getSteps() const;
        double getTemp() const;
        double getVAvg() const;
        
        double getTimestep() const;
        double getCutoff() const;
        
        coord  getBox() const;
        double getWidth() const;
        double getHeight() const;
        
        double getEPot() const;
        double getEKin() const;
        double getMaxEpot() const;
        double getMaxEkin() const;
        
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
        void setPos(int i, double x, double y);
        void setVel(int i, double vx, double vy);
        
        // Set basic system constants
        void setBox(double box_width, double box_height);
        void setCutoff(double cutoff);
        void setTimestep(double timestep);
        void setTemp(double temperature);
        
        // Add and remove particles
        void addParticle(double x, double y, double vx, double vy);
        void addParticle(coord pos, coord vel);
        void addParticlesGrid(int numParticles);
        void removeParticle();
        
        // Add a Gaussian, or remove or update the ith Gaussian
        void addGaussian(double gAmp, double gAlpha, double gex0, double gey0);
        void removeGaussian(int i = 0);
        void updateGaussian(int i, double gAmp, double gAlpha, double gex0, double gey0, double _scale);

        // Calculate forces and energies
        void forcesEnergies(int nthreads);
        void externalForce();
        void forcesThread(int start, int end, std::vector<coord>& ftemp,
                double& etemp, std::vector<coord> postemp,
                double rcut, int npart);
        
        // Main MD integration step
        void integrate(int nthreads);
        
        // save positions and energies in prevPos, prevEPot, prevEKin
        void savePreviousValues();
        
        // run for nsteps time steps
        void run(int nsteps, double freq, int nthreads);

        // Thermostats
        coord randomVel();
        void andersen(double freq);
        void berendsen(double freq);
    };
}

#endif
