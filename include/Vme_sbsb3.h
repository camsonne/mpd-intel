#ifndef __VMESBSB3__H
#define __VMESBSB3__H

#include <sys/types.h>

class VmeSBSb3 : public Bus
{

 private:
  int fd;
  int wsize;

  int Vme_status;
  
  int VmeOpen(void);
  int VmeClose(void);
  int VmeCRScan(int slot, uint32_t *rev, uint32_t *timec);
  int VmeWrite(uint32_t Address, void *Data);
  int VmeRead(uint32_t Address, void *Data);

  int VmeWrite(int AM, int DT, uint32_t Address, void *Data);
  int VmeRead(int AM, int DT, uint32_t Address, void *Data);

  int VmeBlockWrite(uint32_t Address, int Size, void *Buffer, int *Transferred);
  int VmeBlockRead(uint32_t Address, int Size, void *Buffer, int *Transferred);

  int VmeReadA24(int fpoint, uint32_t Address, void *Data);

 public:

  int Open(void) { return VmeOpen(); }
  int Close(void) { return VmeClose(); }
  int Status(void) { return Vme_status; }
  int Scan(int slot, uint32_t *rev, uint32_t *timec) { return VmeCRScan(slot, rev, timec); }
  int Write(uint32_t Address, void *Data) { return VmeWrite(Address, Data); }
  int Read(uint32_t Address, void *Data) { return VmeRead(Address, Data); }

  int Write(int AM, int DT, uint32_t Address, void *Data) { return VmeWrite(AM, DT, Address, Data); }
  int Read(int AM, int DT, uint32_t Address, void *Data) { return VmeRead(AM, DT, Address, Data); }

  int BlockWrite(uint32_t Address, int Size, void *Buffer, int *Transferred)
  { return VmeBlockWrite(Address, Size, Buffer, Transferred); }
  int BlockRead(uint32_t Address, int Size, void *Buffer, int *Transferred)
  { return VmeBlockRead(Address, Size, Buffer, Transferred); }
  int FastBlockRead(uint32_t Address, int Size, void *Buffer, int *Transferred)
  { return VmeBlockRead(Address, Size, Buffer, Transferred); }
  
  VmeSBSb3() { Vme_status = 0; VmeOpen(); }
  ~VmeSBSb3() { VmeClose(); Vme_status = 0; }

};

#endif
