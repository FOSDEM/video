{ pkgs, ... }: let
  index-page = pkgs.writeTextDir "index.html" ''
    <!DOCTYPE html>
    <html>
      <head>
        <meta charset="utf-8">
        <title>FOSDEM box control</title>
      </head>
      <body>
        fazant fazant fazant
        <ul>
        <li><a href="/audio">Audio Control</a></li>
        </ul>
      </body>
    </html>
  '';
in {
  networking.firewall.allowedTCPPorts = [ 80 443 ];
  security.acme.acceptTerms = true;
  services.nginx = {
    enable = true;
    virtualHosts.localhost = {
      default = true;
      enableACME = true;
      addSSL = true;
      locations."= /" = {
        root = "${index-page}";
        index = "index.html";
      };
    };
  };
}
