package mapper

import (
	"io"
	"log/slog"
	"math"
	"math/rand"
	"reflect"
	"testing"

	"github.com/fosdem/video/software/audioctl/ctl"
)

func discardLogger() *slog.Logger {
	return slog.New(slog.NewTextHandler(io.Discard, nil))
}

type generator struct {
	data []byte
	pos  int
}

func (g *generator) byte() byte {
	if g.pos >= len(g.data) {
		g.pos++
		return 0
	}
	b := g.data[g.pos]
	g.pos++
	return b
}

func (g *generator) intn(n int) int {
	return int(g.byte()) % n
}

func (g *generator) bool() bool {
	return g.byte()&1 == 0
}

func (g *generator) volume() float32 {
	u := uint32(g.byte()) | uint32(g.byte())<<8 | uint32(g.byte())<<16 | uint32(g.byte())<<24
	return float32(u)/float32(math.MaxUint32)*72 - 60
}

func randomMapperState(g *generator) *MapperState {
	numChans := g.intn(6) + 1
	numBuses := g.intn(8) + 1

	buses := make([]ctl.BusState, numBuses)
	for i := range buses {
		buses[i] = ctl.BusState{
			Name:   "bus",
			Label:  "b",
			Volume: g.volume(),
		}
	}

	channels := make([]ChannelState, numChans)
	for i := range channels {
		ch := &channels[i]
		ch.ChannelCfg = ctl.ChannelCfg{
			Name:    "chan",
			Label:   "c",
			Gain:    g.volume(),
			Phantom: g.bool(),
		}
		ch.MasterFader = g.volume()
		ch.MasterUnmuted = g.bool()

		ch.Sends = make([]SendState, numBuses)
		for j := range ch.Sends {
			ch.Sends[j] = SendState{
				Unmuted:        g.bool(),
				Volume:         g.volume(),
				PreMasterFader: g.bool(),
				PreMasterMute:  g.bool(),
			}
		}
	}

	return &MapperState{
		Channels: channels,
		Buses:    buses,
	}
}

func checkRoundTrip(t *testing.T, a *MapperState) {
	t.Helper()
	b := a.ToMixerState()
	c := FromMixerState(b, discardLogger())
	d := c.ToMixerState()

	if !approxEqual(b, d, 1e-3) {
		t.Errorf(
			"mixer state B does not survive transform to mapper state C and back to mixer state D:\nB: %+v\nC: %+v\nD:%+v\n",
			b, c, d,
		)
	}

	if !approxEqual(a, c, 1e-3) {
		t.Errorf(
			"mapper state A does not survive transform to mixer state B and back to mixer state C:\nA: %+v\nB: %+v\nC:%+v\n",
			a, b, c,
		)
	}
}

func checkRandomRoundTrip(t *testing.T, data []byte) {
	t.Helper()
	a := randomMapperState(&generator{data: data})
	checkRoundTrip(t, a)
}

func TestRoundTrip(t *testing.T) {
	rng := rand.New(rand.NewSource(1))
	for i := 0; i < 2000; i++ {
		data := make([]byte, rng.Intn(256))
		rng.Read(data)
		checkRandomRoundTrip(t, data)
	}
}

func FuzzRoundTrip(f *testing.F) {
	f.Add([]byte{})
	f.Add([]byte{0, 1, 2, 3, 4, 5, 6, 7})
	f.Add(make([]byte, 128))
	f.Fuzz(func(t *testing.T, data []byte) {
		checkRandomRoundTrip(t, data)
	})
}

func TestSendMuteCombinations(t *testing.T) {
	for _, preChannelMute := range []bool{false, true} {
		for _, sendUnmuted := range []bool{false, true} {
			for _, masterUnmuted := range []bool{false, true} {
				m := &MapperState{
					Channels: []ChannelState{
						{
							MasterUnmuted: masterUnmuted,
							Sends: []SendState{
								{
									Unmuted:       sendUnmuted,
									PreMasterMute: preChannelMute,
								},
							},
						},
					},
					Buses: []ctl.BusState{{}},
				}

				checkRoundTrip(t, m)
			}
		}
	}
}

func approxEqual(x, y any, eps float64) bool {
	return approxEqualValue(reflect.ValueOf(x), reflect.ValueOf(y), eps)
}

// I fuckin hate this function
func approxEqualValue(x, y reflect.Value, eps float64) bool {
	if x.IsValid() != y.IsValid() {
		return false
	}
	if !x.IsValid() {
		return true
	}
	if x.Type() != y.Type() {
		return false
	}

	switch x.Kind() {
	case reflect.Pointer, reflect.Interface:
		if x.IsNil() || y.IsNil() {
			return x.IsNil() == y.IsNil()
		}
		return approxEqualValue(x.Elem(), y.Elem(), eps)
	case reflect.Struct:
		for i := range x.NumField() {
			if !approxEqualValue(x.Field(i), y.Field(i), eps) {
				return false
			}
		}
		return true
	case reflect.Slice, reflect.Array:
		if x.Len() != y.Len() {
			return false
		}
		for i := range x.Len() {
			if !approxEqualValue(x.Index(i), y.Index(i), eps) {
				return false
			}
		}
		return true
	case reflect.Map:
		if x.Len() != y.Len() {
			return false
		}
		for _, k := range x.MapKeys() {
			yv := y.MapIndex(k)
			if !yv.IsValid() || !approxEqualValue(x.MapIndex(k), yv, eps) {
				return false
			}
		}
		return true
	case reflect.Float32, reflect.Float64:
		return math.Abs(x.Float()-y.Float()) <= eps
	case reflect.Bool:
		return x.Bool() == y.Bool()
	case reflect.String:
		return x.String() == y.String()
	case reflect.Int, reflect.Int8, reflect.Int16, reflect.Int32, reflect.Int64:
		return x.Int() == y.Int()
	case reflect.Uint, reflect.Uint8, reflect.Uint16, reflect.Uint32, reflect.Uint64:
		return x.Uint() == y.Uint()
	default:
		return reflect.DeepEqual(x.Interface(), y.Interface())
	}
}
