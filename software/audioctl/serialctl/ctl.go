package serialctl

import (
	"bufio"
	"fmt"
	"log/slog"
	"strings"
	"time"

	"github.com/fosdem/video/software/audioctl/config"
	"go.bug.st/serial"
)

type SerialCtl struct {
	cfg    *config.CtlCfg
	logger *slog.Logger
	port   serial.Port

	slug uint16

	reqs      chan *requestSync
	input     chan string
	readerErr chan error

	numChans int
	numBuses int
}

func New(logger *slog.Logger, cfg *config.CtlCfg) *SerialCtl {
	c := &SerialCtl{}
	c.logger = logger
	c.cfg = cfg
	c.input = make(chan string)
	c.readerErr = make(chan error)
	c.reqs = make(chan *requestSync)
	return c
}

func (c *SerialCtl) Loop() error {
	var err error
	mode := &serial.Mode{
		BaudRate: 115200,
	}
	c.port, err = serial.Open(c.cfg.PortDevice, mode)
	if err != nil {
		return fmt.Errorf("could not open %s: %w", c.cfg.PortDevice, err)
	}
	defer c.port.Close()

	c.logger.Info("successfully opened serial", "port", c.cfg.PortDevice)

	go c.reader()

	for {
		select {
		case err := <-c.readerErr:
			return fmt.Errorf("could not read from port: %w", err)
		case data := <-c.input:
			c.handleUnsolicitedInput(data)
		case req := <-c.reqs:
			<-req.Finish
		}
	}
}

func (c *SerialCtl) RawCmd(argstr string) (string, error) {
	req := newReq()
	defer close(req.Finish)
	c.reqs <- req

	c.slug += 1
	if c.slug == 0 {
		c.slug = 1
	}

	fmt.Fprintf(c.port, "%d %s\n", c.slug, argstr)

	timeout := time.After(1 * time.Second)

	for {
		select {
		case data := <-c.input:
			if resp, err, ok := c.parseResponse(data); ok {
				c.logger.Debug("cmd", "argstr", argstr, "resp", resp)
				return resp, err
			}
			c.handleUnsolicitedInput(data)
		case <-timeout:
			return "", fmt.Errorf("request to hardware timed out")
		}
	}
}

// requestSync is used to serialise requests
type requestSync struct {
	Finish chan struct{}
}

func newReq() *requestSync {
	return &requestSync{
		Finish: make(chan struct{}),
	}
}

func (c *SerialCtl) parseResponse(data string) (string, error, bool) {
	okprefix := fmt.Sprintf("[%d ok]", c.slug)
	if resp, ok := strings.CutPrefix(data, okprefix); ok {
		return strings.TrimSpace(resp), nil, true
	}

	failprefix := fmt.Sprintf("[%d err]", c.slug)
	if resp, ok := strings.CutPrefix(data, failprefix); ok {
		err := fmt.Errorf("hardware responded with error: %s", strings.TrimSpace(resp))
		return "", err, true
	}
	return "", nil, false
}

func (c *SerialCtl) handleUnsolicitedInput(data string) {
	if msg, ok := strings.CutPrefix(data, "[log] "); ok {
		c.logger.Info("log", "msg", msg)
		return
	}
	c.logger.Error("unsolicited data received", "data", data)
}

func (c *SerialCtl) reader() {
	scanner := bufio.NewScanner(c.port)
	for scanner.Scan() {
		c.input <- scanner.Text()
	}
	err := scanner.Err()
	if err == nil {
		err = fmt.Errorf("port was probably closed")
	}
	c.readerErr <- err
}
