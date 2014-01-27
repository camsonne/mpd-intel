/**
 * V260 - Scaler
 */

#ifndef __GI_V260__
#define __GI_V260__

#include <time.h>
#include <iostream>
#include <string.h>

#include "Vme.h"

class GI_V260 {

 public:
  GI_V260(Bus *cl, uint32_t ba);
  ~GI_V260();

  int Initialize();
  //  int Configure();
  /*
   * direction: 0=output, 1=input
   * polarity:  0=negative, 1=positive
   * input mode: 0=glitched, 1=normal
   * transfer mode: 0=transparent, 1=ext. strobe
   */
  //  int Configure(int ch=0, int dir=1, int pol=0, int mode=1, int transfer=0);

  int Enable();
  int Disable();
  int Reset();
  int Clear();
  uint32_t Read(int ch);
  int Write();

  int ReadBunch(int ch0, int ch1);
  int PrintRate(int ch0, int ch1, time_t time0);
  int TotalRate(int ch0, int ch1, time_t time0);
  int WriteScaler(int ch0, int ch1, time_t time0, std::string fout);

 private:

  uint32_t fVal[16]; // latest values from Scaler
  float fRate[16]; // latest rates
  time_t fTime; // time of last read

  bool fDisable;
  char fName[512];
  Bus *fLink;
  uint32_t fBase;

};

#endif
