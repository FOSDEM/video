package mapper

import (
	"fmt"
	"log/slog"
	"sync"

	"github.com/fosdem/video/software/audioctl/ctl"
)

type Mapper struct {
	ctl    ctl.Ctl
	logger *slog.Logger
	mutex  sync.Mutex
	state  *MapperState
}

func New(ctl ctl.Ctl, logger *slog.Logger) *Mapper {
	return &Mapper{ctl: ctl, logger: logger}
}

type ForceGet bool

const (
	Force ForceGet = true
	Lazy  ForceGet = false
)

func (m *Mapper) GetFullState(force ForceGet) (*MapperState, error) {
	m.mutex.Lock()
	defer m.mutex.Unlock()

	return m.getFullState(force)
}

func (m *Mapper) getFullState(force ForceGet) (*MapperState, error) {
	if bool(force) || m.state == nil {
		mixerstate, err := m.ctl.GetFullState()
		if err != nil {
			m.state = nil
			return nil, err
		}

		m.state = BuildMapperState(mixerstate, m.logger)
	}

	return m.state.Copy(), nil
}

func (m *Mapper) SetFullState(state *MapperState) error {
	m.mutex.Lock()
	defer m.mutex.Unlock()

	return m.setFullState(state.Copy())
}

func (m *Mapper) setFullState(state *MapperState) error {
	// TODO: diff the states and send commands for only what changed
	err := m.ctl.SetFullState(state.ToMixerState())
	if err != nil {
		m.state = nil // force refresh after state set fails
		return err
	}

	m.state = state

	return nil
}

func (m *Mapper) apply(f func(*MapperState) error) error {
	m.mutex.Lock()
	defer m.mutex.Unlock()

	state, err := m.getFullState(Lazy)
	if err != nil {
		return err
	}

	if err := f(state); err != nil {
		return err
	}

	return m.setFullState(state)
}

func checkChannel(s *MapperState, ch uint8) error {
	if int(ch) >= len(s.Channels) {
		return fmt.Errorf("no such channel: %d", ch)
	}
	return nil
}

func checkBus(s *MapperState, bus uint8) error {
	if int(bus) >= len(s.Buses) {
		return fmt.Errorf("no such bus: %d", bus)
	}
	return nil
}

func checkSend(s *MapperState, ch, bus uint8) error {
	if err := checkChannel(s, ch); err != nil {
		return err
	}
	if int(bus) >= len(s.Channels[ch].Sends) {
		return fmt.Errorf("no such send: %d.%d", ch, bus)
	}
	return nil
}

func (m *Mapper) SetMatrixSend(ch, bus uint8, unmuted bool) error {
	return m.apply(func(s *MapperState) error {
		if err := checkSend(s, ch, bus); err != nil {
			return err
		}
		s.Channels[ch].Sends[bus].Unmuted = unmuted
		return nil
	})
}

func (m *Mapper) SetMatrixVolume(ch, bus uint8, volume float32) error {
	return m.apply(func(s *MapperState) error {
		if err := checkSend(s, ch, bus); err != nil {
			return err
		}
		s.Channels[ch].Sends[bus].Volume = volume
		return nil
	})
}

func (m *Mapper) SetSendPreMasterFader(ch, bus uint8, v bool) error {
	return m.apply(func(s *MapperState) error {
		if err := checkSend(s, ch, bus); err != nil {
			return err
		}
		s.Channels[ch].Sends[bus].PreMasterFader = v
		return nil
	})
}

func (m *Mapper) SetSendPreMasterMute(ch, bus uint8, v bool) error {
	return m.apply(func(s *MapperState) error {
		if err := checkSend(s, ch, bus); err != nil {
			return err
		}
		s.Channels[ch].Sends[bus].PreMasterMute = v
		return nil
	})
}

func (m *Mapper) SetChannelMasterFader(ch uint8, v float32) error {
	return m.apply(func(s *MapperState) error {
		if err := checkChannel(s, ch); err != nil {
			return err
		}
		s.Channels[ch].MasterFader = v
		return nil
	})
}

func (m *Mapper) SetChannelMasterUnmuted(ch uint8, v bool) error {
	return m.apply(func(s *MapperState) error {
		if err := checkChannel(s, ch); err != nil {
			return err
		}
		s.Channels[ch].MasterUnmuted = v
		return nil
	})
}

func (m *Mapper) SetBusMasterFader(bus uint8, v float32) error {
	return m.apply(func(s *MapperState) error {
		if err := checkBus(s, bus); err != nil {
			return err
		}
		s.Buses[bus].MasterFader = v
		return nil
	})
}

func (m *Mapper) SetBusMasterUnmuted(bus uint8, v bool) error {
	return m.apply(func(s *MapperState) error {
		if err := checkBus(s, bus); err != nil {
			return err
		}
		s.Buses[bus].MasterUnmuted = v
		return nil
	})
}

func (m *Mapper) SetInGain(ch uint8, gain float32) error {
	return m.apply(func(s *MapperState) error {
		if err := checkChannel(s, ch); err != nil {
			return err
		}
		s.Channels[ch].Gain = gain
		return nil
	})
}

func (m *Mapper) SetPhantom(ch uint8, phantom bool) error {
	return m.apply(func(s *MapperState) error {
		if err := checkChannel(s, ch); err != nil {
			return err
		}
		s.Channels[ch].Phantom = phantom
		return nil
	})
}

func (m *Mapper) SetInputEQBand(ch, band, shape uint8, frequency, gain, q float32) error {
	return m.apply(func(s *MapperState) error {
		if err := checkChannel(s, ch); err != nil {
			return err
		}
		channel := &s.Channels[ch]
		for len(channel.Eq) <= int(band) {
			channel.Eq = append(channel.Eq, ctl.EqBand{})
		}
		channel.Eq[band] = ctl.EqBand{
			Type:      int(shape),
			Frequency: frequency,
			Gain:      gain,
			Q:         q,
		}
		return nil
	})
}

func (m *Mapper) FactoryReset() error {
	m.mutex.Lock()
	defer m.mutex.Unlock()

	m.state = nil
	return m.ctl.FactoryReset()
}

func (m *Mapper) RawCmd(argstr string) (string, error) {
	m.mutex.Lock()
	defer m.mutex.Unlock()

	m.state = nil // a raw command can change anything
	return m.ctl.RawCmd(argstr)
}

func (m *Mapper) GetLevels() (*ctl.Levels, error) {
	return m.ctl.GetLevels()
}
