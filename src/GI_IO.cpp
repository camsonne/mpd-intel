/*
 *output file handling object
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "GI_IO.h"

#define BINARYFILE_ID 0xE5C190C0
#define BINARYAPV_ID 0xA0000000
#define BINARYVME_ID 0x10000000
#define BINARYTRG_ID 0x20000000
#define BINARYMRC_ID 0xB0000000
#define BINARYINF_ID 0xD0000000
#define BINARYEND_ID 0xE0000000 // end of event

#define BINARYFILE_VERSION 0x5

GI_IO::GI_IO()
{
  FileHeader   = NULL;
  EventHeader  = NULL;
  SampleHeader = NULL;

  FileHeaderConfig   = 0;
  EventHeaderConfig  = 0;
  SampleHeaderConfig = 0;

  infile       = NULL;
  outfile      = NULL;
  
  NumAPVs      = 1;
  NumEvents    = 0;
  DataFormat   = DATAFMT_binary;
  MaxEvents    = 16;	// (EventSize * MaxEvents) >= DAQ_FIFO_SIZE
  EventSize    = 130;
  Event        = new uint32_t[EventSize*MaxEvents+100]; //TBC
  ShortEvent   = new uint16_t[EventSize];
  Event32 = new uint32_t[EventSize];

  ClearEvent();
}

GI_IO::~GI_IO()
{
  if (FileHeader)
    delete[] FileHeader;
  if (EventHeader)
    delete[] EventHeader;
  if (SampleHeader)
    delete[] SampleHeader;

  delete[] Event;
  delete[] Event32;
  delete[] ShortEvent;

}
//--------------------------------------------------------------//
void GI_IO::SetFileHeader(const char* hdrmsg, int hdrcfg)
{
  if (FileHeader)
    delete[] FileHeader;
  FileHeader = new char[strlen(hdrmsg+1)];
  strcpy(FileHeader, hdrmsg);
  FileHeaderConfig = hdrcfg;
}

void GI_IO::WriteBinaryFileHeader(const uint32_t n_trig, const uint32_t n_apv, 
				      const uint32_t daqmode,
				      MPDhi *gMPD) {

  uint32_t code = BINARYFILE_ID;
  uint32_t version = BINARYFILE_VERSION;
  
  uint32_t mpdconf, adconf,etconf,levelconf, apvconf;

  fwrite(&code,sizeof(uint32_t),1,outfile);
  fwrite(&version,sizeof(uint32_t),1,outfile);
  fwrite(&n_trig,sizeof(uint32_t),1,outfile);
  fwrite(&n_apv,sizeof(uint32_t),1,outfile);
  fwrite(&daqmode, sizeof(int32_t),1,outfile);

  int32_t nMPD = gMPD->GetBoardCount();
  fwrite(&nMPD, sizeof(int32_t),1,outfile);

  MPDlo *mp;
  for (int ib=0;ib<nMPD;ib++) { // loop on boards (bus ??)

    mp = gMPD->getModule(ib);

    mpdconf = mp->ApvGetSize() | (mp->GetSlot() << 4) | (mp->GetTriggerNumber() << 12) | (mp->GetCalibLatency() << 20);
    fwrite(&mpdconf, sizeof(int32_t),1,outfile);

    etconf = mp->GetAdcClockPhase(0) | (mp->GetAdcClockPhase(1) << 8) |
      (mp->GetCommonOffset() << 16) |  (mp->GetCommonNoiseSubtraction() << 29) |
      ((mp->GetEventBuilding() ? 1 : 0) << 30);
    fwrite(&etconf, sizeof(int32_t),1,outfile);

    levelconf = mp->GetZeroLevel() | (mp->GetOneLevel() << 16);
    fwrite(&levelconf, sizeof(int32_t),1,outfile);

    MPD_MSG("Board %d, with conf (mpd/etc/level)= 0x%x 0x%x 0x%x\n",ib,mpdconf, etconf, levelconf);
    for (int adc=0;adc<2;adc++) {
      adconf = 0;
      for (int ich=0;ich<8;ich++) {
	adconf |= (mp->GetAdcGain(adc,ich) << (4*ich));
      }
      fwrite(&adconf, sizeof(int32_t),1,outfile);
      MPD_MSG("Board %d, ADC %d config= 0x%x\n",ib, adc, adconf);
    }

    for (int j=0;j<(int)mp->ApvGetSize();j++) { // look on card/fifo
      apvconf = mp->ApvGetAdc(j) | (mp->ApvGetI2C(j) << 4) |
	(mp->ApvGetSample(j) << 9) | (mp->ApvGetLatency(j) << 16) | // GetMaxLatency ???
	(mp->ApvGetMode(j) << 24);
      fwrite(&apvconf, sizeof(int32_t),1,outfile);
      MPD_MSG("Card %d configuration= 0x%x\n",j,apvconf);
    }
  }

}

//

void GI_IO::WriteBinaryVMEData(const uint32_t evt) {

  uint32_t code = BINARYVME_ID | (evt & 0xFFFFFFF);

  fwrite(&code,sizeof(uint32_t),1,outfile);
  
  // TBD

}

//
void GI_IO::WriteBinaryUInfoData(const uint32_t evt, const uint32_t count, const uint32_t *data) {

  uint32_t code = BINARYINF_ID | (evt & 0xFFFFFFF);
  fwrite(&code,sizeof(uint32_t),1,outfile);
  fwrite(&count,sizeof(uint32_t),1,outfile);
  fwrite(data,sizeof(uint32_t),count,outfile);

}

//
void GI_IO::WriteBinaryEndOfEvent(const uint32_t evt) {
  uint32_t code = BINARYEND_ID | (evt & 0xFFFFFFF);
  fwrite(&code,sizeof(uint32_t),1,outfile);
}

/*
 * Write trigger pattern on file
 */
