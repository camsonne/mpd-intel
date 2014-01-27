#!/bin/bash
now=$(date +"%d%m%y")
tarfile="Daq_$now.tar.gz"
tar cvzf $tarfile *.cpp *.h *.a config.txt Makefile \
	fileio/*.c* fileio/*.h fileio/Makefile \
	display/*.c* display/*.h display/Makefile

