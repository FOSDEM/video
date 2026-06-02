package ctl

type Ctl interface {
	GetFullState() (*MixerState, error)
	GetChannelNames() ([]string, []string, error)
	GetChannelLabels() ([]string, []string, error)
	GetInputGains() ([]float32, error)
	GetBusVolumes() ([]float32, error)
	GetPhantoms() ([]bool, error)
	GetMatrix() ([]SendState, error)
	SetFullState(state *MixerState) error
	SetMatrixSend(ch uint8, bus uint8, unmuted bool) error
	SetMatrixVolume(ch uint8, bus uint8, volume float32) error
	SetInGain(ch uint8, gain float32) error
	SetPhantom(ch uint8, phantom bool) error
	SetBusVolume(bus uint8, volume float32) error
	FactoryReset() error
	Loop() error
	RawCmd(argstr string) (string, error)
	GetLevels() (*Levels, error)
}
