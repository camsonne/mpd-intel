/**
 * V792 - Multievent ADC
 */

#ifndef __GI_V792__
#define __GI_V792__

#include "Bus.h"

class GI_V792 {

 public:
  GI_V792(Bus *cl, uint32_t ba);
  ~GI_V792();

  int Initialize();

  /*
   */
  int Configure(int ch=0, int source=3, int pol=0);
  int SetThreshold(int ch, int_t thr);

  int Enable();
  int Disable();
  int Reset();
  int Clear();
  int Read();

 private:

  bool fDisable;

  char fName[512];
  Bus *fLink;
  uint32_t fBase;

};

#endif
