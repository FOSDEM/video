{ config, lib, pkgs, modulesPath, ... }: {
  # for virtio kernel drivers
  imports = [ "${toString modulesPath}/profiles/qemu-guest.nix" ];

  config = {
    fileSystems."/" = {
      device = "/dev/disk/by-label/nixos";
      autoResize = true;
      fsType = "ext4";
    };

    fileSystems."/boot" = {
      device = "/dev/disk/by-label/ESP";
      fsType = "vfat";
    };

    boot.growPartition = true;
    boot.kernelParams = [ "console=ttyS0,115200" ];
    boot.loader.grub.device = "nodev";

    boot.loader.grub.efiSupport = true;
    boot.loader.grub.efiInstallAsRemovable = true;
    boot.loader.timeout = 0;

    system.build.qcow-efi =
      import "${toString modulesPath}/../lib/make-disk-image.nix" {
        inherit lib config pkgs;
        inherit (config.virtualisation) diskSize;
        format = "qcow2";
        partitionTableType = "efi";
      };
  };
}
