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

// Defines constant strings for the info sections

#ifndef info_text_h
#define info_text_h

#include <string>

// Note that the formatting of this text is very important!

const std::string SYSTEM_INFO = "The coloured blobs zooming round the screen are particles.\n\nTheir colour tells you their speed - how quickly they are moving. Blue particles have less energy\nand are moving more slowly, whilst pink ones have more energy and are going faster.\n\nEnergy is passed between the particles when they exert forces on each other. This can be seen\nby how the blobs deform. The walls of the box also impart a sudden force, called an impulse,\nwhen the particles hit them.\n\nClicking on the box will create a Gaussian. This is an external potential - an outside force -\nacting on the particles. It can be repulsive or attractive. Try making noise and see it change.";


const std::string CONTROLS_INFO_TEMP = "Temperature:\n\nThis controls how hot or cold particles are.\n\nIt takes some time for the particles to heat up or cool down when changed.";

const std::string CONTROLS_INFO_PART = "Particles:\n\nThis is the number of particles in the system.\n\nChanging this causes the system to reset to a rectangular grid arrangement.";

const std::string CONTROLS_INFO_SIM = "Simulation speed:\n\nThis sets the rate at which the simulation runs.\n\nIncreasing this causes this particles to appear to move faster,\nbut they do not gain any energy; effectively this causes time to move faster or slower.";

const std::string CONTROLS_INFO_GAUSS = "Gaussian:\n\nIf the microphone input to Argon is turned off,\nthen selecting a gaussian allows you to control how large it is\nand whether it attracts or repels particles.";

const std::string CONTROLS_INFO_GRAPHS = "Energy graphs:\n\nThis plots the potential and kinetic energy over time.\n\nThe potential energy measures how strongly the particles are attracted to\nor repelled from each other, and with the gaussians.\n\nThe kinetic energy measures how quickly the particles are moving.";

const std::string CONTROLS_INFO_MAXWELL = "Maxwell-Boltzmann:\n\nThis plots the Maxwell-Boltzmann distribution for the system.\n\nThis is the distribution of particle speeds.\n\nIf the peak in the graph is further to the right, particles are on average moving faster.";

const std::string POTENTIALS_INFO_GENERAL = "\n\nThe selected potential decides how particles interact with each other,\nand often different potentials are used depending on the type of system being modelled.\n\nDesigning a good potential depends not only on how accurately it represents real behaviour,\nbut also in how quickly it can be used to calculate the forces between particles.";

const std::string POTENTIALS_INFO_LENNARD = "The Lennard-Jones potential is composed of a short-range repulsive term\nand a long-range attractive term.\n\nThis is commonly used as an approximation of interactions between non-bonded atoms,\nespecially in weakly interacting systems such as noble gases.";

const std::string POTENTIALS_INFO_SQUARE = "The square well potential is one of the simplest models for atomic interactions that still produces some physical properties,\nparticularly in liquids.\n\nAtoms at distances greater than the length of the well do not interact,\nwhereas those within it experience a constant attraction.";


const std::string POTENTIALS_INFO_MORSE = "Like the Lennard-Jones potential, the Morse potential is composed of short-range repulsive\nand long-range attractive terms.\n\nHowever, the steeper attractive component more accurately reflects the potential of a chemical\nbond, and so is generally preferred for modelling molecules.";

const std::string POTENTIALS_INFO_CUSTOM = "The custom potential is calculated by joining the control points you place on the screen.\n\nLeft-click adds a control point, and right-click is used to remove points.";


#endif /* info_text_h */
