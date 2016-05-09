/***************************************************
 *                                                 *
 * LJForces library for MD Pie                     *
 * Stores and calculates positions, velocities,    *
 * forces, temperature, pressure, and energies     *
 * for MD simulation.                              *
 * Robert A Shaw (2016)                            *
 *                                                 *
 ***************************************************/


#ifndef LJFORCES_HEADER_DEF
#define LJFORCES_HEADER_DEF

#include <vector>
#include "gaussian.hpp"

namespace lj{
	class LJContainer
	{
	private:
		int N; // Number of particles
        
        // Matrices of dynamical variables
        std::vector<std::vector <double> > positions, velocities, forces;
        
        // Store the last twenty position matrices for animating trails
        std::vector<std::vector <std::vector <double> > > previousPositions;
        
        // Vectors of the potential and kinetic energies for drawing graphs
        std::vector<double> prevEPot, prevEKin;
        
        // Vector of the simulation box dimensions: width, height
        std::vector<double> box_dimensions;
        
        // Array of external Gaussian potentials
        std::vector<Gaussian> gaussians;
        
        double epot, ekin; // Potential and kinetic energies
        double rcutoff; // Cutoff radius for pair potential
        double dt, T; // MD timestep and desired temperature
        double maxEKin, maxEPot; // Maximum kinetic and potential energies in prevEPot, prevEKin
        double v_avg; // Current average speed of particles
        int enCounter; // Counter so that only every nth energy is stored
        
	public:
        LJContainer(); // Default constructor
        
        void clearSystem(); // Reset object
        
		// Accessors
		// Getters
        
        // Return values of private variables
		int const getN();
		double const getEPot();
		double const getEKin();
        double const getT();
        double const getVAvg();
        double const getWidth();
        double const getHeight();
        double const getMaxEkin();
        double const getMaxEpot();
        
        // Return sizes of vectors
        int const getNGaussians();
        int const getNEnergies();
        
        // Return vectors of dynamical variables of particle i
		std::vector<double> const getPos(int i);
		std::vector<double> const getVel(int i);
		std::vector<double> const getForces(int i);
        std::vector<double> const getPreviousPositions(int npart, int nstep);
        
        // Return ith member of energy vectors
        double const getPreviousEpot(int i);
        double const getPreviousEkin(int i);
        
        // Get a reference to or details of the ith Gaussian
        Gaussian& getGaussian(int i);
        double getGaussianAlpha(int i);
        double getGaussianAmp(int i);
        double getGaussianX0(int i);
        double getGaussianY0(int i);
        
		//Setters
		void setPos(int i, double x, double y);
		void setVel(int i, double vx, double vy);
        void setConsts(double _boxl, double _boxw, double _rcutoff, double _dt);
        void setTemp(double _T);
        void setParticles(int _N);
        
        // Add and remove particles
		void addParticle(double x, double y,
						 double vx, double vy);
		void removeParticle();
        
        // Add a Gaussian, or remove or update the ith Gaussian
        void addGaussian(double gAmp, double gAlpha, double gex0, double gey0);
        void removeGaussian(int i = 0);
        void updateGaussian(int i, double gAmp, double gAlpha, double gex0, double gey0, double _scale);
	
        // Calculate forces and energies
		void forcesEnergies(int nthreads);
        void externalForce();
		void forcesThread(int start, int end, std::vector<std::vector<double> >& ftemp,
						  double& etemp, std::vector<std::vector<double> > postemp,
						  double rcut, int npart);
        
        // Main MD integration step
		void integrate(int nthreads);
		
        // Thermostats
        void andersen(double freq);
        void berendsen(double freq);
	};
}

#endif
