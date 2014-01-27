#!/bin/bash
#
#-----------------------------
# "Loop" over config variables
#
#

execute=1
tconfig=config.temp
fconfig=config.txt

rm -rf lista.txt
touch lista.txt

for ((j=0;j<1;j++)); do
    for ((i=14;i<20;i+=2)); do      # latency
	for ((k=0;k<50;k+=5)); do   # phase
	    sed s/__LATTENCY__/$i/ config.template > $tconfig
	    sed s/__PHASE__/$k/ $tconfig > $fconfig
	    run=`cat last.run`
	    echo "$j $i $k ped $((run+1))" >>lista.txt
	    if [ $execute -eq 1 ]; then 
		./Daq -out out/iss_1102/ped -tsource 2 -evt 500
		sleep 10
	    fi
	    run=`cat last.run`
	    echo "$j $i $k sr90 $((run+1))" >>lista.txt
	    if [ $execute -eq 1 ]; then 
		./Daq -out out/iss_1102/sr90 -tsource 1 -evt 2000
		sleep 10
	    fi
	done
    done
done

# fout=`echo $ftout | sed s/\"// | sed s/\"// | sed s/\;//`

# echo $fconfig "${fout}"
