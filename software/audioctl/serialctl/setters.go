package serialctl

import (
	"fmt"

	"github.com/fosdem/video/software/audioctl/ctl"
)

func (c *SerialCtl) SetFullState(state *ctl.MixerState) error {
	var err error
	for i := range c.numChans {
		for j := range c.numBuses {
			err = c.SetMatrixSend(uint8(i), uint8(j), state.Channels[i].Sends[j].Unmuted)
			if err != nil {
				return fmt.Errorf("could not set send %dx%d: %w", i, j, err)
			}
			err = c.SetMatrixVolume(uint8(i), uint8(j), state.Channels[i].Sends[j].Volume)
			if err != nil {
				return fmt.Errorf("could not set volume %dx%d: %w", i, j, err)
			}
		}

		err = c.SetInGain(uint8(i), state.Channels[i].Gain)
		if err != nil {
			return fmt.Errorf("could not set gain on channel %d: %w", i, err)
		}

		err = c.SetPhantom(uint8(i), state.Channels[i].Phantom)
		if err != nil {
			return fmt.Errorf("could not set phantom on channel %d: %w", i, err)
		}
	}

	for j := range c.numBuses {
		err = c.SetBusVolume(uint8(j), state.Buses[j].Volume)
		if err != nil {
			return fmt.Errorf("could not set volume on bus %d: %w", j, err)
		}
	}

	return nil
}

func (c *SerialCtl) SetMatrixSend(ch uint8, bus uint8, unmuted bool) error {
	if int(ch) >= c.numChans || int(bus) > c.numBuses {
		return fmt.Errorf("malformed input")
	}

	_, err := c.RawCmd(fmt.Sprintf("send.set %d %d %d", ch, bus, boolToNum(unmuted)))
	if err != nil {
		return err
	}

	return nil
}

func (c *SerialCtl) SetMatrixVolume(ch uint8, bus uint8, volume float32) error {
	if int(ch) >= c.numChans || int(bus) > c.numBuses {
		return fmt.Errorf("malformed input")
	}

	_, err := c.RawCmd(fmt.Sprintf("volume.set %d %d %.4f", ch, bus, volume))
	if err != nil {
		return err
	}

	return nil
}

func (c *SerialCtl) SetInGain(ch uint8, gain float32) error {
	if int(ch) >= c.numChans {
		return fmt.Errorf("malformed input")
	}

	_, err := c.RawCmd(fmt.Sprintf("in-gain.set %d %.4f", ch, gain))
	if err != nil {
		return err
	}

	return nil
}

func (c *SerialCtl) SetPhantom(ch uint8, phantom bool) error {
	if int(ch) >= c.numChans {
		return fmt.Errorf("malformed input")
	}

	_, err := c.RawCmd(fmt.Sprintf("phantom.set %d %d", ch, boolToNum(phantom)))
	if err != nil {
		return err
	}

	return nil
}

func (c *SerialCtl) SetBusVolume(bus uint8, volume float32) error {
	if int(bus) > c.numBuses {
		return fmt.Errorf("malformed input")
	}

	_, err := c.RawCmd(fmt.Sprintf("bus-volume.set %d %.4f", bus, volume))
	if err != nil {
		return err
	}

	return nil
}

func (c *SerialCtl) FactoryReset() error {
	_, err := c.RawCmd("factory-reset")
	if err != nil {
		return err
	}

	return nil
}

func boolToNum(b bool) int {
	if b {
		return 1
	} else {
		return 0
	}
}
