{ ... }: {
  hardware.display = {
    edid = {
      enable = true;
      modelines = {
        # assumes CEA-861-compatible display
        # use this to calculate timings for another type of display:
        # https://tomverbeure.github.io/video_timings_calculator
        "F_1080_60" =
          "148.5 1920 2008 2052 2200 1080 1084 1089 1125 +HSync +VSync";
        "F_1080_30" =
          "74.25 1920 2008 2052 2200 1080 1084 1089 1125 +HSync +VSync";
      };
    };
    outputs = {
      "HDMI-A-1".mode = "d"; # off
      "HDMI-A-2" = {
        edid = "F_1080_30.bin";
        mode = "e"; # force on
      };
    };
  };
}
