//
//  audio.cpp
//  StarredMD
//
//  Created by Staszek Welsh on 03/06/2016.
//
//

#include "audio.hpp"

// Default constructor sets the stream up with a default maxAmplitude of 0.04
AudioStream::AudioStream() : volume(0), active(true), maxAmplitude(0.04) {
    // call this class's audioIn event handler if the mic gets input
    stream.setInput(this);
    
    // 0 output channels, 2 input channels
    // sample rate 44100Hz
    // buffer size 256, 4 buffers
    stream.setup(0, 2, 44100, 256, 4);
};

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
    if (_active) { stream.start(); }
    else { stream.stop(); }
    
    active = _active;
}

// toggles whether the stream is active
void AudioStream::toggleActive() {
    if (active) { stream.stop(); }
    else { stream.start(); }
    
    active = not active;
}

// getter and setter for maxAmplitude
double AudioStream::getMaxAmplitude() const { return maxAmplitude; }
void AudioStream::setMaxAmplitude(double set) { maxAmplitude = set; }