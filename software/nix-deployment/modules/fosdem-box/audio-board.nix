{ pkgs, ... }: let
  audioboard-tty = "tty_fosdem_audio_ctl";

  audioctl-listen-port = 8811;
  networking.firewall.allowedTCPPorts = [ audioctl-listen-port ];

  audioctl-config = {
    api = {
      misirka = {
        http = {
          enable = true;
          bind = ":${builtins.toString audioctl-listen-port}";
          prefix = "";
        };
        ws = {
          enable = true;
          url = "/ws";
        };
        doc = {
          enable = true;
          path = "doc";
          html_path = "doc.html";
        };
        mqtt = {
          enable = false;
          broker_url = "mqtt://localhost:1883";
          client_id = "fosdem_audioctl";
          prefix = "/fosdem/audioctl/";
        };
      };
      state_poll_interval_msec = 10000;
      levels_poll_interval_msec = 100;
    };
    ctl = {
      port_device = "/dev/${audioboard-tty}";
      use_db_levels = true;
    };
  };

  audioctl-pkg = pkgs.buildGoModule {
    name = "audioctl";
    src = ../../../audioctl;

    vendorHash = "sha256-4YMoBu3cDicDO9UqdVzM79hXaC2YGJxWBeA1f+sjbNA=";
    goSum = ../../../audioctl/go.sum;

    doCheck = false; # don't check on every build, just check during check phase

    nativeBuildInputs = [];
    buildInputs = [];
  };

  audioctl-config-file = pkgs.writeTextFile {
    name = "audioctl-config.json";
    text = builtins.toJSON audioctl-config;
  };
in {
  services.udev.extraRules = ''
    ACTION=="remove", GOTO="fosdem_audio_end"
    SUBSYSTEM!="tty", GOTO="fosdem_audio_end"
    SUBSYSTEMS=="usb", IMPORT{builtin}="usb_id"
    ENV{ID_SERIAL}!="FOSDEM_Audio*", GOTO="fosdem_audio_end"
    KERNEL!="ttyACM[0-9]*", GOTO="fosdem_audio_end"

    ENV{ID_USB_INTERFACE_NUM}=="00", SYMLINK+="${audioboard-tty}"

    LABEL="fosdem_audio_end"
  '';

  services.udev.packages = [ pkgs.teensy-udev-rules ];

  environment.systemPackages = [
    # pkgs.fosdem-firmware-audio-brd-flash
    # pkgs.fosdem-mixercli
    # pkgs.fosdem-mixerapi
    # pkgs.fosdem-osc-proxy
    pkgs.teensy-loader-cli
    pkgs.picocom
  ];

  systemd.services.audioctl = {
    enable = true;
    description = "FOSDEM audio mixer control software";
    serviceConfig = {
      Type = "simple";
      ExecStart = "${audioctl-pkg}/bin/audioctl ${audioctl-config-file}";
      User = "human";
      Group = "human";
    };
    wantedBy = [ "multi-user.target" ];
  };
}
