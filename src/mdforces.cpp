/*
 Argon
 
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

#include "mdforces.hpp"
#include <cmath> // Basic maths functions
#include <random> // For the Andersen thermostat
#include <thread> // For multithreading
#include <iostream>
#include <algorithm>

namespace md {

    /*
        DEFAULT CONSTRUCTOR:
            Initially sets the box dimensions to 10 x 10, the rcutoff to 3, the
            timestep to 0.002, and the thermostat frequency to 0.1.
            Initialises maximum energies to zero, and starts system as running.
     */
    MDContainer::MDContainer()
    {
        box_dimensions = {10, 10};
        rcutoff = 3.0;
        dt = 0.002;
        freq = 0.1;
        maxEKin = 0.0;
        maxEPot = 0.0;
        potential = &lj;
        running = true;
    }
    
    /*
        ROUTINE resetSystem:
            Cleans out all vectors/matrices, so that they contain no particles. Then,
            add NAfterReset particles to the system in a grid.
     */
    void MDContainer::resetSystem()
    {
        positions.clear();
        velocities.clear();
        forces.clear();
        prevPositions.clear();
        prevEKin.clear();
        prevEPot.clear();
        N = 0;
        
        addParticlesGrid(NAfterReset);
        forcesEnergies(1); // one thread
        savePreviousValues();
    }
    
    //----------------------GETTERS--------------------------------
    
    
    // Return values of private variables without altering them
    bool   MDContainer::getRunning()        const { return running; }
    int    MDContainer::getStepsPerUpdate() const { return stepsPerUpdate; }
    int    MDContainer::getN()              const { return N; }
    int    MDContainer::getNAfterReset()    const { return NAfterReset; }
    double MDContainer::getEPot()           const { return epot; }
    double MDContainer::getEKin()           const { return ekin; }
    double MDContainer::getTemp()           const { return T; }
    double MDContainer::getVAvg()           const { return v_avg; }
    double MDContainer::getTimestep()       const { return dt; }
    double MDContainer::getCutoff()         const { return rcutoff; }
    coord  MDContainer::getBox()            const { return box_dimensions; }
    double MDContainer::getWidth()          const { return box_dimensions.x; }
    double MDContainer::getHeight()         const { return box_dimensions.y; }
    double MDContainer::getFreq()           const { return freq; }
    double MDContainer::getMaxEkin()        const { return maxEKin; }
    double MDContainer::getMaxEpot()        const { return maxEPot; }
    double MDContainer::getMinEkin()        const { return minEKin; }
    double MDContainer::getMinEpot()        const { return minEPot; }
    
    // Return sizes of gaussians, prevPos, and energies vectors, i.e. the number
    // of gaussians, positions and energies stored
    int MDContainer::getNGaussians()        const { return gaussians.size(); }
    int MDContainer::getNEnergies()         const { return prevEKin.size(); }
    int MDContainer::getNPrevPos()          const { return prevPositions.size(); }
    
    // Return (x, y) vectors of the dynamical variables of particle i
    // Safety checks could be added, but index checking is usually slow
    coord MDContainer::getPos(int i)        const { return positions[i]; }
    coord MDContainer::getVel(int i)        const { return velocities[i]; }
    coord MDContainer::getForce(int i)      const { return forces[i]; }
    
    // Return the (x, y) position vector of particle npart, from nstep timesteps previously
    coord MDContainer::getPos(int npart, int nstep) const { return prevPositions[nstep][npart]; }
    
    // Return the ith previous kinetic and potential energy
    double MDContainer::getPreviousEkin(int i) const { return prevEKin[i]; }
    double MDContainer::getPreviousEpot(int i) const { return prevEPot[i]; }
    
    // Return a reference to the ith gaussian in gaussians
    Gaussian& MDContainer::getGaussian(int i) { return gaussians[i]; }
    // Return values of the private variables of the ith gaussian in gaussians
    double MDContainer::getGaussianAlpha(int i) const { return gaussians[i].getgAlpha(); }
    double MDContainer::getGaussianAmp(int i)   const { return gaussians[i].getgAmp(); }
    double MDContainer::getGaussianX0(int i)    const { return gaussians[i].getgex0(); }
    double MDContainer::getGaussianY0(int i)    const { return gaussians[i].getgey0(); }
    
    // Return reference to current PotentialFunctor
    PotentialFunctor& MDContainer::getPotential()      { return *potential; }
    CustomPotential& MDContainer::getCustomPotential() { return customPotential; }
    
    //--------------------------------------SETTERS----------------------------------------
    
    // Set running to a value, or its boolean negation
    void MDContainer::setRunning(bool _running) { running = _running; }
    void MDContainer::toggleRunning() { running = ! running; }
    
    // Set the number of steps to make each time run is called
    void MDContainer::setStepsPerUpdate(int steps) { stepsPerUpdate = steps; }
    
    // Set the number of particles to use when the system is reset
    void MDContainer::setNAfterReset(int N) { NAfterReset = N; }
    
    // Set the position of particle i to (x, y)
    void MDContainer::setPos(int i, double x, double y)
    {
        positions[i].x = x;
        positions[i].y = y;
    }
    
    // Set the velocity of particle i to (vx, vy)
    void MDContainer::setVel(int i, double vx, double vy)
    {
        velocities[i].x = vx;
        velocities[i].y = vy;
    }
    
    // Set the parameters - the box dimensions, temperature, timestep and
    // cutoff radius, checking that each is positive. If not, defaults to:
    // box      - 10 x 10
    // temp     - 0.5 (60K)
    // timestep - 0.002
    // cutoff   - 3
    void MDContainer::setBox(double box_width, double box_length) {
        box_dimensions.x = box_width  > 0 ? box_width  : 10.0;
        box_dimensions.y = box_length > 0 ? box_length : 10.0;
    }
    void MDContainer::setTemp(double temperature) { T = temperature >= 0 ? temperature : 0.5; }
    void MDContainer::setTimestep(double timestep) { dt = timestep > 0 ? timestep : 0.002; }
    void MDContainer::setCutoff(double cutoff) { rcutoff = cutoff > 0 ? cutoff : 3.0; }
    void MDContainer::setFreq(double frequency) { freq = frequency >= 0 ? frequency : 0.1; }
    
    // Set the potential
    
    void MDContainer::setPotential(PotentialFunctor* _potential) { potential = _potential; }
    void MDContainer::setPotential(Potential _potential) {
        switch (_potential) {
            case SQUARE_WELL:
                potential = &squareWell;
                break;
            case MORSE:
                potential = &morse;
                break;
            case CUSTOM:
                potential = &customPotential;
                break;
            default:
                potential = &lj;
        }
    }
    
