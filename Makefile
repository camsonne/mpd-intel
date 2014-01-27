
OBJ=Main.o 

DEPS=include/*.h src/libjgi.so src/libmpd.so

EXE=Daq
CC=g++
CXX=g++

ifeq ($(ARCH),Linux)

ifndef LINUXVME_LIB
	LINUXVME_LIB	= $CODA/extensions/linuxvme/libs
endif
ifndef LINUXVME_INC
	LINUXVME_INC	= $CODA/extensions/linuxvme/include
endif
endif


# ISS 
#CFLAGS=-g -c -Wall -DLINUX -DBLOCK_TRANSFER -DCONTROLLER_SIS3104 -Iinclude
#LDFLAGS=-L/usr/lib/ -lCAENVME -L/usr/local/lib -l_sis3100

# BTF-CERN
#CFLAGS=-g -c -Wall -DLINUX -DBLOCK_TRANSFER -DCONTROLLER_CAEN_V1718 -Iinclude
#LDFLAGS=-L/work/tools/CAEN/CAENVME/lib -lCAENVME -L/usr/local/lib -lconfig++

#
CFLAGS=-g -c -Wall -DLINUX -DBLOCK_TRANSFER -DCONTROLLER_GEXVB -Iinclude
#CFLAGS=-g -c -Wall  -DLINUX -DCONTROLLER_GEXVB -Iinclude
LDFLAGS=-L/work/tools/CAEN/CAENVME/lib -ljvme -L/usr/local/lib -lconfig++
DEFS = -DJLAB
INCS = -I. -I${LINUXVME_INC} -I./include -I./src
CFLAGS += -O ${DEFS} -O2  -L. -L${LINUXVME_LIB} $(INCS)
LDFLAGS=-L${LINUXVME_LIB} -ljvme -L/usr/local/lib -lconfig++ -L/root/CAENVMELib-2.30.2/lib/x86/ -lCAENVME  -Lsrc -ljgi -lmpd -lconfig++


$(EXE): $(OBJ)
	$(CXX) -o $@ $(OBJ) -lconfig++ $(LDFLAGS) -Lsrc -ljgi  -lmpd

.cpp.o: $(DEPS)
	$(CXX) -o $@ $< $(CFLAGS)

test.o: test.cpp
	$(CXX) -o $@ $< $(CFLAGS)

testblt.o: testblt.cpp
	$(CXX) -o $@ $< $(CFLAGS)

test2.o: test2.cpp
	$(CXX) -o $@ $< $(CFLAGS)

clean:
	rm -f $(EXE) $(OBJ) *~ *.log


test: test.o
	$(CXX)  $(LDFLAGS) -ljvme -Lsrc -ljgi -lmpd -lconfig++ test.o -o test

testblt: testblt.o
	$(CXX)  $(LDFLAGS) -ljvme -Lsrc -ljgi -lmpd -lconfig++ test.o -o $@

test2: test2.o
	$(CXX)  $(LDFLAGS)  test2.o -o test2

%.so: %.cpp Makefile-rol
	@echo
	@echo "Building $@ from $<"
	@echo
	$(CXX) -fpic -shared  $(CFLAGS) -DINIT_NAME=$(@:.so=__init) -o $@ $<

mpd.so: mpd.cpp
	@echo
	@echo "Building $@ from $<"
	@echo
	$(CXX) -fpic -shared  $(CFLAGS) -DINIT_NAME=$(@:.so=__init) -o $@ $<

testxvb: testxvb.o
	$(CXX)  $(LDFLAGS) -ljvme  -Lsrc -ljgi -lmpd -lconfig++ testxvb.o -o testxvb