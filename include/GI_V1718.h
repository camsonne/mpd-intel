/**
 * V1718 - as I/O Register Module, scaler, pulser and something more
 */

#ifndef __GI_V1718__
#define __GI_V1718__

#include "Bus.h"

class GI_V1718 {

 public:
  GI_V1718(Bus *cl, uint32_t ba);
  ~GI_V1718();

  int Initialize();
  //  int Configure();
  /*
   */
  int Configure(int ch=0, int source=3, int pol=0);
  int setPulser(int ch, int period=10, int width=1, int range=0, int pulses=2);

  int Enable();
  int Disable();
  int Reset();
  int Clear();
  int Read();
  int Write(int ch, uint32_t data=1);

 private:

  char fName[512];
  Bus *fLink;
  uint32_t fBase;

  int outputReg(int ch, int val);
  int setLEDPol(int ch, int pol);
  int setOutMux(int ch, int source, int pol);
  int setInMux(int ch, int pol);

};

#endif
