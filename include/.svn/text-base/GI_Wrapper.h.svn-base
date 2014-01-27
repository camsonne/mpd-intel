#ifndef __GI_WRAPPER__
#define __GI_WRAPPER__

/*
 * GI_Wrapper.h
 *
 * Author: E. Cisbani
 * First version: April 2012 
 *
 * Wrapping and utility methods for the mpd/apv acquisition program
 *
 */

#include "DaqControl.h"
#include "GI_Config.h"
#include "MPDhi.h"

class GI_Wrapper {

 private:

 public:
  GI_Wrapper() {
  };

  ~GI_Wrapper() {
  }

  int LoadMPDConfig(GI_Config *gDB,  MPDhi *gMPD, Bus *clink);

  void LoadDaqConfig(GI_Config *gDB,  DaqControl *gDAQ);

};

#endif
