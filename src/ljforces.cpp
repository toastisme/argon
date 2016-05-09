/***************************************************
 *                                                 *
 *  MD Pie LJForces Library                        *
 *  Implements ljforces.hpp                        *
 *  Robert A Shaw (2016)                           *
 *                                                 *
 ***************************************************/

#include "ljforces.hpp"
#include <cmath> // Basic maths functions
#include <random> // For the Andersen thermostat
#include <thread> // For multithreading
#include <iostream>

namespace lj{

    /*
        DEFAULT CONSTRUCTOR:
            Initially sets the box dimensions to 10 x 10, the rcutoff to 3,
            and the timstep to 0.01. Initialises maximum energies, and enCounter,
            to zero.
     */
    LJContainer::LJContainer()
    {
        box_dimensions.push_back(0);
        box_dimensions.push_back(0);
        setConsts(10.0, 10.0, 3.0, 0.01);
        maxEKin = 0.0;
        maxEPot = 0.0;
        enCounter = 0;
    }
	
    /*
        ROUTINE clearSystem:
            Cleans out all vectors/matrices, so that the system can be completely
            reset and the simulation started again. This includes setting the number of particles
            back to zero, as all particles are removed.
     */
    void LJContainer::clearSystem()
    {
        positions.clear();
        velocities.clear();
        forces.clear();
        previousPositions.clear();
        prevEKin.clear();
        prevEPot.clear();
        N = 0;
    }
    
    //----------------------GETTERS--------------------------------
    
    
    // Return values of private variables without altering them
	int const LJContainer::getN() { return N; }
	double const LJContainer::getEPot() { return epot; }
	double const LJContainer::getEKin() { return ekin; }
    double const LJContainer::getT() { return T; }
    double const LJContainer::getVAvg() { return v_avg; }
    double const LJContainer::getWidth() { return box_dimensions[0]; }
    double const LJContainer::getHeight() { return box_dimensions[1]; }
    double const LJContainer::getMaxEkin() { return maxEKin; }
    double const LJContainer::getMaxEpot() { return maxEPot; }
    
    // Return sizes of gaussians and energies vectors, i.e. the number of gaussians
    // and the number of energies stored.
    int const LJContainer::getNGaussians() { return gaussians.size(); }
    int const LJContainer::getNEnergies() { return prevEKin.size(); }
	
    // Return (x, y) vectors of the dynamical variables of particle i
    // Safety checks could be added, but index checking is usually slow
    std::vector<double> const LJContainer::getPos(int i) { return positions[i]; }
    std::vector<double> const LJContainer::getVel(int i) { return velocities[i]; }
    std::vector<double> const LJContainer::getForces(int i) { return forces[i]; }
    
    // Return the (x, y) position vector of particle npart, from nstep timesteps previously
    std::vector<double> const LJContainer::getPreviousPositions(int npart, int nstep) { return previousPositions[nstep][npart]; }
    
    // Return the ith previous kinetic and potential energy
    double const LJContainer::getPreviousEkin(int i) { return prevEKin[i]; }
    double const LJContainer::getPreviousEpot(int i) { return prevEPot[i]; }
    
    // Return a reference to the ith gaussian in gaussians
    Gaussian& LJContainer::getGaussian(int i) { return gaussians[i]; }
    
    // Return values of the private variables of the ith gaussian in gaussians
    double LJContainer::getGaussianAlpha(int i) { return gaussians[i].getgAlpha(); }
    double LJContainer::getGaussianAmp(int i) { return gaussians[i].getgAmp(); }
    double LJContainer::getGaussianX0(int i) { return gaussians[i].getgex0(); }
    double LJContainer::getGaussianY0(int i) { return gaussians[i].getgey0(); }
    
    
    
    
    //--------------------------------------SETTERS----------------------------------------
    
    
    // Set the position of particle i to (x, y)
	void LJContainer::setPos(int i, double x, double y)
	{
		positions[i][0] = x;
		positions[i][1] = y;
	}
	
    // Set the velocity of particle i to (vx, vy)
	void LJContainer::setVel(int i, double vx, double vy)
	{
		velocities[i][0] = vx;
		velocities[i][1] = vy;
	}
    
