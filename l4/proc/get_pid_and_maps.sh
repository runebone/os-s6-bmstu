#!/bin/sh

# awk '/1) pid=/{print $2}/^maps:/{flag=1; next} /^$/{flag=0} flag {print $1}'
awk '/1) pid=/{sub(/1) pid=/, ""); print $1}/^maps:/{flag=1; next} /^$/{flag=0} flag {print $1}'
