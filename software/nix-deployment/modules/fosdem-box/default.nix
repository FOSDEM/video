{ ... }: {
  imports = [
    ./dev-mapping.nix
    ./hardware.nix
    ../platforms/x86_64-efi-bootdisk.nix
    ../platforms/intel-gpu.nix
  ];
}
