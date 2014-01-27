/**
 * V792 - Multievent ADC
 *
 */

#include <stdio.h>
#include <iostream>

#include "GI_V792.h"

#define MODULE "V792"

#define AMDEF 0x09
#define DTSIZ 0x02

#define REQ_OUT_BUF  0x0000
#define REG_FW_REV   0x1000
#define REG_GEO_ADDR 0x1002
#define REG_MCST_ADDR 0x1004
#define REG_BIT1_SET 0x1006
#define REG_BIT1_CLR 0x1008
#define REG_INT_LEV  0x100A
#define REG_INT_VEC  0x100C
#define REG_STATUS_1 0x100E
#define REG_CTRL_1   0x1010
#define REG_ADER_H   0x1012
#define REG_ADER_L   0x1014
#define REG_SS_RESET 0x1016
#define REG_MCST_CTRL 0x101A
#define REG_EVT_TRG  0x1020
#define REG_STATUS_2 0x1022
#define REG_EVT_COUNT_L 0x1024
#define REG_EVT_COUNT_H 0x1026
#define REG_EVT_INC 0x1028
#define REG_EVT_OFF 0x102A
#define REG_LOAD_TEST 0x102C
#define REG_FCLR_WIN 0x102E
#define REG_BIT2_SET 0x1032
#define REG_BIT2_CLR 0x1034
#define REG_MEMORY_TEST 0x1036
#define REG_MEMORY_H 0x1038
#define REG_MEMORY_L 0x1038
#define REG_CRATE_SEL 0x103C
#define REG_COUNT_RESET 0x1040
#define REG_IPED 0x1060
#define REG_TEST_ADDR 0x1064
#define REG_SW_COMM 0x1068
#define REG_AAD 0x1070
#define REG_BAD 0x1072
#define REG_THR 0x1080

#define ROM_OUI_MSB 0x8026
#define ROM_OUI     0x802A
#define ROM_OUI_LSB 0x802E
#define ROM_VERSION 0x8032
#define ROM_BID_MSB 0x8036
#define ROM_BID     0x803A
#define ROM_BID_LSB 0x803E
#define ROM_REV     0x804E
#define ROM_SN_MSB  0x8F02
#define ROM_SN_LSB  0x8F06

using namespace std;


GI_V792::GI_V792(Bus *cl, uint32_t ba) {

  fLink = cl;
  fBase = ba;

  fDisable = true;

  sprintf(fName,"%s[%08x]",MODULE,ba);

  cout << __FUNCTION__ << ": " << fName << " allocated" << endl;


}

GI_V792::~GI_V792() { }


int GI_V792::Initialize() {

  uint16_t ecdata;
  uint16_t dd[3];

  // check if it exist
  for (uint16_t i=0;i<0xff;i++) {
    ecdata = i;
    fLink->Write(AMDEF, DTSIZ, fBase+REG_EVT_TRG, &ecdata);
    usleep(1000);
    ecdata=0;
    fLink->Read(AMDEF, DTSIZ, fBase+REG_EVT_TRG, &ecdata);
    if ((ecdata 0xff) != i) {
      fDisable = true;
      return -1;
    }
  }

  fDisable = false;

  fLink->Read(AMDEF, DTSIZ, fBase+REG_FW_REV, &ecdata);
  printf("%s: Firmware Revision: 0x%x\n",fName, ecdata & 0xffff);

  // read rom  
  fLink->Read(AMDEF, DTSIZ, fBase+ROM_OUI_MSB, &dd[0]);
  fLink->Read(AMDEF, DTSIZ, fBase+ROM_OUI, &dd[1]);
  fLink->Read(AMDEF, DTSIZ, fBase+ROM_OUI_LSB, &dd[2]);
  printf("%s: OUI: 0x %02x %02x %02x (should be 0x 00 40 E6)\n",fName, dd[0] & 0xff, dd[1] & 0xff, dd[2] & 0xff);

  fLink->Read(AMDEF, DTSIZ, fBase+ROM_VERSION, &ecdata);
  printf("%s: ROM Version 0x%02x (should be 0x11)\n",fName, ecdata & 0xff);

  fLink->Read(AMDEF, DTSIZ, fBase+ROM_BID_MSB, &dd[0]);
  fLink->Read(AMDEF, DTSIZ, fBase+ROM_BID, &dd[1]);
  fLink->Read(AMDEF, DTSIZ, fBase+ROM_BID_LSB, &dd[2]);
  printf("%s: Board ID: 0x %02x %02x %02x (should be 0x 00 03 18)\n",fName, dd[0] & 0xff, dd[1] & 0xff, dd[2] & 0xff);

  fLink->Read(AMDEF, DTSIZ, fBase+ROM_REV, &ecdata);
  printf("%s: ROM Revision 0x%02x (should be 0x00)\n",fName, ecdata & 0xff);

  fLink->Read(AMDEF, DTSIZ, fBase+ROM_SN_MSB, &dd[0]);
  fLink->Read(AMDEF, DTSIZ, fBase+ROM_SN_LSB, &dd[1]);
  printf("%s: S/N: 0x %02x %02x (should be 0x xx xx)\n",fName, dd[0] & 0xff, dd[1] & 0xff);

  fLink->Read(AMDEF, DTSIZ, fBase+REG_ADER_H, &dd[0]);
  fLink->Read(AMDEF, DTSIZ, fBase+ROM_ADER_L, &dd[1]);
  printf("%s: Base Address: 0x %02x %02x (should be %0x%04x)\n",fName, dd[0] & 0xff, dd[1] & 0xff, (fBase>>16)0xffff);


  // software reset
  Reset();
  
  return 1;

}

