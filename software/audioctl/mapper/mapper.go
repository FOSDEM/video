package mapper

import (
	"fmt"
	"log/slog"

	"github.com/goccy/go-json"

	"github.com/fosdem/video/software/audioctl/ctl"
)

type MapperState struct {
	Channels []ChannelState `json:"channels"`
	Buses    []ctl.BusState `json:"buses"`
}

type ChannelState struct {
	ctl.ChannelCfg `json:",inline"`
	MasterFader    float32     `json:"master_fader"`
	MasterUnmuted  bool        `json:"master_unmuted"`
	Sends          []SendState `json:"sends"`
}

type SendState struct {
	Unmuted        bool    `json:"unmuted"`
	Volume         float32 `json:"volume"`
	PreMasterFader bool    `json:"pre_channel_fader"`
	PreMasterMute  bool    `json:"pre_channel_mute"`
}

type storedInfo struct {
	PreMasterFaders []uint16  `json:"pcf"`
	PreMasterMutes  []uint16  `json:"pcm"`
	MasterFaders    []float32 `json:"mf"`
	MasterUnmuted   []bool    `json:"mu"`
}

func FromMixerState(mixerstate *ctl.MixerState, logger *slog.Logger) *MapperState {
	m := &MapperState{
		Channels: make([]ChannelState, len(mixerstate.Channels)),
		Buses:    mixerstate.Buses,
	}

	storedinfo, err := deblobifyStoredInfo(mixerstate.Blob)
	if err != nil {
		logger.Error("cannot decode stored blob, reinitialising", "err", err)
	}

	storedinfo.Resize(len(mixerstate.Channels))

	for i := range mixerstate.Channels {
		channel := &m.Channels[i]
		mixerchannel := &mixerstate.Channels[i]

		channel.ChannelCfg = mixerchannel.ChannelCfg
		channel.MasterFader = storedinfo.MasterFaders[i]
		channel.MasterUnmuted = storedinfo.MasterUnmuted[i]

		for j := range mixerchannel.Sends {
			if len(channel.Sends) <= j {
				channel.Sends = append(channel.Sends, SendState{})
			}
			send := &channel.Sends[j]
			mixersend := &mixerchannel.Sends[j]

			send.PreMasterFader = storedinfo.PreMasterFader(i, j)
			send.PreMasterMute = storedinfo.PreMasterMute(i, j)
			send.Unmuted = mixersend.Unmuted
			send.Volume = mixersend.Volume
			if !send.PreMasterFader {
				send.Volume -= channel.MasterFader
			}
		}
	}

	return m
}

func (m *MapperState) ToMixerState() *ctl.MixerState {
	channels := make([]ctl.ChannelState, len(m.Channels))

	storedinfo := &storedInfo{}
	storedinfo.Resize(len(m.Channels))

	for i := range channels {
		mapperchannel := &m.Channels[i]
		channels[i].ChannelCfg = mapperchannel.ChannelCfg
		sends := make([]ctl.SendState, len(mapperchannel.Sends))

		storedinfo.SetMasterFader(i, mapperchannel.MasterFader)
		storedinfo.SetMasterUnmuted(i, mapperchannel.MasterUnmuted)

		for j := range sends {
			mappersend := &mapperchannel.Sends[j]
			send := &sends[j]

			send.Unmuted = mappersend.Unmuted
			if !mappersend.PreMasterMute && !mapperchannel.MasterUnmuted {
				send.Unmuted = false
			}
			send.Volume = mappersend.Volume
			if !mappersend.PreMasterFader {
				send.Volume += mapperchannel.MasterFader
			}

			storedinfo.SetPreMasterMute(i, j, mappersend.PreMasterMute)
			storedinfo.SetPreMasterFader(i, j, mappersend.PreMasterFader)
		}

		channels[i].Sends = sends
	}

	return &ctl.MixerState{
		Channels: channels,
		Buses:    m.Buses,
		Blob:     storedinfo.Blobify(),
	}
}

func (s *storedInfo) Resize(numChans int) {
	s.PreMasterFaders = resizeSlice(s.PreMasterFaders, numChans)
	s.PreMasterMutes = resizeSlice(s.PreMasterMutes, numChans)
	s.MasterFaders = resizeSlice(s.MasterFaders, numChans)
	s.MasterUnmuted = resizeSlice(s.MasterUnmuted, numChans)
}

func (s *storedInfo) PreMasterFader(ch, send int) bool {
	return s.PreMasterFaders[ch]&(1<<uint(send)) != 0
}

func (s *storedInfo) PreMasterMute(ch, send int) bool {
	return s.PreMasterMutes[ch]&(1<<uint(send)) != 0
}

func (s *storedInfo) SetPreMasterFader(ch, send int, v bool) {
	s.PreMasterFaders[ch] = setBit(s.PreMasterFaders[ch], send, v)
}

func (s *storedInfo) SetPreMasterMute(ch, send int, v bool) {
	s.PreMasterMutes[ch] = setBit(s.PreMasterMutes[ch], send, v)
}

func (s *storedInfo) SetMasterFader(ch int, v float32) {
	s.MasterFaders[ch] = v
}

func (s *storedInfo) SetMasterUnmuted(ch int, v bool) {
	s.MasterUnmuted[ch] = v
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

func resizeSlice[T any](s []T, n int) []T {
	if len(s) == n {
		return s
	}
	out := make([]T, n)
	copy(out, s)
	return out
}
