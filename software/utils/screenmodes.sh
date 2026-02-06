#! /bin/bash
POSITIONAL_ARGS=()

# Check if wlr-randr is installed.
which wlr-randr 2>/dev/null || echo "This script requires wlr-randr. Please install it first." && exit

# Process parameters.
while [[ $# -gt 0 ]]; do
  case $1 in
    -d|--display)
      display="$2"
      resolutions=$(wlr-randr|grep -A100 ^$display|grep Hz|sed -e 's/ px, /@/g' -e 's/ //g' -e 's/Hz.*//g')
      shift # past argument
      shift # past value
      ;;
    -h|--help)
      shift # past argument
      echo "Usage:
	-d or --display
	  Specify the display you want to use. Hint: get this value from wlr-randr.
	-h or --help
	  Print this help message.
	-l or --listresolutions
	  Requires -d or --display to also be set.
	  Prints all possible combinations of resolution and refresh rate as negotiated between this computer and the display chosen to stdout.
	-x or --execute
	  Starts a continuous loop through all possible combinations of resolution and refresh rate as negotiated between this computer and the display chosen. Accepts 
	  Requires -d or --display to also be set.
	  Accepts an optional loop interval in seconds as a parameter."
      exit
      ;;
    -l|--list-resolutions)
      echo $resolutions
      exit
      ;;
    -x|--execute)
      while true; do
        for r in "${resolutions[@]}"; do
          wlr-randr --output "$display" --mode $r && sleep 10s
        done
      done      
    -*|--*)
      echo "Unknown option $1"
      exit 1
      ;;
    *)
      POSITIONAL_ARGS+=("$1") # save positional arg
      shift # past argument
      ;;
  esac
done

set -- "${POSITIONAL_ARGS[@]}" # restore positional parameters

#mapfile -t resolutions < resolutions.txt

