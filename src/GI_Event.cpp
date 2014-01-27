/**
 * Text example of GI_Event
 */

#include <stdint.h>
#include <iostream>

#include "GI_Event.h"

using namespace std;

int main() {

  int i,j;

  GI_Event evt;

  evt.arm();

  EvtHDR *eHDR = (EvtHDR *) evt.getBlock(EVT_HEADER);

  EvtMPD *eMPD = (EvtMPD *) evt.getBlock(EVT_MPD);

  EvtPU *ePU = (EvtPU *) evt.getBlock(EVT_PU);

  EvtSCA *eSCA = (EvtSCA *) evt.getBlock(EVT_SCA);

  eHDR->setEventID(0x23);

  ePU->setPattern(0x101);

  eSCA->setScaler(0x123);
  eSCA->setScaler(0xAB);
  eSCA->setScaler(0x34123);

  uint16_t bk[130];

  for (j=0;j<3;j++) {
    eMPD->setBoard_APV( 1, j);
       
    for (i=0;i<130;i++) {
      bk[i]=i+j*150;
    }
    eMPD->setAPVBlock( bk);
  }

  i=0;
  int last = 0;

  evt.write();

  FILE *fout = fopen("prova.bin","w");

  evt.writeHead(fout);
  evt.write(fout);

  fclose(fout);

  FILE *fin = fopen("prova.bin","r");

  cout << evt.readHead(fin) << endl;
  cout << evt.read(fin) << endl;
 
  fclose(fin);

  cout << " Event : " << ((EvtHDR *) evt.getBlock(EVT_HEADER))->getEventID() << endl;
  cout << " MPD   : " << ((EvtMPD *) evt.getBlock(EVT_MPD))->getNumberAPV() 
       << "  B: " << ((EvtMPD *) evt.getBlock(EVT_MPD))->getBoard(0) 
       << "  A: " << ((EvtMPD *) evt.getBlock(EVT_MPD))->getAPV(0) 
       << "  H: " << ((EvtMPD *) evt.getBlock(EVT_MPD))->getHeader(0) 
       << "  D: " << ((EvtMPD *) evt.getBlock(EVT_MPD))->getADC(0,10)
       << "  D: " << ((EvtMPD *) evt.getBlock(EVT_MPD))->getADC(0,127)
       << "  T: " << ((EvtMPD *) evt.getBlock(EVT_MPD))->getTrailer(0)
       << endl;

  cout << " Pattern:" << 
    ((EvtPU *) evt.getBlock(EVT_PU))->getPattern() << endl;

  cout << " Scaler: " <<
    ((EvtSCA *) evt.getBlock(EVT_SCA))->getScaler(0) << " " <<
    ((EvtSCA *) evt.getBlock(EVT_SCA))->getScaler(1) << " " << 
    ((EvtSCA *) evt.getBlock(EVT_SCA))->getScaler(2) << endl;



  return 0;

}
  
