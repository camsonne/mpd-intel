/* Vme_vx718.cpp
 */

//#include <stdint.h>

#define _LARGEFILE64_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include "vmedrv.h"


#if 0
#include <asm/page.h>
#else
#define PAGE_SHIFT      12
#define PAGE_SIZE       (1UL << PAGE_SHIFT)
#define PAGE_MASK       (~(PAGE_SIZE-1))
#endif


#include "Bus.h"

#include "Vme_sbsb3.h"
#include "AdcVme64x.h"

int VmeSBSb3::VmeOpen(void)
{

  wsize = 0;
  if ((fd = open("/dev/vmedrv32d32", O_RDWR)) == -1) {
    Vme_status = 0;
    printf(" ERROR opening SBS\n");
    return BUS_ERROR;
  }

  wsize = 4; // assume d32;

  Vme_status = 1;
  return BUS_OK;

}

int VmeSBSb3::VmeClose(void)
{
  close (fd);
  return BUS_OK;
}

int VmeSBSb3::VmeCRScan(int slot, uint32_t *rev_id, uint32_t *timec)
{
  uint32_t cr_data, addr;
  uint32_t j;
  int success;
  int cr_len = 9;
  uint32_t cr_data_OK[13] = {'C', 'R',
			     0x08, 0x00, 0x30,		// Manufacturer ID (CERN)
			     0x00, 0x03, 0x09, 0x04,		// Board ID
			     0x00, 0x00, 0x00, 0x02 };	// Revision ID
  
  uint32_t CrCsrBaseAddr[MAX_BOARDS] = 
    {	0x00000000,	// NOT USED
	0x00080000,
	0x00100000,
	0x00180000,
	0x00200000,
	0x00280000,
	0x00300000,
	0x00380000,
	0x00400000,
	0x00480000,
	0x00500000,
	0x00580000,
	0x00600000,
	0x00680000,
	0x00700000,
	0x00780000,
	0x00800000,
	0x00880000,
	0x00900000,
	0x00980000,
	0x00A00000,
	0x00000000	// DESY trial on VME 32
    };

  //  VmeClose();
  int fdd;
  if ((fdd = open("/dev/vmedrv24d32", O_RDWR)) == -1) {
    Vme_status = 0;
    printf(" ERROR opening SBS\n");
    return BUS_ERROR;
  }

  *rev_id = 0;
  if( slot < 1 || slot > (MAX_BOARDS-1) )
    return BUS_INVALID_PARAM;
  addr = CrCsrBaseAddr[slot] + 0x1C;
  for(j=0; j<cr_len; j++)
    {
      success = VmeReadA24(fdd,addr, &cr_data);
      if( success < 0)
	return success;
      if( cr_data != cr_data_OK[j] )
	return -5;
      addr += 4;
    }
  for(j=0; j<4; j++)
    {
      success = VmeReadA24(fdd,addr, &cr_data);
      *rev_id <<= 8;
      *rev_id |= (cr_data & 0xFF);
      addr += 4;
      if( success < 0 )
	return success;
    }
  *timec = 0;
  if( *rev_id > 1 )
    {
      addr = CrCsrBaseAddr[slot] + 0x5C;
      for(j=0; j<4; j++)
	{
	  success = VmeReadA24(fdd,addr, &cr_data);
	  *timec <<= 8;
	  *timec |= (cr_data & 0xFF);
	  addr += 4;
	  if( success < 0 )
	    return success;
	}
    }

  close(fdd);
  //  VmeOpen();
  return success;
}

int VmeSBSb3::VmeWrite(uint32_t Address, void *Data)
{
  int written_size;

  uint32_t ddd;

  /*
  VmeRead(Address, &ddd);
  printf("R0: %x %x\n",Address, ddd);
  */

  if (lseek64(fd, Address, SEEK_SET) == -1) {
    return BUS_INVALID_PARAM;
  } 

  if ((written_size = write(fd, (char *) Data, wsize)) == -1) {
    perror("ERROR WRITE ");
    return BUS_ERROR;
  }
  /*
  printf("W: %x %x %d\n",Address, * (uint32_t *)Data, written_size);
  VmeRead(Address, &ddd);
  printf("R1: %x %x\n",Address, ddd);
  */
  return BUS_OK;

}

int VmeSBSb3::VmeRead(uint32_t Address, void *Data)
{
  int read_size;
  if (lseek64(fd, Address, SEEK_SET) == -1) {
    return BUS_INVALID_PARAM;
  }

  if ((read_size = read(fd, Data, wsize)) == -1) {
    return BUS_ERROR;
  }

  return BUS_OK;
}

int VmeSBSb3::VmeReadA24(int fpoint, uint32_t Address, void *Data)
{
  int read_size;
  if (lseek64(fpoint, Address, SEEK_SET) == -1) {
    return BUS_INVALID_PARAM;
  }

  if ((read_size = read(fpoint, Data, 4)) == -1) {
    return BUS_ERROR;
  }

  return BUS_OK;
}

int VmeSBSb3::VmeWrite(int AM, int DT, uint32_t Address, void *Data)
{

  /*
    if (ioctl(fd,VMEDRV_IOC_SET_ACCESS_MODE||VMEDRV_IOC_SET_TRANSFER_METHOD,"/dev/vmedrv32d32") == -1) {
    return BUS_COMM_ERROR;    
    }
  */
  return VmeWrite(Address, Data);

}

int VmeSBSb3::VmeRead(int AM, int DT, uint32_t Address, void *Data)
{
  //  ioctl(fd,VMEDRV_IOC_SET_ACCESS_MODE||VMEDRV_IOC_SET_TRANSFER_METHOD,"/dev/vmedrv16d32");

  return VmeRead(Address, Data);

}

int VmeSBSb3::VmeBlockWrite(uint32_t Address, int Size, void *Buffer, int *Transferred)
{
  int written_size;
  
  *Transferred = 0;

  if (lseek64(fd, Address, SEEK_SET) == -1) {
    return BUS_INVALID_PARAM;
  } 
  
  if ((written_size = write(fd, Buffer, Size*wsize)) == -1) {
    return BUS_ERROR;
  }

  *Transferred = written_size;
  return BUS_OK;

}

int VmeSBSb3::VmeBlockRead(uint32_t Address, int Size, void *Buffer, int *Transferred)
{

  int read_size;
  if (lseek64(fd, Address, SEEK_SET) == -1) {
    return BUS_INVALID_PARAM;
  }

  if ((read_size = read(fd, Buffer, Size*wsize)) == -1) {
    return BUS_ERROR;
  }

  *Transferred = read_size;
  return BUS_OK;

}
