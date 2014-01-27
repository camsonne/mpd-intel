/* Vme.cpp
 */

#include <unistd.h>
#include <stdint.h>

#include "CAENVMElib.h"
#include "Vme.h"
#include "AdcVme64x.h"


int VmeBus::VmeOpen(void)
{
	CVBoardTypes VMEController;
	short Link;
	short Device;
	int success;

	VmeError[0] = BUS_OK;
	VmeError[1] = BUS_ERROR;
	VmeError[2] = BUS_COMM_ERROR;
	VmeError[3] = BUS_GENERIC_ERROR;
	VmeError[4] = BUS_INVALID_PARAM;
	VmeError[5] = BUS_TIMEOUT;

	VMEController = cvV1718;
	Device = 0;
	Link = 0;
	success = CAENVME_Init(VMEController, Device, Link, &V17x8Handle);
	if( success == cvSuccess )
	{
		Sleep(500);
		success = CAENVME_SystemReset(V17x8Handle);
		Sleep(500);
		if( success == cvSuccess )
			Vme_status = 1;
		else
			Vme_status = 0;
	}
	else
		Vme_status = 0;
	return success;
}

int VmeBus::VmeClose(void)
{
  return CAENVME_End(V17x8Handle);
}

int VmeBus::VmeWrite(uint32_t Address, void *Data)
{
  return CAENVME_WriteCycle(V17x8Handle, Address, Data, cvA32_U_DATA, cvD32);
}

int VmeBus::VmeRead(uint32_t Address, void *Data)
{
  return CAENVME_ReadCycle(V17x8Handle, Address, Data, cvA32_U_DATA, cvD32);
}

int VmeBus::VmeWrite(int AM, int DT, uint32_t Address, void *Data)
{
  // to be extended
  return CAENVME_WriteCycle(V17x8Handle, Address, Data, (CVAddressModifier) AM, (CVDataWidth) DT);
}

int VmeBus::VmeRead(int AM, int DT, uint32_t Address, void *Data)
{
  return CAENVME_ReadCycle(V17x8Handle, Address, Data, (CVAddressModifier) AM, (CVDataWidth) DT);
}

int VmeBus::VmeReadCR(uint32_t Address, void *Data) {
  return CAENVME_ReadCycle(V17x8Handle, Address, Data, cvCR_CSR, cvD32);
}

int VmeBus::VmeBlockWrite(uint32_t Address, int Size, void *Buffer, int *Transferred)
{
  return CAENVME_BLTWriteCycle(V17x8Handle, Address, (unsigned char *)Buffer, Size, cvA32_U_BLT, cvD32, Transferred);
}

int VmeBus::VmeBlockRead(uint32_t Address, int Size, void *Buffer, int *Transferred)
{
  return CAENVME_BLTReadCycle(V17x8Handle, Address, (unsigned char *)Buffer, Size, cvA32_U_BLT, cvD32, Transferred);
}
