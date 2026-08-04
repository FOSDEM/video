{ inputs, lib, config, pkgs, ... }: {
  imports = [
    ../../modules/fazant

    ../../modules/base-config.nix
    ../../modules/networking-dhcp.nix
  ];
}
