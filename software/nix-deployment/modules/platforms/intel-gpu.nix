{ config, lib, pkgs, ... }: {
  # Enable OpenGL
  hardware.graphics = {
    enable = true;
    extraPackages = [ pkgs.intel-media-driver ];
  };

  environment.sessionVariables = {
    LIBVA_DRIVER_NAME = "iHD";
  }; # Force intel-media-driver

  # enable firmware blobs
  hardware.enableRedistributableFirmware = true;
}