/*
 * crate_num : select the crate number, ti be written in the output data stream
 * iped: better to be >= 120
 * chenable: each bit one channel (if set channel is enabled
 */

int GI_V792::Configure(uint16_t crate_num, uint16_t iped) {

  if (fDisable) { return 1; }

  uint16_t data;

  // disable interrupt
  data = 0;
  fLink->Write(AMDEF, DTSIZ, fBase+REG_EVT_TRG, &data);

  // write crate select register
  fLink->Write(AMDEF, DTSIZ, fBase+REG_EVT_TRG, &crate_num);

  // read bit2 register
  fLink->Read(AMDEF, DTSIZ, fBase+REG_BIT2_SET, &data);
  printf("%s: BIT2 register 0x%04x\n",fName, data & 0xffff);

  for (int i=0;i<32;i++) { // disable all channels (channels must be enabled setting the threshold)
    data = 0x100;
    fLink->Write(AMDEF, DTSIZ, fBase+REG_THR + 2*i, &data);
  }

}

// depend on module !!
//
// if thr = 0x1ff

int GI_V792::SetThr(int ch, int thr) {

  uint16_t data;

  data = thr &0xff;

  fLink->Write(AMDEF, DTSIZ, fBase+REG_THR+ 2*ch, &data);

  return 0;

}


int GI_V792::Reset() {

  uint16_t ecdata=0x80;

  fLink->Write(AMDEF, DTSIZ, fBase+REG_BIT1_SET, &ecdata); // software reset
  ecdata=0x80;
  fLink->Write(AMDEF, DTSIZ, fBase+REG_BIT1_CLR, &ecdata); // software reset clear

  return 0;

}

int GI_V792::Clear() {

  uint16_t ecdata=0x4;

  fLink->Write(AMDEF, 0x02, fBase+REG_BIT2_SET, &ecdata); // clear data
  ecdata=0x4;
  fLink->Write(AMDEF, 0x02, fBase+REG_BIT2_CLR, &ecdata); // clear data

  ecdata=0x0;
  fLink->Write(AMDEF, 0x02, fBase+REG_COUNT_RESET, &ecdata); // clear counter

  return 0;

}

//
// Return status of the buffer:
// 0 = data in buffer, buffer is not full
// 1 = buffer is empy
// 2 = buffer is full
// 3 = NOT Possible

int GI_V792::BufferSatus() {

  uint16_t data=0;
  fLink->Read(AMDEF, 0x02, fBase+REG_STATUS_2, &data);
  return (data>>1)&0x3;
  
}

int GI_V792::Read() {

  uint32_t data=0;
  fLink->Read(AMDEF, 0x02, fBase+REG_OUT_BUF, &data);
  
  
  return (int) (data&0xffff);

}


/*
 * write the output reg channel
 */

int GI_V792::Write(int ch, uint32_t data) {


  outputReg(ch, data);

  return 0;

}

/*
 * val = 1 - set, 0 - clear 
 */

int GI_V792::outputReg(int ch, int val)  {


  uint16_t data;

  data = 1 << (ch+6);

  if (val) {
    fLink->Write(AMDEF, 0x02, fBase+REG_OUT_REG_S, &data);
  } else {
    fLink->Write(AMDEF, 0x02, fBase+REG_OUT_REG_C, &data);
  }

  return val;

}
