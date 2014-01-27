/**
 * V1495 - General Purpose VME Board
 */

#ifndef __GI_V1495__
#define __GI_V1495__

#include "Vme.h"

class GI_V1495 {

 public:
  GI_V1495(Bus *cl, uint32_t ba);
  ~GI_V1495();

  int Initialize();
  //  int Configure();

  int Configure(int ch=0, int dir=1, int pol=0, int mode=1, int transfer=0);

  int Enable();
  int Disable();
  int Reset();
  int Clear();
  int Read();
  int Write(uint16_t data);

  int CIO_Init();
  int CIO_PortConfig(int port, int dir, int format);
  int CIO_ModeConfig(int dsel, int wmode, int lope, int pulse_mode);
  int CIO_Clear();
  uint32_t CIO_Read(int port);
  int CIO_Write(int port, uint32_t val);
  uint32_t CIO_ReadLH(int port, int lh);
  int CIO_WriteLH(int port, uint32_t val, int lh);


 private:

  bool fDisable;
  char fName[512];
  Bus *fLink;
  uint32_t fBase;

};

#endif
