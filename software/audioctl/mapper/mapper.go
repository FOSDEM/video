package mapper

import (
	"fmt"
	"log/slog"

	"github.com/goccy/go-json"

	"github.com/fosdem/video/software/audioctl/ctl"
)

type MapperState struct {
	Channels []ChannelState `json:"channels"`
	Buses    []BusState     `json:"buses"`
}

type ChannelState struct {
	ctl.ChannelCfg `json:",inline"`
	MasterFader    float32     `json:"master_fader"`
	MasterUnmuted  bool        `json:"master_unmuted"`
	Sends          []SendState `json:"sends"`
}

type BusState struct {
	ctl.BusCfg    `json:",inline"`
	MasterFader   float32 `json:"master_fader"`
	MasterUnmuted bool    `json:"master_unmuted"`
}

type SendState struct {
	Unmuted        bool    `json:"unmuted"`
	Volume         float32 `json:"volume"`
	PreMasterFader bool    `json:"pre_channel_fader"`
	PreMasterMute  bool    `json:"pre_channel_mute"`
}

type storedInfo struct {
	PreMasterFaders []uint16 `json:"pcf"`
	PreMasterMutes  []uint16 `json:"pcm"`
	Unmutes         []uint16 `json:"u"`

	ChannelMasterFaders   []float32 `json:"cmf"`
	ChannelMasterUnmuteds uint16    `json:"cmu"`

	BusMasterFaders   []float32 `json:"bmf"`
	BusMasterUnmuteds uint16    `json:"bmu"`
}

func FromMixerState(mixerstate *ctl.MixerState, logger *slog.Logger) *MapperState {
	m := &MapperState{
		Channels: make([]ChannelState, len(mixerstate.Channels)),
		Buses:    make([]BusState, len(mixerstate.Buses)),
	}

	storedinfo, err := deblobifyStoredInfo(mixerstate.Blob)
	if err != nil {
		logger.Error("cannot decode stored blob, reinitialising", "err", err)
	}

	storedinfo.Resize(len(mixerstate.Channels), len(mixerstate.Buses))

	for j := range mixerstate.Buses {
		bus := &m.Buses[j]
		bus.BusCfg = mixerstate.Buses[j].BusCfg
		bus.MasterFader = storedinfo.BusMasterFaders[j]
		bus.MasterUnmuted = storedinfo.BusMasterUnmuted(j)
	}

	for i := range mixerstate.Channels {
		channel := &m.Channels[i]
		mixerchannel := &mixerstate.Channels[i]

		channel.ChannelCfg = mixerchannel.ChannelCfg
		channel.MasterFader = storedinfo.ChannelMasterFaders[i]
		channel.MasterUnmuted = storedinfo.ChannelMasterUnmuted(i)

		for j := range mixerchannel.Sends {
			if len(channel.Sends) <= j {
				channel.Sends = append(channel.Sends, SendState{})
			}
			send := &channel.Sends[j]
			mixersend := &mixerchannel.Sends[j]

			send.PreMasterFader = storedinfo.PreMasterFader(i, j)
			send.PreMasterMute = storedinfo.PreMasterMute(i, j)
			send.Unmuted = storedinfo.Unmuted(i, j)
			send.Volume = mixersend.Volume
			if !send.PreMasterFader {
				send.Volume -= channel.MasterFader
			}
			send.Volume -= m.Buses[j].MasterFader
		}
	}

	return m
}

func (m *MapperState) ToMixerState() *ctl.MixerState {
	channels := make([]ctl.ChannelState, len(m.Channels))
	buses := make([]ctl.BusState, len(m.Buses))

	storedinfo := &storedInfo{}
	storedinfo.Resize(len(m.Channels), len(m.Buses))

	for j := range m.Buses {
		buses[j].BusCfg = m.Buses[j].BusCfg
		storedinfo.SetBusMasterFader(j, m.Buses[j].MasterFader)
		storedinfo.SetBusMasterUnmuted(j, m.Buses[j].MasterUnmuted)
	}

	for i := range channels {
		mapperchannel := &m.Channels[i]
		channels[i].ChannelCfg = mapperchannel.ChannelCfg
		sends := make([]ctl.SendState, len(mapperchannel.Sends))

		storedinfo.SetChannelMasterFader(i, mapperchannel.MasterFader)
		storedinfo.SetChannelMasterUnmuted(i, mapperchannel.MasterUnmuted)

		for j := range sends {
			mappersend := &mapperchannel.Sends[j]
			mapperbus := &m.Buses[j]
			send := &sends[j]

			send.Unmuted = mappersend.Unmuted && mapperbus.MasterUnmuted
			if !mappersend.PreMasterMute && !mapperchannel.MasterUnmuted {
				send.Unmuted = false
			}
			send.Volume = mappersend.Volume
			if !mappersend.PreMasterFader {
				send.Volume += mapperchannel.MasterFader
			}
			send.Volume += mapperbus.MasterFader

			storedinfo.SetPreMasterMute(i, j, mappersend.PreMasterMute)
			storedinfo.SetPreMasterFader(i, j, mappersend.PreMasterFader)
			storedinfo.SetUnmuted(i, j, mappersend.Unmuted)
		}

		channels[i].Sends = sends
	}

	return &ctl.MixerState{
		Channels: channels,
		Buses:    buses,
		Blob:     storedinfo.Blobify(),
	}
}

