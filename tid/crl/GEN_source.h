/******************************************************************************
*
* Header file for use General USER defined rols with CODA crl (version 2.0)
* 
*   This file implements use of the JLAB TID Module as a trigger interface
*   (substitute for the JLAB TI Board)
*
*                             Bryan Moffit  March 2012
*
*******************************************************************************/
#ifndef __GEN_ROL__
#define __GEN_ROL__

static int GEN_handlers,GENflag;
static int GEN_isAsync;
static unsigned int *GENPollAddr = NULL;
static unsigned int GENPollMask;
static unsigned int GENPollValue;
static unsigned long GEN_prescale = 1;
static unsigned long GEN_count = 0;



/* Put any global user defined variables needed here for GEN readout */
#include "tidLib.h"
extern unsigned int tidIntCount;
extern struct GEN_A24RegStruct *TIDp;
extern int tidDoAck;

void
GEN_int_handler()
{
  theIntHandler(GEN_handlers);                   /* Call our handler */
  tidDoAck=0; /* Make sure the library doesn't automatically ACK */
}



/*----------------------------------------------------------------------------
  gen_trigLib.c -- Dummy trigger routines for GENERAL USER based ROLs

 File : gen_trigLib.h

 Routines:
           void gentriglink();       link interrupt with trigger
	   void gentenable();        enable trigger
	   void gentdisable();       disable trigger
	   char genttype();          return trigger type 
	   int  genttest();          test for trigger  (POLL Routine)
------------------------------------------------------------------------------*/


static void
gentriglink(int code, VOIDFUNCPTR isr)
{
  int stat=0;

  tidIntConnect(TID_INT_VEC,isr,0);

}

static void 
gentenable(int code, int card)
{
  int iflag = 1; /* Clear Interrupt scalers */
  int lockkey;

  if(GEN_isAsync==0)
    {
      GENflag = 1;
    }
  
  tidIntEnable(1); 
}

static void 
gentdisable(int code, int card)
{

  if(GEN_isAsync==0)
    {
      GENflag = 0;
    }
  tidDisableTriggerSource(1);
  tidIntDisable();
  tidIntDisconnect();

}

static unsigned int
genttype(int code)
{
  unsigned int tt=0;

  if(code == 2) {
    tt = 1 ;
  } else {
    tt = 1;
  }

  return(tt);
}

static int 
genttest(int code)
{
  unsigned int ret=0;
  unsigned int tiddata=0;

  tiddata = tidIntPoll();

  if(tiddata!=-1)
    {
      if(tiddata)
	ret = 1;
      else 
	ret = 0;
    }
  else
    {
      ret = 0;
    }

  return ret;
}

static inline void 
gentack(int code, unsigned int intMask)
{
    {
      tidIntAck();
    }
}


/* Define CODA readout list specific Macro routines/definitions */

#define GEN_TEST  genttest

#define GEN_INIT { GEN_handlers =0;GEN_isAsync = 0;GENflag = 0;}

#define GEN_ASYNC(code,id)  {printf("linking async GEN trigger to id %d \n",id); \
			       GEN_handlers = (id);GEN_isAsync = 1;gentriglink(code,GEN_int_handler);}

#define GEN_SYNC(code,id)   {printf("linking sync GEN trigger to id %d \n",id); \
			       GEN_handlers = (id);GEN_isAsync = 0;}

#define GEN_SETA(code) GENflag = code;

#define GEN_SETS(code) GENflag = code;

#define GEN_ENA(code,val) gentenable(code, val);

#define GEN_DIS(code,val) gentdisable(code, val);

#define GEN_CLRS(code) GENflag = 0;

#define GEN_GETID(code) GEN_handlers

#define GEN_TTYPE genttype

#define GEN_START(val)	 {;}

#define GEN_STOP(val)	 {;}

#define GEN_ENCODE(code) (code)

#define GEN_ACK(code,val)   gentack(code,val);

#endif

