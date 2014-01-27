/**
 * V462 - Scaler
 */

#ifndef __GI_V462__
#define __GI_V462__

#include <time.h>

#include "Vme.h"

class GI_V462 {

 public:
  GI_V462(Bus *cl, uint32_t ba);
  ~GI_V462();

  int Initialize();
  int Configure(uint32_t gate0=0x1, uint32_t gate1=0x1);

  int Enable();
  int Disable();
  int Reset();
  int Clear();
  uint32_t Read();
  int Start(int ch);

 private:

  bool fDisable;
  char fName[512];
  Bus *fLink;
  uint32_t fBase;

};

#endif
