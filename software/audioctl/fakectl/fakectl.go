package fakectl

import (
	"fmt"
	"math"
	"math/rand"
	"time"

	"github.com/fosdem/video/software/audioctl/config"
	"github.com/fosdem/video/software/audioctl/ctl"
)

type FakeCtl struct {
	state  *ctl.MixerState
	cfg    *config.CtlCfg
	levels *ctl.Levels
}

func New(cfg *config.CtlCfg) *FakeCtl {
	return &FakeCtl{
		state:  DefaultState,
		cfg:    cfg,
		levels: DefaultLevels(),
	}
}

func (c *FakeCtl) GetFullState() (*ctl.MixerState, error) {
	return c.state.Copy(), nil
}

func (c *FakeCtl) GetChannelNames() ([]string, []string, error) {
	chanNames := make([]string, len(c.state.Channels))
	for i := range c.state.Channels {
		chanNames[i] = c.state.Channels[i].Name
	}

	busNames := make([]string, len(c.state.Buses))
	for i := range c.state.Buses {
		busNames[i] = c.state.Buses[i].Name
	}

	return chanNames, busNames, nil
}

func (c *FakeCtl) GetChannelLabels() ([]string, []string, error) {
	chanLabels := make([]string, len(c.state.Channels))
	for i := range c.state.Channels {
		chanLabels[i] = c.state.Channels[i].Label
	}

	busLabels := make([]string, len(c.state.Buses))
	for i := range c.state.Buses {
		busLabels[i] = c.state.Buses[i].Label
	}

	return chanLabels, busLabels, nil
}

func (c *FakeCtl) GetInputGains() ([]float32, error) {
	gains := make([]float32, len(c.state.Channels))
	for i := range c.state.Channels {
		gains[i] = c.state.Channels[i].Gain
	}
	return gains, nil
}

func (c *FakeCtl) GetBusVolumes() ([]float32, error) {
	volumes := make([]float32, len(c.state.Buses))
	for i := range c.state.Buses {
		volumes[i] = c.state.Buses[i].Volume
	}
	return volumes, nil
}

func (c *FakeCtl) GetPhantoms() ([]bool, error) {
	phantoms := make([]bool, len(c.state.Channels))
	for i := range c.state.Channels {
		phantoms[i] = c.state.Channels[i].Phantom
	}
	return phantoms, nil

}

func (c *FakeCtl) GetMatrix() ([]ctl.SendState, error) {
	result := make([]ctl.SendState, len(c.state.Channels)*len(c.state.Buses))
	for i := range c.state.Channels {
		for j := range c.state.Channels[i].Sends {
			result[i*len(c.state.Buses)+j] = c.state.Channels[i].Sends[j]
		}
	}
	return result, nil
}

func (c *FakeCtl) SetFullState(state *ctl.MixerState) error {
	if len(state.Channels) != len(c.state.Channels) {
		return fmt.Errorf("trying to change number of channels")
	}
	if len(state.Buses) != len(c.state.Buses) {
		return fmt.Errorf("trying to change number of buses")
	}
	for i := range state.Channels {
		if len(state.Channels[i].Sends) != len(state.Buses) {
			return fmt.Errorf("matrix is not rectangular")
		}
	}
	c.state = state
	return nil
}

func (c *FakeCtl) SetMatrixSend(ch uint8, bus uint8, unmuted bool) error {
	if int(ch) >= len(c.state.Channels) || int(bus) > len(c.state.Buses) {
		return fmt.Errorf("malformed input")
	}

	c.state.Channels[ch].Sends[bus].Unmuted = unmuted
	return nil
}

func (c *FakeCtl) SetMatrixVolume(ch uint8, bus uint8, volume float32) error {
	if int(ch) >= len(c.state.Channels) || int(bus) > len(c.state.Buses) {
		return fmt.Errorf("malformed input")
	}

	c.state.Channels[ch].Sends[bus].Volume = volume
	return nil
}

func (c *FakeCtl) SetInGain(ch uint8, gain float32) error {
	if int(ch) >= len(c.state.Channels) {
		return fmt.Errorf("malformed input")
	}

	c.state.Channels[ch].Gain = gain
	return nil
}

