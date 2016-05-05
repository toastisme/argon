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

namespace lj{
	class LJContainer
	{
	private:
		int N; // Number of particles
        std::vector<std::vector <double> > positions, velocities, forces;
        std::vector<double> box_dimensions;
		double epot, ekin, rcutoff, dt, T;
	public:
        LJContainer(); // Default constructor
		LJContainer(double _boxl, double _boxw, double _rcutoff, double _dt, double _T); // Constructor
        
		// Accessors
		// Getters
		int const getN();
		double const getEPot();
		double const getEKin();
        double const getT();
        std::vector<double> const getBox();
		std::vector<double> const getPos(int i);
		std::vector<double> const getVel(int i);
		std::vector<double> const getForces(int i);
		//Setters
		void setPos(int i, double x, double y);
		void setVel(int i, double vx, double vy);
		void setConsts(double _boxl, double _boxw, double _rcutoff, double _dt, double _T);
	
		// Routines
		void addParticle(double x, double y,
						 double vx, double vy);
		void removeParticle();
	
		void forcesEnergies(int nthreads);
		void forcesThread(int start, int end, std::vector<std::vector<double> >& ftemp,
						  double& etemp, std::vector<std::vector<double> > postemp,
						  double rcut, int npart);
		void integrate(int nthreads);
		void andersen(double freq);
	};
}

#endif
