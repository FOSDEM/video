#include "channel_strip.h"

void InputChannel::update()
{
    float rms = this->_rms->read() * this->_level_multiplier;
    this->level_peak = this->_peak->read() * this->_level_multiplier;
    this->level_smooth = (this->level_smooth * 9 + rms) / 10;

    if (rms < this->level_rms)
    {
        this->level_rms *= 0.97;
    }
    else
    {
        this->level_rms = rms;
    }
}
