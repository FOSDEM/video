{ lib, ... }: {
  allowUnfreePredicate = pkg: builtins.elem (lib.getName pkg) [
    # allow these unfree (sadly) packages
    "teensy-udev-rules"
  ];
}
