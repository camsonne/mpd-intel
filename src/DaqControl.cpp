/* DaqControl.cpp
*/
#include <iostream>
#include <string>
#include <time.h>

#include "DaqControl.h"

using namespace std;

// DaqControl DaqCtrl;

DaqControl* DaqControl::fDAQ = 0;
DaqControl* DaqControl::GetDAQControl() { return fDAQ; }


DaqControl::DaqControl()
{

    if (fDAQ) {
      cout << __FUNCTION__ << ": DAQControl is a singleton, cannot be allocated twice!" << endl;
      return;
    }

    fDAQ = this;

    controller_type = -1;
    loglevel = 0;
    max_retry = 0;
    log = new Logger(); // singleton, done once only

    cout << __FUNCTION__ << ": allocated" << endl;

    fTrgSource = 0;

}

void DaqControl::Start(void)
{
	event_loop = 0;

	for (int i=0; i<MAX_TRIGGER_TYPE;i++) {
	  fTrigCount[i] = 0;
	}
	fTimeoutCount=0;

	time_real = 0;
	progress_old = 0;

	time(&time_acqstart);

}

void DaqControl::Stop(void)
{
}

void DaqControl::printEventSummary(void)
{
	char logstring[256];

	sprintf(logstring,"# Events = %d\n# Time_Real_sec = %d\n# Events/Real_sec = %f\n",
		event_loop, (int) time_real, ((float) event_loop)/((float)time_real));
	cout << logstring;
	log->Write(logstring);

	for (int i=0;i<MAX_TRIGGER_TYPE;i++) {
	  if (fTrigCount[i]>0) {
	    sprintf(logstring,"#  Trg %d: total= %d Rate= %f Hz\n",i,fTrigCount[i], ((float) fTrigCount[i])/time_real);
	    cout << logstring;
	    log->Write(logstring);
	  }
	}

	sprintf(logstring,"#  Timeout: total= %d Rate= %f Hz\n", fTimeoutCount, ((float) fTimeoutCount)/time_real);
	cout << logstring;
	log->Write(logstring);
	
}

void DaqControl::progressBar(void)
{

	int progress;
	int progress_time, progress_evt;

	progress_time = (((float) time_real)/time_preset)*progress_step;
	progress_evt = (((float) event_loop)/event_preset)*progress_step;
	progress = progress_time > progress_evt ? progress_time : progress_evt;

	progress = progress < progress_step ? progress : progress_step;
					
	for (int jprog = 0; jprog<(progress - progress_old); jprog++)
	{
		if (progress_time > progress_evt)
		{
			printf("T");
		}
		else
		{
			printf("E");
		}
		fflush(stdout);
	}
						
	if (progress == progress_step)
		printf("\n");

	progress_old = progress;
}


void DaqControl::incEvt(long int evt)
{

	event_loop += evt;

}

void DaqControl::incEvt()
{
	incEvt(1);
}


void DaqControl::incTrig(int trg_type, long int ntrg) {
  if (trg_type < MAX_TRIGGER_TYPE) {
    fTrigCount[trg_type] += ntrg;
  }
}

void DaqControl::incTrigVec(int trg_mask, long int ntrg) {

  int i;

  //  cout << __FUNCTION__ << " " << trg_mask << endl;

  if (trg_mask == 0) {
    incTimeout();
  } else {
    i=0;
    while (((trg_mask>>i) > 0) && (i<MAX_TRIGGER_TYPE)) {
      fTrigCount[i] += ((trg_mask>>i) & 1)*ntrg;
      //      cout << i << " " << fTrigCount[i] << endl;
      i++;
    }
  }

}


void DaqControl::incTimeout(long int ntout) {
  fTimeoutCount += ntout;
}


bool DaqControl::acqRunning()
{

	time_t temp;

	time(&temp);
	time_real = temp - time_acqstart;

	progressBar();

	if (event_loop >= event_preset)
		return false;

	if (time_real >= time_preset)
		return false;

	return true;
}

/**
 * return true if time elapsed from previos call has exiceed time_step
 * time step is in second
 * time_old is changed (internal use)
 */

bool DaqControl::timeExceed(time_t time_step, time_t &time_old) {

  time_t temp;
  time_t diff;

  time(&temp);

  diff = temp - time_old;

  if (diff > time_step) {
    time_old = temp;
    fTime_diff = diff;
    return true;
  } 

  return false;

}

void DaqControl::LogAll() {

  log->WritePar("Time_Scaler", GetTimeScaler());
  log->WritePar("Pulser_Period", GetPulserPeriod());
  log->WritePar("Controller_Type", GetController());
  log->WritePar("Log_File_Name", GetLogFile());

  log->WritePar("Logging_Level", GetLogLevel());
  log->WritePar("Daq_Mode", GetDaqMode());
  log->WritePar("APV_Mode", GetApvMode());
  log->WritePar("Daq_Format", GetDaqFormat());
  log->WritePar("Progress_Step", GetProgressStep());
  log->WritePar("Time_Preset", GetTimePreset());

  log->WritePar("Histogram_Time", GetHistoTime());
  log->WritePar("Histogram_Channel", GetHistoChannel());
  log->WritePar("Histogram_Channel Start", GetHistoChannelStart());
  log->WritePar("Histogram_Channel Stop", GetHistoChannelStop());
  log->WritePar("Histogram_Board", GetHistoBoard());
  log->WritePar("Histogram_Board_Start", GetHistoBoardStart());
  log->WritePar("Histogram_Board_Stop", GetHistoBoardStop());

  log->WritePar("Event_Preset", GetEventPreset());
  log->WritePar("Event_Number", GetEventNumber());

  log->WritePar("Max_Retry", GetMaxRetry());
  log->WritePar("Output_File_Prefix", GetFilePrefix());
  log->WritePar("Trigger_Mode", GetTrigMode());
  log->WritePar("Trigger_Number", GetTrigNumber());
  log->WritePar("Trigger_Source", GetTrigSource());

  log->WritePar("Latency_Start", GetLatencyStart());
  log->WritePar("Latency_Stop", GetLatencyStop());
  log->WritePar("Latency_Step", GetLatencyStep());

  log->WritePar("Calibration_Latency", GetCalibLatency());

  log->WritePar("EnSync_P0", GetEnSync_P0());
  log->WritePar("EnSync_Front", GetEnSync_Front());
  log->WritePar("EnTrig1_P0", GetEnTrig1_P0());
  log->WritePar("EnTrig2_P0", GetEnTrig2_P0());
  log->WritePar("EnTrig_Front", GetEnTrig_Front());
  log->WritePar("Run_Number", GetRunNumber());
  log->WritePar("Common_Mode_Flag", GetCommonModeFlag());
  log->WritePar("Event_Builder_Flag", GetEventBuilderFlag());
  log->WritePar("Common_Offset", GetCommonOffset());
  log->WritePar("Info_String", GetInfoString());

}
