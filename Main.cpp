/*
  -----------------------------------------------------------------------------
  
  --- I.N.F.N. Genova - Electronic Department --- 
  --- I.N.F.N. Rome - Sanita' group III       --- 
 
  -----------------------------------------------------------------------------
  
  Name		:	Main.cpp
  
  Project	:	MPD
  
  Description   :	Main program for MPD control using a V1718 or a V2718 
  
  First Date	:	October 2010
  Release       :	>1.0
  Authors	:	Paolo Musico, E. Cisbani
    
  -----------------------------------------------------------------------------
  
  -----------------------------------------------------------------------------
*/


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

#include "jvme.h"

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

static int sig_int = 1;

static int fRun_num = 0;

void DoRamTest(int channel, int board=0);
void DoHistogrammig(int channel, int board=0);
void DoSamples(void);
void DoEvent(void);
void FindAllApv(void);

/*----------------      signal handling      ----------------*/

void ctrl_c(int){
  sig_int = 0;
  printf("\n CTRL-C pressed \n\r");
  exit(0);
}//ctrl_c


// handle run numbers

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

//----
//----

int main(int argc, char *argv[]) 
{

  int i;

  int rval;

  char fn_cnf[1000];                /* config input file name */

  GI_Wrapper * gWrapper = new GI_Wrapper();

  signal(SIGINT, ctrl_c);

  sprintf(fn_cnf,DAQ_CONFIG_FILE);
  for (i=1;i<argc; i++) {
    if (strcmp("-cnf", argv[i]) == 0) {
      i++;
      sprintf(fn_cnf,"%s",argv[i]);
    }
  }

  cout << __FUNCTION__ << ": Input configuration file = " << fn_cnf << endl;
 
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

  log = Logger::GetLogger(); // must follow DaqControl

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

  gWrapper->LoadDaqConfig(fDB, DaqCtrl);

  switch( gMPD->getAcqMode() ) // TO BE IMPROVED
    {
      
    case 6: // MPD_DAQ_RAM_TEST // see MPDlo.h
      gMPD->InitAndReset();
      gMPD->LoadPedAndThr();
      gMPD->EnableAcq();
      for(int ii=fDB->get<int>(rval,"run","histo_board_start"); ii<=fDB->get<int>(rval, "run", "histo_board_stop"); ii++)
	if ((ii>=0) && (ii<AdcBoardCount)) {
	  for(i=fDB->get<int>(rval,"run","histo_channel_start"); i<=fDB->get<int>(rval, "run","histo_channel_stop"); i++) 
	    DoRamTest(i, ii);

	}
    
      break;

    case 4: // MPD_DAQ_HISTO // see MPDlo.h
      gMPD->InitAndReset();
      gMPD->LoadPedAndThr();
      gMPD->EnableAcq();
      for(int ii=fDB->get<int>(rval,"run","histo_board_start"); ii<=fDB->get<int>(rval, "run", "histo_board_stop"); ii++)
	if ((ii>=0) && (ii<AdcBoardCount)) {
	  for(i=fDB->get<int>(rval,"run","histo_channel_start"); i<=fDB->get<int>(rval, "run","histo_channel_stop"); i++)
	    DoHistogrammig(i, ii);
	}

      break;
      

    case 2: // MPD_DAQ_SAMPLE
      DoSamples();
      break;
      
    case 1: // MPD_DAQ_EVENT
    case 3: // MPD_DAQ_PROCESS
      DoEvent();
      break;
    default: 
      cout << __FUNCTION__ << " WARNING: daq_mode not recognized!"<<endl;
      break;
    }
  
  delete gMPD;

  fTrg->StopRun();

  log->Write("Program Ended");
  log->Close();
  delete CommLink;

  return 0;

}

/**
 *
 *
 */

void DoHistogrammig(int channel, int board)
{
  char fname_out[256];
  time_t histo_t0, histo_t1;
  uint32_t histo_data[2][4096], histo_count[2];
  FILE *fout;
	
  if ((channel<0) || (channel>7)) {
    GI_WRN(" Channel %d is out of range (0-15) on board %d\n",channel,board);
    return;
  }
  
  GI_MSG("doing board= %d, channels= %d and %d for %d sec\n", board, channel, channel+8,DaqCtrl->GetHistoTime());
  
  gMPD->HISTO_Clear(board,channel);
  gMPD->HISTO_Clear(board,channel+8);

   gMPD->HISTO_Start(board,channel);
   gMPD->HISTO_Start(board,channel+8);
  
  time(&histo_t0);
  
  histo_t1 = histo_t0;

  while( (histo_t1 - histo_t0) < DaqCtrl->GetHistoTime() ) {
    Sleep(1000);
    time(&histo_t1);
    if( sig_int == 0 ) break;
  }

  gMPD->HISTO_Stop(board,channel);
   gMPD->HISTO_Stop(board,channel+8);

  for (int i=0;i<2;i++) {
    histo_count[i] = gMPD->HISTO_Read(board, channel + i*8, histo_data[i]);
    GI_MSG(" ... %d acquired %d histo events\n",channel+i*8, histo_count[i]);

    //	fprintf(fout, "# Board Channel Integral and then 4096 lines of data\n");
    //	fprintf(fout, "# %d %d %d\n", board, channel, histo_count);

    if (histo_count[i] > 0) {
      sprintf(fname_out, "%s_%04d_histo_%d_%d.dat", (DaqCtrl->GetFilePrefix()).data(), fRun_num,board, channel+i*8);
      if( (fout = fopen(fname_out, "w")) == NULL) {
	GI_ERR(" Cannot open output file %s\n", fname_out);
	exit(1);
      }
      
      for(int j=0; j<4096; j++) fprintf(fout, "%d\n", histo_data[i][j]);

      GI_MSG(" board=%d ch=%d out on file %s\n",board, channel+i*8,fname_out);

      fclose(fout);
    }
  }
}


