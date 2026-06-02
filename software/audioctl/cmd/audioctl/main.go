package main

import (
	"fmt"
	"log/slog"
	"os"

	"github.com/fosdem/video/software/audioctl/api"
	"github.com/fosdem/video/software/audioctl/config"
	"github.com/fosdem/video/software/audioctl/ctl"
	"github.com/fosdem/video/software/audioctl/fakectl"
	"github.com/fosdem/video/software/audioctl/serialctl"
)

func main() {
	if len(os.Args) != 2 {
		fmt.Fprintf(os.Stderr, "usage: %s <config file>\n", os.Args[0])
		os.Exit(1)
	}

	logger := slog.Default()

	cfg, err := config.Parse(os.Args[1])
	if err != nil {
		logger.Error("Could not parse config server", "err", err)
		os.Exit(1)
	}

	var c ctl.Ctl
	if cfg.Ctl.PortDevice == "fake" {
		logger.Info("using fake ctl")
		c = fakectl.New(cfg.Ctl)
	} else {
		c = serialctl.New(logger.With("prefix", "ctl"), cfg.Ctl)
	}
	go func() {
		err := c.Loop()
		if err != nil {
			logger.Error("Control loop exited", "err", err)
			os.Exit(1)
		}
		os.Exit(0)
	}()

	a := api.New(logger.With("prefix", "api"), cfg.Api, c)
	err = a.Serve()
	if err != nil {
		logger.Error("Could not start server", "err", err)
		os.Exit(1)
	}
}
