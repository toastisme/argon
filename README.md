# [Download Argon for Mac OSX](http://bitbucket.org/stargonauts/argon/downloads/Argon.zip)

---

# README #

### Argon ###

Argon is a real-time MD toy allowing the user to play with both the pairwise and external potentials in a simulation of point particles. Beta release was 22/07/16.

The Argon team consists of David McDonagh, Robert Shaw, and Staszek Welsh.

### Building ###

Argon is built on [openFrameworks](http://openframeworks.cc/), and this repository should be cloned into the main directory of an openFrameworks project to correctly set up the dependencies. Currently, Argon only supports OSX.

#### Build instructions for OSX ####

1. Download the latest version of [openFrameworks](http://openframeworks.cc/download) for OSX.
2. Extract the .zip file to a directory, and navigate to that directory in a terminal window.
3. openFrameworks applications are, by default, located within the 'myApps' folder, which can be found by executing
    ```
    cd apps/myApps
    ```.
4. Make a local clone of the Argon repository by executing
    ```
    git clone https://bitbucket.org/stargonauts/argon
    ```.
5. Navigate into the directory you have just created:
    ```
    cd argon
    ```.
6. Build Argon using the command
    ```
    make Release
    ```.
This will take some time as openFrameworks is distributed as source code, and so needs to be compiled in full.
7. Run Argon by opening the application with
    ```
    open bin/argon.app
    ```
, or alternatively using the command
    ```
    make RunRelease
    ```.

### License ###

Copyright © 2016 David McDonagh, Robert Shaw, Staszek Welsh

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.