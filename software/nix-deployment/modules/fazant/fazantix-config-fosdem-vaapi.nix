{ pkgs, inputs, system, ... }: let
  get-asset-tools = import ../tools/get-asset.nix { inherit pkgs; };
  inherit (get-asset-tools) get-asset-path;
in {
  imports = [ inputs.fazantix.nixosModules.${system}.fazantix ];

  networking.firewall.allowedTCPPorts = [ 8899 8898 ];

  services.fazantix.enable = true;
  services.fazantix.openApiPort = true;
  services.fazantix.config = {
    sources = {
      background = {
        type = "image";
        path = get-asset-path {
          url = "https://rnd.qtrp.org/fazant-2026.png";
          sha256 = "sha256-whRLw1SziDydlDdhGNl0NekC2Ora0CQe0pqDYH3Re7Y=";
          name = "fazant-2026.png";
        };
      };
      camera = {
        type = "v4l";
        path = "/dev/video-cam";
        fmt = "yuyv";
        fps = 30;
        frames = {
          width = 1920;
          height = 1080;
          num_allocated_frames = 15;
        };
        num_frames_in_writing = 10;
      };
      slides = {
        type = "v4l";
        path = "/dev/video-slides";
        fmt = "yuyv";
        fps = 30;
        frames = {
          width = 1920;
          height = 1080;
          num_allocated_frames = 15;
        };
        num_frames_in_writing = 10;
      };
    };
    scenes = {
      break = {
        tag = "brk";
        layers = [{
          source = "background";
          transform = {
            x = 0;
            y = 0;
            scale = 1;
            opacity = 1;
          };
        }];
      };
      cam-over-slides = {
        layers = [
          {
            source = "background";
            transform = {
              x = 0;
              y = 0;
              scale = 1;
              opacity = 1;
            };
          }
          {
            source = "camera";
            transform = {
              left = -2.0e-2;
              top = -2.0e-2;
              scale = 0.8;
              opacity = 1;
            };
            warp = {
              opacity = 0;
              cx = 0.5;
              cy = 0.5;
              scale = 0.1;
            };
          }
          {
            source = "slides";
            transform = {
              right = -2.0e-2;
              bottom = -2.0e-2;
              scale = 0.25;
              opacity = 1;
            };
            warp = {
              opacity = 1;
              scale = 1.0e-3;
              right = -4.0e-2;
              bottom = -0.1;
            };
          }
        ];
      };
      slides-over-cam = {
        layers = [
          {
            source = "background";
            transform = {
              x = 0;
              y = 0;
              scale = 1;
              opacity = 1;
            };
          }
          {
            source = "slides";
            transform = {
              left = -2.0e-2;
              top = -2.0e-2;
              scale = 0.8;
              opacity = 1;
            };
            warp = {
              opacity = 0;
              cx = 0.5;
              cy = 0.5;
              scale = 0.1;
            };
          }
          {
            source = "camera";
            transform = {
              right = -2.0e-2;
              bottom = -2.0e-2;
              scale = 0.25;
              opacity = 1;
            };
            warp = {
              opacity = 1;
              scale = 1.0e-3;
              right = -4.0e-2;
              bottom = -0.1;
            };
          }
        ];
      };
      side-by-side = {
        layers = [
          {
            source = "background";
            transform = {
              x = 0;
              y = 0;
              scale = 1;
              opacity = 1;
            };
          }
          {
            source = "slides";
            transform = {
              x = 3.0e-2;
              y = 0.25;
              scale = 0.45;
              opacity = 1;
            };
          }
          {
            source = "camera";
            transform = {
              x = 0.52;
              y = 0.25;
              scale = 0.45;
              opacity = 1;
            };
          }
        ];
      };
      full-slides = {
        layers = [
          {
            source = "background";
            transform = {
              x = 0;
              y = 0;
              scale = 1;
              opacity = 1;
            };
          }
          {
            source = "slides";
            transform = {
              x = 0;
              y = 0;
              scale = 1;
              opacity = 1;
            };
          }
        ];
      };
      full-cam = {
        layers = [
          {
            source = "background";
            transform = {
              x = 0;
              y = 0;
              scale = 1;
              opacity = 1;
            };
          }
          {
            source = "camera";
            transform = {
              x = 0;
              y = 0;
              scale = 1;
              opacity = 1;
            };
          }
        ];
      };
    };
    sinks = {
      projector = {
        type = "window";
        frames = {
          width = 1920;
          height = 1080;
        };
        default_scene = "full-slides";
        transition_time_ms = 1500;
      };
      stream = {
        type = "ffmpeg_stdin";
        cmd = ''
          ${pkgs.ffmpeg}/bin/ffmpeg \
            -y -v verbose \
            -init_hw_device vaapi=intel:/dev/dri/renderD128 \
            -fflags '+genpts+nobuffer+igndts' \
            -hwaccel vaapi -hwaccel_output_format vaapi \
            -hwaccel_device intel -filter_hw_device intel  \
            -flags low_delay \
            -probesize 32 \
            -analyzeduration 0 \
            -f rawvideo -video_size ''${SIZE} \
            -pixel_format rgba -framerate ''${RATE} -re -i - \
            -itsoffset 0.16 -f alsa -sample_rate 48000 -channels 2 -i hw:0 \
            -threads:0 0 \
            -filter_complex "[1:a] volume=volume=0dB [ain]; [0:v] format=nv12,hwupload [vout]" \
            -map '[vout]:0' \
            -c:v h264_vaapi -rc_mode CBR \
            -maxrate:v:0 5000k -bufsize:v:0 8192k \
            -b:v:0 3000k \
            -qmin:v:0 1 \
            -fps_mode cfr \
            -pix_fmt yuv420p \
            -map '[ain]:0' \
            -ac 2 -strict -2 -c:a aac -b:a 128k -ar 48000 \
            -f mpegts pipe:1 \
            | ${pkgs.fosdem-sproxy}/bin/sproxy
        '';
        frames = {
          width = 1920;
          height = 1080;
          num_allocated_frames = 5;
        };
        default_scene = "slides-over-cam";
        transition_time_ms = 1500;
      };
    };
    api = {
      bind = ":8000";
      enable_profiler = true;
    };
    fallback_colour = "#ebac54";
    bg_colour = "#54aceb";
    base_framerate = -1;
  };
}