/*
    ROUTINE addParticlesGrid:
        Adds numParticles to the system, in a grid-like manner. The grid chosen is centred in the
        box, has points evenly spaced, and is chosen to have at least as many points as numParticles.
        We add particles at grid positions, starting from the top left - depending on the ratio
        between the box width and height and the number of particles the gridding may be too biased
        in the top of the box.
 */
    void MDContainer::addParticlesGrid(int numParticles) {
        int i = 0, j = 0;
        double posx, posy;
        
        double box_ratio = box_dimensions.x / box_dimensions.y;
        int n_grid_x = ceil(sqrt(numParticles * box_ratio));
        int n_grid_y = ceil(sqrt(numParticles / box_ratio));
        double xspacing = box_dimensions.x / n_grid_x;
        double yspacing = box_dimensions.y / n_grid_y;
        
        // grid particles
        for (int n = 0; n < numParticles; ++n){
            coord pos;
            pos.x = xspacing * (i + 0.5);
            pos.y = yspacing * (j + 0.5);
            
            coord vel = randomVel();
            
            addParticle(pos, vel);
            
            i = (i + 1) % n_grid_x;
            if (i == 0) ++j;
        }
    }

    
    /*
        ROUTINE addParticle:
            Creates a particle at position (x, y) with velocity (vx, vy) specified
            by either four doubles or 2 coord structs
            Appends this to the positions and velocities matrices. 
            If there were N particles, this is now particle i = N+1.
            The particle counter, N, increments.
     */
    void MDContainer::addParticle(coord pos, coord vel)
    {
        coord acc = {0.0, 0.0}; // zero initial acceleration
        
        // Push the particle onto the positions, velocities, and forces matrices
        positions.push_back(pos);
        velocities.push_back(vel);
        forces.push_back(acc);
        
        // Increment the number of particles
        ++N;
    }
    
    void MDContainer::addParticle(double x, double y, double vx, double vy) {
        coord pos = {x, y};
        coord vel = {vx, vy};
        addParticle(pos, vel);
    }
    
    /*
        ROUTINE removeParticle:
            Removes the most recently added particle, if there are any particles to remove.
            Decrements the particle counter, N, and removes the particle variables from
            the positions, velocities, forces arrays.
     */
    void MDContainer::removeParticle()
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
    void MDContainer::addGaussian(double gex0, double gey0){
        // Set amplitude/alpha to default values
        double GAMP =  50.0;
        double GALPHA = 0.8;
        
        // Create new gaussian
        Gaussian newGaussian(GAMP, GALPHA, gex0, gey0);
        
        // Push onto the end of the gaussians array
        gaussians.push_back(newGaussian);
        
        // If too many gaussians, remove the first in the array
        // First in - first out (FIFO)
        // removeGaussian has default argument i = 0
        if (gaussians.size() > 6) removeGaussian();
    }
    
    /*
        ROUTINE removeGaussian:
            Removes the ith Gaussian from the gaussians array by using the built in
            iterator 'erase' from std::vector. This will simply not remove anything
            if i is < 0 or i > the number of Gaussians in the array.
     
            i = 0 by default if no argument is provided
     */
    void MDContainer::removeGaussian(int i){
        // Remove ith gaussian (first by default)
        gaussians.erase(gaussians.begin()+i, gaussians.begin()+i+1);
    }
    

    /*
        ROUTINE updateGaussian:
            Set the amplitude, exponent, centre, and scale of the ith Gaussian to gAmp, gAlpha,
            (gex0, gey0), and scale, respectively. 
     
            This does not perform index checking.
     */
    void MDContainer::updateGaussian(int i, double gAmp, double gAlpha, double gex0, double gey0){
        gaussians[i].setParams(gAmp, gAlpha, gex0, gey0);
    }
    
    
    /* 
        ROUTINE externalForces:
            Loops over all the Gaussians in the gaussians array and calculates the forces on each particle
            due to each Gaussian (if any exist), and the contribution to the potential energy.
     */
    void MDContainer::externalForce()
    {
        std::array<double, 3> forceEnergy; // Temporary array to store force and potential energy
        double x, y; // Unpack the x and y positions of particle i
        
        for (int g = 0; g < gaussians.size(); g++){ // Loop over Gaussians
            
            for (int i = 0; i < N; i++){ // Loop over particles
                
                x = positions[i].x;
                y = positions[i].y;
                
                // Calculate the force and energy into forceEnergy
                // due to Gaussian g at position (x, y)
                gaussians[g].calcForceEnergy(x, y, forceEnergy);
                
                // Update forces on particle i
                forces[i].x += forceEnergy[0];
                forces[i].y += forceEnergy[1];
                
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
    void MDContainer::forcesEnergies(int nthreads)
    {
        // Initialise forces and energies to zero
        epot = 0.0;
        for (int i = 0; i < N; i++){
            forces[i].x = 0.0;
            forces[i].y = 0.0;
        }

        // Divide the N x N force matrices into roughly equal
        // columnwise chunks of N/nthreads.
        int spacing = ceil(N/nthreads);

        // Make temporary arrays to store the values calculated on each thread
        int start = 0, end = spacing, counter = 0; // Counters

        std::vector<std::thread> thrds(nthreads); // Vector of threads
        std::vector<double> etemps(nthreads); // Vector of potential energies

        // Vector of temporary force matrices
        std::vector <std::vector <coord>>  ftemps;
        for (int i = 0; i < nthreads; i++){
            // For large simulations, this would be very memory intensive,
            // but for small simulations is more efficient than trying to deal
            // with potential threading gridlocks.

            ftemps.push_back(forces); // Makes sure they are the right shape
        }

        // Farm each portion of the forces matrix out to a thread
        while (counter < nthreads){

            // Create thread of forcesThread procedure
            thrds[counter] = std::thread(&MDContainer::forcesThread, *this,
                    start, end, std::ref(ftemps[counter]),
                    std::ref(etemps[counter]), positions);

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
                forces[j].x += ftemps[i][j].x;
                forces[j].y += ftemps[i][j].y;
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
    void MDContainer::forcesThread(int start, int end,
            std::vector<coord> &ftemp, double &eptemp,
            std::vector<coord> postemp)
    {        
        // Calculate the correction due to the shift of the Lennard-Jones potential
        double rcut2 = rcutoff*rcutoff;
        double shift = -4.0*(pow(rcut2, -6) - pow(rcut2, -3));

        // Set potential energy to zero
        eptemp = 0.0;

        // Placeholders for the separation (rij) and forces (fij) between particles i and j
        // and the position of particle i (ipos)
        coord rij, fij, ipos;

        double d2, r;  // d2 = |rij|^2, r = |rij|
        double f; // force(rij) / rij

        // Loop over all particles from start to end
        for (int i = start; i < end; ++i) {
            // Unpack position of particle i
            ipos.x = postemp[i].x;
            ipos.y = postemp[i].y;

            for(int j = i+1; j < N; ++j){
                // Compute rij
                rij.x = postemp[j].x - ipos.x;
                rij.y = postemp[j].y - ipos.y;
                
                d2 = rij.x * rij.x + rij.y * rij.y;
                if (d2 < rcut2) { // Check if within cutoff radius
                    r = sqrt(d2);
                    
                    // Energy and forces
                    eptemp += potential->potential(r);
                    f = potential->force(r) / r;
                    
                    fij.x = f * rij.x;
                    fij.y = f * rij.y;
                    
                    ftemp[i].x += fij.x;
                    ftemp[i].y += fij.y;
                    
                    ftemp[j].x -= fij.x;
                    ftemp[j].y -= fij.y;
                } // End if
            } // End inner for-loop
        } // End outer for-loop
    }
    
    /*
        ROUTINE integrate:
            Performs the main velocity-Verlet integration step of the MD simulation. After the 
            MDContainer is set up, and a single forcesEnergies calculation performed, this routine
            is all that needs to be called to propagate the system by one timestep. 
     
            nthreads is the number of threads the forces calculations should be performed on
     */
    void MDContainer::integrate(int nthreads)
    {
        double dt2 = 0.5 * dt * dt; // Useful quantity for velocity verlet, dt^2/2

        for (int i = 0; i < N; ++i){ // Loop over particles

            // Update positions
            positions[i].x += dt * velocities[i].x + dt2 * forces[i].x;
            positions[i].y += dt * velocities[i].y + dt2 * forces[i].y;
            
            // Half-update velocities
            velocities[i].x += 0.5 * dt * forces[i].x;
            velocities[i].y += 0.5 * dt * forces[i].y;

            // Hard-wall boundary conditions - reflect if collided with box wall
            if (positions[i].x > box_dimensions.x) {
                positions[i].x = 2 * box_dimensions.x - positions[i].x;
                velocities[i].x *= -1;
            } else if (positions[i].x < 0) {
                positions[i].x *= -1;
                velocities[i].x *= -1;
            }
            
            if (positions[i].y > box_dimensions.y) {
                positions[i].y = 2 * box_dimensions.y - positions[i].y;
                velocities[i].y *= -1;
            } else if (positions[i].y < 0) {
                positions[i].y *= -1;
                velocities[i].y *= -1;
            }

        }

        // Compute forces and energies on nthreads threads
        forcesEnergies(nthreads);

        ekin = 0.0;
        for (int i = 0; i < N; i++){
            // Second half-update to velocities
            velocities[i].x += 0.5 * dt * forces[i].x;
            velocities[i].y += 0.5 * dt * forces[i].y;
            
            // Calculate kinetic energy of i
            ekin += velocities[i].x * velocities[i].x;
            ekin += velocities[i].y * velocities[i].y;
        }
        ekin *= 0.5;
    }
    
    /*
        ROUTINE savePreviousValues:
            Saves the current position in prevPos and the current energies
            in prevEpot and prevEKin. A maximum of 15 positions and 120
            energies are kept, using a deque for the FIFO behaviour and
            storing the most recent position / energy at index 0
     */
    void MDContainer::savePreviousValues()
    {
        prevPositions.push_front(positions);
        prevEPot.push_front(epot);
        prevEKin.push_front(ekin);
        
        if (prevPositions.size() == 20) prevPositions.pop_back();
        if (prevEPot.size() == 120) prevEPot.pop_back();
        if (prevEKin.size() == 120) prevEKin.pop_back();
        
        // Find the maximum/minimum elements in prevEKin and prevEPot
        std::deque<double>::iterator EKMAX = std::max_element(prevEKin.begin(), prevEKin.end());
        std::deque<double>::iterator EPMAX = std::max_element(prevEPot.begin(), prevEPot.end());
        std::deque<double>::iterator EKMIN = std::min_element(prevEKin.begin(), prevEKin.end());
        std::deque<double>::iterator EPMIN = std::min_element(prevEPot.begin(), prevEPot.end());
        maxEKin = prevEKin[std::distance(prevEKin.begin(), EKMAX)];
        maxEPot = prevEPot[std::distance(prevEPot.begin(), EPMAX)];
        minEKin = prevEKin[std::distance(prevEKin.begin(), EKMIN)];
        minEPot = prevEPot[std::distance(prevEPot.begin(), EPMIN)];
    }
    
    /*
        ROUTINE run:
            Runs the integrator nsteps times, with a thermostat frequency
            freq, on nthreads threads. Saves the positions and energies after
            all nsteps integrations are completed
     */
    void MDContainer::run(int nthreads) {
        if (running) {
            for (int i = 0; i < stepsPerUpdate; ++i) {
                integrate(nthreads);
                berendsen(freq);
            }
            savePreviousValues();
        }
    }


    /*
        ROUTINE rdf:
            Calculates a histogram of particle separations, binning based on a given number of
            bins, and a minimum and maximum separation to include.
     */
    std::vector <double> MDContainer::rdf(double min, double max, int bins) const {
        std::vector <double> dists;
        int N = getN();
        int N_dists = (N * (N-1)) / 2;
        dists.reserve(N_dists);
        coord posi, posj;
        double dx, dy;
        
        for (int i = 0; i < N; ++i) {
            for (int j = i+1; j < N; ++j) {
                posi = getPos(i);
                posj = getPos(j);
                dx = posi.x - posj.x;
                dy = posi.y - posj.y;
                dists.push_back(sqrt(dx * dx + dy * dy));
            }
        }

        // NOTE: this does not properly weight the RDF by the distance
        // but the RDF plot looks better this way anyway
        return histogram(dists, min, max, bins);
    }

    
    /*
        ROUTINE maxwell:
            Calculates a histogram of particle speeds, binning based on a given number of
            bins, and a minimum and maximum speed to include.
     */
    std::vector <double> MDContainer::maxwell(double min, double max, int bins) const {
        std::vector <double> speeds;
        int N = getN();
        speeds.reserve(N);
        
        for (int i = 0; i < N; ++i) {
            coord vel = getVel(i);
            speeds.push_back(sqrt(vel.x * vel.x + vel.y * vel.y));
        }

        return histogram(speeds, min, max, bins);
    }

    
    
    //----------------------------------------THERMOSTATS----------------------------------------
    /*
        ROUTINE random_vel:
            Returns a random velocity from the Maxwell-Boltzmann distribution
     
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
    
    coord MDContainer::randomVel() const
    {
        // Set up random number generator, rd, to sample from:
        // normal distribution, nDist, mean 0, std. dev. sqrt(T)
        std::random_device rd;
        std::mt19937 mt(rd());
        std::normal_distribution<double> nDist(0.0, sqrt(T));
        
        coord vel;
        vel.x = nDist(mt);
        vel.y = nDist(mt);
        return vel;
    }
    
    void MDContainer::andersen(double freq)
    {
        // Set up random number generator, rd, to sample from:
        // uniform distribution, uDist, on [0, 1]
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_real_distribution<double> uDist(0.0, 1.0);
        
        // Randomly select particles to collide with the heat bath
        for (int i = 0; i < N; i++) {
            if(uDist(mt) < freq*dt) velocities[i] = randomVel();
        }
    }

    void MDContainer::berendsen(double freq)
    {
        //Calculate the average velocity
        v_avg = 0;
        for (int i = 0; i < N; i++){
            v_avg += fabs(velocities[i].x);
            v_avg += fabs(velocities[i].y);
        }
        v_avg /= N;
        
        //Calculate scaling factor (lambda)
        if (v_avg > 1e-5) {
            double lambda = sqrt(1+((dt*freq)*((T/v_avg)-1)));
            for (int i = 0; i < N; i++){
                //Scale the velocity of each particle
                velocities[i].x *= lambda;
                velocities[i].y *= lambda;
            }
        } else {
            // if the particles aren't moving and we want them to, collide everything with an andersen heatbath
            andersen(1000000);
        }
    }
}
