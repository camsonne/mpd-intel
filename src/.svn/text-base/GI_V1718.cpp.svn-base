/**
 * V1718 - (as) IO Register, Scaler and Pulser and something more
 * when not a master of the bus!
 */

#include <stdio.h>
#include <iostream>

#include "GI_V1718.h"

#define MODULE "V1718"

#define AMDEF 0x09

#define REG_LM_C   0x2C
#define REG_LM_ADH   0x29
#define REG_LM_ADL   0x28
#define REG_DISP_PC2   0x25
#define REG_DISP_PC1   0x23
#define REG_DISP_DTH   0x23
#define REG_DISP_DTL   0x22
#define REG_DISP_ADH   0x21
#define REG_DISP_ADL   0x20
#define REG_SCALER1   0x1D
#define REG_SCALER0   0x1C
#define REG_PULSEB_1  0x1A
#define REG_PULSEB_0  0x19
#define REG_PULSEA_1  0x17
#define REG_PULSEA_0  0x16
#define REG_LED_POL_C 0x13
#define REG_OUT_MUX_C 0x12
#define REG_IN_MUX_C  0x11
#define REG_OUT_REG_C 0x10
#define REG_LED_POL_S 0x0D
#define REG_OUT_MUX_S 0x0C
#define REG_IN_MUX_S  0x0B
#define REG_OUT_REG_S 0x0A
#define REG_IN_REG    0x08

#define REG_IRQ_STAT 0x05
#define REG_FL_ENA   0x04
#define REG_FW_DWNLD 0x03
#define REG_FW_REV   0x02
#define REG_VME_CTRL 0x01
#define REG_STATUS   0x00

using namespace std;


GI_V1718::GI_V1718(Bus *cl, uint32_t ba) {

  fLink = cl;
  fBase = ba;

  sprintf(fName,"%s[%08x]",MODULE,ba);

  cout << __FUNCTION__ << ": " << fName << " allocated" << endl;


}

GI_V1718::~GI_V1718() { }


int GI_V1718::Initialize() {

  uint16_t ecdata;

  fLink->Read(AMDEF, 0x02, fBase+REG_FW_REV, &ecdata);
  printf("%s: Firmware Revision: 0x%x\n",fName, ecdata & 0xffff);

  /*
  if (((ecdata & 0xff) != FIXEDCODE0) || (((ecdata >>8) & 0xff) != FIXEDCODE1)) {
    cout << __FUNCTION__ << ": cannot identify " << fName << endl;
    return 0;
  }
  */

  Reset();
  
  return 1;

}

/*
 *  source: 0 = vme bus line
 *          1 = in0 AND in 1
 *          2 = pulse A/B out / Scaler End Count
 *          3 = output register
 *
 *  pol: 0 = direct, 1 = Inverted
 *
 */

int GI_V1718::Configure(int ch, int source, int pol) {

  if (ch<5) {
    setOutMux(ch, source, pol);
  } else {
    setInMux(ch-5, pol);
  }

  setLEDPol(ch, pol);

  return 1;

}

/*
 * ch: 0 = A, 1= B
 */
int GI_V1718::setPulser(int ch, int period, int width, int range, int pulses) {

  uint16_t data;

  data = (period &0xFF) | ((width << 8) & 0xFF00); 
  fLink->Write(AMDEF, 0x02, fBase+REG_PULSEA_0 + ch*3, &data);
  data = (pulses & 0xFF) | ((range << 8) & 0x300);
  fLink->Write(AMDEF, 0x02, fBase+REG_PULSEA_1 + ch*3, &data);

  cout << __FUNCTION__ << ": " << fName << " " << ch << " " << hex << data<< endl;
  
  return 0;
}

/*
 * ch=0,4 out, ch5,6 in
 * pol = 0 direct, 1 invert
 */

int GI_V1718::setLEDPol(int ch, int pol) {

  uint16_t data, m0;

  m0 = 1 << ch;

  fLink->Write(AMDEF, 0x02, fBase+REG_LED_POL_C, &m0);

  data = (pol &1) << ch;

  fLink->Write(AMDEF, 0x02, fBase+REG_LED_POL_S, &data);

  cout << __FUNCTION__ << ": " << fName << " LED polarity = " << ch << " " << hex << data<< endl;

  return (int) data;

}

/* only in0 and in 1 at the moment
 * 
 */ 
int GI_V1718::setInMux(int ch, int pol) {

  uint16_t data, m0;
  
  m0 = 1 << (ch*2);
  data = pol << (ch*2);

  fLink->Write(AMDEF, 0x02, fBase+REG_IN_MUX_C, &m0);

  fLink->Write(AMDEF, 0x02, fBase+REG_IN_MUX_S, &data);

  cout << __FUNCTION__ << ": " << fName << " intMUX = " << hex << data << endl;

  return (int) data;

}


int GI_V1718::setOutMux(int ch, int source, int pol) {

  uint16_t data, datain;

  int s, p;
  
  uint16_t m0,m1;

  s = (source&3) << (ch*2); 
  p = (pol&1) << (ch+10);

  m0= (0x3 << (ch*2)) + (1 << (ch+10));
  m1 = (0x7FFF ^ m0);

  fLink->Read(AMDEF, 0x02, fBase+REG_OUT_MUX_S, &datain);

  fLink->Write(AMDEF, 0x02, fBase+REG_OUT_MUX_C, &m0);

  cout << __FUNCTION__ << ": " << fName << " outMUX = " << hex << datain;

  data = (s & 0x3FF)|(p&0x7C00);

  fLink->Write(AMDEF, 0x02, fBase+REG_OUT_MUX_S, &data);

  cout << " -> " << hex << ((datain&m1) || data) << endl;

  return (int) data;

}

int GI_V1718::Reset() {

  uint16_t ecdata=0xFFFF;

  fLink->Write(AMDEF, 0x02, fBase+REG_OUT_REG_C, &ecdata); // clear output register
  fLink->Write(AMDEF, 0x02, fBase+REG_OUT_MUX_C, &ecdata); // clear output multiplex
  fLink->Write(AMDEF, 0x02, fBase+REG_IN_MUX_C, &ecdata); // clear input mux
  fLink->Write(AMDEF, 0x02, fBase+REG_LED_POL_C, &ecdata); // clear led polarity

  return 1;

}

int GI_V1718::Clear() {

  uint16_t ecdata=1;

  fLink->Write(AMDEF, 0x02, fBase+REG_OUT_REG_C, &ecdata); // clear output register


  return 1;

}

int GI_V1718::Read() {

  uint16_t data=0;
  fLink->Read(AMDEF, 0x02, fBase+REG_IN_REG, &data);
  
  return (int) (data&0xffff);

}


/*
 * write the output reg channel
 */

int GI_V1718::Write(int ch, uint32_t data) {


  outputReg(ch, data);

  return 0;

}

/*
 * val = 1 - set, 0 - clear 
 */

int GI_V1718::outputReg(int ch, int val)  {


  uint16_t data;

  data = 1 << (ch+6);

  if (val) {
    fLink->Write(AMDEF, 0x02, fBase+REG_OUT_REG_S, &data);
  } else {
    fLink->Write(AMDEF, 0x02, fBase+REG_OUT_REG_C, &data);
  }

  return val;

}
