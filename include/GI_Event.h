/**
 * Event Handler
 */

#ifndef __GI_EVENT__
#define __GI_EVENT__

#include <vector>
#include <iostream>
#include <stdio.h>

#define EVT_UID 0x9A010E5A
#define EVT_VERSION 0x001  // VERSION[2-3] || REVISION[0-1]

#define EVT_TAG_BASE 0xC900

#define EVT_HEADER 0
#define EVT_MPD 1
#define EVT_PU 2
#define EVT_SCA 3


/**
 * Common class to all events block
 */

class EvtBlock {

 private:
  uint16_t fTag;
  std::vector<uint16_t>::iterator fRit; // read iterator
  int fCount, fCurrent;
  int *fError;
  int fELen; // error vector length
 protected:

  // Error handling
  void resetErrVector() {
    int i;
    if (fError) {
      for (i=0;i<fELen;i++) {
	fError[i]=0;
      }
    }
  };

  void allocErrVector(int len=1 ) {
    fError = new int[len];
    fELen = len;
    resetErrVector();
  };

  void incErr(int idx=0, int delta=1) {
    fError[idx % fELen] += delta;
  };

  int getErr(int idx=0) {
    return fError[idx % fELen];
  };

  int getTotErr() {
    int i;
    int e = 0;
    for (i=0;i<fELen;i++) {
      e += fError[i];
    }

    return e;
  };

  //
  std::vector<uint16_t> fBuffer;

  // clear buffer and prepend the Block Tag;
  void newBuffer(int elements=0) {
    fBuffer.clear();
    if (elements >0) {
      fBuffer.reserve(elements);
    };
    fBuffer.push_back(fTag);
    fBuffer.push_back(0); // number of word in block
    fRit = fBuffer.begin();
    fCount = 2; // tag + number of word
    fCurrent = 0;
  };
  
  void setTag(uint16_t tag) { fTag = EVT_TAG_BASE | (tag & 0xFF); };
  
  void incCounter(int delta=1) { 
    fCount+=delta; 
    fBuffer[1] = fCount-2; // nword
  };

  void setCounter(int num) { 
    fCount=num; 
    fBuffer[1] = fCount-2; // nword
  };

  int getCounter() { return fCount; };

 public:

  EvtBlock() {
    fCount = 0;
    fCurrent = 0;
    fTag = 0;
    fBuffer.push_back(0); // TAG
    fBuffer.push_back(0); // NWORD
    fRit = fBuffer.begin();
    fError = 0;
    fELen = 0;

  };

  ~EvtBlock() {
    if (fError) delete fError;
  };

  int getLength() {
    return fBuffer.size();
  };

  uint16_t getTag() { return fTag; };

  // return the buffer of data and its lenght in uint16 word
  // likely not used
  std::vector<uint16_t> *getBuffer() { return &fBuffer; };

  uint16_t *getData() { 
    return fBuffer.data(); };

  /**
   * return the next element in the vector
   * should be used to retrieve the elements for storing
   * last = 1 if last element
   */
  /*
  uint16_t getNext( int &last) {

    uint16_t val = *fRit;
    fRit++;
    last = (fRit == fBuffer.end()) ? 1 : 0;
    return val;

  };
  */

  uint16_t getNext( int &last) {
    uint16_t val = fBuffer.at(fCurrent);
    fCurrent++;
    last = (fCurrent < fCount) ? 0 : 1;
    return val;
    
  };

  /*
   * Return 1 if block has valid data
   */
  int validData() {
    if (fCount <= 2) return 0;
    return 1;
  };

  int size() { return fCount; };

  void rewind() {
    fRit = fBuffer.begin();
  };

  // arm for a new event
  virtual void arm() {};

  // read buffer of data;
  // return 0 if conversion is fine
  virtual int ingest(uint16_t len, uint16_t *b) { return 0;};

};

// --- specialized classes

/**
 * Blcok Header 
 * Tag = 0xC900
 * EventID = uint32
 */

