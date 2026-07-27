{ config, lib, pkgs, ... }:
let
  makeV4LPathRule = path: data: ''
    KERNEL=="video*", \
    ENV{ID_PATH_WITH_USB_REVISION}=="${path}", \
    ENV{ID_V4L_CAPABILITIES}==":capture:", \
    SYMLINK+="${data.name}"
  '';
  makeV4LPathRules = linkMap: lib.mapAttrsToList makeV4LPathRule linkMap;
  makeV4LRuleStr = cfg:
    builtins.concatStringsSep "\n" (makeV4LPathRules cfg.by-path);

  makeWireplumberPathMatcher = devpath: data: {
    matches = [{ "device.bus-path" = devpath; }];
    actions = {
      update-props = {
        "device.profile" = "pro-audio";
        "device.description" = data.name;
        "device.disabled" = false;
      };
    };
  };
  makeWireplumberNameMatcher = name: data: {
    matches = [{ "device.name" = name; }];
    actions = {
      update-props = {
        "device.profile" = "pro-audio";
        "device.description" = data.name;
        "device.disabled" = false;
      };
    };
  };
  makeWireplumberCfg = audioDevMap: {
    "50-disable-devices-by-default" = {
      "monitor.alsa.rules" = [{
        matches = [{ "device.api" = "alsa"; }];
        actions = { update-props = { "device.disabled" = true; }; };
      }];
    };

    "51-capture-mapping" = {
      "monitor.alsa.rules" =
        (lib.mapAttrsToList makeWireplumberPathMatcher audioDevMap.by-path)
        ++ (lib.mapAttrsToList makeWireplumberNameMatcher audioDevMap.by-name);
    };
  };
in {
  options.mixos.devMap = {
    videoCapture.by-path = lib.mkOption {
      type = lib.types.attrsOf (lib.types.submodule {
        options = {
          name = lib.mkOption {
            type = lib.types.str;
            description = "Logical name assigned to this video capture device.";
          };
        };
      });
      default = { };
      description = ''
        Mapping of video capture device IDs to configuration.
        Keys are device IDs (e.g. "pci-0000:03:00.0-usbv3-0:1:1.0").
        Each entry must specify at least a `name`.
      '';
    };
    audio.by-path = lib.mkOption {
      type = lib.types.attrsOf (lib.types.submodule {
        options = {
          name = lib.mkOption {
            type = lib.types.str;
            description = "Logical name assigned to this audio device.";
          };
        };
      });
      default = { };
      description = ''
        Mapping of audio device IDs to configuration.
        Keys are device IDs (e.g. "pci-0000:03:00.0-usbv3-0:1:1.0").
        Each entry must specify at least a `name`.
      '';
    };
    audio.by-name = lib.mkOption {
      type = lib.types.attrsOf (lib.types.submodule {
        options = {
          name = lib.mkOption {
            type = lib.types.str;
            description = "Logical name assigned to this audio device.";
          };
        };
      });
      default = { };
      description = ''
        Mapping of audio device names to configuration.
        Keys are device names (e.g. "mySoundCard.alsa0").
        Each entry must specify at least a `name`.
      '';
    };
  };

  config = {
    services.udev.extraRules = makeV4LRuleStr config.mixos.devMap.videoCapture;
    services.pipewire.wireplumber.extraConfig =
      makeWireplumberCfg config.mixos.devMap.audio;
  };
}
