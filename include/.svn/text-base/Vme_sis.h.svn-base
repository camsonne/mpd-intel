
#include <stdlib.h>

#include "Bus.h"


class VmeSis3104 : public Bus
{
	private:
		int Sis3104Handle;
		int Vme_status;

		int VmeOpen(void);
		int VmeClose(void);
		int VmeWrite(uint32_t Address, void *Data);
		int VmeRead(uint32_t Address, void *Data);
		int VmeBlockWrite(uint32_t Address, int Size, void *Buffer, int *Transferred);
		int VmeBlockRead(uint32_t Address, int Size, void *Buffer, int *Transferred);
		int VmeFastRead(uint32_t Address, int Size, void *Buffer, int *Transferred);
		int vme_CSRD32_read(int p, u_int32_t vme_adr, u_int32_t* vme_data);
		int VmeReadCR(uint32_t vme_adr, void *vme_data) { return vme_CSRD32_read(Sis3104Handle, vme_adr, (uint32_t*)vme_data); };
		int vme_TurnedOn(void);


	public:

		int Open(void) { return VmeOpen(); }
		int Close(void) { return VmeClose(); }
		int Status(void) { return Vme_status; }

		int Write(uint32_t Address, void *Data) { return VmeWrite(Address, Data); }
		int Read(uint32_t Address, void *Data) { return VmeRead(Address, Data); }

		/*
		int Write(int AM, int DT, uint32_t Address, void *Data) { return VmeWrite(Address, Data); }
		int Read(int AM, int DT, uint32_t Address, void *Data) { return VmeRead(Address, Data); }
		*/

		int Write(int AM, int DT, uint32_t Address, void *Data);
		int Read(int AM, int DT, uint32_t Address, void *Data);

		int ReadCR(uint32_t Address, void *Data) { return VmeReadCR(Address, Data); }

		int BlockWrite(uint32_t Address, int Size, void *Buffer, int *Transferred)
			 { return VmeBlockWrite(Address, Size, Buffer, Transferred); }
		int BlockRead(uint32_t Address, int Size, void *Buffer, int *Transferred)
			 { return VmeBlockRead(Address, Size, Buffer, Transferred); }

		int FastBlockRead(uint32_t Address, int Size, void *Buffer, int *Transferred)
			 { return VmeFastRead(Address, Size, Buffer, Transferred); }

		VmeSis3104() { Vme_status = 0; VmeOpen(); }
		~VmeSis3104() { VmeClose(); Vme_status = 0; }

// Specific method for Struck Controller
		int SetOutReg(int osel);
		int ClearOutReg(int osel);
		int PulseOutReg(int osel);
		int GetInTransparent(int *isel);
		int GetInLatch(int *isel);
		int ClearInLatch(int isel);

};
