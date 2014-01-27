/**
 * V1495 - General Purpose VME Board
 *
 */

#include <stdio.h>
#include <iostream>

#include "GI_V1495.h"

#include "GI_Common.h"

#define MODULE "V1495"

#define FIXEDCODE0 0xF5
#define FIXEDCODE1 0xFA
#define MANUFAC    0x832

#define REG_CONTROL 0x8000
#define REG_STATUS 0x8002
#define REG_INTLEVEL 0x8004
#define REG_INTSTATUS 0x8006
#define REG_GEOADDR 0x8008
#define REG_MODULERESET 0x800A
#define REG_REVISION 0x800C

#define SCRATCH32 0x8020

#define ROM_OUI2 0x8124
#define ROM_OUI1 0x8128
#define ROM_OUI0 0x812C
#define ROM_VERSION 0x8130

#define ROM_BOARD2 0x8134
#define ROM_BOARD1 0x8138
#define ROM_BOARD0 0x813C

#define ROM_REV3 0x8140
#define ROM_REV2 0x8144
#define ROM_REV1 0x8148
#define ROM_REV0 0x814C

#define ROM_SERNUM1 0x8180
#define ROM_SERNUM0 0x8184

// - coincidence and i/o register D16
#define CIO_STATUS_L 0x1000
#define CIO_STATUS_H 0x1002
#define CIO_STATUS_STEP 0x0004
#define CIO_MASK_L 0x100C
#define CIO_MASK_H 0x100E
#define CIO_MASK_STEP 0x0004
#define CIO_GATEWIDTH 0x1018
#define CIO_C_STATUS_H 0x000C

#define CIO_MODE 0x101E
#define CIO_SCRATCH 0x1020

#define CIO_G_CONTROL 0x1022

#define CIO_CONTROL_L 0x1024
#define CIO_CONTROL_H 0x1026
#define CIO_DATA_L 0x1028
#define CIO_DATA_H 0x102A
#define CIO_CONTROL_STEP 0x0008

#define CIO_REVISION 0x103C

#define CIO_IDCODE 0x1042
#define CIO_IDCODE_STEP 0x0002


using namespace std;


GI_V1495::GI_V1495(Bus *cl, uint32_t ba) {

  fLink = cl;
  fBase = ba;

  sprintf(fName,"%s[%08x]",MODULE,ba);

  fDisable = 1;

  cout << __FUNCTION__ << ": " << fName << " allocated" << endl;


}

GI_V1495::~GI_V1495() { }


