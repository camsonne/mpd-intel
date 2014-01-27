/**
 * V513 - IO Register
 *
 */

#include <stdio.h>
#include <iostream>

#include "GI_V513.h"

#define MODULE "V513"

#define FIXEDCODE0 0xF5
#define FIXEDCODE1 0xFA
#define MANUFAC    0x832

#define REG_VERSION 0xFE
#define REG_MANUFAC 0xFC
#define REG_FIXEDCODE 0xFA
#define REG_CLEARINPUT 0x48
#define REG_INITIALIZE 0x46
#define REG_CLEARSTROBE 0x44
#define REG_MODULERESET 0x42
#define REG_CLEARINTERR 0x40

#define REG_CH0STATUS 0x10
#define REG_CHSTEP 0x2

#define REG_INTMASK 0x08
#define REG_STROBE  0x06
#define REG_INPUT   0x04
#define REG_OUTPUT  0x04
#define REG_INTLEVEL 0x02
#define REG_INTVECTOR 0x00

using namespace std;


GI_V513::GI_V513(Bus *cl, uint32_t ba) {

  fDisable = 1;
  fLink = cl;
  fBase = ba;

  sprintf(fName,"%s[%08x]",MODULE,ba);

  cout << __FUNCTION__ << ": " << fName << " allocated" << endl;


}

GI_V513::~GI_V513() { }


int GI_V513::Initialize() {

  uint32_t ecdata;

  fLink->Read(0x09, 0x02, fBase+REG_FIXEDCODE, &ecdata);
  printf("%s: Fixed (0x%x%x) 0x%x\n",fName, FIXEDCODE1, FIXEDCODE0, ecdata & 0xffff);
  if (((ecdata & 0xff) != FIXEDCODE0) || (((ecdata >>8) & 0xff) != FIXEDCODE1)) {
    cout << __FUNCTION__ << ": cannot identify " << fName << " module is DISABLED" << endl;
    return 0;
  }

  fLink->Read(0x09, 0x02, fBase+REG_VERSION, &ecdata);
  printf("%s: Ver/SN 0x%x\n",fName, ecdata & 0xffff);
  fLink->Read(0x09, 0x02, fBase+REG_MANUFAC, &ecdata);
  printf("%s: Module (0x%x) 0x%x\n",fName,MANUFAC, ecdata&0xffff);

  fDisable = 0;
  Reset();
  
  return 1;

}

/*
 *
 */

int GI_V513::Configure(int ch, int dir, int pol, int mode, int transfer) {

  if (fDisable) { return 0; };

  uint32_t data;

  data = (dir&1) | ((pol&1)<<1) | ((mode&1)<<2) | ((transfer&1)<<3);

  cout << __FUNCTION__ << ": " << fName << " config register " << ch << " with " << hex << data << endl;
  fLink->Write(0x09, 0x02, fBase+REG_CH0STATUS+ch*REG_CHSTEP, &data);

  return 1;

}


int GI_V513::Reset() {

  if (fDisable) { return 0; };

  uint32_t ecdata;

  fLink->Write(0x09, 0x02, fBase+REG_MODULERESET, &ecdata); // status default

  return 1;

}

int GI_V513::Clear() {

  if (fDisable) { return 0; };

  uint32_t ecdata;

  fLink->Write(0x09, 0x02, fBase+REG_CLEARINPUT, &ecdata); // status default

  return 1;

}

int GI_V513::Read() {

  if (fDisable) { return 0; };

  uint16_t data=0;

  fLink->Read(0x09, 0x02, fBase+REG_INPUT, &data);
  
  return (int) (data&0xffff);

}


int GI_V513::Write(uint16_t data) {

  if (fDisable) { return 0; };

  fLink->Write(0x09, 0x02, fBase+REG_OUTPUT, &data);
  
  return (int) (data&0xffff);

}
