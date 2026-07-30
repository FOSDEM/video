package mapper

import (
	"log/slog"

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

func BuildMapperState(mixerstate *ctl.MixerState, logger *slog.Logger) *MapperState {
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
