{ ... }:
let
  fosboxes = [ "00" "58" "89" ];
  mkFosbox = id: rec {
    hostname = "fazant-${id}";
    system = "x86_64-linux";
    image = { format = "raw"; };
    moduleArgs = { inherit hostname system; };
    deploy = {
      # hostname = "box${id}.video.fosdem.org";
      hostname = "fazant-${id}.pit.protopit.eu";
      sshUser = "human";
    };
  };
in map mkFosbox fosboxes
