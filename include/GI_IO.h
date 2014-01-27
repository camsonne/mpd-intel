#ifndef  __GI_IO_H__
#define __GI_IO_H__

#include "MPDhi.h"

class GI_IO
{
 public:
  GI_IO();
  ~GI_IO();

  // methods to set headers for
  void SetFileHeader(const char* hdrmsg, int hdrcfg);    // beginning of file
  void SetEventHeader(const char* hdrmsg, int hdrcfg);   // beginning of an event
  void SetSampleHeader(const char* hdrmsg, int hdrcfg);  // beginning of a sample

  // open and close files for reading or writing
  int OpenInfile(const char* filename);
  int OpenOutfile(const char* filename);
  int CloseInfile();
  int CloseOutfile();
  // read or write events from/to file:
  int SetEventSize(int eventsize);
  inline int GetEventSize(){return EventSize;};
  int SetNumberEvents(int numevents);
  int SetNumberAPVs(int numapvs);
  int SetDataFormat(int dataformat);
  int ReadEvent();
  int WriteEvent(int evt, int board, int apv, int n);
  int WriteSingleAPV(int evt, int board, uint32_t *buf, int i0, int i1);

  uint32_t *EventBuffer(){return Event;};

  void WriteBinaryFileHeader(const uint32_t n_trig, const uint32_t n_apv,  // out file must be open
			     const uint32_t daqmode,
			     MPDhi *gMPD);

  void WriteBinaryUInfoData(const uint32_t evt, const uint32_t count, const uint32_t *data);

  void WriteBinaryEndOfEvent(const uint32_t evt); // end of event

  void WriteBinaryVMEData(const uint32_t evt);

  void WriteBinaryTriggerPattern(const uint32_t evt, const uint32_t trg);

  void WriteMPDEvent(MPDhi *gMPD, const uint32_t evt);
 
  FILE *getOutFileDesc() { return outfile; };

enum DataFormat
  {
    DATAFMT_binary,
    DATAFMT_hexadecimal,
    DATAFMT_decimal,
    DATAFMT_binary2
  };

 protected:

 private:
  char *FileHeader, *EventHeader, *SampleHeader;
  int   FileHeaderConfig, EventHeaderConfig, SampleHeaderConfig;

  FILE *infile, *outfile;

  int EventSize, MaxEvents, NumEvents, NumAPVs, DataFormat;
  uint32_t *Event, *Event32;
  uint16_t *ShortEvent;

  void ClearEvent();

};

#endif
