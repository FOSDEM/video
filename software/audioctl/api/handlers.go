package api

import (
	"fmt"

	"github.com/fosdem/video/software/audioctl/ctl"
)

func (a *Api) handleRawCmd(param string) (string, error) {
	resp, err := a.ctl.RawCmd(param)
	if err != nil {
		return "", err
	}
	return resp, nil
}

func (a *Api) handleSetFullState(param *ctl.MixerState) (string, error) {
	err := a.ctl.SetFullState(param)
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
	err = a.ctl.SetMatrixVolume(*param.Chan, *param.Bus, *param.Volume)
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
	err = a.ctl.SetMatrixSend(*param.Chan, *param.Bus, *param.Unmuted)
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
	err = a.ctl.SetInGain(*param.Chan, *param.Gain)
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
	err = a.ctl.SetPhantom(*param.Chan, *param.Phantom)
	if err != nil {
		return "", err
	}
	a.forceRefresh()
	return "ok", nil
}

type FactoryResetParam struct {
}

func (a *Api) handleFactoryReset(param FactoryResetParam) (string, error) {
	err := a.ctl.FactoryReset()
	if err != nil {
		return "", err
	}
	a.forceRefresh()
	return "ok", nil
}

type SetBusVolumeParam struct {
	Bus     *uint8   `json:"bus,omitempty"`
	BusName *string  `json:"bus_name,omitempty"`
	Volume  *float32 `json:"volume"`
}

func (a *Api) handleSetBusVolume(param SetBusVolumeParam) (string, error) {
	err := a.getBusByName(&param.Bus, param.BusName)
	if err != nil {
		return "", err
	}

	if param.Bus == nil || param.Volume == nil {
		return "", fmt.Errorf("missing fields (need bus, volume)")
	}
	err = a.ctl.SetBusVolume(*param.Bus, *param.Volume)
	if err != nil {
		return "", err
	}
	a.forceRefresh()
	return "ok", nil
}

func (a *Api) pollState() {
	state, err := a.ctl.GetFullState()
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
	levels, err := a.ctl.GetLevels()
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
