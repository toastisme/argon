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

#ifndef audio_hpp
#define audio_hpp

#include "ofMain.h"

// inherit from ofBaseSoundInput so that we can set the class itself to handle stream's input events
class AudioStream : ofBaseSoundInput
{
private:
    ofSoundStream *stream;   // interal sound stream
    bool active;            // whether the stream is active or not
    
    double maxAmplitude;    // the maximum amplitude needed to max out the scaled volume (renamed from sensitivity)
    double volume;          // internal, unscaled volume
    
public:
    // default constructor sets everything up and starts the stream
    AudioStream();
    
    void setStream(ofSoundStream *stream);
    
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
    
    // handle audio input event
    void audioIn(ofSoundBuffer &buffer);
};

#endif /* audio_hpp */
