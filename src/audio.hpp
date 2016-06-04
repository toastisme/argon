//
//  audio.hpp
//  StarredMD
//
//  Created by Staszek Welsh on 03/06/2016.
//
//

#ifndef audio_hpp
#define audio_hpp

#include <vector>
#include "ofMain.h"

class AudioStream : ofBaseSoundInput
{
private:
    ofSoundStream stream;
    bool active;
    
    double maxAmplitude;
    double volume;
    
    void audioIn(ofSoundBuffer &buffer);
    
public:
    AudioStream();
    
    double getRawVolume();
    double getVolume();
    
    bool getActive();
    void setActive(bool active);
    
    double getMaxAmplitude();
    void setMaxAmplitude(double maxAmplitude);
};

#endif /* audio_hpp */
