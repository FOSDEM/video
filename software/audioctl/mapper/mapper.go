package mapper

import (
	"log/slog"

	"github.com/goccy/go-json"
)

type MapperState struct {
	Channels []ChannelState `json:"channels"`
	Buses    []ctl.BusState `json:"buses"`
}

type ChannelState struct {
	ctl.ChannelCfg
	MasterFader   float32     `json:"master_fader"`
	MasterUnmuted float32     `json:"master_unmuted"`
	Sends         []SendState `json:"sends"`
}

type SendState struct {
	Unmuted         bool    `json:"unmuted"`
	Volume          float32 `json:"volume"`
	PreChannelFader bool    `json:"pre_channel_fader"`
	PreChannelMute  bool    `json:"pre_channel_mute"`
}

type storedInfo struct {
	// bitmasks for the sends of each channel
	PreChannelFaders []uint16 `json:"pcf"`
	PreChannelMutes  []uint16 `json:"pcm"`

	// channel masters
	MasterFaders  []float32 `json:"mf"`
	MasterUnmuted []float32 `json:"mu"`
}

func (s *storedInfo) Resize(numChans int) {
	// TODO: resize all arrays
}

func (s *storedInfo) Blobify() string {
	return string(json.Marshal(s))
}

func FromMixerState(mixerstate *MixerState, logger *slog.Logger) {
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

			send.PreChannelFader = storedinfo.PreChannelFader(i, j)
			send.PreChannelMute = storedinfo.PreChannelMute(i, j)
			send.Unmuted = mixersend.Unmuted
			// FIXME: handle PreChannelMute properly - do we also need to store mute data in the blob?
			send.Volume = mixersend.Volume
			if !mixersend.PreChannelFader {
				send.Volume -= mixerchannel.MasterFader
			}
		}
	}
}

func (m *MapperState) ToMixerState() *ctl.MixerState {
	channels := make([]ctl.ChannelState, len(m.Channels))

	storedinfo := &storedInfo{}
	storedinfo.Resize(len(m.Channels))

	for i := range channels {
		mapperchannel := m.Channels[i]
		sends := make([]ctl.SendState, len(mapperchannel.Sends))
		for j := range sends {
			mappersend := m.Channels[i].Sends[j]
			send := &sends[j]

			send.Unmuted = mappersend.Unmuted
			if !mappersend.PreChannelMute && !mapperchannel.MasterUnmuted {
				send.Unmuted = false
			}
			send.Volume = mappersend.Volume
			if !mappersend.PreChannelFader {
				send.Volume += mapperchannel.MasterFader
			}

			storedinfo.SetPreChannelMute(i, j, mappersend.PreChannelMute)
			storedinfo.SetPreChannelFader(i, j, mappersend.PreChannelFader)
			storedinfo.SetMasterFader(i, mapperchannel.MasterFader)
			storedinfo.SetMasterUnmuted(i, mapperchannel.MasterUnmuted)
		}
	}

	return &ctl.MixerState{
		Channels: channels,
		Buses:    h.state.Buses,
		Blob:     storedinfo.Blobify(),
	}
}
