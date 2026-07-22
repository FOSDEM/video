#pragma once
#include "analyze_peak.h"
#include "analyze_rms.h"
#include "channels.h"
#include "debug.h"
#include "mixer.h"

struct EepromInput
{
    float gain;
    bool phantom;
    uint8_t checksum;
};

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

        instanceId = instances++;
    }

    void update();

    void SetGain(float gainDb);
    float GetGain() const
    {
        return digital_gain + analog_gain;
    }

    bool EepromSave();
    bool EepromLoad();

    void SetPhantomPowerPin(uint8_t pin)
    {
        pinMode(pin, OUTPUT);
        has_phantom = true;
        phantom_pin = pin;
    }

    void HandleAnalogGain(int arg, float (*set_gain)(int, float))
    {
        this->has_analog_gain = true;
        this->analog_gain_arg = arg;
        this->adc_handler = set_gain;
    }

    [[nodiscard]] bool IsPhantomEnabled() const
    {
        return phantom_enabled;
    }

    void SetPhantom(bool enable)
    {
        if (!has_phantom)
        {
            return;
        }
        digitalWrite(phantom_pin, enable);
        phantom_enabled = enable;
        eepromDirty = true;
        // Immediately save phantom state changes so it doesn't accidentally get re-enabled when rebooting quickly
        EepromSave();
    }

    void SetLevelCalibration(float multiplier)
    {
        _level_multiplier = multiplier;
    }

protected:
    float _level_multiplier;
    float digital_gain = 0.0f;

private:
    // Phantom control
    bool has_phantom;
    bool phantom_enabled;
    uint8_t phantom_pin = 0;

    // ADC control
    bool has_analog_gain;
    int analog_gain_arg;
    float (*adc_handler)(int, float);

    // Gain
    float analog_gain = 0.0f;

    // Storage
    static int instances;
    int instanceId;
    bool eepromDirty;

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
        _crosspoint_gain[input_index] = gain;
        this->apply_matrix();
    }

    float GetCrosspointLevel(int input_index)
    {
        return _crosspoint_gain[input_index];
    }

    void SetCrosspointMute(int input_index, bool mute)
    {
        _crosspoint_mute[input_index] = mute;
        this->apply_matrix();
    }

    bool GetCrosspointMute(int input_index)
    {
        return _crosspoint_mute[input_index];
    }

private:
    void apply_matrix() const;
    std::vector<AudioMixer4*> _matrix_bus;
    float _crosspoint_gain[CHANNELS]; // dB
    bool _crosspoint_mute[CHANNELS];
};

class ChannelStrip
{
public:
    ChannelStrip(InputChannel* channel)
    {
        is_stereo = false;
        left = channel;
    }

    ChannelStrip(InputChannel* left, InputChannel* right)
    {
        is_stereo = true;
        this->left = left;
        this->right = right;
    }

private:
    bool is_stereo;
    InputChannel* left;
    InputChannel* right;
};

class Bus
{
public:
    Bus(OutputChannel* channel)
    {
        is_stereo = false;
        left = channel;
    }

    Bus(OutputChannel* left, OutputChannel* right)
    {
        is_stereo = true;
        this->left = left;
        this->right = right;
    }

private:
    bool is_stereo;
    OutputChannel* left;
    OutputChannel* right;
};
