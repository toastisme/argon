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
		double epot, ekin, virial, box, rcutoff, dt;
	public:
		LJContainer(double _box, double _rcutoff, double _dt); // Constructor
	
		// Accessors
		// Getters
		int const getN();
		double const getEPot();
		double const getEKin();
		double const getVirial();
		std::vector<double> const getPos(int i);
		std::vector<double> const getVel(int i);
		std::vector<double> const getForces(int i);
		//Setters
		void setPos(int i, double x, double y, double z);
		void setVel(int i, double vx, double vy, double vz);
		void setConsts(double _box, double _rcutoff, double _dt);
	
		// Routines
		void addParticle(double x, double y, double z,
						 double vx, double vy, double vz);
		void removeParticle();
	
		void forcesEnergies(int nthreads);
		void forcesThread(int start, int end, std::vector<std::vector<double> >& ftemp,
						  double& etemp, double &vtemp, std::vector<std::vector<double> > postemp,
						  double rcut, int npart, double lbox);
		void integrate(int nthreads);
		void andersen(double T, double freq);
	};
}

#endif
