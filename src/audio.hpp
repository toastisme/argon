//
//  audio.hpp
//  StarredMD
//
//  Created by Staszek Welsh on 03/06/2016.
//
//

#ifndef audio_hpp
#define audio_hpp

#include "ofMain.h"

// inherit from ofBaseSoundInput so that we can set the class itself to handle stream's input events
class AudioStream : ofBaseSoundInput
{
private:
    ofSoundStream stream;   // interal sound stream
    bool active;            // whether the stream is active or not
    
    double maxAmplitude;    // the maximum amplitude needed to max out the scaled volume (renamed from sensitivity)
    double volume;          // internal, unscaled volume
    
    // handle audio input event
    void audioIn(ofSoundBuffer &buffer);
    
public:
    // default constructor sets everything up and starts the stream
    AudioStream();
    
    double getRawVolume() const;   // get raw value of volume
    double getVolume() const;      // get volume scaled between 0 and 1 based on maxAmplitude
    
    // getter and setter for whether the stream is active
    bool getActive() const;
    void setActive(bool active);
    
    // toggles whether the stream is active
    void toggleActive();
    
    // getter and setter for maximum amplitude
    double getMaxAmplitude() const;
    void setMaxAmplitude(double maxAmplitude);
};

#endif /* audio_hpp */
