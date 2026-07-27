{ pkgs }: rec {
  get-asset = {
    url, name, sha256,
    convert ? ''cp -vf "$inf" "$outf"'',  # pass any shell command that references $inf and $outf
  }:
    pkgs.stdenvNoCC.mkDerivation {
      name = "asset-${name}";
      meta.description = "asset: ${name}";
      src = pkgs.fetchurl { inherit url sha256; };
      buildInputs = [ pkgs.coreutils ];
      phases = [ "unpackPhase" "installPhase" ];

      unpackPhase = "true";

      installPhase = ''
        mkdir -p "$(dirname $out/"${name}")"
        outf="$out/${name}"
        inf="$src"
        ${convert}
      '';
    };
  get-asset-path = spec: let
    der = get-asset spec;
  in
    "${der}/${spec.name}";
}
