/*
 *
 * Trigger logic and some additional operations (e.g. scaler count ...)
 *
 */

#include <unistd.h>
#include "GI_Common.h"

#include "GI_Trigger.h"

GI_Trigger::GI_Trigger(Bus *cl) {

  fBus = cl;
  GI_MSG("allocated (%d)\n",0);
  fioPulserSleep=100;

}

GI_Trigger::~GI_Trigger() {

  GI_MSG("deleted (%d)\n",0);

}

// Before Start Run

int GI_Trigger::Initialize(GI_Config *gDB) {

  uint32_t ba;
  int rval;
  string model;

  model = gDB->get<string>(rval,"bus","user","pulser","model");
  ba = gDB->get<int>(rval,"bus","user","pulser","ba");

  // *** allocate V462 Gate Generator    
  if (model == "v462") {
    fV462 = new GI_V462(fBus, ba);
    fV462->Initialize();
  } else {
    fV462 = new GI_V462(fBus,0);
    GI_WRN("Pulser model is unknow (%s)\n", model.data());
  };

  // *** allocate io reg
  model = gDB->get<string>(rval,"bus","user","ioreg","model");
  ba = gDB->get<int>(rval,"bus","user","ioreg","ba");
  if (model == "v1495") {
    fV1495 = new GI_V1495(fBus, ba); // ...
    fV1495->Initialize();
  } else {
    fV1495 = new GI_V1495(fBus, 0);
  }

  if (model == "v513") {
    fV513 = new GI_V513(fBus, ba);
    fV513->Initialize();
  } else {
    fV513 = new GI_V513(fBus, 0); // disabled
  }

  // *** allocate V260 module

  model = gDB->get<string>(rval,"bus","user","scaler","model");
  ba = gDB->get<int>(rval,"bus","user","scaler","ba");
  if (model=="v260") {
    fV260 = new GI_V260(fBus, ba); // ...
    fV260->Initialize();
  } else {
    fV260 = new GI_V260(fBus, 0); 
  }

  GI_MSG("all device initialized (%d)\n",0);

  return 0;

}

int GI_Trigger::Configure(GI_Config *gDB) {

  int rval;

 //  gV1495->CIO_ModeConfig(0,1,0,0);
  fV1495->CIO_PortConfig(3,0,1); // output/NIM (open)
  fV1495->CIO_PortConfig(4,1,1); // input/NIM (50 ohm)

  fV513->Configure(0,1,0,0,0);
  fV513->Configure(1,1,0,0,0);
  fV513->Configure(2,1,0,0,0);
  fV513->Configure(8,0,1,1,0); // clear veto of the trigger 
  fV513->Configure(9,0,1,1,0); // veto of PULSER 
  fV513->Configure(10,0,1,1,0); // veto of PMTs coincidence
  fV513->Configure(11,0,1,1,0); // pulser for pedestal 


  fV462->Configure(gDB->get<int>(rval,"bus","user","pulser","period"),0x1); // 100 us / 100 ns (assume bus=0)
  fV462->Start(1);
  
  fV260->Disable();
  fV260->Clear();
  fV513->Clear();  
  fV1495->CIO_Clear();
  fV260->Enable();

  fV260->ReadBunch(0,6);

  uint16_t vPhys, vPuls;
  vPhys = 1 << gDB->get<int>(rval,"bus","user","ioreg","veto_phys");
  vPuls = 1 << gDB->get<int>(rval,"bus","user","ioreg","veto_puls");
  fResetVeto = 1 << gDB->get<int>(rval,"bus","user","ioreg","reset_veto");

  fioPulser = gDB->get<int>(rval,"bus","user","ioreg","pulse_on");
  fioPulserSleep = gDB->get<int>(rval,"bus","user","ioreg","pulse_period");

  if (fioPulser <0) {
    fioPulser = 0;
  } else {
    GI_MSG("Pulser from I/O reg. on channel %d\n",fioPulser);
    fioPulser = 1 << fioPulser;
  }

  int ts=gDB->get<int>(rval,"run","trigger_source");
  switch ( gDB->get<int>(rval,"run","trigger_source") ) {
  case 0: fVeto = 0;
    break;
  case 1: fVeto = vPuls; // pulser is vetoed
    break;
  case 2: fVeto = vPhys; // coinc is vetoed
    break;
  case 99: fVeto = vPuls | vPhys; // all vetoed
    break;
  default:
    fVeto = 0;
  }

  GI_MSG("Trigger source is = %d -> veto = 0x%x (Phys=%x, Puls=%x)\n",ts,fVeto, vPhys, vPuls);
  GI_MSG("Reset Veto Bit = 0x%x\n",fResetVeto);

  // fV1495->CIO_WriteLH(3, fVeto, 0);
  fV513->Write(fVeto);

  GI_MSG("configured, Veto=0x%x\n",fVeto);

  return 0;

}

// In event loop

int GI_Trigger::PreEvent() { 

  //  fV1495->CIO_WriteLH(3, 0x1 | fVeto, 0);
  //  fV1495->CIO_WriteLH(3, 0x0 | fVeto, 0);
  fV513->Write(fioPulser | fResetVeto | fVeto); // reset veto of trigger (and send pulser if set)
  fV513->Write(fVeto);

#ifdef CONTROLLER_SIS3104
  fBus->ClearInLatch(3); // clear trigger latch
  fBus->PulseOutReg(1); // reset veto
  fBus->PulseOutReg(2); // send trigger
#endif

  return 0;

}

// timeout in count

int GI_Trigger::PostEvent() {

  // check busy from devices ... if timeout reset something
  // clean Input register
  fV513->Clear();

  return 0;

}

int GI_Trigger::GetEvent() {

  //  gV260->ReadBunch(0,6);
  //  gV260->PrintRate(0,6,DaqCtrl->startTime());
  //  gV260->WriteScaler(0,6,DaqCtrl->startTime(), fout_scaler);

  return 0;

}

int GI_Trigger::IsBusy() {

  int busy;

  busy = fV513->Read();
  return busy;

}


int GI_Trigger::Polling() {

  int trig;

  //  trig = fV1495->CIO_ReadLH(4,0);
  trig = fV513->Read();
  if (fioPulser>0) { trig |= 1; usleep(fioPulserSleep); }

#ifdef CONTROLLER_SIS3104
  fBus->GetInLatch(&trig); // read trigger latch
#endif

#ifdef CONTROLLER_GEXVB
  trig=1; // read trigger latch
#endif

  return trig;

}

// After Stop Run

int GI_Trigger::StopRun() {

  fV462->Disable();

  GI_MSG("done (%d)\n",0);
  return 0;

}
