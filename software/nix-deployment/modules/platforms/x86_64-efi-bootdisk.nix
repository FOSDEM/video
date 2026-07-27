{ config, lib, options, pkgs, modulesPath, ... }: {
  fileSystems."/boot" = {
    device = "/dev/disk/by-label/ESP";
    fsType = "vfat";
  };

  fileSystems."/" = {
    device = "/dev/disk/by-label/nixos";
    autoResize = true;
    fsType = "ext4";
  };

  boot = {
    growPartition = true;
    kernelParams = [ ];
    loader.timeout = lib.mkDefault 0;
    initrd.availableKernelModules = [ "uas" ];
    loader.grub = {
      device = "nodev";
      efiSupport = true;
      efiInstallAsRemovable = true;
    };
  };

  system.build.raw = lib.mkOverride 99
    (import "${toString modulesPath}/../lib/make-disk-image.nix" {
      inherit lib config pkgs;
      partitionTableType = "efi";
      inherit (config.virtualisation) diskSize;
      format = "raw";
    });
}
