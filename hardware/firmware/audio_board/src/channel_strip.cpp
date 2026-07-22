#include "channel_strip.h"

#include "db_conversion.h"
#include "debug.h"
#include "storage.h"

int InputChannel::instances = 0;


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

void InputChannel::SetGain(float gainDb)
{
    float dgain = gainDb;
    if (has_analog_gain)
    {
        this->analog_gain = this->adc_handler(this->analog_gain_arg, dgain);
        dgain -= this->analog_gain;
    }
    this->digital_gain = dgain;
    this->eepromDirty = true;
}

bool InputChannel::EepromSave()
{
    if (this->filter.dirty)
    {
        this->eepromDirty = true;
    }
    if (!this->eepromDirty)
    {
        return false;
    }

    char fname[10];
    sprintf(fname, "input.%d", this->instanceId);

    EepromInput data = {
        .gain = this->analog_gain + this->digital_gain,
        .phantom = this->phantom_enabled,
        .checksum = 0,
    };

    for (uint8_t i = 0; i < 4; i++)
    {
        data.eq[i].type = this->filter._band[i].type;
        data.eq[i].frequency = this->filter._band[i].frequency;
        data.eq[i].gain = this->filter._band[i].gain;
        data.eq[i].q = this->filter._band[i].q;
    }

    auto checksum = struct_checksum(data);
    data.checksum = checksum;

    if (!storage_save(fname, &data, sizeof(data)))
    {
        debug_printf("storage: issue writing\n");
        return false;
    }

    this->eepromDirty = false;
    this->filter.dirty = false;
    return true;
}

bool InputChannel::EepromLoad()
{
    char fname[10];
    sprintf(fname, "input.%d", this->instanceId);
    EepromInput data = {0};

    storage_load(fname, &data, sizeof(data));

    uint8_t checksum = struct_checksum(data);
    if (checksum != 0)
    {
        debug_printf("storage: invalid checksum on index %d [%02x]\n", this->instanceId, checksum);
        this->eepromDirty = true;
        return false;
    }

    debug_printf("input: Restored %s\n", fname);

    this->SetGain(data.gain);
    this->SetPhantom(data.phantom);

    for (uint8_t i = 0; i < 4; i++)
    {
        this->filter._band[i].type = data.eq[i].type;
        this->filter._band[i].frequency = data.eq[i].frequency;
        this->filter._band[i].gain = data.eq[i].gain;
        this->filter._band[i].q = data.eq[i].q;
    }
    this->filter.Apply();

    return true;
}

void OutputChannel::apply_matrix() const
{
    for (uint8_t i = 0; i < CHANNELS; i++)
    {
        float gain = _crosspoint_gain[i];
        gain += this->digital_gain;

        if (_crosspoint_mute[i])
        {
            gain = 0.0f;
        }

        size_t m = i / 4;
        _matrix_bus[m]->gain(i % 4, coef_from_dB(gain));
    }
}