class EvtHDR : public EvtBlock {

 private:

  uint32_t fEvtID;

 public:

  EvtHDR(uint16_t tag=EVT_HEADER) {
    fEvtID = 0;
    setTag(tag);
    allocErrVector(1);
  };

  ~EvtHDR() { };

  uint32_t getEventID() { return fEvtID; };

  void setEventID( uint32_t id) { 
    fEvtID = id; 
    fBuffer.reserve(4);
    std::vector<uint16_t>::iterator it;
    it=fBuffer.begin();
    fBuffer.insert(it+2, fEvtID & 0xFFFF);
    fBuffer.insert(it+3, (fEvtID >> 16)& 0xFFFF);
    //    fBuffer.push_back((fEvtID >> 16)& 0xFF);
    //    fBuffer.push_back(fEvtID &0xFF);
    setCounter(4);
  };

  uint32_t incEventID( uint32_t d=1) {
    fEvtID += d; 
    setEventID(fEvtID);
    return fEvtID;
  };

  void arm() {
    newBuffer();
  };

  int ingest(uint16_t len, uint16_t *b) {

    arm();
    fEvtID = 0;
    if (len<2) {
      incErr();
      return getTotErr();
    }

    fEvtID = b[0] + (b[1]<<16);

    return 0;

  };

};


/**
 * MPD - APV data block
 *
 */

#define EVT_MPD_BSIZE 130

#define EVT_MPD_ERR_HDR 0
#define EVT_MPD_ERR_TRA 1
#define EVT_MPD_ERR_MUL 2

class EvtMPD : public EvtBlock {

 private:

  int fNAPV;

 public:

  EvtMPD(uint16_t tag=EVT_MPD) {
    fNAPV=0;
    setTag(tag);
    allocErrVector(3);
  };

  ~EvtMPD() { };

  int getBoard(int i) {
    return (fBuffer.at(2+i*(EVT_MPD_BSIZE+1)) & 0x3FF0) >> 4;
  };

  int getAPV(int i) {
    return (fBuffer.at(2+i*(EVT_MPD_BSIZE+1)) & 0xF);
  };

  int getNumberAPV() { return fNAPV; };

  uint16_t getHeader(int i) {
    return fBuffer.at(3+i*(EVT_MPD_BSIZE+1));
  }

  uint16_t getTrailer(int i) {
    return fBuffer.at(2+EVT_MPD_BSIZE+i*(EVT_MPD_BSIZE+1));
  }

  uint16_t getADC(int i, int ch) {
    return fBuffer.at(4+ch+i*(EVT_MPD_BSIZE+1));
  }


  void arm() {
    newBuffer();
    fNAPV=0;
  };

  void setBoard_APV(int Board, int APV) {
    fBuffer.push_back(((Board<<4) & 0x3FF0) | (APV & 0xf));
    incCounter();
  }

  // header + 128xadc + trailer
  // no check on input buffer
  void setAPVBlock(uint16_t *block) {
    for (int i=0;i<EVT_MPD_BSIZE;i++) fBuffer.push_back(block[i]);
    incCounter(EVT_MPD_BSIZE);
    fNAPV +=1;
  }

  int ingest(uint16_t len, uint16_t *b) {

    uint16_t header, trailer;
    int i,j;
    arm();

    if ((len%(EVT_MPD_BSIZE+1)) != 0) {
      incErr(EVT_MPD_ERR_MUL);
    }

    for (i=0;i<len;i+=(EVT_MPD_BSIZE+1)) {

      setBoard_APV((b[i]&0x3FF0) >> 4, b[i]&0xF);
      header = b[i+1];
      trailer = b[i+EVT_MPD_BSIZE];

      if ((header & 0xE00) != 0xE00) {
	incErr(EVT_MPD_ERR_HDR);
      };

      if (trailer != 0) {
	incErr(EVT_MPD_ERR_TRA);
      };

      for (j=0;j<EVT_MPD_BSIZE;j++) {
	fBuffer.push_back(b[i+1+j]);
      }
      incCounter(EVT_MPD_BSIZE);
      fNAPV +=1;

    }

    return getTotErr();

  };

};

