/**
 * V260 - Scaler
 *
 */

#include <stdio.h>
#include <iostream>
#include <iomanip>

#include "GI_V260.h"

#define MODULE "V260"

#define AMDEF 0x39

#define FIXEDCODE0 0xF5
#define FIXEDCODE1 0xFA
#define MANUFAC    0x80E

#define REG_VERSION 0xFE
#define REG_MANUFAC 0xFC
#define REG_FIXEDCODE 0xFA

#define REG_INTERRUPT 0x58
#define REG_SCALEINC  0x56
#define REG_INHRESET  0x54
#define REG_INHSET    0x52
#define REG_CLEARSCA  0x50

#define REG_COUNT0    0x10
#define REG_COUNTSTEP 0x4

#define REG_CLEARINT   0x0C
#define REG_DISABLEINT 0x0A
#define REG_ENABLEINT  0x08
#define REG_INTLEVEL   0x06
#define REG_INTVECTOR  0x04

using namespace std;


GI_V260::GI_V260(Bus *cl, uint32_t ba) {

  fDisable = 1;
  fLink = cl;
  fBase = ba;

  sprintf(fName,"%s[%08x]",MODULE,ba);

  cout << __FUNCTION__ << ": " << fName << " allocated" << endl;


}

GI_V260::~GI_V260() { }


int GI_V260::Initialize() {

  uint32_t ecdata;
  int i;

  for (i=0;i<16;i++) {
    fVal[i] = 0;
    fRate[i]=0;
  }

  fLink->Read(AMDEF, 0x02, fBase+REG_FIXEDCODE, &ecdata);
  printf("%s: Fixed (0x%x%x) 0x%x\n",fName, FIXEDCODE1, FIXEDCODE0, ecdata & 0xffff);
  if (((ecdata & 0xff) != FIXEDCODE0) || (((ecdata >>8) & 0xff) != FIXEDCODE1)) {
    cout << __FUNCTION__ << ": cannot identify " << fName << " module is DISABLED" << endl;
    return 0;
  }

  fLink->Read(AMDEF, 0x02, fBase+REG_VERSION, &ecdata);
  printf("%s: Ver/SN 0x%x\n",fName, ecdata & 0xffff);
  fLink->Read(AMDEF, 0x02, fBase+REG_MANUFAC, &ecdata);
  printf("%s: Module (0x%x) 0x%x\n",fName,MANUFAC, ecdata&0xffff);

  fDisable = 0;

  Reset();
  
  fLink->Read(AMDEF, 0x02, fBase+REG_DISABLEINT, &ecdata); // disable interrupt

  return 1;

}

/*
 *
 */


int GI_V260::Reset() {

  if (fDisable) return 0;

  uint32_t ecdata;

  fLink->Write(AMDEF, 0x02, fBase+REG_CLEARSCA, &ecdata); 

  return 1;

}

int GI_V260::Clear() {

  if (fDisable) return 0;

  uint32_t ecdata;

  fLink->Write(AMDEF, 0x02, fBase+REG_CLEARSCA, &ecdata);

  return 1;

}

int GI_V260::Enable() { // enable module (reset inhibit)

  if (fDisable) return 0;

  uint32_t ecdata;

  fLink->Write(AMDEF, 0x02, fBase+REG_INHRESET, &ecdata);

  return 1;

}


int GI_V260::Disable() { // disable module (set inhibit)

  if (fDisable) return 0;

  uint32_t ecdata;

  fLink->Write(AMDEF, 0x02, fBase+REG_INHSET, &ecdata);

  return 1;

}


uint32_t GI_V260::Read(int ch) {

  if (fDisable) return 0;

  uint32_t data=0;
  fLink->Read(AMDEF, 0x04, fBase+REG_COUNT0+ch*REG_COUNTSTEP, &data);
  
  return (data&0xffffff);

}

int GI_V260::ReadBunch(int ch0, int ch1) {

  if (fDisable) return 0;

  int i;
  uint32_t cVal;
  time_t cTime;

  time(&cTime);
  for (i=ch0;i<=ch1;i++) {
    cVal = Read(i);
    fRate[i] = ((float) (cVal - fVal[i])/((float) (cTime-fTime)));
    fVal[i] = cVal;
  };
  fTime = cTime;
  
  return 0;

}


int GI_V260::Write() { // increment scaler

  if (fDisable) return 0;

  uint32_t data=0;
  fLink->Write(AMDEF, 0x02, fBase+REG_SCALEINC, &data);
  
  return (int) (data&0xffff);

}

//
// Print "differential" rate
//
int GI_V260::PrintRate(int ch0, int ch1, time_t time0) {

  if (fDisable) return 0;

  int i;

  cout << "\nV260::" << __FUNCTION__ << " ( " << dec << setw(6) << fTime-time0 << " s ) ";
  for (i=ch0;i<=ch1;i++) {
    cout << setprecision(3) << setw(6) << fRate[i] << " ";
  }
  cout << " Hz (Running) [ ";
  
  for (i=ch0;i<=ch1;i++) {
    cout << setw(6) << fVal[i] << " ";
  }
  cout << " Total counts ]" << endl;

  return 0;

}

//
// Print total rate (since the beginning of acquisition)
//

int GI_V260::TotalRate(int ch0, int ch1, time_t time0) {

  if (fDisable) return 0;

  int i;

  float delta;

  cout << "\nV260::" << __FUNCTION__ << " ( " << dec << setw(6) << fTime-time0 << " s ) ";

  delta = (float) (fTime-time0);

  if (delta == 0.) {
    delta = 0.000001;
  }

  for (i=ch0;i<=ch1;i++) {
    cout << setprecision(3) << setw(6) << fVal[i]/delta << " ";
  }
  cout << " Hz (Total) [";

  for (i=ch0;i<=ch1;i++) {
    cout << setw(6) << fVal[i] << " ";
  }
  cout << " Total counts ]" << endl;

  return 0;

}


//
//
//

int GI_V260::WriteScaler(int ch0, int ch1, time_t time0, std::string fout) {

  int delta = (int) (fTime-time0);

  struct tm *timestruct;
  char *ts;

  timestruct = localtime(&fTime);
  ts = asctime(timestruct); // not save in multi proc/thread
  ts[strlen(ts)-1] = 0;

  FILE *ff;
  ff = fopen(fout.data(),"a");

  if (ff) {

    fprintf(ff,"%s / %d :",ts,delta);
    for (int i=ch0;i<=ch1;i++) {
      fprintf(ff," %10d", fVal[i]);
    }
    fprintf(ff," Total counts\n");

    fclose(ff);

    return 0;

  }

  return 1;

}


/*
int GI_V260::WriteBinRate(int ch0, int ch1, time_t time0) {

  int i;

  cout << "\nV260::" << __FUNCTION__ << " ( " << dec << fTime-time0 << " s ) ";
  for (i=ch0;i<=ch1;i++) {
    cout << fRate[i] << " ";
  }
  cout << " Hz" << endl;

  return 0;

}
*/
