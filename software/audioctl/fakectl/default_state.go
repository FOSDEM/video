package fakectl

import "github.com/fosdem/video/software/audioctl/ctl"

var DefaultState *ctl.MixerState = &ctl.MixerState{
	Channels: []ctl.ChannelState{
		{
			Name:    "fubalina",
			Label:   "foo",
			Gain:    -60,
			Phantom: false,
			Sends: []ctl.SendState{
				{
					Unmuted: true,
					Volume:  0,
				},
				{
					Unmuted: false,
					Volume:  0,
				},
			},
		},
		{
			Name:    "barabela",
			Label:   "bar",
			Gain:    0,
			Phantom: false,
			Sends: []ctl.SendState{
				{
					Unmuted: true,
					Volume:  -18,
				},
				{
					Unmuted: true,
					Volume:  -30,
				},
			},
		},
		{
			Name:    "bazinga",
			Label:   "baz",
			Gain:    0,
			Phantom: true,
			Sends: []ctl.SendState{
				{
					Unmuted: false,
					Volume:  0,
				},
				{
					Unmuted: true,
					Volume:  -20,
				},
			},
		},
	},
	Buses: []ctl.BusState{
		{
			Name:   "penka",
			Label:  "pe",
			Volume: 0,
		},
		{
			Name:   "donka",
			Label:  "do",
			Volume: -10,
		},
	},
}

func DefaultLevels() *ctl.Levels {
	return &ctl.Levels{
		RMS: ctl.LevelsBlock{
			Input: make([]float32, len(DefaultState.Channels)),
			Bus:   make([]float32, len(DefaultState.Buses)),
		},
		Peak: ctl.LevelsBlock{
			Input: make([]float32, len(DefaultState.Channels)),
			Bus:   make([]float32, len(DefaultState.Buses)),
		},
		Smooth: ctl.LevelsBlock{
			Input: make([]float32, len(DefaultState.Channels)),
			Bus:   make([]float32, len(DefaultState.Buses)),
		},
	}
}