	// Set the parameters - the box dimensions [width, height] to [_boxw, _boxl],
    // the cutoff radius for the pair potential to rcutoff = _rcutoff,
    // and the timestep dt = _dt
    // subject to conditions on the suitability of each number.
	void LJContainer::setConsts(double _boxl, double _boxw, double _rcutoff, double _dt)
	{
        
        // Width and height should be positive numbers - DEFAULTS TO 10 x 10
        if ( _boxw > 0 ) { box_dimensions[0] = _boxw;}
        else { box_dimensions[0] = 10.0; }
        if ( _boxl > 0 ) { box_dimensions[1] = _boxl;}
        else { box_dimensions[1] = box_dimensions[0]; }
        
        // rcutoff and dt should also be positive; negative radius makes no sense,
        // and negative timestep would work fine, but seems weird.
        // DEFAULTS TO rcutoff = 3 AND dt = 0.01
        if ( _rcutoff > 0 ) { rcutoff = _rcutoff; }
        else { rcutoff = 3.0; }
        if ( _dt > 0 ) { dt = _dt; }
        else { dt = 0.01; }
    }

    // Set the temperature to _T
    // This should probably check that the temperature isn't negative
    void LJContainer::setTemp(double _T){
            T = _T;

    }
    
    // Set the number of particles - this is redundant due to the number of particles being
    // counted via the addParticle function
    void LJContainer::setParticles(int _N){
        if ( _N > 0){
            N = _N;
        }
        else{
            N = 10;
        }
    }
	
    
    
    /*
        ROUTINE addParticle:
            Creates a particle at position (x, y) with velocity (vx, vy)/
            Appends this to the positions and velocities matrices. 
            If there were N particles, this is now particle i = N+1.
            The particle counter, N, increments.
     */
	void LJContainer::addParticle(double x, double y, double vx, double vy)
	{
	
        std::vector<double> pos, vel, acc; // Temporary vectors of variables
        
        // Set the values of these vectors - zero acceleration to start with
		pos.push_back(x); vel.push_back(vx);
		pos.push_back(y); vel.push_back(vy);
		for (int k = 0; k < 2; k++) acc.push_back(0.0);
        
        // Push the particle onto the positions, velocities, and forces matrices
		positions.push_back(pos);
		velocities.push_back(vel);
		forces.push_back(acc);
        
        // Increment the number of particles
		N++;
	}
	
    /*
        ROUTINE removeParticle:
            Removes the most recently added particle, if there are any particles to remove.
            Decrements the particle counter, N, and removes the particle variables from
            the positions, velocities, forces arrays.
     */
	void LJContainer::removeParticle()
	{
		if ( N > 0 ) { // Make sure there is a particle to be removed
			N--;
            
            // pop_back is a last in-first out (LIFO) command
			positions.pop_back();
			velocities.pop_back();
			forces.pop_back();
		}
	}
    
    
    /*
        ROUTINE addGaussian:
            Adds a new Gaussian external potential with amplitude gAmp,
            exponent gAlpha, and centre (gex0, gey0). 
            If there are already 4 gaussians, it removes the first Gaussian in the
            gaussians array.
     */
    void LJContainer::addGaussian(double gAmp, double gAlpha, double gex0, double gey0){
        // Scale set to 1, so no scaling occurs before audio input
        Gaussian newGaussian(gAmp, gAlpha, gex0, gey0, 1.0);
        
        // Push onto the end of the gaussians array
        gaussians.push_back(newGaussian);
        
        // If too many gaussians, remove the first in the array
        // First in - first out (FIFO)
        // removeGaussian has default argument i = 0
        if (gaussians.size() > 4) removeGaussian();
    }
    
    /*
        ROUTINE removeGaussian:
            Removes the ith Gaussian from the gaussians array by using the built in
            iterator 'erase' from std::vector. This will simply not remove anything
            if i is < 0 or i > the number of Gaussians in the array.
     
            i = 0 by default if no argument is provided
     */
    void LJContainer::removeGaussian(int i){
        // Remove ith gaussian (first by default)
        gaussians.erase(gaussians.begin()+i, gaussians.begin()+i+1);
    }
    
    /*
        ROUTINE updateGaussian:
            Set the amplitude, exponent, centre, and scale of the ith Gaussian to gAmp, gAlpha,
            (gex0, gey0), and scale, respectively. 
     
            This does not perform index checking.
     */
    void LJContainer::updateGaussian(int i, double gAmp, double gAlpha, double gex0, double gey0, double scale){
        gaussians[i].setParams(gAmp, gAlpha, gex0, gey0, scale);
    }
    
    
    /* 
        ROUTINE externalForces:
            Loops over all the Gaussians in the gaussians array and calculates the forces on each particle
            due to each Gaussian (if any exist), and the contribution to the potential energy.
     */
    void LJContainer::externalForce()
    {

        std::array<double, 3> forceEnergy; // Temporary array to store force and potential energy
        double x, y; // Unpack the x and y positions of particle i
        
        for (int g = 0; g < gaussians.size(); g++){ // Loop over Gaussians
            
            for (int i = 0; i < N; i++){ // Loop over particles
                
                x = positions[i][0];
                y = positions[i][1];
                
                // Calculate the force and energy into forceEnergy
                // due to Gaussian g at position (x, y)
                gaussians[g].calcForceEnergy(x, y, forceEnergy);
                
                // Update forces on particle i
                forces[i][0] += forceEnergy[0];
                forces[i][1] += forceEnergy[1];
                
                // Update potential energy
                epot += forceEnergy[2];
            }
        }
    }
	
