package config

import (
	"fmt"
	"os"

	"github.com/dexterlb/misirka/go/msksrvbuilder"
	"github.com/goccy/go-yaml"
)

type Config struct {
	Api *ApiCfg
	Ctl *CtlCfg
}

func Parse(filename string) (*Config, error) {
	f, err := os.Open(filename)
	if err != nil {
		return nil, fmt.Errorf("could not open %s: %s", filename, err)
	}
	defer func(f *os.File) {
		err := f.Close()
		if err != nil {
			_ = fmt.Errorf("could not close %s: %s", filename, err)
		}
	}(f)

	// uncomment for path hacks (check fazantix source for ideas)
	// absFilename, err := filepath.Abs(filename)
	// if err != nil {
	// 	return nil, fmt.Errorf("somehow, %s is malformed: %w", filename, err)
	// }
	// UnmarshalBase = filepath.Dir(absFilename)

	m := yaml.NewDecoder(f, yaml.Strict())

	cfg := &Config{
		Api: &ApiCfg{
			Misirka: msksrvbuilder.DefaultServerBuildConfig,
		},
	}
	err = m.Decode(cfg)
	if err != nil {
		return nil, err
	}
	err = cfg.Validate()
	if err != nil {
		return nil, err
	}
	return cfg, err
}

func (c *Config) Validate() error {
	var err error
	if c.Api == nil {
		return fmt.Errorf("`api` section missing")
	}
	err = c.Api.Validate()
	if err != nil {
		return fmt.Errorf("error in `api` section: %w", err)
	}
	if c.Ctl == nil {
		return fmt.Errorf("`ctl` section missing")
	}
	err = c.Ctl.Validate()
	if err != nil {
		return fmt.Errorf("error in `ctl` section: %w", err)
	}
	return nil
}