func (s *storedInfo) Resize(numChans int, numBuses int) {
	brandNew := len(s.ChannelMasterFaders) == 0

	falseMask := uint16(0)
	trueMask := ^uint16(0)

	s.PreMasterFaders = resizeSlice(s.PreMasterFaders, numChans, falseMask)
	s.PreMasterMutes = resizeSlice(s.PreMasterMutes, numChans, falseMask)
	s.Unmutes = resizeSlice(s.Unmutes, numChans, trueMask)
	s.ChannelMasterFaders = resizeSlice(s.ChannelMasterFaders, numChans, 0)
	s.BusMasterFaders = resizeSlice(s.BusMasterFaders, numBuses, 0)

	if brandNew {
		s.ChannelMasterUnmuteds = trueMask
		s.BusMasterUnmuteds = trueMask
	}
}

func (s *storedInfo) PreMasterFader(ch, send int) bool {
	return s.PreMasterFaders[ch]&(1<<uint(send)) != 0
}

func (s *storedInfo) PreMasterMute(ch, send int) bool {
	return s.PreMasterMutes[ch]&(1<<uint(send)) != 0
}

func (s *storedInfo) Unmuted(ch, send int) bool {
	return s.Unmutes[ch]&(1<<uint(send)) != 0
}

func (s *storedInfo) ChannelMasterUnmuted(ch int) bool {
	return s.ChannelMasterUnmuteds&(1<<uint(ch)) != 0
}

func (s *storedInfo) BusMasterUnmuted(bus int) bool {
	return s.BusMasterUnmuteds&(1<<uint(bus)) != 0
}

func (s *storedInfo) SetPreMasterFader(ch, send int, v bool) {
	s.PreMasterFaders[ch] = setBit(s.PreMasterFaders[ch], send, v)
}

func (s *storedInfo) SetPreMasterMute(ch, send int, v bool) {
	s.PreMasterMutes[ch] = setBit(s.PreMasterMutes[ch], send, v)
}

func (s *storedInfo) SetUnmuted(ch, send int, v bool) {
	s.Unmutes[ch] = setBit(s.Unmutes[ch], send, v)
}

func (s *storedInfo) SetChannelMasterFader(ch int, v float32) {
	s.ChannelMasterFaders[ch] = v
}

func (s *storedInfo) SetChannelMasterUnmuted(ch int, v bool) {
	s.ChannelMasterUnmuteds = setBit(s.ChannelMasterUnmuteds, ch, v)
}

func (s *storedInfo) SetBusMasterFader(bus int, v float32) {
	s.BusMasterFaders[bus] = v
}

func (s *storedInfo) SetBusMasterUnmuted(bus int, v bool) {
	s.BusMasterUnmuteds = setBit(s.BusMasterUnmuteds, bus, v)
}

func (s *storedInfo) Blobify() string {
	data, err := json.Marshal(s)
	if err != nil {
		panic(fmt.Sprintf("cannot blobify stored info: %s", err))
	}
	return string(data)
}

func deblobifyStoredInfo(blob string) (*storedInfo, error) {
	s := &storedInfo{}
	if blob == "" {
		return s, nil
	}
	if err := json.Unmarshal([]byte(blob), s); err != nil {
		return &storedInfo{}, err
	}
	return s, nil
}

func setBit(mask uint16, pos int, v bool) uint16 {
	if v {
		return mask | (1 << uint(pos))
	}
	return mask &^ (1 << uint(pos))
}

func resizeSlice[T any](s []T, n int, dflt T) []T {
	if n <= len(s) {
		return s[:n]
	}
	extras := make([]T, n-len(s))
	for i := range extras {
		extras[i] = dflt
	}
	return append(s, extras...)
}