void GI_IO::WriteBinaryTriggerPattern(const uint32_t evt, const uint32_t trg) {

  uint32_t code = BINARYTRG_ID | (evt & 0xFFFFFFF);

  fwrite(&code,sizeof(uint32_t),1,outfile);
  fwrite(&trg,sizeof(uint32_t),1,outfile);

}

void GI_IO::SetEventHeader(const char* hdrmsg, int hdrcfg)
{
  if (EventHeader)
    delete[] EventHeader;
  EventHeader = new char[strlen(hdrmsg+1)];
  strcpy(EventHeader, hdrmsg);
  EventHeaderConfig = hdrcfg;
}
void GI_IO::SetSampleHeader(const char* hdrmsg, int hdrcfg)
{
  if (SampleHeader)
    delete[] SampleHeader;
  SampleHeader = new char[strlen(hdrmsg+1)];
  strcpy(SampleHeader, hdrmsg);
  SampleHeaderConfig = hdrcfg;
}
//--------------------------------------------------------------//
int GI_IO::OpenInfile(const char* filename)
{
  if (infile)
    {
      printf("%s::%s(%d): Closing infile before opening new one...\n",
	     __FILE__, __FUNCTION__, __LINE__);
      CloseInfile();
    };
  // try to open file:
  infile = fopen(filename, "r");
  if (infile==NULL)
    {
      printf("%s::%s(%d): When trying to open >%s< for reading",
	     __FILE__, __FUNCTION__, __LINE__, filename);
      perror("");
      return 1;
    };
  return 0;
}
int GI_IO::OpenOutfile(const char* filename)
{
  if (outfile)
    {
      printf("%s::%s(%d): Closing outfile before opening new one...\n",
	     __FILE__, __FUNCTION__, __LINE__);
      CloseOutfile();
    };
  // try to open file:
  //  printf("opening >%s<...\n", filename);
  if (DataFormat == DATAFMT_binary )
    outfile = fopen(filename, "wb");
  else
    outfile = fopen(filename, "w");
  if (outfile==NULL)
    {
      printf("%s::%s(%d): When trying to open >%s< for writing",
	     __FILE__, __FUNCTION__, __LINE__, filename);
      perror("");
      return 1;
    };
  // write file header (if defined):
  if (FileHeader)
    fprintf(outfile, "%s", FileHeader);
  // write configuration header:
  if (EventHeader && DataFormat != DATAFMT_binary )
    fprintf(outfile, "%d %d %d", NumEvents, NumAPVs, EventSize);
  //for (int i=0; i<NumAPVs; i++)
  //  fprintf(outfile, " %d", APVconfig[i]);
  //if (DataFormat != DATAFMT_binary )
  //  fprintf(outfile, "\n");
  return 0;
}

int GI_IO::CloseInfile()
{
  if (infile)
    return fclose(infile);
  printf("%s::%s(%d): Cannot close infile, not opened yet...\n",
	 __FILE__, __FUNCTION__, __LINE__);
  return 1;
}
int GI_IO::CloseOutfile()
{
  if (outfile)
    return fclose(outfile);
  printf("%s::%s(%d): Cannot close outfile, not opened yet...\n",
	 __FILE__, __FUNCTION__, __LINE__);
  return 0;
}

