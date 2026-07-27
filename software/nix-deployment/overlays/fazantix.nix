self: super:
let
  fazantix-pkgs =
    self.config.flake-inputs.fazantix.packages.${self.stdenv.hostPlatform.system};
in {
  fazantix-wayland = fazantix-pkgs.fazantix-wayland;
  fazantix-xorg = fazantix-pkgs.fazantix-xorg;
  fazantix-sample-images = fazantix-pkgs.fazantix-sample-images;
}