/**
 * Pattern Unit Data
 *
 */
class EvtPU : public EvtBlock {

 public:
  EvtPU(uint16_t tag=EVT_PU) {
    setTag(tag);
    allocErrVector(1);
  };

  ~EvtPU() { };

  void arm() {
    newBuffer();
  };

  uint16_t getPattern() {
    return fBuffer.at(2);
  };

  void setPattern( uint16_t pa ) {
    fBuffer.push_back(pa);
    incCounter();
  };

  int ingest(uint16_t len, uint16_t *b) { 
    if (len>1) {
      incErr();
    }
    fBuffer.push_back(b[0]);

    return getTotErr();

  };

};

/**
 *  Scaler Data
 *
 */

class EvtSCA : public EvtBlock {

 private:
  uint16_t fNScaler;

 public:
  EvtSCA(uint16_t tag=EVT_SCA) {
    setTag(tag);
    fNScaler = 0;
    allocErrVector(1);
  };

  ~EvtSCA() { };

  void arm() {
    newBuffer();
    fNScaler = 0;
    //    fBuffer.push_back(0); // prepare for nscaler
    //    incCounter();
  };

  uint32_t getScaler(int ch) {
    return ((fBuffer.at(3+2*ch)<<16) & 0xFFFF0000) | fBuffer.at(2+2*ch);
  };

  void setScaler(uint32_t val ) {
    fBuffer.push_back(val&0xFFFF);
    fBuffer.push_back((val>>16)&0xFFFF);
    fNScaler++;
    //    std::vector<uint16_t>::iterator it = fBuffer.begin();
    //    fBuffer[1] = fNScaler;
    incCounter(2);
  };

  int ingest(uint16_t len, uint16_t *b) { 
    
    int i;
    if ( (len%2) != 0) { // must be even number > 0
      incErr();
    } 
    for (i=0;i<len;i+=2) {
      setScaler((b[i+1]<<16 & 0xFFFF0000) | (b[i] & 0xFFFF));
    }
    
    return getTotErr(); 
  };

};

//----------------------------------------------------------
//

/**
 *
 */

class GI_Event {

 private:

  uint32_t fUID;
  uint16_t fVersion;

  std::vector<EvtBlock *> fEvBck;
  std::vector<EvtBlock *>::iterator fIter;

  /**
   * last = 1 meanst last element reached
   */
  uint16_t getNext( int &last ) {

    int blast;
    
    uint16_t val = (*fIter)->getNext(blast);
    fIter+=blast;
    last = (fIter == fEvBck.end()) ? 1 : 0;

    return val;

  };

 public:

  GI_Event() { 
    std::cout << __FUNCTION__ << " allocating" << std::endl;

    fEvBck.reserve(4);
    fEvBck.clear();
    fIter = fEvBck.begin();

    std::cout << __FUNCTION__ << " init" << std::endl;

    fEvBck.insert(fIter+EVT_HEADER, new EvtHDR(EVT_HEADER));

    fEvBck.insert(fIter+EVT_MPD, new EvtMPD(EVT_MPD));

    fEvBck.insert(fIter+EVT_PU, new EvtPU(EVT_PU));

    fEvBck.insert(fIter+EVT_SCA, new EvtSCA(EVT_SCA));

    std::cout << __FUNCTION__ << " allocated with " << fEvBck.size() << std::endl;
  };

  ~GI_Event() { };

  void insertBlock(EvtBlock *bck) {
    fEvBck.push_back(bck);
  }

  EvtBlock *getBlock(int i) {
    return fEvBck.at(i);
  };

  void arm() {
    std::vector<EvtBlock *>::iterator it;
    
    for (it=fEvBck.begin(); it<fEvBck.end(); it++) {
      (*it)->arm();
    }

    fIter = fEvBck.begin();
  };

