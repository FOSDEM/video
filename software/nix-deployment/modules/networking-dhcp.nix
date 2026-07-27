{ lib, hostname, ... }: {
  networking = rec {
    hostName = hostname;
    domain = "video.fosdem.org";
    dhcpcd.enable = true;
    hosts = {
      "::1" = [ "${hostName}.${domain}" hostName ];
      "127.0.0.1" = [ "${hostName}.${domain}" hostName ];
    };
    firewall = { allowedTCPPorts = [ 22 5900 ]; };
    # nameservers = [ "2001:67c:21bc:1e::53" "185.117.82.68" ];
  };
}
