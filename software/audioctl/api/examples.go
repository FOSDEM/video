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

var exampleBusVolumeParam1 = SetBusVolumeParam{
	Bus:    &exampleOne,
	Volume: &exampleFourTwo,
}

var exampleBusVolumeParam2 = SetBusVolumeParam{
	BusName: &examplePenka,
	Volume:  &exampleFourTwo,
}

var exampleTrue = true
var exampleFalse = false
var exampleFoo = "foo"
var exampleBar = "bar"
var examplePenka = "penka"
var exampleThree = uint8(3)
var exampleOne = uint8(1)
var exampleZero = uint8(0)
var exampleFourTwo = float32(42)
