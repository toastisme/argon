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

#include "audio.hpp"

// Default constructor
AudioStream::AudioStream() : volume(0), active(true), maxAmplitude(0.04) {

}

void AudioStream::setStream(ofSoundStream *_stream) {
    stream = _stream;
}

void AudioStream::audioIn(ofSoundBuffer &buffer) {
    // smooth input by taking 93:7 ratio of old to new volume
    volume = 0.93 * volume + 0.07 * buffer.getRMSAmplitude();
}

// get raw, unscaled volume
double AudioStream::getRawVolume() const { return volume; }

// get volume scaled between 0 and 1 based maxAmplitude
double AudioStream::getVolume() const { return ofMap(volume, 0, maxAmplitude, 0, 1, true); }

// getter for active
bool AudioStream::getActive() const { return active; }

// if setting the stream active, call stream.start(), and vice-versa for stream.stop()
// otherwise a regular setter for active
void AudioStream::setActive(bool _active) {
    if (_active) { (*stream).start(); }
    else { (*stream).stop(); }
    
    active = _active;
}

// toggles whether the stream is active
void AudioStream::toggleActive() {
    if (active) { (*stream).stop(); }
    else { (*stream).start(); }
    
    active = ! active;
}

// getter and setter for maxAmplitude
double AudioStream::getMaxAmplitude() const { return maxAmplitude; }
void AudioStream::setMaxAmplitude(double set) { maxAmplitude = set; }
