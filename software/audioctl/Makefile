.PHONY: build
build: build/audioctl

LDFLAGS=-compressdwarf=false


build/%:
	go mod edit -replace github.com/fosdem/video/software/audioctl=.
	go build -ldflags="$(LDFLAGS)" -o $@ ./cmd/$*

.PHONY: install
install:
	install -Dm755 build/audioctl $(DESTDIR)$(PREFIX)/bin/audioctl
	install -Dm644 example_config.yaml $(DESTDIR)$(PREFIX)/etc/audioctl.conf
