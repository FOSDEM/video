awk '{ split($5, a, "."); print ""a[4]"\tIN\tPTR\t"$4".video.fosdem.org." }'
