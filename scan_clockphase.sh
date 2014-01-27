#! /bin/bash

#
# clock phase scan
#

for ((i=0;i<100;i+=2)) {
    echo
    echo "################## " $i
    echo 
    sleep 1
#    ./Daq bus.[0].mpd.[0].adc.[0].clock_phase=$i bus.[0].mpd.[0].adc.[1].clock_phase=$i run.mode="histo"

    ./Daq default.bus.[0].mpd.[0].apv.[0].Latency=$i  run.mode="event"

}