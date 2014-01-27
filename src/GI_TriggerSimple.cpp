/*
 *
 * Simple trigger logic, use in/out of the VME master
 *
 */

#include "GI_Common.h"
#include "GI_Config.h"
#include "GI_Trigger.h"

GI_Trigger::GI_Trigger(Bus *cl) {

  fBus = cl;
  GI_MSG("allocated (%d)\n",0);

}

GI_Trigger::~GI_Trigger() {

  GI_MSG("deleted (%d)\n",0);

}

// Before Start Run

int GI_Trigger::Initialize() {

  GI_MSG("no device to initialize\n",0);

  return 0;

}

int GI_Trigger::Configure(GI_Config *gDB) {

#ifdef CONTROLLER_CAEN_V1718
  // init and set Vx718 output register for a given channel
  fBus->SetOutConf(0);
  fBus->SetOutConf(1);
  fBus->SetOutConf(2);
  fBus->SetOutConf(3);
  fBus->SetOutConf(4);
  fBus->SetInConf(0);
  fBus->SetInConf(1);
#endif


  // reset scaler counter
#ifdef CONTROLLER_CAEN_V1718
  CommLink->DisableScaler();
  CommLink->ResetScaler();
  CommLink->EnableScaler();
#endif

#ifdef CONTROLLER_SIS3104
  CommLink->ClearInLatch(3);
#endif

  fcounter_old = 0;

  GI_MSG("configured, (counter = %d)\n",fcounter_old);

  return 0;

}

// In event loop

int GI_Trigger::PreEvent() {

  fBus->PulseOutReg(1); // reset veto (if any)

  fBus->PulseOutReg(2); // V2718 generate the trigger on output ch 4

  return 0;

}

int GI_Trigger::PostEvent() {

  return 0;

}

int GI_Trigger::GetEvent() {

  //  gV260->ReadBunch(0,6);
  //  gV260->PrintRate(0,6,DaqCtrl->startTime());
  //  gV260->WriteScaler(0,6,DaqCtrl->startTime(), fout_scaler);

  return 0;

}


int GI_Trigger::Polling() {
  
  int counter;
  
#ifdef CONTROLLER_CAEN_V1718
  counter = fBus->GetScalerCount(); // use Vx718 to catch the trigger on IN0 at NIM level
  if (counter != fcounter_old) {
    trig = 1;
    fcounter_old = counter;
  }
#endif
#ifdef CONTROLLER_SIS3104
  fBus->GetInLatch(&counter); // use SIS3104 to catch the trigger on IN0 at NIM level
  //  cout << __FUNCTION__ << " SIS3104 IN0 status = " << counter << endl;
  if( (counter & 1) == 1 )
    {
      //  cout << __FUNCTION__ << " SIS3104 Got trigger" << endl;
      trig = 1;
      fBus->ClearInLatch(1);
    }
#endif

  return trig;

}

// After Stop Run

int GI_Trigger::StopRun() {

  GI_MSG("done (%d)\n",0);
  return 0;

}
