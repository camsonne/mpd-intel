#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <stdint.h>
#include <string.h>
#include <iostream>

#include "GI_Common.h"

extern "C"{
#include "jvme.h"
}
#include "Bus.h"
int Bus::fBus_Idx=-1; 

#ifdef CONTROLLER_CAEN_V1718
#include "Vme_v1718.h"
#endif
#ifdef CONTROLLER_SIS3104
#include "Vme_sis.h"
#endif
#ifdef CONTROLLER_GEXVB
#include "Vme_gexvb.h"
#endif
#ifdef CONTROLLER_SBSB3
#include "Vme_sbsb3.h"
#endif

#include "GI_Config.h"

#include "DaqControl.h"
#include "Logger.h"

#include "MPDhi.h"

#include "GI_Wrapper.h"

#include "GI_IO.h"

#include "GI_Trigger.h"

#define MAX_SAMPLES	8192
#define DAQ_RUN_FILE "cfg/last.run"
#define DAQ_CONFIG_FILE "cfg/config_apv.txt"

Bus *CommLink;

int AdcBoardCount;

GI_Config *fDB;

GI_Trigger *fTrg;

DaqControl *DaqCtrl;

Logger *log;

MPDhi *gMPD;
MPDlo *glowMPD;


using namespace std;

int main()
{
  uint32_t rev_id,timec;
   GI_Wrapper * gWrapper = new GI_Wrapper();

   fDB = new GI_Config();
   fDB->parseFile("cfg/config_apv.txt");
   fDB->insertInline();

   CommLink = new VmeXVB;
   uint32_t histo_data[4096];

   glowMPD = new MPDlo(CommLink,1,0);
   glowMPD->CR_Scan(0,&rev_id,&timec);

   gMPD = new MPDhi();
   gWrapper->LoadMPDConfig(fDB, gMPD, CommLink);

   int AdcBoardCount = gMPD->CountAndRegisterBoards();

   gMPD->DiscoverFEC();
   gMPD->InitAndReset();
   //gMPD->LoadPedAndThr(); 
   gMPD->HISTO_Clear(0,0,-1);
   gMPD->HISTO_Read(0,0, histo_data);
   for (int i = 0 ; i< 4096 ;i++)
     {
       cout<<histo_data[i]<<endl;
     }
   // gMPD->EnableAcq();
   // gMPD->EVENT_Read(1000000);
   delete CommLink;
   return 1;
}
