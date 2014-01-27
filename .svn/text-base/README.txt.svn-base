
Data Acquisition System based on APV25 chip, initially developed for the readout of GEM tracking chamber for the SBS spectrometer at JLab.

Required packages:
  Require root for display
  Require libconfig-1.4.5
  CAENVME library

VME Controller:
  Currently work with CAEN USB-VME interface VX1718
  Tested also with Struck optical link controller (model SIS3104)
  Partially working with SBS3 (UVa controller)

Sources:
  Main program is Main.cpp
  BUS/VME interface: Vme*, Bus.h
  VME MPD high level interface: GI_MPD
  VME MPD low level interface: AdcVme64x, AdcConfig, ApvConfig, Histo, TrigConfig
  I2C interface: i2c.cpp
  Ancillary lib: Logger, Configure, DaqControl
  I/O file manipulation in directory: fileio/
  Analysis software (based on root) in directory: display/
  BaselinePlot.cpp is used to display data acquired in "histo" mode

  Default Config File: config_new.txt
  Default Output Directory: out/@@@
  Map configuration files (for analysis) in directory: cfg/

  Documentation in directory doc/
  
To build the executable "Daq"
 from the main directory:
  make clean
  cd src
  make clean
  make 
  cd ..
  make 


To run Daq:

./Daq -h

shows the in-line parameters

TBD
---
If out dir not present issue an error
Add controller run time selection
Meaning of rotary ??? must match the position ???
Histogramming mode still not working



