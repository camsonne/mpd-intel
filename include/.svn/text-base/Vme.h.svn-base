
#ifndef __GI_VMEBUS__
#define __GI_VMEBUS__

#include <stdlib.h>

#include "Bus.h"

class VmeBus : public Bus
{
 private:

  long V17x8Handle;
  int Vme_status;
  
  int VmeError[6];

  int ToErr(int error) { // convert from CAEN to BUS error convention
    return VmeError[abs(error)];
  };

  int VmeOpen(void);
  int VmeClose(void);

  int VmeWrite(uint32_t Address, void *Data);
  int VmeRead(uint32_t Address, void *Data);
  
  int VmeWrite(int AM, int DT, uint32_t Address, void *Data);
  int VmeRead(int AM, int DT, uint32_t Address, void *Data);
  
  int VmeReadCR(uint32_t Address, void *Data);

  int VmeBlockWrite(uint32_t Address, int Size, void *Buffer, int *Transferred);
  int VmeBlockRead(uint32_t Address, int Size, void *Buffer, int *Transferred);

 public:

  int Open(void) { return ToErr(VmeOpen()); }
  int Close(void) { return ToErr(VmeClose()); }
  int Status(void) { return Vme_status; }
  
  int Write(uint32_t Address, void *Data) { return ToErr(VmeWrite(Address, Data)); }
  int Read(uint32_t Address, void *Data) { return ToErr(VmeRead(Address, Data)); }
  
  int Write(int AM, int DT, uint32_t Address, void *Data) { return ToErr(VmeWrite(AM, DT, Address, Data)); }
  int Read(int AM, int DT, uint32_t Address, void *Data) { return ToErr(VmeRead(AM, DT, Address, Data)); }

  int ReadCR(uint32_t Address, void *Data) { return ToErr(VmeReadCR(Address, Data)); }
  
  int BlockWrite(uint32_t Address, int Size, void *Buffer, int *Transferred)
  { return ToErr(VmeBlockWrite(Address, Size, Buffer, Transferred)); }
  int BlockRead(uint32_t Address, int Size, void *Buffer, int *Transferred)
  { return ToErr(VmeBlockRead(Address, Size, Buffer, Transferred)); }
  
  VmeBus() { Vme_status = 0; VmeOpen(); }
  ~VmeBus() { VmeClose(); Vme_status = 0; }
  
};

#endif
