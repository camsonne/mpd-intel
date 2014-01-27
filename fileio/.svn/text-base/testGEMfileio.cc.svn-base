#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "GEMfileio.h"

int main()
{
  GEMfileio *io = new GEMfileio();
  io->SetDataFormat(GEMfileio::DATAFMT_hexadecimal);
  io->SetEventHeader("EVENT\n", 1);
  io->OpenOutfile("testout.dat");
  io->WriteEvent(0,0,0,1);
  io->CloseOutfile();


  return 0;
}
