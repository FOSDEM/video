package api

import (
	"fmt"

	"github.com/fosdem/video/software/audioctl/ctl"
	"github.com/fosdem/video/software/audioctl/mapper"
)

func (a *Api) handleRawCmd(param string) (string, error) {
	resp, err := a.mapper.RawCmd(param)
	if err != nil {
		return "", err
	}
	return resp, nil
}

func (a *Api) handleSetFullState(param *mapper.MapperState) (string, error) {
	err := a.mapper.SetFullState(param)
	if err != nil {
		return "", err
	}
	a.forceRefresh()
	return "ok", nil
}

type SetMatrixVolumeParam struct {
	Chan     *uint8   `json:"channel,omitempty"`
	Bus      *uint8   `json:"bus,omitempty"`
	ChanName *string  `json:"channel_name,omitempty"`
	BusName  *string  `json:"bus_name,omitempty"`
	Volume   *float32 `json:"volume"`
}

func (a *Api) handleSetMatrixVolume(param SetMatrixVolumeParam) (string, error) {
	err := a.getChanByName(&param.Chan, param.ChanName)
	if err != nil {
		return "", err
	}
	err = a.getBusByName(&param.Bus, param.BusName)
	if err != nil {
		return "", err
	}

	if param.Chan == nil || param.Bus == nil || param.Volume == nil {
		return "", fmt.Errorf("missing fields (need channel, bus, unmuted)")
	}
	err = a.mapper.SetMatrixVolume(*param.Chan, *param.Bus, *param.Volume)
	if err != nil {
		return "", err
	}
	a.forceRefresh()
	return "ok", nil
}

type SetMatrixSendParam struct {
	Chan     *uint8  `json:"channel"`
	Bus      *uint8  `json:"bus"`
	ChanName *string `json:"channel_name"`
	BusName  *string `json:"bus_name"`
	Unmuted  *bool   `json:"unmuted"`
}

func (a *Api) handleSetMatrixSend(param SetMatrixSendParam) (string, error) {
	err := a.getChanByName(&param.Chan, param.ChanName)
	if err != nil {
		return "", err
	}
	err = a.getBusByName(&param.Bus, param.BusName)
	if err != nil {
		return "", err
	}

	if param.Chan == nil || param.Bus == nil || param.Unmuted == nil {
		return "", fmt.Errorf("missing fields (need channel, bus, volume)")
	}
	err = a.mapper.SetMatrixSend(*param.Chan, *param.Bus, *param.Unmuted)
	if err != nil {
		return "", err
	}
	a.forceRefresh()
	return "ok", nil
}

type SetInGainParam struct {
	Chan     *uint8   `json:"channel"`
	ChanName *string  `json:"channel_name"`
	Gain     *float32 `json:"gain"`
}

func (a *Api) handleSetInGain(param SetInGainParam) (string, error) {
	err := a.getChanByName(&param.Chan, param.ChanName)
	if err != nil {
		return "", err
	}

	if param.Chan == nil || param.Gain == nil {
		return "", fmt.Errorf("missing fields (need channel, gain)")
	}
	err = a.mapper.SetInGain(*param.Chan, *param.Gain)
	if err != nil {
		return "", err
	}
	a.forceRefresh()
	return "ok", nil
}

type SetInEqBandParam struct {
	Chan      *uint8   `json:"channel"`
	ChanName  *string  `json:"channel_name"`
	Band      *uint8   `json:"band"`
	Shape     *uint8   `json:"shape"`
	ShapeName *string  `json:"shape_name"`
	Frequency *float32 `json:"frequency"`
	Gain      *float32 `json:"gain"`
	Q         *float32 `json:"q"`
}