//--------------------------------------------------------------//
int GI_IO::SetEventSize(int eventsize)
{
  if (eventsize<0)
    {
      printf("%s::%s(%d): Invalid eventsize, keeping current eventsize of %d\n",
	     __FILE__, __FUNCTION__, __LINE__, EventSize);
      return 1;
    };

  EventSize = eventsize;
  delete[] Event;
  Event = new uint32_t[EventSize*MaxEvents];
  ClearEvent();
  return 0;
}
int GI_IO::SetNumberEvents(int numevents)
{
  if (numevents<0)
    {
      printf("%s::%s(%d): Invalid event number, keeping current event number of %d\n",
	     __FILE__, __FUNCTION__, __LINE__, NumEvents);
      return 1;
    };

  NumEvents = numevents;
  return 0;
}
int GI_IO::SetNumberAPVs(int numapvs)
{
  if (numapvs<0)
    {
      printf("%s::%s(%d): Invalid number of APVs, keeping current number of APVs of %d\n",
	     __FILE__, __FUNCTION__, __LINE__, NumAPVs);
      return 1;
    };

  NumAPVs = numapvs;
  return 0;
}
int GI_IO::SetDataFormat(int dataformat)
{
  DataFormat = dataformat;
  return 0;
}
//--------------------------------------------------------------//
int GI_IO::ReadEvent()
{
  if (infile==NULL)
    {
      printf("%s::%s(%d): Cannot read from infile, not opened yet...\n",
	     __FILE__, __FUNCTION__, __LINE__);
      return 1;
    };

  return 0;
}

/*
 *
 */

int GI_IO::WriteSingleAPV(int evt, int board, uint32_t *buf, int i0, int i1) {

  uint16_t bac;

  if (outfile==NULL)
    {
      printf("%s::%s(%d): Cannot write to outfile, not opened yet...\n", __FILE__, __FUNCTION__, __LINE__);
      return 1;
    };
  
  // write events header:

  if (EventHeader && DataFormat != DATAFMT_binary )
    fprintf(outfile, "\n%10d %4d %4x\n",evt, board, buf[i0]);
  // write event:
  switch (DataFormat)
    {
    case DATAFMT_binary2:
      bac = ((board<<4)&0x3FF0);
      fwrite(&bac,sizeof(uint16_t),1,outfile);
      for (int i=i0; i<i1; i++)
	{
	  ShortEvent[i] = (uint16_t)buf[i];
	  //short int sdummy;
	  //sdummy = (uint16_t)Event[((n-1)*EventSize)+i];
	  //fwrite(&sdummy, sizeof(short int), 1, outfile);
	};
      fwrite(ShortEvent, sizeof(uint16_t), i1-i0+1, outfile);
      break;

    case DATAFMT_binary:
      //        fwrite(&evt,sizeof(uint32_t),1,outfile);
      //      fwrite(&board,sizeof(uint32_t),1,outfile);
      // for (int i=i0; i<i1; i++)
      fwrite(&buf[i0], sizeof(uint32_t), i1-i0+1, outfile);
      break;

    case DATAFMT_hexadecimal:
      fprintf(outfile, "Event: %d\tBoard: %d\n", evt, board);
      for (int i=i0; i<i1; i++)
  	{
  	  fprintf(outfile, "%06x ", (uint32_t) buf[i]);
  	  if( (i-i0)%16 == 0 ) fprintf(outfile, "\n");
  	};
      break;
    case DATAFMT_decimal:
      fprintf(outfile, "Event: %d\tBoard: %d\n", evt, board);
      for (int i=i0; i<i1; i++)
  	{
  	  fprintf(outfile, " %4d", buf[i]);
  	  if( (i-i0)%16 == 0 ) fprintf(outfile, "\n");
  	};
      break;
    };
  //  fflush(outfile);
  //  ClearEvent();
  return 0;

}

