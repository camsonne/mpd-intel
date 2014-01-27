/**
 * V513 - I/O Register Module
 */

#ifndef __GI_V513__
#define __GI_V513__

#include "Vme.h"

class GI_V513 {

 public:
  GI_V513(Bus *cl, uint32_t ba);
  ~GI_V513();

  int Initialize();
  //  int Configure();
  /*
   * direction: 0=output, 1=input
   * polarity:  0=negative, 1=positive
   * input mode: 0=glitched, 1=normal
   * transfer mode: 0=transparent, 1=ext. strobe
   */
  int Configure(int ch=0, int dir=1, int pol=0, int mode=1, int transfer=0);

  int Enable();
  int Disable();
  int Reset();
  int Clear();
  int Read();
  int Write(uint16_t data);

 private:

  bool fDisable; // if set, the module is disabled

  char fName[512];
  Bus *fLink;
  uint32_t fBase;

};

#endif
