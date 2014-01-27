#include <stdio.h>

#define	EV_SIZE	130

CheckFile(char *fname)
{
	FILE *fin;
	int evt, brd, apv, ev_cnt, event[EV_SIZE];
	int header, trailer, bad_header_cnt, bad_trailer_cnt;

	if( (fin=fopen(fname,"rb")) == NULL )
	{
		printf("Can't open input file '%s'\n", fname);
		exit(1);
	}

	ev_cnt = 0;
	bad_header_cnt = bad_trailer_cnt = 0;
	while( !feof(fin) )
	{
		fread(&evt, sizeof(int), 1, fin);
		fread(&brd, sizeof(int), 1, fin);
		fread(&apv, sizeof(int), 1, fin);
		fread(event, sizeof(short int), EV_SIZE, fin);
		header = event[0];
		trailer = event[EV_SIZE-1];
		if( ((header & 0xF00) != 0xE00) && ((header & 0xF00) != 0xF00) )
		{
			printf("Bad header (0x%03X) in event %d\n", header, ev_cnt);
			bad_header_cnt++;
		}
		if( trailer != 0 )
		{
			printf("Bad trailer (0x%03X) in event %d\n", trailer, ev_cnt);
			bad_trailer_cnt++;
		}
		ev_cnt++;
	}
	printf("Read %d events, bad headers = %d, bad trailers = %d\n", ev_cnt-1, bad_header_cnt, bad_trailer_cnt);
	fclose(fin);
}

