package api

var exampleMatrixSendParam1 = SetMatrixSendParam{
	Chan:    &exampleOne,
	Bus:     &exampleThree,
	Unmuted: &exampleFalse,
}

var exampleMatrixSendParam2 = SetMatrixSendParam{
	ChanName: &exampleFoo,
	BusName:  &exampleBar,
	Unmuted:  &exampleTrue,
}

var exampleInGainParam1 = SetInGainParam{
	Chan: &exampleOne,
	Gain: &exampleFourTwo,
}

var exampleInGainParam2 = SetInGainParam{
	ChanName: &exampleFoo,
	Gain:     &exampleFourTwo,
}

var exampleInEqBandParam1 = SetInEqBandParam{
	Chan:      &exampleOne,
	Band:      &exampleZero,
	Shape:     &exampleOne,
	Frequency: &exampleFourTwo,
}

var exampleInEqBandParam2 = SetInEqBandParam{
	ChanName:  &exampleFoo,
	Band:      &exampleZero,
	ShapeName: &exampleLowpass,
	Frequency: &exampleFourTwo,
	Q:         &exampleSqrtHalf,
}

var examplePhantomParam1 = SetPhantomParam{
	Chan:    &exampleOne,
	Phantom: &exampleTrue,
}

var examplePhantomParam2 = SetPhantomParam{
	ChanName: &exampleFoo,
	Phantom:  &exampleFalse,
}

var exampleMatrixVolumeParam1 = SetMatrixVolumeParam{
	Chan:   &exampleOne,
	Bus:    &exampleZero,
	Volume: &exampleFourTwo,
}

var exampleMatrixVolumeParam2 = SetMatrixVolumeParam{
	ChanName: &exampleFoo,
	BusName:  &examplePenka,
	Volume:   &exampleFourTwo,
}

var exampleBusMasterFaderParam1 = SetBusMasterFaderParam{
	Bus:   &exampleOne,
	Fader: &exampleFourTwo,
}

var exampleBusMasterFaderParam2 = SetBusMasterFaderParam{
	BusName: &examplePenka,
	Fader:   &exampleFourTwo,
}

var exampleChannelMasterFaderParam1 = SetChannelMasterFaderParam{
	Chan:  &exampleOne,
	Fader: &exampleFourTwo,
}

var exampleChannelMasterFaderParam2 = SetChannelMasterFaderParam{
	ChanName: &exampleFoo,
	Fader:    &exampleFourTwo,
}

var exampleChannelMasterUnmutedParam1 = SetChannelMasterUnmutedParam{
	Chan:    &exampleOne,
	Unmuted: &exampleTrue,
}

var exampleChannelMasterUnmutedParam2 = SetChannelMasterUnmutedParam{
	ChanName: &exampleFoo,
	Unmuted:  &exampleFalse,
}

var exampleBusMasterUnmutedParam1 = SetBusMasterUnmutedParam{
	Bus:     &exampleOne,
	Unmuted: &exampleTrue,
}

var exampleBusMasterUnmutedParam2 = SetBusMasterUnmutedParam{
	BusName: &examplePenka,
	Unmuted: &exampleFalse,
}

var exampleSendPreMasterFaderParam1 = SetSendPreMasterFaderParam{
	Chan:           &exampleOne,
	Bus:            &exampleThree,
	PreMasterFader: &exampleTrue,
}

var exampleSendPreMasterFaderParam2 = SetSendPreMasterFaderParam{
	ChanName:       &exampleFoo,
	BusName:        &exampleBar,
	PreMasterFader: &exampleFalse,
}

var exampleSendPreMasterMuteParam1 = SetSendPreMasterMuteParam{
	Chan:          &exampleOne,
	Bus:           &exampleThree,
	PreMasterMute: &exampleTrue,
}

var exampleSendPreMasterMuteParam2 = SetSendPreMasterMuteParam{
	ChanName:      &exampleFoo,
	BusName:       &exampleBar,
	PreMasterMute: &exampleFalse,
}

var exampleTrue = true
var exampleFalse = false
var exampleFoo = "foo"
var exampleBar = "bar"
var examplePenka = "penka"
var exampleLowpass = "lowpass"
var exampleThree = uint8(3)
var exampleOne = uint8(1)
var exampleZero = uint8(0)
var exampleFourTwo = float32(42)
var exampleSqrtHalf = float32(0.7071)
