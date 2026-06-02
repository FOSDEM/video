package serialctl

import (
	"fmt"

	"github.com/fosdem/video/software/audioctl/ctl"
)

func (c *SerialCtl) GetLevels() (*ctl.Levels, error) {
	if c.numChans == 0 || c.numBuses == 0 {
		return nil, fmt.Errorf("no channels/buses known (maybe GetFullState() did not get called)")
	}

	cmd := "levels"
	if *c.cfg.UseDBLevels {
		cmd = "levels.db"
	}

	resp, err := c.RawCmd(cmd)
	if err != nil {
		return nil, err
	}

	vals, err := parseFloatList(resp)
	if err != nil {
		return nil, err
	}

	if *c.cfg.UseDBLevels {
		for i := range vals {
			if vals[i] < -120 {
				vals[i] = -120
			}
		}
	}

	if len(vals) != (c.numChans+c.numBuses)*3 {
		return nil, fmt.Errorf("got %d values instead of %d", len(vals), (c.numChans+c.numBuses)*3)
	}
	levels := &ctl.Levels{
		RMS: ctl.LevelsBlock{
			Input: make([]float32, c.numChans),
			Bus:   make([]float32, c.numBuses),
		},
		Peak: ctl.LevelsBlock{
			Input: make([]float32, c.numChans),
			Bus:   make([]float32, c.numBuses),
		},
		Smooth: ctl.LevelsBlock{
			Input: make([]float32, c.numChans),
			Bus:   make([]float32, c.numBuses),
		},
	}

	for i := range c.numChans {
		levels.RMS.Input[i] = vals[i*3+0]
		levels.Peak.Input[i] = vals[i*3+1]
		levels.Smooth.Input[i] = vals[i*3+1]
	}
	for i := range c.numBuses {
		j := c.numChans + i
		levels.RMS.Bus[i] = vals[j*3+0]
		levels.Peak.Bus[i] = vals[j*3+1]
		levels.Smooth.Bus[i] = vals[j*3+1]
	}

	return levels, nil
}
