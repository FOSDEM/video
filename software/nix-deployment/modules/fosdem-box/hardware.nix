{ inputs, lib, config, pkgs, ... }: {
  boot.initrd.kernelModules = [ "sdhci" "sdhci-pci" "mmc_block" ];
  hardware.cpu.intel.updateMicrocode = true;
}
