module github.com/fosdem/video/software/audioctl

go 1.25.0

replace github.com/dexterlb/misirka/go => ../../../../../misirka/go

require (
	github.com/dexterlb/misirka/go v0.0.0-00010101000000-000000000000
	github.com/goccy/go-yaml v1.19.2
	go.bug.st/serial v1.7.1
)

require (
	github.com/eclipse/paho.golang v0.23.0 // indirect
	github.com/goccy/go-json v0.10.6 // indirect
	github.com/gorilla/websocket v1.5.3 // indirect
	golang.org/x/net v0.43.0 // indirect
	golang.org/x/sys v0.43.0 // indirect
)
