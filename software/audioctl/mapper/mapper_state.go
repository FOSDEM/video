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

func MapperStateEqual(x, y *MapperState) bool {
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
		a.MasterFader != b.MasterFader ||
		a.MasterUnmuted != b.MasterUnmuted ||
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
		a.MasterFader == b.MasterFader &&
		a.MasterUnmuted == b.MasterUnmuted
}

func SendStateEqual(a, b *SendState) bool {
	return a.Unmuted == b.Unmuted &&
		a.Volume == b.Volume &&
		a.PreMasterFader == b.PreMasterFader &&
		a.PreMasterMute == b.PreMasterMute
}

func (m *MapperState) Copy() *MapperState {
	out := &MapperState{
		Channels: make([]ChannelState, len(m.Channels)),
		Buses:    make([]BusState, len(m.Buses)),
	}

	copy(out.Buses, m.Buses)

	for i := range m.Channels {
		src := &m.Channels[i]
		dst := &out.Channels[i]

		dst.ChannelCfg = src.ChannelCfg
		dst.Eq = append([]ctl.EqBand(nil), src.Eq...)
		dst.MasterFader = src.MasterFader
		dst.MasterUnmuted = src.MasterUnmuted
		dst.Sends = append([]SendState(nil), src.Sends...)
	}

	return out
}

func BuildMapperState(mixerstate *ctl.MixerState, logger *slog.Logger) *MapperState {
	m := &MapperState{
		Channels: make([]ChannelState, len(mixerstate.Channels)),
		Buses:    make([]BusState, len(mixerstate.Buses)),
	}

	storedinfo, err := deblobifyStoredInfo(mixerstate.Blob)
	if err != nil && logger != nil {
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
