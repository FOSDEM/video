package config

import (
	"fmt"

	"github.com/dexterlb/misirka/go/msksrvbuilder"
)

type ApiCfg struct {
	Misirka                msksrvbuilder.ServerBuildConfig
	StatePollIntervalMsec  uint `yaml:"state_poll_interval_msec"`
	LevelsPollIntervalMsec uint `yaml:"levels_poll_interval_msec"`
}

func (a *ApiCfg) Validate() error {
	if a.StatePollIntervalMsec < 5 {
		return fmt.Errorf("`state_poll_interval_msec` should be >= 5")
	}
	if a.LevelsPollIntervalMsec < 5 {
		return fmt.Errorf("`levels_poll_interval_msec` should be >= 5")
	}
	return nil
}
