/*
  -----------------------------------------------------------------------------
  
  
  -----------------------------------------------------------------------------
  
  Name		:	Rate.cpp
  
  Project	:	ADCVme64x
  
  Description   :	Show Scaler Rate
  
  Date		:	September 2011
  Release       :	1.0
  Author	:	E. Cisbani
  
  
  
  -----------------------------------------------------------------------------

  Generate GI_V260.o and Vme.o by make
  
  Then compile with: g++ -o Rate -lCAENVME Rate.cpp GI_V260.o Vme.o

  -----------------------------------------------------------------------------
*/


#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <stdint.h>
#include <string.h>
#include <iostream>

#include "Bus.h"
#include "Vme.h"

#include "GI_V260.h"

Bus *CommLink;


GI_V260 *gV260; // scaler

static int sig_int = 1;

static int fRun_num = 0;

/*----------------      signal handling      ----------------*/

void ctrl_c(int){
  sig_int = 0;
  printf("\n CTRL-C pressed \n\r");
}//ctrl_c




//----
//-----
//----

int main(int argc, char *argv[]) 
{

  int i;

  int dtime=5;

  int c0=0, c1=5;

  int mode=0;

  char fout[1000];

  sprintf(fout,"out/rate.out");

  for (i=1;i<argc; i++) {

    if (strcmp("-out", argv[i]) == 0) {
      i++;
      sprintf(fout,"%s",argv[i]);
    }

    if (strcmp("-delta", argv[i]) == 0) {
      i++;
      sscanf(argv[i],"%d",&dtime);
    }

    if (strcmp("-ch0", argv[i]) == 0) {
      i++;
      sscanf(argv[i],"%d",&c0);
    }

    if (strcmp("-ch1", argv[i]) == 0) {
      i++;
      sscanf(argv[i],"%d",&c1);
    }

    if (strcmp("-mode", argv[i]) == 0) {
      i++;
      sscanf(argv[i],"%d",&mode);
    }

    if (strcmp("-h", argv[i]) == 0) {
      printf("./Rate [par] : Read and print running rate of scaler\n");
      printf(" -out filename: output filename, where counts are recorded in append mode [%s]\n", fout);
      printf(" -delta time  : show rate every time seconds [%d]\n",dtime);
      printf(" -ch0 c0      : first channel of scaler to read and show [%d]\n",c0);
      printf(" -ch1 c1      : last channels of scaler to read and show [%d]\n",c1);
      printf(" -mode m      : show running (=0) or average rate (<>0) [%d]\n",mode); 
      printf(" -h           : this help\n");
      exit(1);
    }
  }

  if (dtime <= 0) { dtime = 1; }

  // Create link

  CommLink = new VmeBus;
  if( CommLink == NULL )
    {
      std::cout << std::endl << std::endl << " VME controller structure not initialized" << std::endl;
      std::cout << " Exiting..." << std::endl << std::endl;
      exit(1);
    }
  if( CommLink->Status() == 0 )
    {
      std::cout << std::endl << std::endl << " Error initializing VME controller" << std::endl;
      std::cout << " Exiting..." << std::endl << std::endl;
      delete CommLink;
      exit(1);
    }
  
  // allocate V260 module
  gV260 = new GI_V260(CommLink, 0xFD0000); // ...
  gV260->Initialize();

  gV260->Disable();
  gV260->Clear();

  gV260->Enable();

  time_t time_scaler;
  time(&time_scaler);

  std::cout << " ... please wait " << dtime << " seconds" << std::endl;
 
  gV260->ReadBunch(c0,c1);
  do {  // event loop

    sleep(dtime);
    
    gV260->ReadBunch(c0,c1);
    if (mode) {
      gV260->TotalRate(c0,c1,time_scaler);
    } else {
      gV260->PrintRate(c0,c1,time_scaler);
    }

    gV260->WriteScaler(c0, c1, time_scaler, fout);

  } while (sig_int == 1);
  
}