int GI_V1495::Initialize() {

  uint32_t ecdata[4];
  uint16_t ecsdata[4];

  ecdata[0]=0xE5A0C180;
  ecdata[1]=0x0;
  fLink->Write(0x09, 0x04, fBase+SCRATCH32, &ecdata[0]);
  usleep(1000);
  fLink->Read(0x09, 0x04, fBase+SCRATCH32, &ecdata[1]);
  usleep(1000);
  if (ecdata[0] == ecdata[1]) {
    fDisable=0;
    printf("%s: W/R Test Passed (w: %08x == r: %08x)\n",fName, ecdata[0], ecdata[1]);
  } else {
    fDisable=1;
    printf("%s: W/R Test Failed (w: %08x != r: %08x), board DISABLED\n",fName, ecdata[0], ecdata[1]);
    return 0;
  }

  fLink->Read(0x09, 0x02, fBase+REG_REVISION, &ecdata[0]);
  printf("%s: Firmware Revision (1.0) 0x %x.%x\n",fName, (ecdata[0]>>8)&0xff, ecdata[0]&0xff);
  usleep(1000);


  /*
  ecsdata[0]=1;
  fLink->Write(0x09, 0x02, fBase+0x8016, &ecsdata[0]); // A32D16 - USER FPGA conf register
  usleep(100000);
  */  

  fLink->Read(0x09, 0x02, fBase+ROM_BOARD2, &ecsdata[2]); // A32D16
  fLink->Read(0x0A, 0x02, fBase+ROM_BOARD1, &ecsdata[1]);
  fLink->Read(0x0D, 0x02, fBase+ROM_BOARD0, &ecsdata[0]);
  printf("%s: Board ID (0x 00 05 D7) 0x %x %x %x\n",fName, ecsdata[2], ecsdata[1], ecsdata[0]);
  
  if (((ecsdata[0] & 0xff) != 0xd7) || ((ecsdata[1] & 0xff) !=  0x05)) {
    cout << __FUNCTION__ << ": cannot identify " << fName << " do not mind ..." << endl;
    //    return 0;
  }

  fLink->Read(0x09, 0x02, fBase+ROM_OUI2, &ecsdata[2]);
  fLink->Read(0x09, 0x02, fBase+ROM_OUI1, &ecsdata[1]);
  fLink->Read(0x09, 0x02, fBase+ROM_OUI0, &ecsdata[0]);

  printf("%s: Manufacturer ID (0x 00 40 E6) 0x %x %x %x\n",fName, ecsdata[2], ecsdata[1], ecsdata[0]);

  fLink->Read(0x09, 0x02, fBase + ROM_VERSION, &ecdata[0]);
  printf("%s: Purchased Version 0x%x\n",fName, ecdata[0]&0xffff);

  fLink->Read(0x09, 0x02, fBase+ROM_REV3, &ecdata[3]);
  fLink->Read(0x09, 0x02, fBase+ROM_REV2, &ecdata[2]);
  fLink->Read(0x09, 0x02, fBase+ROM_REV1, &ecdata[1]);
  fLink->Read(0x09, 0x02, fBase+ROM_REV0, &ecdata[0]);

  printf("%s: HW Revision 0x %x %x %x %x\n",fName, ecdata[3] & 0xff, ecdata[2] & 0xff, ecdata[1] & 0xff, ecdata[0] & 0xff);

  fLink->Read(0x09, 0x02, fBase+ROM_SERNUM1, &ecdata[1]);
  fLink->Read(0x09, 0x02, fBase+ROM_SERNUM0, &ecdata[0]);

  printf("%s: Serial Number: %d \n",fName, (ecdata[1] & 0xff) *0x100 + (ecdata[0] & 0xff));
  
  //  Reset();

  CIO_Init();

  return 1;

}

/*
 *
 */

int GI_V1495::Configure(int ch, int dir, int pol, int mode, int transfer) {

  uint32_t data;

  data = (dir&1) | ((pol&1)<<1) | ((mode&1)<<2) | ((transfer&1)<<3);

  cout << __FUNCTION__ << ": " << fName << " config register " << ch << " with " << hex << data << endl;
  //  fLink->Write(0x09, 0x02, fBase+REG_CH0STATUS+ch*REG_CHSTEP, &data);

  return 1;

}


int GI_V1495::Reset() {

  uint16_t ecdata;

  fLink->Write(0x09, 0x02, fBase+REG_MODULERESET, &ecdata); // status default

  return 1;

}

int GI_V1495::Clear() {

  uint16_t ecdata;

  fLink->Write(0x09, 0x02, fBase+REG_MODULERESET, &ecdata); // status default

  return 1;

}

int GI_V1495::Read() {

  uint16_t data=0;
  //  fLink->Read(0x09, 0x02, fBase+REG_INPUT, &data);
  
  return (int) (data&0xffff);

}


int GI_V1495::Write(uint16_t data) {

  //  fLink->Write(0x09, 0x02, fBase+REG_OUTPUT, &data);
  
  return (int) (data&0xffff);

}

/* 
 * Initialization of the FPGA User Coincidence Unit and IO Register Functions
 */

int GI_V1495::CIO_Init() {

  if (fDisable) return 0;

  uint16_t ecdata;
  char smezid[9][6]={"A395A","A395B","A395C","A395D","A395E","none","none","none","none"};
  int i;

  ecdata=0;
  usleep(1000);
  GI_MSG(" cIO REV = 0x%x\n", fLink->Read(0x09, 0x02, fBase+CIO_REVISION, &ecdata));
  GI_MSG("%s: CIO Revision 0x%x . %x\n",fName, (ecdata>>8)&0xff, ecdata&0xff);

  for (i=0;i<3;i++) {
  usleep(1000);
    fLink->Read(0x09, 0x02, fBase+CIO_IDCODE+i*CIO_IDCODE_STEP, &ecdata);
    printf("%s: CIO IDCODE Port %d = 0x%x (%s mezzanine)\n",fName, i+3, ecdata&0xffff, smezid[ecdata&0xffff]);
  }

  return 1;

}

/* configure overal feature of CIO
 *
 * dsel = delay select 
 * wmode = working mode (0: logic, 1 I/O Register
 * lope = logic operation (in logic mode, 0 : C = A AND B, 1 : C = A OR B)
 * pulse_mode
 */
