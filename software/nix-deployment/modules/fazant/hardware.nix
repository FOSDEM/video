{ inputs, lib, config, pkgs, ... }: {
  boot.initrd.kernelModules = [ "sdhci" "sdhci-pci" "mmc_block" ];
  boot.kernelParams = [ "sdhci.debug_quirks=0x20000" ]; # fix emmc instability on radxa
  hardware.cpu.intel.updateMicrocode = true;
}
