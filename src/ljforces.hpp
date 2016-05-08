/***************************************************
 *                                                 *
 * LJForces library for MD Pie                     *
 * Stores and calculates positions, velocities,    *
 * forces, temperature, pressure, and energies     *
 * for MD simulation.                              *
 * Robert A Shaw (2016)
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
        std::vector<std::vector <double> > positions, velocities, forces;
        std::vector<std::vector <std::vector <double> > > previousPositions;
        std::vector<double> prevEPot, prevEKin;
        std::vector<double> box_dimensions;
        std::vector<Gaussian> gaussians;
        double epot, ekin, rcutoff, dt, T, maxEKin, maxEPot, v_avg;
        int enCounter;
	public:
        LJContainer(); // Default constructor
        
		// Accessors
		// Getters
		int const getN();
		double const getEPot();
		double const getEKin();
        double const getT();
        int const getNGaussians();
        int const getNEnergies();
        double const getVAvg();
        
        double const getWidth();
        double const getHeight();
		std::vector<double> const getPos(int i);
		std::vector<double> const getVel(int i);
		std::vector<double> const getForces(int i);
        std::vector<double> const getPreviousPositions(int npart, int nstep);
        double const getPreviousEpot(int i);
        double const getPreviousEkin(int i);
        double const getMaxEkin();
        double const getMaxEpot();
		//Setters
		void setPos(int i, double x, double y);
		void setVel(int i, double vx, double vy);
        void setConsts(double _boxl, double _boxw, double _rcutoff, double _dt);
        void setTemp(double _T);
        void setParticles(int _N);
		// Routines
		void addParticle(double x, double y,
						 double vx, double vy);
		void removeParticle();
        
        Gaussian& getGaussian(int i);
        void addGaussian(double gAmp, double gAlpha, double gex0, double gey0);
        void removeGaussian(int i = 0);
        void updateGaussian(int i, double gAmp, double gAlpha, double gex0, double gey0, double _scale);
        double getGaussianAlpha(int i);
        double getGaussianAmp(int i);
        double getGaussianX0(int i);
        double getGaussianY0(int i);
	
		void forcesEnergies(int nthreads);
        void externalForce();
		void forcesThread(int start, int end, std::vector<std::vector<double> >& ftemp,
						  double& etemp, std::vector<std::vector<double> > postemp,
						  double rcut, int npart);
		void integrate(int nthreads);
		void andersen(double freq);
        
        void clearSystem();
        void berendsen(double freq);
	};
}

#endif
