grep -rc . src/ | awk -F ':' '{sum+=$NF; print $0} END {print sum}'
