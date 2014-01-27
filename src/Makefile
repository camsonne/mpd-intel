MPD_OBJ = MPDhi.o MPDlo.o  
MPD_DEPS = MPDhi.h MPDlo.h
MPD_LIB = libmpd.so

OBJ=GI_V462.o GI_V1495.o GI_V1718.o GI_V260.o GI_V513.o DaqControl.o Logger.o Vme_v1718.o Vme_gexvb.o  Vme_sis.o GI_Wrapper.o GI_IO.o GI_Trigger.o

#OBJ= DaqControl.o Logger.o Vme_gexvb.o  Vme_sis.o GI_Wrapper.o GI_IO.o  GI_Trigger.o

OBJ=GI_V462.o GI_V1495.o GI_V1718.o GI_V260.o GI_V513.o DaqControl.o Logger.o Vme_v1718.o GI_Wrapper.o GI_IO.o GI_Trigger.o Vme_gexvb.o

DEPS=GI_V1495.h GI_V1718.h GI_V260.h GI_V513.h Bus.h Vme_v1718.h GI_Config.h GI_Wrapper.h GI_IO.h Logger.h DaqControl.h CAENVMElib.h CAENVMEoslib.h CAENVMEtypes.h Vme_sis.h sis1100_var.h sis1100_vme_calls.h GI_Trigger.h GI_Common.h Vme_gexvb.h

#DEPS= Bus.h Vme_v1718.h GI_Config.h GI_Wrapper.h GI_IO.h Logger.h DaqControl.h CAENVMElib.h CAENVMEoslib.h CAENVMEtypes.h GI_Common.h

DEPS=GI_V1495.h GI_V1718.h GI_V260.h GI_V513.h Bus.h Vme_v1718.h GI_Config.h GI_Wrapper.h GI_IO.h Logger.h DaqControl.h CAENVMElib.h CAENVMEoslib.h CAENVMEtypes.h GI_Trigger.h GI_Common.h

ifeq ($(ARCH),Linux)

ifndef LINUXVME_LIB
	LINUXVME_LIB	= $CODA/extensions/linuxvme/libs
endif
ifndef LINUXVME_INC
	LINUXVME_INC	= $CODA/extensions/linuxvme/include
endif
endif


CFLAGS=-g -c -Wall -DLINUX  -DCONTROLLER_GEXVB -Iinclude
CFLAGS=-g -c -Wall -DLINUX -DBLOCK_TRANSFER -DCONTROLLER_GEXVB -Iinclude
LDFLAGS=-L/work/tools/CAEN/CAENVME/lib -ljvme -L/usr/local/lib -lconfig++
DEFS = -DJLAB
INCS = -I. -I${LINUXVME_INC} -I ../include -I /root/CAENVMELib-2.30.2/include/
CFLAGS += -O ${DEFS} -O2  $(INCS)
LDFLAGS=-L${LINUXVME_LIB}  -ljvme -L/usr/local/lib -lconfig++

LIB=libjgi.so
CC=g++ 
LD=g++
#CFLAGS=-g -c -fPIC -Wall -DLINUX -DBLOCK_TRANSFER -DCONTROLLER_SIS3104 -I ../include
#CFLAGS=-g -c -fPIC -Wall -DLINUX -DBLOCK_TRANSFER -I ./include  -I/work/tools/CAEN/CAENVME/include/
LDFLAGS=-shared -fPIC -L/root/CAENVMELib-2.30.2/lib/x86/ -lCAENVME -L${LINUXVME_LIB}  -ljvme -L/usr/local/lib -lconfig++
LDFLAGS2=-fPIC -L/root/CAENVMELib-2.30.2/lib/x86/ -lCAENVME -L${LINUXVME_LIB}  -ljvme -L/usr/local/lib -lconfig++

$(MPD_LIB): $(LIB) $(MPD_OBJ)
#	ar cr $(MPD_LIB) $(MPD_OBJ)
	$(LD) $(LDFLAGS)  $(MPD_OBJ) -o  $@

$(LIB): $(OBJ)
#	ar cr $(LIB) $(OBJ)
	$(LD) $(LDFLAGS)  $(OBJ) -o  $@

.cpp.o: $(DEPS) $(MPD_DEPS)
	$(CC) -o $@ $< $(CFLAGS)

clean:
	rm -f $(MPD_OBJ) $(MPD_LIB) $(EXE) $(OBJ) $(LIB) *~ *.log

testxvb: testxvb.o Vme_gexvb.o
	$(LD) $(LDFLAGS2)  $^ -o  $@