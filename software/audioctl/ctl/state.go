package ctl

type MixerState struct {
	Channels []ChannelState `json:"channels"`
	Buses    []BusState     `json:"buses"`
}

type ChannelState struct {
	Name    string      `json:"name"`  // descriptive name of the input
	Label   string      `json:"label"` // short label
	Gain    float32     `json:"gain"`  // input gain in dB (0 means identity)
	Phantom bool        `json:"phantom"`
	Sends   []SendState `json:"sends"`
}

type BusState struct {
	Name   string  `json:"name"`   // descriptive name of the output bus
	Label  string  `json:"label"`  // short label
	Volume float32 `json:"volume"` // volume in dB (0 means identity)
}

type SendState struct {
	Unmuted bool    `json:"unmuted"`
	Volume  float32 `json:"volume"` // crosspoint volume in dB (0 means identity)
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
		a.Label == b.Label &&
		a.Volume == b.Volume
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
		Name:    c.Name,
		Label:   c.Label,
		Gain:    c.Gain,
		Phantom: c.Phantom,
		Sends:   make([]SendState, len(c.Sends)),
	}

	for i, send := range c.Sends {
		copyChannel.Sends[i] = send.Copy()
	}

	return copyChannel
}

func (b *BusState) Copy() BusState {
	return BusState{
		Name:   b.Name,
		Label:  b.Label,
		Volume: b.Volume,
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
