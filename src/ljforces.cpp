/***************************************************
 *                                                 *
 *  MD Pie LJForces Library                        *
 *  Implements ljforces.hpp                        *
 *  Robert A Shaw (2016)                           *
 *                                                 *
 ***************************************************/

#include "ljforces.hpp"
#include <cmath>
#include <random>
#include <thread>
#include <iostream>

namespace lj{

    LJContainer::LJContainer()
    {
        setConsts(10.0, 10.0, 3.0, 0.01);
    }
    
	LJContainer::LJContainer(double _boxl, double _boxw, double _rcutoff, double _dt) : N(0), epot(0.0), ekin(0.0)	{
		setConsts(_boxl, _boxw, _rcutoff, _dt);
	}
	
	int const LJContainer::getN() { return N; }
	double const LJContainer::getEPot() { return epot; }
	double const LJContainer::getEKin() { return ekin; }
    std::vector<double> const LJContainer::getBox() { return box_dimensions; }
	
	std::vector<double> const LJContainer::getPos(int i) { return positions[i]; }
	std::vector<double> const LJContainer::getVel(int i) { return velocities[i]; }
	std::vector<double> const LJContainer::getForces(int i) { return forces[i]; }
	
	void LJContainer::setPos(int i, double x, double y)
	{
		positions[i][0] = x;
		positions[i][1] = y;
	}
	
	void LJContainer::setVel(int i, double vx, double vy)
	{
		velocities[i][0] = vx;
		velocities[i][1] = vy;
	}
    
	
	void LJContainer::setConsts(double _boxl, double _boxw, double _rcutoff, double _dt)
	{
        if ( _boxw > 0 ) { box_dimensions.push_back(_boxw);}
        else { box_dimensions.push_back(10.0); }
        if ( _boxl > 0 ) { box_dimensions.push_back(_boxl);}
        else { box_dimensions.push_back(10.0); }
	if ( _rcutoff > 0 ) { rcutoff = _rcutoff; }
	else { rcutoff = 3.0; }
	if ( _dt > 0 ) { dt = _dt; }
	else { dt = 0.001; }
        box_dimensions.push_back(boxw);
        box_dimensions.push_back(boxl);
	}
	
	void LJContainer::addParticle(double x, double y, double vx, double vy)
	{
		std::vector<double> pos, vel, acc;
		pos.push_back(x); vel.push_back(vx);
		pos.push_back(y); vel.push_back(vy);
		for (int k = 0; k < 2; k++) acc.push_back(0.0);
		positions.push_back(pos);
		velocities.push_back(vel);
		forces.push_back(acc);
		N++;
	}
	
	void LJContainer::removeParticle()
	{
		if ( N > 0 ) {
			N--;
			positions.pop_back();
			velocities.pop_back();
			forces.pop_back();
		}
	}
	
	void LJContainer::forcesEnergies(int nthreads)
	{
		// Initialise forces and energies
		epot = 0.0;
		for (int i = 0; i < N; i++){
			for (int k = 0; k < 2; k++) forces[i][k] = 0.0;
		}
		
		int spacing = ceil(N/nthreads);
		int start = 0, end = spacing, counter = 0;
		std::vector<std::thread> thrds(nthreads);
        std::vector<double> etemps(nthreads);
		std::vector<std::vector<std::vector<double> > >  ftemps;
		for (int i = 0; i < nthreads; i++){
			ftemps.push_back(forces);
		}
		while (counter < nthreads){
			thrds[counter] = std::thread(&LJContainer::forcesThread, *this, start, end, std::ref(ftemps[counter]), std::ref(etemps[counter]), positions, rcutoff, N);
			start = end;
			end += spacing;
			if (end > N-1) { end = N-1; }
			counter++;
		}
		
		for (int i = 0; i < nthreads; i++){
			thrds[i].join();
			epot += etemps[i];
			for (int j = 0; j < N; j++){
				for (int k = 0; k < 2; k++) forces[j][k] += ftemps[i][j][k];
			}
		}
		
	}
	
	void LJContainer::forcesThread(int start, int end, std::vector<std::vector<double> > &ftemp,
								   double &eptemp, std::vector<std::vector<double> > postemp,
								   double rcut, int npart)
	{		
		// Calculate some useful quantities
		double rcut2 = rcut*rcut;
		double shift = -4.0*(pow(rcut2, -6) - pow(rcut2, -3));
		eptemp = 0.0;
		
		// Placeholders
		std::vector<double> rij, fij, ipos;
		for (int k = 0; k < 2; k++){
			rij.push_back(0.0);
			fij.push_back(0.0);
			ipos.push_back(0.0);
		}
		double fcoeff, d2, od2, od6, od12;
		// Loop over all particles
		for (int i = start; i < end; i++){
			for (int k = 0; k < 2; k++) ipos[k] = postemp[i][k];
	
			for(int j = i+1; j < npart; j++){
				d2 = 0.0;
				for (int k = 0; k < 2; k++){
					// Compute rij, account for periodic boundaries
					rij[k] = postemp[j][k] - ipos[k];
					d2 += rij[k]*rij[k];
				}
				if (d2 < rcut2) {
					od2 = 1.0/d2;
					od6 = od2*od2*od2;
					od12 = od6*od6; 
					
					eptemp += 4.0*(od12 - od6) + shift; 
					fcoeff = (-48.0*od12 + 24.0*od6)*od2;
					for (int k = 0; k < 2; k++){
						fij[k] = rij[k]*fcoeff;
						ftemp[i][k] += fij[k];
						ftemp[j][k] -= fij[k];
					} 
				} // End if				
			} // End inner for-loop
		} // End outer for-loop
	}
	
	void LJContainer::integrate(int nthreads)
	{
		double dt2 = 0.5*dt*dt;
		// Update positions and half-update velocities
		for (int i = 0; i < N; i++){
			for (int k = 0; k < 2; k++){
<<<<<<< HEAD
				positions[i][k] += dt*velocities[i][k] + dt2*forces[i][k];
                velocities[i][k] += 0.5*dt*forces[i][k];
                if (positions[i][k] > box_dimensions[k]){
                    double difference = positions[i][k] - box_dimensions[k];
                    positions[i][k] -= difference;
                    velocities[i][k] *= -1;
                } else if ( positions[i][k] < 0.0 ) {
                    positions[i][k] *= -1;
                    velocities[i][k] *= -1;
                }
			}
		}
		
		// Compute forces and energies
		forcesEnergies(nthreads);
		
		// Second half-update to velocities, and calculate kinetic energy
		ekin = 0.0;
		for (int i = 0; i < N; i++){
			double temp = 0.0;
			for (int k = 0; k < 2; k++){
				velocities[i][k] += 0.5*dt*forces[i][k];
				temp += velocities[i][k]*velocities[i][k];
			}
			ekin += 0.5*temp;
		}
	}
	
	void LJContainer::andersen(double T, double freq)
	{
		std::random_device rd;
		std::mt19937 mt(rd());
		std::uniform_real_distribution<double> uDist(0.0, 1.0);
		std::normal_distribution<double> nDist(0.0, sqrt(T));
		
		for (int i = 0; i < N; i++)
		{
			if(uDist(mt) < freq*dt){
				velocities[i][0] = nDist(mt);
				velocities[i][1] = nDist(mt);
			}
		}
	}

}