    /* 
        ROUTINE forcesEnergies:
            Load-balance the force calculations for the system onto nthreads threads
            using the standard threads library.
     
            Results in the forces and potential energy being stored in the forces matrix
            and epot. Also calls externalForces.
     */
    void LJContainer::forcesEnergies(int nthreads)
    {
            // Initialise forces and energies to zero
            epot = 0.0;
            for (int i = 0; i < N; i++){
                    for (int k = 0; k < 2; k++) forces[i][k] = 0.0;
            }
        
            // Divide the N x N force matrices into roughly equal
            // columnwise chunks of N/nthreads.
            int spacing = ceil(N/nthreads);
        
            // Make temporary arrays to store the values calculated on each thread
            int start = 0, end = spacing, counter = 0; // Counters
        
            std::vector<std::thread> thrds(nthreads); // Vector of threads
            std::vector<double> etemps(nthreads); // Vector of potential energies
        
            // Vector of temporary force matrices
            std::vector<std::vector<std::vector<double> > >  ftemps;
            for (int i = 0; i < nthreads; i++){
                    // For large simulations, this would be very memory intensive,
                    // but for small simulations is more efficient than trying to deal
                    // with potential threading gridlocks.
                
                    ftemps.push_back(forces); // Makes sure they are the right shape
            }
        
            // Farm each portion of the forces matrix out to a thread
            while (counter < nthreads){
                
                    // Create thread of forcesThread procedure
                    thrds[counter] = std::thread(&LJContainer::forcesThread, *this,
                                                 start, end, std::ref(ftemps[counter]),
                                                 std::ref(etemps[counter]), positions,
                                                 rcutoff, N);
                
                    // Move to the next chunk of the forces matrix
                    start = end;
                    end += spacing;
                
                    // If nthreads does not divide N exactly, the last thread will have
                    // a different number of particles assigned, the last of which has
                    // index N-1
                    if (end > N-1) { end = N-1; }
                
                    // Increment thread counter
                    counter++;
            }
        
            // Collect the results from each thread into the forces matrix, and epot variable
            for (int i = 0; i < nthreads; i++){
                    thrds[i].join(); // Waits for thread i to finish before proceeding
                    epot += etemps[i];
                    for (int j = 0; j < N; j++){
                            for (int k = 0; k < 2; k++) forces[j][k] += ftemps[i][j][k];
                    }
            }
        
            // Calculate the forces due to the external Gaussian potentials
            externalForce();
    }
    