void DoRamTest(int channel, int board)
{
  char fname_out[256];
  time_t histo_t0, histo_t1;
  uint32_t histo_data[2][4096], histo_count[2];
  FILE *fout;
	
  if ((channel<0) || (channel>7)) {
    GI_WRN(" Channel %d is out of range (0-15) on board %d\n",channel,board);
    return;
  }
  
  GI_MSG("doing board= %d, channels= %d and %d for %d sec\n", board, channel, channel+8,DaqCtrl->GetHistoTime());
  
  gMPD->HISTO_Clear(board,channel,-1);
  gMPD->HISTO_Clear(board,channel+8,-1);

  for (int i=0;i<2;i++) {
    histo_count[i] = gMPD->HISTO_Read(board, channel + i*8, histo_data[i]);
    GI_MSG(" ... %d acquired %d histo events\n",channel+i*8, histo_count[i]);

    //	fprintf(fout, "# Board Channel Integral and then 4096 lines of data\n");
    //	fprintf(fout, "# %d %d %d\n", board, channel, histo_count);

    // if (histo_count[i] > 0) {
      sprintf(fname_out, "%s_%04d_ram_%d_%d.dat", (DaqCtrl->GetFilePrefix()).data(), fRun_num,board, channel+i*8);
      if( (fout = fopen(fname_out, "w")) == NULL) {
	GI_ERR(" Cannot open output file %s\n", fname_out);
	exit(1);
      }
      
      for(int j=0; j<4096; j++) fprintf(fout, "%d\n", histo_data[i][j]);

      GI_MSG(" board=%d ch=%d out on file %s\n",board, channel+i*8,fname_out);

      fclose(fout);
      //    }
  }
}

void DoSamples(void)
{
  char logstring[256], fname_out[256];
  uint32_t SampleBuffer[MAX_SAMPLES];
  FILE *fout;
  int i, j, k;
  int y, err, SampleCount;
  bool brd_full[MAX_BOARDS];
	
  // Open out file
  sprintf(fname_out, "%s.dat", (DaqCtrl->GetFilePrefix()).data());
  sprintf(logstring, "Start Getting %d Full FIFO samples data on all enabled channels",
	  DaqCtrl->GetEventPreset());
  cout << logstring << endl;
  log->Write(logstring);
  sprintf(logstring, "Writing on file: '%s'", fname_out);
  cout << logstring << endl;

  log->Write(logstring);

  if( (fout = fopen(fname_out, "w")) == NULL)
    {
      sprintf(logstring, "Can't open output file '%s'", fname_out);
      cout << logstring << endl;
      log->Write(logstring);
      exit(1);
    }

  DaqCtrl->Start();
  
  do
    {
      // EnableAcq()
      gMPD->EnableAcq();

      for(j=0; j<AdcBoardCount; j++)
	brd_full[j] = false;
	    
      // Wait for all FIFO full
      i = 0;

      /***** TO BE CHANGED
      do
	{
	  // CheckFIFOnotFull()
	  for(j=0; j<AdcBoardCount; j++)
	    {
	      Adc[j]->GetAllFifoFlags(&full, &empty);
	      if( full == Adc[j]->GetApvEnableMask() )
		brd_full[j] = true;
	    }
	  y = 0;
	  for(j=0; j<AdcBoardCount; j++)
	    if( brd_full[j] )
	      y++;
	}
      
      while( (y != AdcBoardCount) && (sig_int == 1) && (i++ < DaqCtrl->GetMaxRetry()) );
      */
      if( i >= DaqCtrl->GetMaxRetry() )
	{
	  sprintf(logstring, "Got max_retry while waiting for all FIFO full at event %d",
		  DaqCtrl->GetEventNumber());
	  cout << logstring << endl;
	  log->Write(logstring);
	}
      
      // Disable acquisition
      
      gMPD->DisableBoards();
      
      // Get data and write to file
      
      if( (sig_int == 1) && (i < DaqCtrl->GetMaxRetry()) ) {
	  
	gMPD->ReadSampleData(fout, DaqCtrl->GetDaqFormat(), DaqCtrl->GetEventNumber());
	  
	DaqCtrl->incEvt();
      }
    }
  while( DaqCtrl->acqRunning() && (sig_int == 1));
  
  DaqCtrl->Stop();
  DaqCtrl->printEventSummary();
  fclose(fout);
}

