{ modulesPath, lib, ... }: {
  imports = [
    # turns out that the lxc-container profile is actually compatible
    # with nspawn
    "${toString modulesPath}/virtualisation/lxc-container.nix"
  ];
}