func (a *Api) handleSetInEqBand(param SetInEqBandParam) (string, error) {
	err := a.getChanByName(&param.Chan, param.ChanName)
	if err != nil {
		return "", err
	}
	if param.ShapeName != nil {
		shape, err := ctl.EqBand{}.ShapeToInt(*param.ShapeName)
		if err != nil {
			return "", err
		}
		shape8 := uint8(shape)
		param.Shape = &shape8
	}
	if param.Chan == nil || param.Band == nil || param.Shape == nil {
		return "", fmt.Errorf("missing fields (need channel, band, shape)")
	}
	err = a.mapper.SetInputEQBand(*param.Chan, *param.Band, *param.Shape, *param.Frequency, *param.Gain, *param.Q)
	if err != nil {
		return "", err
	}
	a.forceRefresh()
	return "ok", nil
}

type SetPhantomParam struct {
	Chan     *uint8  `json:"channel,omitempty"`
	ChanName *string `json:"channel_name,omitempty"`
	Phantom  *bool   `json:"phantom"`
}

func (a *Api) handleSetPhantom(param SetPhantomParam) (string, error) {
	err := a.getChanByName(&param.Chan, param.ChanName)
	if err != nil {
		return "", err
	}

	if param.Chan == nil || param.Phantom == nil {
		return "", fmt.Errorf("missing fields (need channel, phantom)")
	}
	err = a.mapper.SetPhantom(*param.Chan, *param.Phantom)
	if err != nil {
		return "", err
	}
	a.forceRefresh()
	return "ok", nil
}

type FactoryResetParam struct {
}

func (a *Api) handleFactoryReset(param FactoryResetParam) (string, error) {
	err := a.mapper.FactoryReset()
	if err != nil {
		return "", err
	}
	a.forceRefresh()
	return "ok", nil
}

type SetBusMasterFaderParam struct {
	Bus     *uint8   `json:"bus,omitempty"`
	BusName *string  `json:"bus_name,omitempty"`
	Fader   *float32 `json:"fader"`
}

func (a *Api) handleSetBusMasterFader(param SetBusMasterFaderParam) (string, error) {
	err := a.getBusByName(&param.Bus, param.BusName)
	if err != nil {
		return "", err
	}

	if param.Bus == nil || param.Fader == nil {
		return "", fmt.Errorf("missing fields (need bus, fader)")
	}

	err = a.mapper.SetBusMasterFader(*param.Bus, *param.Fader)
	if err != nil {
		return "", err
	}
	a.forceRefresh()
	return "ok", nil
}

type SetChannelMasterFaderParam struct {
	Chan     *uint8   `json:"channel,omitempty"`
	ChanName *string  `json:"channel_name,omitempty"`
	Fader    *float32 `json:"fader"`
}

func (a *Api) handleSetChannelMasterFader(param SetChannelMasterFaderParam) (string, error) {
	err := a.getChanByName(&param.Chan, param.ChanName)
	if err != nil {
		return "", err
	}

	if param.Chan == nil || param.Fader == nil {
		return "", fmt.Errorf("missing fields (need channel, fader)")
	}
	err = a.mapper.SetChannelMasterFader(*param.Chan, *param.Fader)
	if err != nil {
		return "", err
	}
	a.forceRefresh()
	return "ok", nil
}

type SetChannelMasterUnmutedParam struct {
	Chan     *uint8  `json:"channel,omitempty"`
	ChanName *string `json:"channel_name,omitempty"`
	Unmuted  *bool   `json:"unmuted"`
}

func (a *Api) handleSetChannelMasterUnmuted(param SetChannelMasterUnmutedParam) (string, error) {
	err := a.getChanByName(&param.Chan, param.ChanName)
	if err != nil {
		return "", err
	}

	if param.Chan == nil || param.Unmuted == nil {
		return "", fmt.Errorf("missing fields (need channel, unmuted)")
	}
	err = a.mapper.SetChannelMasterUnmuted(*param.Chan, *param.Unmuted)
	if err != nil {
		return "", err
	}
	a.forceRefresh()
	return "ok", nil
}

type SetBusMasterUnmutedParam struct {
	Bus     *uint8  `json:"bus,omitempty"`
	BusName *string `json:"bus_name,omitempty"`
	Unmuted *bool   `json:"unmuted"`
}

