{ ... }: {
  services.udev.extraRules = ''
    SUBSYSTEM=="tty", ATTRS{idVendor}=="f05d", ATTRS{idProduct}=="4001", SYMLINK+="tty_fosdem_box_ctl"
  '';
}
