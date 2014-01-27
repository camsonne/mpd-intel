/* DaqControl.h
 *
 * Singleton
 */

#ifndef __GI_DAQCONTROL__
#define __GI_DAQCONTROL__

#include <iostream>
#include <fstream>
#include <string>
#include <time.h>

#include "Logger.h"

#define DAQ_HEX	1
#define DAQ_DEC	2
#define DAQ_BIN	3

#define MAX_TRIGGER_TYPE 16

using namespace std;

class DaqControl {

 public:
  DaqControl();
  ~DaqControl() { };
  
  static DaqControl *GetDAQControl();
  
  void SetController(int type) { controller_type = type; }
  void SetLogFile(string name) { logfilename = name; }
  void SetLogLevel(int l) { loglevel = l; }
  void SetDaqMode(int m) { daq_mode = m; }
  void SetDaqFormat(int f) { daq_format = f; }
  void SetProgressStep(int s) { progress_step = s; }
  void SetTimePreset(int t) { time_preset = t; }
  void SetHistoTime(int t) { histo_time = t; }
  void SetHistoChannel(int c) { histo_channel = c; }
  void SetHistoChannelStart(int c) { histo_channel_start = c; }
  void SetHistoChannelStop(int c) { histo_channel_stop = c; }
  void SetHistoBoard(int b) { histo_board = b; }
  void SetHistoBoard(int b0, int b1) { histo_board_start = b0; histo_board_stop = b1; }
  void SetEventPreset(int e) { event_preset = e; }
  void SetMaxRetry(int m) { max_retry = m; }
  void SetFilePrefix(string f) { file_prefix = f; }
  void SetTrigMode(int m) { trig_mode = m; }
  void SetTrigNumber(int n) { trig_number = n; }
  void SetTrigSource(int t) { fTrgSource = t;}

  void SetApvMode(int m) { fApvMode = m;}

  void SetTimeScaler(int t) { fTimeScaler = t;}
  int GetTimeScaler() { return fTimeScaler; }

  void SetPulserPeriod(int t) { fPulserPeriod = t;}
  int GetPulserPeriod() { return fPulserPeriod; }

  int GetController(void) { return controller_type; }
  string GetLogFile(void) { return logfilename; }
  int GetLogLevel(void) { return loglevel; }
  int GetDaqMode(void) { return daq_mode; }

  int GetApvMode(void) { return fApvMode; }

  int GetDaqFormat(void) { return daq_format; }
  int GetProgressStep(void) { return progress_step; }
  int GetTimePreset(void) { return time_preset; }
  int GetHistoTime(void) { return histo_time; }
  int GetHistoChannel(void) { return histo_channel; }
  int GetHistoChannelStart(void) { return histo_channel_start; }
  int GetHistoChannelStop(void) { return histo_channel_stop; }
  int GetHistoBoard(void) { return histo_board; }
  int GetHistoBoardStart(void) { return histo_board_start; }
  int GetHistoBoardStop(void) { return histo_board_stop; }
  int GetEventPreset(void) { return event_preset; }
  int GetEventNumber(void) { return event_loop; }
  int GetMaxRetry(void) { return max_retry; }
  string GetFilePrefix(void) { return file_prefix; }
  int GetTrigMode(void) { return trig_mode; }
  int GetTrigNumber(void) { return trig_number; }
  int GetTrigSource(void) { return fTrgSource; }
  void Start(void);
  void Stop(void);

  void incEvt(long int evt);
  void incEvt(void);
  void incTrig(int trg_type, long int ntrg=1);
  void incTrigVec(int trg_mask, long int ntrg=1);

  void incTimeout(long int ntout=1);

  bool timeExceed(time_t time_step, time_t &time_old);
  bool acqRunning(void);
  void progressBar(void);
  void printEventSummary(void);
  time_t startTime() { return time_acqstart; };

  int GetLatencyStart(void) { return latency_start; }
  int GetLatencyStop(void) { return latency_stop; }
  int GetLatencyStep(void) { return latency_step; }
  int GetCalibLatency(void) { return calib_latency; }

  void SetLatencyStart(int l) { latency_start = l; }
  void SetLatencyStop(int l) { latency_stop = l; }
  void SetLatencyStep(int l) { latency_step = l; }
  void SetCalibLatency(int l) { calib_latency = l; }
  
  void SetEnSync_P0(int s) { EnSync_P0 = s; }
  void SetEnSync_Front(int s) { EnSync_Front = s; }
  void SetEnTrig1_P0(int s) { EnTrig1_P0 = s; }
  void SetEnTrig2_P0(int s) { EnTrig2_P0 = s; }
  void SetEnTrig_Front(int s) { EnTrig_Front = s; }
  int GetEnSync_P0(void) { return EnSync_P0; }
  int GetEnSync_Front(void) { return EnSync_Front; }
  int GetEnTrig1_P0(void) { return EnTrig1_P0; }
  int GetEnTrig2_P0(void) { return EnTrig2_P0; }
  int GetEnTrig_Front(void) { return EnTrig_Front; }

  void SetRunNumber(int l) { run_num = l; }
  int GetRunNumber(void) { return run_num; }

  void SetCommonModeFlag(int cms) { common_mode_subtraction_flag = cms; }
  int  GetCommonModeFlag(void) { return common_mode_subtraction_flag; }
  void SetEventBuilderFlag(int eb) { event_builder_enable_flag = eb; }
  int  GetEventBuilderFlag(void) { return event_builder_enable_flag; }
  void SetCommonOffset(int co) { common_offset = co; }
  int  GetCommonOffset(void) { return common_offset; }

  void SetInfoString(string s) { fInfo = s; }
  string GetInfoString() { return fInfo; }

  void LogAll();

  int GetTimeDiff() { return fTime_diff; }

 private:

  int controller_type;
  string logfilename;
  int loglevel;
  int daq_mode;
  int fApvMode;

  int common_mode_subtraction_flag;
  int event_builder_enable_flag;
  int common_offset;
  int daq_format;
  int progress_step;
  int time_preset;
  int histo_time;
  int histo_channel;
  int histo_channel_start;
  int histo_channel_stop;
  int histo_board;
  int histo_board_start, histo_board_stop;
  int event_preset;
  int max_retry;
  string file_prefix;
  int latency_start;
  int latency_stop;
  int latency_step;

  int calib_latency;
  int EnSync_P0, EnSync_Front, EnTrig1_P0, EnTrig2_P0, EnTrig_Front;

  int trig_mode;
  int trig_number;

  int fTrgSource; // trigger source (0=any, 1=signal, 2=pulser, ... 99=disabled)

  int fTimeScaler; // approx time between scaler read (s)

  int fPulserPeriod; // period of the pulser (for pedestal or other apps)

  int fTrigCount[MAX_TRIGGER_TYPE];
  int fTimeoutCount;

  time_t time0, time1;  // timeout times
  time_t time_acqstart, time_real;
  int event_loop;
  int progress_old;

  string fInfo;

  int run_num;

  ofstream fout;

  Logger *log;

  static DaqControl *fDAQ;

  int fTime_diff;

};

// extern DaqControl DaqCtrl;

#endif
