#!/bin/sh
mkdir ./captured
avconv -i ./racecar.avi -ss 0.3 -r 1 -t 13 ./captured/racecar_%02d.png
