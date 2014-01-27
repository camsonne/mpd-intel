CC=g++

OBJ= GEMfileio.o testGEMfileio.o
DEPS=GEMfileio.h MPDhi.h

EXE=testGEMfileio

CFLAGS=-c -Wall -DLINUX -I../include
LDFLAGS=-I../include

$(EXE):	$(OBJ)
	$(CC) -o $@ $(LDFLAGS) $(OBJ)

.cpp.o:	$(DEPS)
	$(CC) -o $@ $< $(CFLAGS)
#	cp $@ ../$@

clean:
	rm -f $(OBJ) *~
