package mapper

import (
	"encoding/json"
	"fmt"
)

type storedInfo struct {
	PreMasterFaders []uint16 `json:"pcf"`
	PreMasterMutes  []uint16 `json:"pcm"`
	Unmutes         []uint16 `json:"u"`

	ChannelMasterFaders   []float32 `json:"cmf"`
	ChannelMasterUnmuteds uint16    `json:"cmu"`

	BusMasterFaders   []float32 `json:"bmf"`
	BusMasterUnmuteds uint16    `json:"bmu"`
}

func (s *storedInfo) Resize(numChans int, numBuses int) {
	brandNew := len(s.ChannelMasterFaders) == 0

	falseMask := uint16(0)
	trueMask := ^uint16(0)

	s.PreMasterFaders = resizeSlice(s.PreMasterFaders, numChans, falseMask)
	s.PreMasterMutes = resizeSlice(s.PreMasterMutes, numChans, falseMask)
	s.Unmutes = resizeSlice(s.Unmutes, numChans, trueMask)
	s.ChannelMasterFaders = resizeSlice(s.ChannelMasterFaders, numChans, 0)
	s.BusMasterFaders = resizeSlice(s.BusMasterFaders, numBuses, 0)

	if brandNew {
		s.ChannelMasterUnmuteds = trueMask
		s.BusMasterUnmuteds = trueMask
	}
}

func (s *storedInfo) PreMasterFader(ch, send int) bool {
	return s.PreMasterFaders[ch]&(1<<uint(send)) != 0
}

func (s *storedInfo) PreMasterMute(ch, send int) bool {
	return s.PreMasterMutes[ch]&(1<<uint(send)) != 0
}

func (s *storedInfo) Unmuted(ch, send int) bool {
	return s.Unmutes[ch]&(1<<uint(send)) != 0
}

func (s *storedInfo) ChannelMasterUnmuted(ch int) bool {
	return s.ChannelMasterUnmuteds&(1<<uint(ch)) != 0
}

func (s *storedInfo) BusMasterUnmuted(bus int) bool {
	return s.BusMasterUnmuteds&(1<<uint(bus)) != 0
}

func (s *storedInfo) SetPreMasterFader(ch, send int, v bool) {
	s.PreMasterFaders[ch] = setBit(s.PreMasterFaders[ch], send, v)
}

func (s *storedInfo) SetPreMasterMute(ch, send int, v bool) {
	s.PreMasterMutes[ch] = setBit(s.PreMasterMutes[ch], send, v)
}

func (s *storedInfo) SetUnmuted(ch, send int, v bool) {
	s.Unmutes[ch] = setBit(s.Unmutes[ch], send, v)
}

func (s *storedInfo) SetChannelMasterFader(ch int, v float32) {
	s.ChannelMasterFaders[ch] = v
}

func (s *storedInfo) SetChannelMasterUnmuted(ch int, v bool) {
	s.ChannelMasterUnmuteds = setBit(s.ChannelMasterUnmuteds, ch, v)
}

func (s *storedInfo) SetBusMasterFader(bus int, v float32) {
	s.BusMasterFaders[bus] = v
}

func (s *storedInfo) SetBusMasterUnmuted(bus int, v bool) {
	s.BusMasterUnmuteds = setBit(s.BusMasterUnmuteds, bus, v)
}

func (s *storedInfo) Blobify() string {
	data, err := json.Marshal(s)
	if err != nil {
		panic(fmt.Sprintf("cannot blobify stored info: %s", err))
	}
	return string(data)
}

func deblobifyStoredInfo(blob string) (*storedInfo, error) {
	s := &storedInfo{}
	if blob == "" {
		return s, nil
	}
	if err := json.Unmarshal([]byte(blob), s); err != nil {
		return &storedInfo{}, err
	}
	return s, nil
}
