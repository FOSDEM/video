wlr-randr|grep -A100 ^DP-1|grep Hz|sed -e 's/ px, /@/g'|sed -e 's/ //g'|sed -e 's/Hz.*//g'
