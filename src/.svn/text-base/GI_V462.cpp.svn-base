/**
 * V462 - Dual Gate Generator
 *
 */

#include <stdio.h>
#include <iostream>

#include "GI_V462.h"

#define MODULE "V462"

#define AMDEF 0x39

#define FIXEDCODE0 0xF5
#define FIXEDCODE1 0xFF
#define MANUFACTID 0x382

#define REG_FIXEDCODE 0xFA
#define REG_MANUFAC   0xFC
#define REG_VERSION   0xFE

#define CH1_GEN_L 0x08
#define CH1_GEN_H 0x06
#define CH0_GEN_L 0x04
#define CH0_GEN_H 0x02

#define REG_CONTROL 0x00

using namespace std;


GI_V462::GI_V462(Bus *cl, uint32_t ba) {

  fDisable = 1;
  fLink = cl;
  fBase = ba;

  sprintf(fName,"%s[%08x]",MODULE,ba);

  cout << __FUNCTION__ << ": " << fName << " allocated" << endl;


}

GI_V462::~GI_V462() { }


int GI_V462::Initialize() {

  uint32_t ecdata;

  fLink->Read(AMDEF, 0x02, fBase+REG_FIXEDCODE, &ecdata);
  printf("%s: Fixed (0x%x%x) 0x%x\n",fName, FIXEDCODE1, FIXEDCODE0, ecdata & 0xffff);
  if (((ecdata & 0xff) != FIXEDCODE0) || (((ecdata >>8) & 0xff) != FIXEDCODE1)) {
    cout << __FUNCTION__ << ": cannot identify " << fName << " module is DISABLED" << endl;
    return 0;
  }

  fLink->Read(AMDEF, 0x02, fBase+REG_VERSION, &ecdata);
  printf("%s: Ver/SN 0x%x\n",fName, ecdata & 0xffff);
  fLink->Read(AMDEF, 0x02, fBase+REG_MANUFAC, &ecdata);
  printf("%s: Module (0x%x) 0x%x\n",fName,MANUFACTID, ecdata&0xffff);

  fDisable = 0;

  Reset();
  
  return 1;

}

/*
 *
 */


int GI_V462::Reset() {

  if (fDisable) return 0;

  Disable();

  return 1;

}

int GI_V462::Clear() {

  if (fDisable) return 0;

  Disable();

  return 1;

}

int GI_V462::Enable() { // enable module (reset inhibit)

  if (fDisable) return 0;

  return 1;

}


int GI_V462::Disable() { // disable module (set inhibit)

  if (fDisable) return 0;

  Configure(0,0);
  Start(0);

  return 1;

}

/*
 * Gate width ch 0 and 1
 */
int GI_V462::Configure(uint32_t gate0, uint32_t gate1) {

  if (fDisable) return 0;

  uint32_t data,v0,v1;

  printf("%s: Setting gates with 0x%x 0x%x\n",fName, gate0, gate1);

  data = gate0 & 0xffff;
  fLink->Write(AMDEF, 0x02, fBase+CH0_GEN_L, &data);

  data = (gate0>>16) & 0xffff;
  fLink->Write(AMDEF, 0x02, fBase+CH0_GEN_H, &data);

  data = gate1 & 0xffff;
  fLink->Write(AMDEF, 0x02, fBase+CH1_GEN_L, &data);

  data = (gate1>>16) & 0xffff;
  fLink->Write(AMDEF, 0x02, fBase+CH1_GEN_H, &data);
  

  // read
  fLink->Read(AMDEF, 0x02, fBase+CH0_GEN_L, &v0);
  fLink->Read(AMDEF, 0x02, fBase+CH0_GEN_H, &v1);
  
  data = v0 | (v1<<16);

  printf("%s: Ch0 Gate Width = 0x%x\n",fName, data);

  fLink->Read(AMDEF, 0x02, fBase+CH1_GEN_L, &v0);
  fLink->Read(AMDEF, 0x02, fBase+CH1_GEN_H, &v1);
  
  data = v0 | (v1<<16);

  printf("%s: Ch1 Gate Width = 0x%x\n",fName, data);

  return 1;

}

/*
 * Read Status Register
 */

uint32_t GI_V462::Read() {

  if (fDisable) return 0;

  uint32_t data=0;
  fLink->Read(AMDEF, 0x02, fBase+REG_CONTROL, &data);
  
  return (data&0xffff);

}

/*
 *
 */

int GI_V462::Start(int ch) { 

  if (fDisable) return 0;

  uint32_t data=0;

  if (ch==0) {
    data = 0x100;
  } else {
    data = 0x200;
  }

  fLink->Write(AMDEF, 0x02, fBase+REG_CONTROL, &data);
  
  return 1;

}

