package ctl

import "fmt"

type MixerState struct {
	Channels []ChannelState `json:"channels"`
	Buses    []BusState     `json:"buses"`
	Blob     string         `json:"blob"`
}

type ChannelCfg struct {
	Name    string   `json:"name"`  // descriptive name of the input
	Label   string   `json:"label"` // short label
	Gain    float32  `json:"gain"`  // input gain in dB (0 means identity)
	Phantom bool     `json:"phantom"`
	Eq      []EqBand `json:"eq"`
}

type ChannelState struct {
	ChannelCfg `json:",inline"`
	Sends      []SendState `json:"sends"`
}

type BusCfg struct {
	Name  string `json:"name"`  // descriptive name of the output bus
	Label string `json:"label"` // short label
}

type BusState struct {
	BusCfg `json:",inline"`
}

type SendState struct {
	Unmuted bool    `json:"unmuted"`
	Volume  float32 `json:"volume"` // crosspoint volume in dB (0 means identity)
}

type EqBand struct {
	Type      int     `json:"type"`
	Frequency float32 `json:"frequency"`
	Gain      float32 `json:"gain"`
	Q         float32 `json:"q"`
}

type Levels struct {
	RMS    LevelsBlock `json:"rms"`
	Peak   LevelsBlock `json:"peak"`
	Smooth LevelsBlock `json:"smooth"`
}

type LevelsBlock struct {
	Input []float32 `json:"inputs"`
	Bus   []float32 `json:"buses"`
}

func MixerStateEqual(x, y *MixerState) bool {
	if x == nil || y == nil {
		return x == y
	}

	if x.Blob != y.Blob {
		return false
	}

	if len(x.Channels) != len(y.Channels) ||
		len(x.Buses) != len(y.Buses) {
		return false
	}

	for i := range x.Channels {
		if !ChannelStateEqual(&x.Channels[i], &y.Channels[i]) {
			return false
		}
	}

	for i := range x.Buses {
		if !BusStateEqual(&x.Buses[i], &y.Buses[i]) {
			return false
		}
	}

	return true
}

func ChannelStateEqual(a, b *ChannelState) bool {
	if a.Name != b.Name ||
		a.Label != b.Label ||
		a.Gain != b.Gain ||
		a.Phantom != b.Phantom ||
		len(a.Sends) != len(b.Sends) {
		return false
	}

	for i := range a.Sends {
		if !SendStateEqual(&a.Sends[i], &b.Sends[i]) {
			return false
		}
	}

	return true
}

func BusStateEqual(a, b *BusState) bool {
	return a.Name == b.Name &&
		a.Label == b.Label
}

func SendStateEqual(a, b *SendState) bool {
	return a.Unmuted == b.Unmuted &&
		a.Volume == b.Volume
}

func (m *MixerState) Copy() *MixerState {
	copyMixerState := &MixerState{
		Channels: make([]ChannelState, len(m.Channels)),
		Buses:    make([]BusState, len(m.Buses)),
	}

	for i, channel := range m.Channels {
		copyMixerState.Channels[i] = *channel.Copy()
	}

	for i, bus := range m.Buses {
		copyMixerState.Buses[i] = bus.Copy()
	}

	return copyMixerState
}

func (c *ChannelState) Copy() *ChannelState {
	copyChannel := &ChannelState{
		ChannelCfg: ChannelCfg{
			Name:    c.Name,
			Label:   c.Label,
			Gain:    c.Gain,
			Phantom: c.Phantom,
			Eq:      make([]EqBand, len(c.Eq)),
		},
		Sends: make([]SendState, len(c.Sends)),
	}

	for i, band := range c.Eq {
		copyChannel.Eq[i] = band.Copy()
	}

	for i, send := range c.Sends {
		copyChannel.Sends[i] = send.Copy()
	}

	return copyChannel
}

func (b *BusState) Copy() BusState {
	return BusState{
		BusCfg: BusCfg{
			Name:  b.Name,
			Label: b.Label,
		},
	}
}

func (b *EqBand) Copy() EqBand {
	return EqBand{
		Type:      b.Type,
		Frequency: b.Frequency,
		Gain:      b.Gain,
		Q:         b.Q,
	}
}

func (b EqBand) ShapeToInt(shape string) (int, error) {
	switch shape {
	case "allpass":
		return 0, nil
	case "lowpass":
		return 1, nil
	case "highpass":
		return 2, nil
	default:
		return 0, fmt.Errorf("invalid shape: %s", shape)
	}
}

func (s *SendState) Copy() SendState {
	return SendState{
		Unmuted: s.Unmuted,
		Volume:  s.Volume,
	}
}

func (l *Levels) Copy() Levels {
	return Levels{
		RMS:    l.RMS.Copy(),
		Peak:   l.Peak.Copy(),
		Smooth: l.Smooth.Copy(),
	}
}

func (lb *LevelsBlock) Copy() LevelsBlock {
	return LevelsBlock{
		Input: append([]float32(nil), lb.Input...),
		Bus:   append([]float32(nil), lb.Bus...),
	}
}
