self: super: {
  fosdem-sproxy = super.stdenv.mkDerivation rec {
    pname = "sproxy";
    version = "d37138bee6000663dc273e16c12724090387a857";
    src = super.fetchFromGitHub {
      owner = "fosdem";
      repo = "video-sproxy";
      rev = "${version}";
      hash = "sha256-LYWrOcWSImLJcnztQfa4yZtzXzUL3LzufnUzoK0MpqA=";
    };
    nativeBuildInputs = [ super.gnumake super.gcc super.SDL2 ];
    installPhase = ''
      mkdir -p $out/bin
      install sproxy wait_next_second usb_reset $out/bin
    '';
  };
}
