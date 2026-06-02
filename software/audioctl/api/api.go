package api

import (
	"errors"
	"log/slog"
	"time"

	"github.com/dexterlb/misirka/go/mskbus"
	"github.com/dexterlb/misirka/go/msksrv"
	"github.com/dexterlb/misirka/go/msksrv/backends"
	"github.com/dexterlb/misirka/go/msksrvbuilder"
	"github.com/fosdem/video/software/audioctl/config"
	"github.com/fosdem/video/software/audioctl/ctl"
	"github.com/fosdem/video/software/audioctl/fakectl"
)

type Api struct {
	srv          *msksrv.Server
	mainLoop     *msksrvbuilder.MainLoop
	logger       *slog.Logger
	cfg          *config.ApiCfg
	ctl          ctl.Ctl
	dying        chan struct{}
	refreshState chan (chan struct{})
	chanNames    []string
	busNames     []string

	heartbeatBus *mskbus.BusOf[Heartbeat]
	stateBus     *mskbus.BusOf[*ctl.MixerState]
	levelsBus    *mskbus.BusOf[*ctl.Levels]
}

func New(logger *slog.Logger, cfg *config.ApiCfg, ctlInst ctl.Ctl) *Api {
	a := &Api{}
	a.cfg = cfg
	a.logger = logger
	a.ctl = ctlInst
	a.dying = make(chan struct{})
	a.refreshState = make(chan (chan struct{}))

	evtHandlers := backends.EventHandlers{
		Err: func(err error) {
			var berr *backends.BackendSpecificError
			if errors.As(err, &berr) {
				logger.Error("API error", "backend", berr.BackendName, "err", berr.Err)
			} else {
				logger.Error("API error", "err", err)
			}
		},
		Info: func(msg string, data map[string]interface{}) {
			attrs := make([]any, 0, len(data)*2)
			for k, v := range data {
				attrs = append(attrs, k, v)
			}
			logger.Info(msg, attrs...)
		},
	}

	a.srv, a.mainLoop = msksrvbuilder.BuildServer(evtHandlers, &cfg.Misirka)

	a.srv.
		Name("FOSDEM AudioCtl").
		Descr("control API for the FOSDEM audio board")

	a.heartbeatBus = msksrv.AddTopic[Heartbeat](a.srv, "heartbeat").
		Descr("sends a heartbeat every now and then").
		Example(Heartbeat{Now: time.Now()}).
		Bus()

	a.stateBus = msksrv.AddTopic[*ctl.MixerState](a.srv, "state").
		Descr("sends the full audio control state").
		Example(fakectl.DefaultState).
		Bus().
		DedupBy(ctl.MixerStateEqual)

	a.levelsBus = msksrv.AddTopic[*ctl.Levels](a.srv, "levels").
		Descr("sends the audio levels of all inputs and outputs, in decibels").
		Example(fakectl.DefaultLevels()).
		Bus()

	msksrv.AddCall(a.srv, "set-full-state", a.handleSetFullState).
		Descr("set the full state of the audio mixer at once").
		Example(fakectl.DefaultState, "ok")

	msksrv.AddCall(a.srv, "set-matrix-send", a.handleSetMatrixSend).
		Descr("set the unmuted status of the given matrix cross-point").
		Example(exampleMatrixSendParam1, "ok").
		Example(exampleMatrixSendParam2, "ok").
		PathValueAlias("set-matrix-send/i/{channel}/{bus}/{unmuted}").
		PathValueAlias("set-matrix-send/{channel_name}/{bus_name}/{unmuted}")

	msksrv.AddCall(a.srv, "set-matrix-volume", a.handleSetMatrixVolume).
		Descr("set the volume (in decibels) of the given matrix cross-point").
		Example(exampleMatrixVolumeParam1, "ok").
		Example(exampleMatrixVolumeParam2, "ok").
		PathValueAlias("set-matrix-volume/i/{channel}/{bus}/{volume}").
		PathValueAlias("set-matrix-volume/{channel_name}/{bus_name}/{volume}")

	msksrv.AddCall(a.srv, "set-phantom", a.handleSetPhantom).
		Descr("turn phantom power for the given input on or off").
		Example(examplePhantomParam1, "ok").
		Example(examplePhantomParam2, "ok").
		PathValueAlias("set-phantom/i/{channel}/{phantom}").
		PathValueAlias("set-phantom/{channel_name}/{phantom}")

	msksrv.AddCall(a.srv, "set-in-gain", a.handleSetInGain).
		Descr("set the input gain (in decibels) of the given input channel").
		Example(exampleInGainParam1, "ok").
		Example(exampleInGainParam2, "ok").
		PathValueAlias("set-in-gain/i/{channel}/{volume}").
		PathValueAlias("set-in-gain/{channel_name}/{volume}")

	msksrv.AddCall(a.srv, "set-bus-volume", a.handleSetBusVolume).
		Descr("set the volume (in decibels) of the given output bus").
		Example(exampleBusVolumeParam1, "ok").
		Example(exampleBusVolumeParam2, "ok").
		PathValueAlias("set-bus-volume/i/{bus}/{volume}").
		PathValueAlias("set-bus-volume/{bus_name}/{volume}")

	msksrv.AddCall(a.srv, "raw-cmd", a.handleRawCmd).
		Descr("execute a raw command on the audio hardware").
		Example("volume.set 0 1 2.5", "ok")

	msksrv.AddCall(a.srv, "factory-reset", a.handleFactoryReset).
		Descr("factory reset the audio hardware").
		Example(FactoryResetParam{}, "ok")

	return a
}

func (a *Api) Serve() error {
	defer close(a.dying)
	go a.doHeartbeat()
	go a.poller()
	return a.mainLoop.Run()
}

type Heartbeat struct {
	Now time.Time `json:"now"`
}

func (a *Api) doHeartbeat() {
	ticker := time.NewTicker(1 * time.Second)
	defer ticker.Stop()

	var h Heartbeat
	for {
		select {
		case <-a.dying:
			return
		case t := <-ticker.C:
			h.Now = t
			a.heartbeatBus.Send(h)
		}
	}
}

func (a *Api) poller() {
	pollState := time.NewTicker(time.Duration(a.cfg.StatePollIntervalMsec) * time.Millisecond)
	defer pollState.Stop()

	pollLevels := time.NewTicker(time.Duration(a.cfg.LevelsPollIntervalMsec) * time.Millisecond)
	defer pollLevels.Stop()

	a.pollState()
	a.pollLevels()
	for {
		select {
		case <-a.dying:
			return
		case done := <-a.refreshState:
			a.pollState()
			close(done)
		case <-pollState.C:
			a.pollState()
		case <-pollLevels.C:
			a.pollLevels()
		}
	}
}

func (a *Api) forceRefresh() {
	// TODO: throttling
	done := make(chan struct{})
	a.refreshState <- done
	<-done
}
