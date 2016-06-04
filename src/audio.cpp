//
//  audio.cpp
//  StarredMD
//
//  Created by Staszek Welsh on 03/06/2016.
//
//

#include "audio.hpp"

AudioStream::AudioStream() : volume(0), active(true), maxAmplitude(0.04) {
    stream.setInput(this);
    stream.setup(0, 2, 44100, 256, 4);
};

void AudioStream::audioIn(ofSoundBuffer &buffer) {
    // smooth input by taking 93:7 ratio of old to new volume
    volume = 0.93 * volume + 0.07 * buffer.getRMSAmplitude();
}

double AudioStream::getRawVolume() { return volume; }
double AudioStream::getVolume() { return ofMap(volume, 0, maxAmplitude, 0, 1, true); }

bool AudioStream::getActive() { return active; }

void AudioStream::setActive(bool _active) {
    if (_active) { stream.start(); }
    else { stream.stop(); }
    
    active = _active;
}

double AudioStream::getMaxAmplitude() { return maxAmplitude; }

void AudioStream::setMaxAmplitude(double set) { maxAmplitude = set; }