func (a *Api) handleSetBusMasterUnmuted(param SetBusMasterUnmutedParam) (string, error) {
	err := a.getBusByName(&param.Bus, param.BusName)
	if err != nil {
		return "", err
	}

	if param.Bus == nil || param.Unmuted == nil {
		return "", fmt.Errorf("missing fields (need bus, unmuted)")
	}
	err = a.mapper.SetBusMasterUnmuted(*param.Bus, *param.Unmuted)
	if err != nil {
		return "", err
	}
	a.forceRefresh()
	return "ok", nil
}

type SetSendPreMasterFaderParam struct {
	Chan           *uint8  `json:"channel,omitempty"`
	Bus            *uint8  `json:"bus,omitempty"`
	ChanName       *string `json:"channel_name,omitempty"`
	BusName        *string `json:"bus_name,omitempty"`
	PreMasterFader *bool   `json:"pre_master_fader"`
}

func (a *Api) handleSetSendPreMasterFader(param SetSendPreMasterFaderParam) (string, error) {
	err := a.getChanByName(&param.Chan, param.ChanName)
	if err != nil {
		return "", err
	}
	err = a.getBusByName(&param.Bus, param.BusName)
	if err != nil {
		return "", err
	}

	if param.Chan == nil || param.Bus == nil || param.PreMasterFader == nil {
		return "", fmt.Errorf("missing fields (need channel, bus, pre_master_fader)")
	}
	err = a.mapper.SetSendPreMasterFader(*param.Chan, *param.Bus, *param.PreMasterFader)
	if err != nil {
		return "", err
	}
	a.forceRefresh()
	return "ok", nil
}

type SetSendPreMasterMuteParam struct {
	Chan          *uint8  `json:"channel,omitempty"`
	Bus           *uint8  `json:"bus,omitempty"`
	ChanName      *string `json:"channel_name,omitempty"`
	BusName       *string `json:"bus_name,omitempty"`
	PreMasterMute *bool   `json:"pre_master_mute"`
}

func (a *Api) handleSetSendPreMasterMute(param SetSendPreMasterMuteParam) (string, error) {
	err := a.getChanByName(&param.Chan, param.ChanName)
	if err != nil {
		return "", err
	}
	err = a.getBusByName(&param.Bus, param.BusName)
	if err != nil {
		return "", err
	}

	if param.Chan == nil || param.Bus == nil || param.PreMasterMute == nil {
		return "", fmt.Errorf("missing fields (need channel, bus, pre_master_mute)")
	}
	err = a.mapper.SetSendPreMasterMute(*param.Chan, *param.Bus, *param.PreMasterMute)
	if err != nil {
		return "", err
	}
	a.forceRefresh()
	return "ok", nil
}

func (a *Api) pollState(force mapper.ForceGet) {
	state, err := a.mapper.GetFullState(force)
	if err != nil {
		a.logger.Error("could not poll state", "err", err)
		return
	}

	if len(a.chanNames) != len(state.Channels) {
		a.chanNames = make([]string, len(state.Channels))
	}
	for i := range state.Channels {
		a.chanNames[i] = state.Channels[i].Name
	}

	if len(a.busNames) != len(state.Buses) {
		a.busNames = make([]string, len(state.Buses))
	}
	for i := range state.Buses {
		a.busNames[i] = state.Buses[i].Name
	}

	a.stateBus.Send(state)
}

func (a *Api) pollLevels() {
	levels, err := a.mapper.GetLevels()
	if err != nil {
		a.logger.Error("could not poll levels", "err", err)
	}

	a.levelsBus.Send(levels)
}

func (a *Api) getChanByName(idx **uint8, name *string) error {
	if name == nil {
		return nil
	}
	for i := range a.chanNames {
		if *name == a.chanNames[i] {
			j := uint8(i)
			*idx = &j
			return nil
		}
	}

	return fmt.Errorf("no such chan: %s", *name)
}

func (a *Api) getBusByName(idx **uint8, name *string) error {
	if name == nil {
		return nil
	}
	for i := range a.busNames {
		if *name == a.busNames[i] {
			j := uint8(i)
			*idx = &j
			return nil
		}
	}

	return fmt.Errorf("no such bus: %s", *name)
}
