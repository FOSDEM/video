{ inputs, lib, config, pkgs, ... }: {
  imports = [
    ../../modules/fosdem-box
    ../../modules/fazant

    ../../modules/base-config.nix
    ../../modules/networking-dhcp.nix
    # ../../common/audio-config.nix
    # ../../common/fazantix
    # ../../common/fosdem-box-status
    # ../../common/fosdem-audio-board
  ];
}
