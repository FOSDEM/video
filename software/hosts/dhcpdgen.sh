awk '{print "host "$4" { # "$2"\n\tfixed-address "$5";\n\thardware ethernet "$3";\n}"}'