int GI_IO::WriteEvent(int evt, int board, int apv, int n)
{
  int i;
  uint16_t bac;

  uint32_t head;
  uint32_t trai;
  uint32_t endblo;

  int i0;

  if (outfile==NULL)
    {
      printf("%s::%s(%d): Cannot write to outfile, not opened yet...\n", __FILE__, __FUNCTION__, __LINE__);
      return 1;
    };
  
  // write events header:
  for(i = 0; i<n; i++)
  {
    if (EventHeader && DataFormat != DATAFMT_binary )
      //    fprintf(outfile, "\n%s", EventHeader);
      fprintf(outfile, "\n%10d %4d %4d\n",evt, board, apv);
    // write event:
    switch (DataFormat)
      {
      case DATAFMT_binary2:
	bac = ((board<<4)&0x3FF0) | (apv&0xF);
        fwrite(&bac,sizeof(uint16_t),1,outfile);
        for (int i=0; i<EventSize; i++)
	  {
	    ShortEvent[i] = (uint16_t)Event[((n-1)*EventSize)+i];
	    //short int sdummy;
	    //sdummy = (uint16_t)Event[((n-1)*EventSize)+i];
	    //fwrite(&sdummy, sizeof(short int), 1, outfile);
	  };
  	fwrite(ShortEvent, sizeof(uint16_t), EventSize, outfile);
        break;
      case DATAFMT_binary: // currently used
	evt += (n-1);
	i0 = (n-1)*EventSize;
	
	head = ((Event[i0] & 0xFFF) <<4) | (apv & 0xF);
        fwrite(&head,sizeof(uint32_t),1,outfile);
        for (int i=0; i<EventSize-2; i++)
	  {
	    Event32[i] = 0x80000 | ((i << 12) & 0x7F000) | (Event[i0+i+1] & 0xFFF);
	  };
  	fwrite(Event32, sizeof(uint32_t), EventSize-2, outfile);
	trai = 0x100000 | ((board & 0x1F) << 12) | (Event[i0+EventSize-1] & 0xFFF);
	endblo = 0x180000 | (EventSize & 0xFF);
	fwrite(&trai,sizeof(uint32_t),1,outfile);
	fwrite(&endblo,sizeof(uint32_t),1,outfile);

	/*
	printf(" HDR / TRL / ENDBLK / EVENTSIZE = %x %x %x %d\n",head, trai, endblo, EventSize);
	for (int i=1; i<EventSize-1;i++) {
	  printf(" %06x",Event32[i]
	*/

        break;
      case DATAFMT_hexadecimal:
        fprintf(outfile, "Event: %d\tBoard: %d\tApv: %d\n", evt, board, apv);
        fprintf(outfile, "%03x\n", (uint32_t)Event[0]);
        for (int i=1; i<(EventSize-1); i++)
  	{
  	  fprintf(outfile, "%03x ", (uint32_t)Event[i]);
  	  if( i%16 == 0 ) fprintf(outfile, "\n");
  	};
        fprintf(outfile, "%03x\n", (uint32_t)Event[EventSize-1]);
        break;
      case DATAFMT_decimal:
        fprintf(outfile, "Event: %d\tBoard: %d\tApv: %d\n", evt, board, apv);
        for (int i=0; i<EventSize; i++)
  	{
  	  fprintf(outfile, " %4d", Event[i]);
  	  if( i%16 == 0 ) fprintf(outfile, "\n");
  	};
        break;
      };
//  fflush(outfile);
//  ClearEvent();
  }  
  return 0;
}

//--------------------------------------------------------------//
void GI_IO::ClearEvent()
{
  for (int i=0; i<(EventSize*MaxEvents); i++)
    Event[i] = (uint32_t)0;
}

//--------------------------------------------------------------//

void GI_IO::WriteMPDEvent(MPDhi *gMPD, const uint32_t evt) {
  
  int ib,j,is;
  uint32_t head;
  uint32_t trai;
  uint32_t endblo;
  int k; // element
  int slot;
  MPDlo *mp;

  // APV header
  uint32_t code = BINARYAPV_ID | (evt & 0xFFFFFFF);
  fwrite(&code,sizeof(uint32_t),1,outfile);

  for (ib=0;ib<gMPD->GetBoardCount();ib++) { // loop on boards (bus ??)

    mp = gMPD->getModule(ib);
    slot = mp->GetSlot();

    MPD_DBG("Write Board = %d, with cards= %d\n",ib,mp->ApvGetSize());

    for (j=0;j<(int)mp->ApvGetSize();j++) { // look on card/fifo

      k=0;

      int mpsa = mp->ApvGetBufferSample(j);
      MPD_DBG("Write Card = %d, with samples= %d\n",j,mpsa);

      for (is=0;is<mpsa;is++) { // loop on samples

	int evtsize = mp->ApvGetEventSize(j);
	int head0 = mp->ApvGetBufferElement(j, k);
	k++;
	head = ((head0 & 0xFFF) <<4) | (mp->ApvGetAdc(j) & 0xF);
	fwrite(&head,sizeof(uint32_t),1,outfile);
	MPD_DBG("Sample = %d, Header = 0x%x\n",is, head);
	for (int i=0; i<evtsize-2; i++)
	{
	  Event32[i] = 0x80000 | ((i << 12) & 0x7F000) | (mp->ApvGetBufferElement(j,k) & 0xFFF);
	  k++;
	};
	fwrite(Event32, sizeof(uint32_t), evtsize-2, outfile);
	trai = 0x100000 | ((slot & 0x1F) << 12) | (mp->ApvGetBufferElement(j,k) & 0xFFF);
	k++;
	endblo = 0x180000 | (evtsize & 0xFF);
	fwrite(&trai,sizeof(uint32_t),1,outfile);
	fwrite(&endblo,sizeof(uint32_t),1,outfile);
	MPD_DBG("Sample idx = %d, Trailer / EndBlock = 0x %x / %x\n", is, trai, endblo);
      }

      MPD_DBG("%d words from card %d written on file\n",k,j);
      mp->ApvShiftDataBuffer(j,k); // clean out elements written on file
    }

  }

  head = 0x40000; // end of APV event block
  fwrite(&head,sizeof(uint32_t),1,outfile);

};

//
