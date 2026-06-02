package config

import "fmt"

type CtlCfg struct {
	PortDevice  string `yaml:"port_device"`
	UseDBLevels *bool  `yaml:"use_db_levels"`
}

func (a *CtlCfg) Validate() error {
	if a.PortDevice == "" {
		return fmt.Errorf("`port_device` should be nonempty")
	}
	if a.UseDBLevels == nil {
		return fmt.Errorf("`use_db_levels` should be true or false")
	}
	return nil
}