  void rewind() {
    std::vector<EvtBlock *>::iterator it;
    
    for (it=fEvBck.begin(); it<fEvBck.end(); it++) {
      (*it)->rewind();
    }
    
    fIter = fEvBck.begin();
  };

  void writeHead( FILE *stream) {
    uint16_t cc[3];

    cc[0] = EVT_UID & 0xFFFF; // Less Sign.
    cc[1] = (EVT_UID >> 16) & 0xFFFF; // Most Sign.
    cc[2] = EVT_VERSION;

    fwrite(cc, sizeof(uint16_t), 3, stream);

  };


  void write( FILE *stream=0 ) {

    //    rewind();
    int i = 0;
    int last = 0;
    uint16_t *v;
    int len;
    std::vector<EvtBlock *>::iterator it;

    it = fEvBck.begin();

    do {

      if ((*it)->validData()) {
	v = (*it)->getData();
	len = (*it)->size();

	
	if (stream==0) { // preant on screen
	  for (i=0;i<len;i++) {
	    
	    if ((v[i] & EVT_TAG_BASE) == EVT_TAG_BASE) {
	      std::cout << " -- ";
	    }
	    std::cout << std::dec << i << " " << std::hex << v[i] << std::endl;
	    
	  }
	} else { // binary output
	  
	  fwrite(v,sizeof(uint16_t),len, stream);

	}

      }
      i++;
      it++;
	
    } while (it != fEvBck.end());

  };

  /**
   * read the file header
   * return the number of world read, 0 in case of error
   */ 
  int readHead(FILE *stream) {

    int cnt = 0;

    cnt += 2 * fread(&fUID, sizeof(uint32_t), 1, stream);

    if (fUID != EVT_UID) {
      std::cout << __FUNCTION__ << ": error, file format not recognized" << std::endl;
      return 0;
    }

    cnt += fread(&fVersion, sizeof(uint16_t), 1, stream);

    std::cout << __FUNCTION__ << ": UID= " << std::hex << fUID 
	      << " version " << fVersion << std::endl;

    return cnt;

  };

  /*
   *
   */
  
#define EVT_ERROR_BADTAG 0
#define EVT_ERROR_BLWORD 1

  int read(FILE *stream) {

    int cnt = 0;

    int nr;

    uint16_t bb[2], ctag;
    uint16_t nword = 0;

    uint32_t *b32;
    uint16_t *b16=0;

    int error[256];

    int level;
 
    // reset error count
    //

    std::vector<EvtBlock *>::iterator it;

    it = fEvBck.begin();

    level = 0; // tag block

    while ((feof(stream)==0) || (ferror == 0)) {

      if (level == 0) { // look for tag block + nword
	nr = fread(bb, sizeof(uint16_t), 2, stream);

	std::cout << "Block Tag: " << nr << " " << bb[0] << " " << bb[1] << std::endl;

	if (nr < 2) {
	  std::cout << __FUNCTION__ << ": End of file reached " << std::endl;
	  nword = 0;
	  break;
	} else {
	  nword = bb[1];
	}

	if ((bb[0] & EVT_TAG_BASE) != EVT_TAG_BASE) {
	  error[EVT_ERROR_BADTAG] += 1;
	  std::cout << __FUNCTION__ << ": error reading block begin, not enough words " << std::endl;
	} else {
	  level=1;
	  ctag = bb[0] & 0xFF;
	}
	continue;
      }

      if ((level == 1) && (nword>0)) {

	if (b16) delete b16;

	b16 = new uint16_t[nword];

	nr = fread(b16, sizeof(uint16_t), nword, stream);

	std::cout << "Ingest " << ctag << " " << nword << std::endl;
	if (nr < nword) {
	  error[EVT_ERROR_BLWORD] += 1;
	  std::cout << __FUNCTION__ << ": error reading block, not enough words " << std::endl;
	  nword = 0;
	} else {
	  (*(it+ctag))->ingest(nword, b16);
	}
	level = 0;
	continue;
      }

    }

    return cnt;

  }


};


#endif
