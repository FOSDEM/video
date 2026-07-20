#pragma once
#include "analyze_peak.h"
#include "analyze_rms.h"
#include "mixer.h"

class InputChannel
{
public:
    float level_peak;
    float level_rms;
    float level_smooth;

    InputChannel(AudioAnalyzePeak* in_peak, AudioAnalyzeRMS* in_rms)
    {
        _peak = in_peak;
        _rms = in_rms;
        _level_multiplier = 1.0f;

        level_peak = 0.0f;
        level_rms = 0.0f;
        level_smooth = 0.0f;
    }

    void update();


    void SetLevelCalibration(float multiplier)
    {
        _level_multiplier = multiplier;
    }

protected:
    float _level_multiplier;

private:
    AudioAnalyzePeak* _peak;
    AudioAnalyzeRMS* _rms;
};

class OutputChannel : public InputChannel
{
public:
    OutputChannel(AudioAnalyzePeak* out_peak, AudioAnalyzeRMS* out_rms,
                  std::initializer_list<AudioMixer4*> mixers) : InputChannel(out_peak, out_rms)
    {
        for (auto m : mixers)
        {
            _matrix_bus.push_back(m);
        }
    }

    void SetCrosspointLevel(int input_index, float gain)
    {
        size_t m = input_index / 4;
        _matrix_bus[m]->gain(input_index % 4, gain);
    }

    float GetCrosspointLevel(int input_index)
    {
        return _matrix_bus[input_index / 4]->getGain(input_index % 4);
    }

    void CrosspointMute(int input_index, bool mute)
    {
    }

private:
    std::vector<AudioMixer4*> _matrix_bus;
};