    /*
        ROUTINE forcesThread:
            Calculates the Lennard-Jones forces and potential energy due to particles start through end interacting
            with every other particle (in the upper triangular part of the forces matrix).
     
            Stores the results in ftemp and eptemp, respectively.
            
            npart is the number of particles, and postemp is a temporary array of the positions of the
            particles.
     */
    void LJContainer::forcesThread(int start, int end,
                                   std::vector<std::vector<double> > &ftemp,
                                   double &eptemp, std::vector<std::vector<double> > postemp,
                                   double rcut, int npart)
    {		
            // Calculate the correction due to the shift of the Lennard-Jones potential
            double rcut2 = rcut*rcut;
            double shift = -4.0*(pow(rcut2, -6) - pow(rcut2, -3));
        
            // Set potential energy to zero
            eptemp = 0.0;
            
            // Placeholders for the separation (rij) and forces (fij) between particles i and j
            // and the position of particle i (ipos)
            std::vector<double> rij, fij, ipos;
            for (int k = 0; k < 2; k++){
                    rij.push_back(0.0);
                    fij.push_back(0.0);
                    ipos.push_back(0.0);
            }
        
            double fcoeff; // Magnitude of fij
            double d2, od2, od6, od12; // d2 = rij^2, and one over rij^n = odn
        
            // Loop over all particles from start to end
            for (int i = start; i < end; i++){
                    for (int k = 0; k < 2; k++) ipos[k] = postemp[i][k]; // Unpack position of particle i
    
                    for(int j = i+1; j < npart; j++){
                            d2 = 0.0;
                            for (int k = 0; k < 2; k++){
                                    // Compute rij
                                    rij[k] = postemp[j][k] - ipos[k];
                                    d2 += rij[k]*rij[k];
                            }
                            if (d2 < rcut2) { // Check if within cutoff radius
                                    od2 = 1.0/d2;
                                    od6 = od2*od2*od2;
                                    od12 = od6*od6; 
                                
                                    // Lennard-Jones energy and forces
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
    
    /*
        ROUTINE integrate:
            Performs the main velocity-Verlet integration step of the MD simulation. After the 
            LJContainer is set up, and a single forcesEnergies calculation performed, this routine
            is all that needs to be called to propagate the system by one timestep. 
     
            nthreads is the number of threads the forces calculations should be performed on
     */
    void LJContainer::integrate(int nthreads)
    {
        
            double dt2 = 0.5*dt*dt; // Useful quantity for velocity verlet, dt^2/2
        
            for (int i = 0; i < N; i++){ // Loop over particles
                
                    for (int k = 0; k < 2; k++){ // Loop over dimensions
                        
                        // Update positions and half-update velocities
                        positions[i][k] += dt*velocities[i][k] + dt2*forces[i][k];
                        velocities[i][k] += 0.5*dt*forces[i][k];
                        
                        // Hard-wall boundary conditions - reflect if collided with box wall
                        if (positions[i][k] > box_dimensions[k]){ // Too far to the right or down
                            double difference = positions[i][k] - box_dimensions[k];
                            positions[i][k] -= difference;
                            velocities[i][k] *= -1;
                        } else if ( positions[i][k] < 0.0 ) { // Too far up or to the left
                            positions[i][k] *= -1;
                            velocities[i][k] *= -1;
                        }
                    }
                
            }
            
            // Compute forces and energies on nthreads threads
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
        
            // Store the last 20 positions matrices
        
            // FIFO removal of previous position matrix if 20 are already being stored
            if (previousPositions.size() == 20){
                previousPositions.erase(previousPositions.begin(), previousPositions.begin()+1);
            }
            previousPositions.push_back(positions); // Push current positions onto previousPositions
        
            // Store the previous 600 kinetic and potential energies at intervals of 5 timesteps
            if (enCounter % 5 == 0){ // Check 5 timesteps have passed
                
                // FIFO removal of previous kinetic/potential energies if 120 are already being stored
                if (prevEKin.size() == 120) {
                    prevEKin.erase(prevEKin.begin(), prevEKin.begin()+1);
                    prevEPot.erase(prevEPot.begin(), prevEPot.begin()+1);
                }
                
                // Push back current kinetic/potential energies for graphing
                prevEKin.push_back(ekin);
                prevEPot.push_back(epot);
        
                // Find the maximum elements in prevEKin and prevEPot
                std::vector<double>::iterator EKMAX = std::max_element(prevEKin.begin(), prevEKin.end());
                std::vector<double>::iterator EPMAX = std::max_element(prevEPot.begin(), prevEPot.end());
                maxEKin = prevEKin[std::distance(prevEKin.begin(), EKMAX)];
                maxEPot = prevEPot[std::distance(prevEPot.begin(), EPMAX)];
            }
    
            enCounter++; // Incremement the number of timesteps performed
    }
    
    
    
    
    //----------------------------------------THERMOSTATS----------------------------------------
    /*
        ANDERSEN:
            Random collisions with the heat bath, so that velocities are sampled from (roughly)
            a Maxwell distribution corresponding to the desired temperature, T.
     
            Depending on the value set for the frequency (freq) of collisions, this can be very slow to
            update the temperature of the system.
        
        BERENDSEN:
            Rescales the velocities of all particles, with a damping strength controlled by the frequency (freq)
            so that they obey:
            
            3/2 NkT = 1/2 mv^2
     */
    
    void LJContainer::andersen(double freq)
    {
            // Set up random number generator, rd, to sample from:
            // uniform distribution, uDist, on [0, 1]
            // normal distribution, nDist, mean 0, std. dev. sqrt(T)
            std::random_device rd;
            std::mt19937 mt(rd());
            std::uniform_real_distribution<double> uDist(0.0, 1.0);
            std::normal_distribution<double> nDist(0.0, sqrt(T));
            
            // Randomly select particles to collide with the heat bath
            for (int i = 0; i < N; i++)
            {
                    if(uDist(mt) < freq*dt){
                            velocities[i][0] = nDist(mt);
                            velocities[i][1] = nDist(mt);
                    }
            }
    }

    void LJContainer::berendsen(double freq){
        //Calculate the average velocity
        v_avg = 0;
        for (int i = 0; i < N; i++){
            for (int k = 0; k < 2; k++){
                v_avg += fabs(velocities[i][k]);
            }
        }
        v_avg /= N;
        
        //Calculate scaling factor (lambda)
        double lambda = sqrt(1+((dt/freq)*((T/v_avg)-1)));
        for (int i = 0; i < N; i++){
            for (int k = 0; k < 2; k++){
                //Scale the velocity of each particle
                velocities[i][k]*=lambda;
            }
        }
    }
}