int GI_V1495::CIO_ModeConfig(int dsel, int wmode, int lope, int pulse_mode) {

  uint16_t data;

  //  fLink->Read(0x09, 0x02, fBase+CIO_MODE, &data);
  //  printf("%s: CIO Mode Reg/Read = 0x%x\n",fName, data&0xff);

  data = dsel | ((wmode << 3) & 0x8) | ((lope << 4) & 0x10) | ((pulse_mode << 5) & 0x20);
  fLink->Write(0x09, 0x02, fBase+CIO_MODE, &data);

  //  fLink->Read(0x09, 0x02, fBase+CIO_MODE, &data);
  printf("%s: CIO Mode Reg/Set to = 0x%x\n",fName, data);

  return 1;
}

/* Port configuration
 * port = 3, 4 or 5 (mezzanine D, E F)
 * dir = 0 output / 1 input
 * format = 0 TTL / 1 = NIM
 * Only used for A935D Mezzanine (code 0x3)
 */
int GI_V1495::CIO_PortConfig(int port, int dir, int format) {

  uint16_t data;
  int i;

  if (fDisable) return 0;

  i = port-3;

  fLink->Read(0x09, 0x02, fBase+CIO_IDCODE+i*CIO_IDCODE_STEP, &data);

  if ((data & 0xff) == 0x03) { // ok A935D

    data = (format & 0x1) | ((dir << 1) & 0x2);

    fLink->Write(0x09, 0x02, fBase + CIO_CONTROL_L + i * CIO_CONTROL_STEP, &data);

    data = 0xfafa;

    fLink->Read(0x09, 0x02, fBase + CIO_CONTROL_L + i * CIO_CONTROL_STEP, &data);
    printf("%s: CIO Port %d config = 0x%x\n", fName, port, data &0xff);
    return 1;

  } else {
    
    printf("%s: CIO Error, cannot find mezzanine %d\n", fName, port);
    return 0;
  }

}

int GI_V1495::CIO_Clear() {

  if (fDisable) return 0;

  CIO_Write(3,0);
  CIO_Write(4,0);

  return 1;

}


uint32_t GI_V1495::CIO_Read(int port) {

  uint32_t data[2], out;
  int i;

  i = port-3;

  fLink->Read(0x09, 0x02, fBase+CIO_DATA_L+i*CIO_CONTROL_STEP, &data[0]);
  fLink->Read(0x09, 0x02, fBase+CIO_DATA_H+i*CIO_CONTROL_STEP, &data[1]);

  out = ( ( data[1] << 16 ) & 0xffff0000 ) | (data[0] & 0xffff ); 

  return out;

}

/*
 * LH = 0 low 16 bits, 1 high 16 bits
 */
uint32_t GI_V1495::CIO_ReadLH(int port, int lh) {

  if (fDisable) return 0xFF;

  uint32_t data;
  int i;

  i = port-3;

  if (lh == 0) {
    fLink->Read(0x09, 0x02, fBase+CIO_DATA_L+i*CIO_CONTROL_STEP, &data);
  } else {
    fLink->Read(0x09, 0x02, fBase+CIO_DATA_H+i*CIO_CONTROL_STEP, &data);
  }

  return (data & 0xffff); 

}

int GI_V1495::CIO_Write(int port, uint32_t val) {

  uint32_t d0, d1;
  int i;

  i = port-3;

  d0 = (val & 0xffff);
  d1 = (val >> 16) & 0xffff;

  fLink->Write(0x09, 0x02, fBase+CIO_DATA_L+i*CIO_CONTROL_STEP, &d0);
  fLink->Write(0x09, 0x02, fBase+CIO_DATA_H+i*CIO_CONTROL_STEP, &d1);

  return 1;

}

int GI_V1495::CIO_WriteLH(int port, uint32_t val, int lh) {

  if (fDisable) return 0;

  uint32_t d0;
  int i;

  i = port-3;

  d0 = (val & 0xffff);

  if (lh==0) {
    fLink->Write(0x09, 0x02, fBase+CIO_DATA_L+i*CIO_CONTROL_STEP, &d0);
  } else {
    fLink->Write(0x09, 0x02, fBase+CIO_DATA_H+i*CIO_CONTROL_STEP, &d0);
  }

  return 1;

}
