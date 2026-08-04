{ ... }: {
  imports = [
    ./force-display-mode.nix
    ./fazantix-config-fosdem-vaapi.nix
    ./web-server.nix
    ./dev-mapping.nix
    ./fhwctl.nix
    ./audio-board.nix
    ./web-server.nix
    ./hardware.nix
    ../platforms/x86_64-efi-bootdisk.nix
    ../platforms/intel-gpu.nix
  ];
}