func (c *FakeCtl) SetPhantom(ch uint8, phantom bool) error {
	if int(ch) >= len(c.state.Channels) {
		return fmt.Errorf("malformed input")
	}

	c.state.Channels[ch].Phantom = phantom
	return nil
}

func (c *FakeCtl) SetBusVolume(bus uint8, volume float32) error {
	if int(bus) > len(c.state.Buses) {
		return fmt.Errorf("malformed input")
	}

	c.state.Buses[bus].Volume = volume
	return nil
}

func (c *FakeCtl) FactoryReset() error {
	c.state = DefaultState
	return nil
}

func (c *FakeCtl) Loop() error {
	lowThresholds := make([]float32, len(c.state.Channels))
	highThresholds := make([]float32, len(c.state.Channels))
	diffs := make([]float32, len(c.state.Channels))
	inputLevels := make([]float32, len(c.state.Channels))

	for {
		time.Sleep(10 * time.Millisecond)

		for i := range diffs {
			if diffs[i] == 0 {
				diffs[i] = -0.4 - rand.Float32()
			}

			if diffs[i] > 0 && inputLevels[i] > highThresholds[i] {
				diffs[i] = -0.4 - rand.Float32()
				highThresholds[i] = -20*rand.Float32() + 4
			}
			if diffs[i] < 0 && inputLevels[i] < lowThresholds[i] {
				diffs[i] = 0.4 + rand.Float32()
				lowThresholds[i] = -50*rand.Float32() - 70
			}

			inputLevels[i] += diffs[i]

			c.levels.RMS.Input[i] = inputLevels[i] + c.state.Channels[i].Gain
			if c.levels.RMS.Input[i] < -120 {
				c.levels.RMS.Input[i] = -120
			}
		}

		for j := range c.levels.RMS.Bus {
			busLevel := float32(0)
			for i := range c.levels.RMS.Input {
				mul := float32(0)
				send := c.state.Channels[i].Sends[j]
				if send.Unmuted {
					gain_dB := send.Volume + c.state.Buses[j].Volume
					mul = coef_from_dB(gain_dB)
				}
				busLevel += coef_from_dB(c.levels.RMS.Input[i]) * mul
			}
			busLevel_dB := coef_to_dB(busLevel)
			if busLevel_dB < -120 {
				busLevel_dB = -120
			}
			c.levels.RMS.Bus[j] = busLevel_dB
		}

		for i := range c.levels.RMS.Input {
			// TODO: prettier fake peaks and smooths
			c.levels.Peak.Input[i] = c.levels.RMS.Input[i]
			c.levels.Smooth.Input[i] = c.levels.RMS.Input[i]
		}
		for i := range c.levels.RMS.Bus {
			// TODO: prettier fake peaks and smooths
			c.levels.Peak.Bus[i] = c.levels.RMS.Bus[i]
			c.levels.Smooth.Bus[i] = c.levels.RMS.Bus[i]
		}
	}
}

func (c *FakeCtl) RawCmd(argstr string) (string, error) {
	return "", fmt.Errorf("this API does not support raw input (unlike yo mama)")
}

func (c *FakeCtl) GetLevels() (*ctl.Levels, error) {
	return &ctl.Levels{
		RMS: ctl.LevelsBlock{
			Input: c.copyLevels(c.levels.RMS.Input),
			Bus:   c.copyLevels(c.levels.RMS.Bus),
		},
		Peak: ctl.LevelsBlock{
			Input: c.copyLevels(c.levels.Peak.Input),
			Bus:   c.copyLevels(c.levels.Peak.Bus),
		},
		Smooth: ctl.LevelsBlock{
			Input: c.copyLevels(c.levels.Smooth.Input),
			Bus:   c.copyLevels(c.levels.Smooth.Bus),
		},
	}, nil
}

func (c *FakeCtl) copyLevels(levels []float32) []float32 {
	result := make([]float32, len(levels))
	for i := range result {
		if *c.cfg.UseDBLevels {
			result[i] = levels[i]
		} else {
			panic("not implemented: calculate linear from db")
		}
	}
	return result
}

func coef_to_dB(x float32) float32 {
	return 20.0 * float32(math.Log10(float64(x)))
}

func coef_from_dB(db float32) float32 {
	return float32(math.Pow(10, float64(db/20.0)))
}
