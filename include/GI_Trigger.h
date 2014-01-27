

#ifndef __GI_TRIGGER__
#define __GI_TRIGGER__

#include "Bus.h"
#include "GI_Config.h"

#include "GI_V1495.h"
#include "GI_V513.h"
#include "GI_V260.h"
#include "GI_V462.h"

class GI_Trigger {

 private:
   GI_V260 *fV260; // scaler
  GI_V462 *fV462; // dual gate
  GI_V513 *fV513; // io register
  GI_V1495 *fV1495; // general purpose / io reg

  Bus *fBus;

  uint16_t fVeto;
  uint16_t fResetVeto;

  uint16_t fioPulser;
  uint32_t fioPulserSleep;

  int fcounter_old; // used by TriggerSimple
 public:

  GI_Trigger(Bus * cl);

  ~GI_Trigger();

  // Before Start run

  int Initialize(GI_Config *gDB);

  int Configure(GI_Config *gDB);

  // event loop (run)

  int PreEvent();

  int PostEvent();

  int GetEvent();

  int Polling();

  // After Stop run

  int IsBusy();

  int StopRun();

};

#endif
