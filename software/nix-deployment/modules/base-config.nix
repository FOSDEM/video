{ pkgs, inputs, ... }:
let
  ring-bearers = (import ../auth/ring-bearers.nix);
in {
  system.stateVersion = "25.11";

  nix.settings.experimental-features = [ "nix-command" "flakes" ];
  nix.settings.require-sigs = false;

  boot.kernelModules = [
    "dm-crypt" # to be able to mount encrypted hard drives
  ];

  boot.kernelParams = [ "mitigations=off" ];

  environment.systemPackages = with pkgs;
    [
      # absolutely essential
      bc
      coreutils-full
      git
      htop
      killall
      less
      lsof
      moreutils
      neovim
      rsync
      sl
      sshfs
      tmux
      tree
      nettools

      # video shit
      v4l-utils

      # utils
      usbutils
      lshw
      usbtop
    ] ++ (if pkgs.stdenv.hostPlatform.system == "x86_64-linux" then
      [ pcm ]
    else
      [ ]);

  time.timeZone = "Europe/Sofia";

  services.sshd.enable = true;

  security.sudo = {
    enable = true;
    wheelNeedsPassword = false;
    extraRules = [{
      commands = [{
        command = "ALL";
        options = [ "NOPASSWD" ];
      }];
      groups = [ "wheel" ];
    }];
  };

  users.users.human = {
    home = "/home/human";
    description = "human";
    extraGroups = [ "wheel" "video" "audio" "power" "adm" "dialout" ];
    isSystemUser = false;
    isNormalUser = true;
    group = "human";
    uid = 1000;
    password = "asdf";
    shell = pkgs.zsh;
  };
  users.groups.human = { gid = 1000; };

  services.getty.autologinUser = "human";

  programs.zsh.enable = true;
  programs.zsh.interactiveShellInit = ''
    source ${pkgs.grml-zsh-config}/etc/zsh/zshrc

    # Make user colour green in prompt instead of default blue
    zstyle ':prompt:grml:left:items:user' pre '%F{green}%B'
  '';
  programs.zsh.promptInit = ""; # otherwise it'll override the grml prompt

  users.users.root.openssh.authorizedKeys.keys = ring-bearers;
  users.users.human.openssh.authorizedKeys.keys = ring-bearers;
  services.openssh = {
    enable = true;
    settings = {
      PermitRootLogin = "yes";
      PasswordAuthentication = false;
    };
  };
}
