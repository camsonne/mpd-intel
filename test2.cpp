// #include <stdlib.h>
// #include <stdio.h>
// #include <stdarg.h>
// #include <time.h>
// #include <unistd.h>
// #include <signal.h>
// #include <stdint.h>
// #include <string.h>
// #include <iostream>
// #include <cmath>

// #include "GI_Common.h"

// extern "C"{
// #include "jvme.h"
// }
// #include "Bus.h"
// int Bus::fBus_Idx=-1; 

// #ifdef CONTROLLER_CAEN_V1718
// #include "Vme_v1718.h"
// #endif
// #ifdef CONTROLLER_SIS3104
// #include "Vme_sis.h"
// #endif
// #ifdef CONTROLLER_GEXVB
// #include "Vme_gexvb.h"
// #endif
// #ifdef CONTROLLER_SBSB3
// #include "Vme_sbsb3.h"
// #endif

// #include "GI_Config.h"

// #include "DaqControl.h"
// #include "Logger.h"

// #include "MPDhi.h"

// #include "GI_Wrapper.h"

// #include "GI_IO.h"

// #include "GI_Trigger.h"

// #define MAX_SAMPLES	8192
// #define DAQ_RUN_FILE "cfg/last.run"
// #define DAQ_CONFIG_FILE "cfg/config_apv.txt"

// Bus *CommLink;

// int AdcBoardCount;

// GI_Config *fDB;

// GI_Trigger *fTrg;

// DaqControl *DaqCtrl;

// MPDhi *gMPD;
// MPDlo *glowMPD;
// //Logger *log;

// static int sig_int = 1;

// static int fRun_num = 0  ;

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <stdint.h>
#include <string.h>
#include <iostream>
#include <cmath>

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

MPDhi *gMPD;
MPDlo *glowMPD;
//Logger *log;

static int sig_int = 1;

static int fRun_num = 0;

///////////////////////////////////////////////////////////////
void ctrl_c(int){
  sig_int = 0;
  printf("\n CTRL-C pressed \n\r");
  exit(0);
}//ctrl_c


int readRunNum(const char *runfile) {
  FILE *ff;
  int rr;
  ff = fopen(runfile,"r");
  if (ff) {
    fscanf(ff,"%d",&rr);
    fclose(ff);
  } else {
    rr = -1;
  }
  return rr;
}

int writeRunNum(int rr, const char *runfile) {
  FILE *ff;
  ff = fopen(runfile,"w");
  if (ff) {
    fprintf(ff,"%d",rr);
    fclose(ff);
  } else {
    printf("WARNING: cannot update run file %s",runfile);
  }
  return 0;
}
///////////////////////////////////////////////////////////////


int main(int argc, char *argv[])
{

  cout<<endl<<"testing"<<endl;
 int i;

   int rval;

   char fn_cnf[1000];                /* config input file name */

   GI_Wrapper * gWrapper = new GI_Wrapper();


  sprintf(fn_cnf,DAQ_CONFIG_FILE);
  for (i=1;i<argc; i++) {
    if (strcmp("-cnf", argv[i]) == 0) {
      i++;
      sprintf(fn_cnf,"%s",argv[i]);
    }
  }


 // *** read and store config data

  fDB = new GI_Config();
  if (fDB->parseInline(argc, argv) < 0 ) { exit(0); }
  if (fDB->parseFile(fn_cnf) < 0) { exit(0); }
  fDB->insertInline();

 // *** update run number

  std::string lastrun_path;
  lastrun_path = (fDB->get<std::string>(rval, "run", "lastrun"));
  fRun_num = fDB->get<int>(rval, "run", "index");
  if (fRun_num < 0) {
    fRun_num = readRunNum(lastrun_path.data())+abs(fRun_num);
  }
  writeRunNum(fRun_num, lastrun_path.data());

  cout << __FUNCTION__ << ": Input run number file = " << lastrun_path << endl;

  std::string file_prefix = fDB->get<std::string>(rval, "run", "file_prefix");


// *** save config

  char fout_cnf[1000];
  sprintf(fout_cnf, "%s_%04d_config.txt", file_prefix.data(), 
	  fRun_num);
  fDB->save(fout_cnf);






 // *** start logging
  DaqCtrl = new DaqControl();

 Logger * log = Logger::GetLogger(); // must follow DaqControl

  log->SetVerboseLevel(fDB->get<int>(rval,"loglevel"));
  char fname_log[1000];
  sprintf(fname_log, "%s_%04d.log", file_prefix.data(), fRun_num);
  log->Open(fname_log);
  log->Write("Program Started");

  char dummyline[1000];
  sprintf(dummyline,"Time Preset = %d (s)",fDB->get<int>(rval, "run", "time_preset"));
  log->Write(dummyline);
  sprintf(dummyline,"Event Preset = %d (s)",fDB->get<int>(rval, "run", "event_preset"));
  log->Write(dummyline);


// *** init Bus "Link"

#ifdef CONTROLLER_CAEN_V1718
  CommLink = new VmeV1718;
  cout << "Bus Controller CAEN Vx718 allocated" << endl;
#endif
#ifdef CONTROLLER_SIS3104
  CommLink = new VmeSis3104;
  cout << "Bus Controller SIS3104 allocated" << endl;
#endif
#ifdef CONTROLLER_GEXVB
  CommLink = new VmeXVB;
  cout << "Bus Controller GEXVB allocated" << endl;
#endif
#ifdef CONTROLLER_SBSB3
  CommLink = 0;
  cout << "Bus Controller SBSB3 not defined, must quit" << endl;
#endif
  
  if( CommLink == NULL ) {
    cout << endl << endl << " VME controller structure not initialized" << endl;
    cout << " quit ..." << endl << endl;
    log->Write("VME controller structure not initialized");
    log->Close();
    exit(1);
  }
  if( CommLink->Status() == 0 ) {
    cout << endl << endl << " Error initializing VME controller" << endl;
    cout << " quit ..." << endl << endl;
    log->Write("Error initializing VME controller");
    log->Close();
    delete CommLink;
    exit(1);
  }

  // *** allocate MPD object and initialize readout electronics

  gMPD = new MPDhi(fDB->get<int>(rval,"loglevel"));

  gWrapper->LoadMPDConfig(fDB, gMPD, CommLink);

  AdcBoardCount = gMPD->CountAndRegisterBoards();

  gMPD->DiscoverFEC();

  gMPD->InitAndReset();
  
  log->Write("MPD/APV initialization done file scanning done");

  // *** Init additional hardware

  fTrg = new GI_Trigger(CommLink);
  fTrg->Initialize(fDB);
  fTrg->Configure(fDB);







  // *** Load configuration of DAQ
   int ii;

  gWrapper->LoadDaqConfig(fDB, DaqCtrl);

 gMPD = new MPDhi(fDB->get<int>(rval,"loglevel"));
        gMPD->InitAndReset();//These 2 Functions Cause Segmentation Fault Errors
//   cout<<endl<<"ECHO 2"<<endl;
      gMPD->LoadPedAndThr();
    
      for( ii=fDB->get<int>(rval,"run","histo_board_start"); ii<=fDB->get<int>(rval, "run", "histo_board_stop"); ii++);
	if ((ii>=0) && (ii<AdcBoardCount)) {
	  for(i=fDB->get<int>(rval,"run","histo_channel_start"); i<=fDB->get<int>(rval, "run","histo_channel_stop"); i++);

	    }

   delete gMPD;

  cout<<endl<<"SUCCESS"<<endl;
  return 0;
}