void DoEvent()
{
  char logstring[256], fname_out[1000];

  int j;
  int global_fifo_error;
  uint32_t multiple_events;
  
  int itimeout;
  int tritout=0;

  int trig;
  int mpd_trg = 0;

  int ApvSamples=0; // 1 = 1 sample x trigger, 3 = 3 samples x trigger (internal APV Trigger Mode)

  char fout_scaler[1000];
  sprintf(fout_scaler, "%s_%04d_scaler.txt", (DaqCtrl->GetFilePrefix()).data(), fRun_num);
 
  sprintf(logstring, "Start Data Acquisition for %d events", DaqCtrl->GetEventPreset());
  cout << logstring << endl;
  log->Write(logstring);

  gMPD->InitAndReset();

  ApvSamples = 1; // gMPD->GetMaxSamples(); // give 1 or 3 @@@@@

  // init other VME modules
  // ...

  // *** open output file
  sprintf(fname_out, "%s_%04d.dat", (DaqCtrl->GetFilePrefix()).data(), fRun_num);
  sprintf(logstring, "Writing on file: '%s'", fname_out);
  cout << logstring << endl;
  log->Write(logstring);
  GI_IO *io = new GI_IO();

  if( DaqCtrl->GetDaqFormat() == DAQ_BIN )
    io->SetDataFormat(GI_IO::DATAFMT_binary);
  if( DaqCtrl->GetDaqFormat() == DAQ_HEX )
    io->SetDataFormat(GI_IO::DATAFMT_hexadecimal);
  if( DaqCtrl->GetDaqFormat() == DAQ_DEC )
    io->SetDataFormat(GI_IO::DATAFMT_decimal);
  //io->SetEventHeader("EVENT\n", 1);

  if (io->OpenOutfile(fname_out)) // return 1 if file didn't open
    {
      sprintf(logstring, "Can't open output file '%s'", fname_out);
      cout << logstring << endl;
      log->Write(logstring);
    }

  io->WriteBinaryFileHeader(ApvSamples,gMPD->GetNumberEnabledCards(), // to be improved
			    1, // old daq mode, must be changed
			    gMPD);

  cout << __FUNCTION__ << " Number of Enabled cards = " << gMPD->GetNumberEnabledCards() << endl;

  gMPD->LoadPedAndThr();
  gMPD->EnableAcq();

  DaqCtrl->Start();

  multiple_events = 0;
  global_fifo_error = 0;

  time_t time_scaler;
  time(&time_scaler);

  int counter, counter_old;

  usleep(500);

  do {  // ********* event loop

    fTrg->PreEvent();

    itimeout = 0;

    do { // ******** wait for trigger
      trig = 0;

      trig = fTrg->Polling();

      itimeout++;
    }  while ((trig == 0) && (sig_int == 1) && (itimeout<DaqCtrl->GetMaxRetry()));

    // *** Read MPD
    if (trig & 1) { // ***** expected trigger in MPD

      //      printf(" Next Event\n");

      usleep(1); // width of the veto output (?)
      
      io->WriteBinaryTriggerPattern(DaqCtrl->GetEventNumber(), trig);

      int aaa = DaqCtrl->GetMaxRetry();
      if (gMPD->EVENT_Read(aaa)) { // true means readout done, data can be saved

	// save MPD data;
       	io->WriteMPDEvent(gMPD, DaqCtrl->GetEventNumber());
	mpd_trg++;
      }

      // Read scaler
      if (DaqCtrl->timeExceed(DaqCtrl->GetTimeScaler(),time_scaler)) {

	cout << __FUNCTION__ << " Events with trigger= " << DaqCtrl->GetEventNumber() << " MPD samples on file = " << mpd_trg << endl;
	//      io->WriteBinaryVMEData(DaqCtrl->GetEventNumber());     // TO BE DEFINED
      }

      io->WriteBinaryEndOfEvent(DaqCtrl->GetEventNumber()); // end of event tag

      DaqCtrl->incEvt();

      tritout=0;

    } else { // no trigger .. maybe cosmic or low repetition beam
      if ((tritout % 1000) == (999)) {
	cout << __FUNCTION__ << " Trigger timeout (or CTRL-c pressed), count= " 
	     << dec << tritout << " ... waiting for beam or cosmics? -> do not worry" << endl;
      }
      tritout +=1;
    }
    
    fTrg->PostEvent();

  } while( DaqCtrl->acqRunning() && (sig_int == 1));
  
  DaqCtrl->Stop();
  DaqCtrl->printEventSummary();
  io->CloseOutfile();
  delete io;
  
  sprintf(logstring, "Got %d multiple events", multiple_events);
  cout << logstring << endl;
  log->Write(logstring);
  
  for(j=0; j<gMPD->GetBoardCount(); j++)
    sprintf(logstring, "Board %d: Written %d events, %d triggers lost (skipped)",
	    j, DaqCtrl->GetEventNumber(), gMPD->getMissedTriggers(j));

  cout << logstring << endl;
  log->Write(logstring);
  
  printf("Data on file: %s\n", fname_out);
  printf("MPD with triggers %d\n", mpd_trg);

}

