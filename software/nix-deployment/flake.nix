{
  description = "fazant videobox deployment";
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs";

    deploy-o-matic.url = "github:dexterlb/deploy-o-matic";
    deploy-o-matic.inputs.nixpkgs.follows = "nixpkgs";

    fazantix.url = "github:FOSDEM/video-fazantix";
    fazantix.inputs.nixpkgs.follows = "nixpkgs";
  };

  outputs = { self, nixpkgs, deploy-o-matic, ... }@inputs:
    let
      dom = deploy-o-matic.lib.deployOMatic {
        templatesDir = ./templates;
        overlaysDir = ./overlays;
        moduleArgs = { inherit inputs; };
        nixpkgsConfig = ((import ./nixpkgs-global-config.nix) { inherit lib; }) // {
          flake-inputs = inputs;
        };
      };

      lib = nixpkgs.lib;
      forAllSystems = lib.genAttrs [ "x86_64-linux" ];
    in {
      nixosConfigurations = dom.nixosConfigurations;
      packages = dom.packages;
      deploy = dom.deploy;
      checks = dom.checks;
      apps = dom.apps;

      devShells = forAllSystems (system:
        let pkgs = import nixpkgs { inherit system; };
        in {
          default = pkgs.mkShell {
            packages = with pkgs; [
              OVMF.fd
              findutils
              gnumake
              rsync
            ];
          };
        });
    };
}
