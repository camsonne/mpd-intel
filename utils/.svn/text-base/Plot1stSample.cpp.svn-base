#include <fstream>
#include "Riostream.h"
#include "TMath.h"
#include "TF1.h"
#include "TH1I.h"
#include "TH2I.h"
#include "TCanvas.h"
#include "TStopwatch.h"
#include "TStyle.h"
#include "TLine.h"
#include "TFile.h"
#include "TTree.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TMarker.h"
#include "TText.h"

using namespace std;

#define TEMP_FNAME	"datafile.dat"

//================================= GLOBAL VARIABLES


TCanvas *c1 = NULL;
TGraph *gr = NULL;


//
//================================================ Routines
//

void open_canvas()
{ 
	if( c1 != NULL )
		delete c1;
	c1 = new TCanvas("c1", "Data Display", 1000, 500);
}

Int_t read_data1(char *fnameprefix, int brd0 = 0, int ch0 = 0)
{
	char fname[256], sdummy[256];
	int j, flag, ev_n, brd, ch, nsamples, data[10000], x[10000];
	ifstream in;

	sprintf(fname, "%s.dat", fnameprefix);
	in.open( fname );
	cout << "Reading '" << fname << "' data file" << endl;
	if( !in.good() )
	{
		cout << "Can't open input file " << fname << endl;
		return 0;
	}
	else
	{
		flag = 0;
		while( in.good() && flag == 0 )
		{
			in >> sdummy;
			in >> ev_n;
			in >> sdummy;
			in >> brd;
			in >> sdummy;
			in >> ch;
			in >> sdummy;
			in >> nsamples;
			for(j=0; j<nsamples; j++)
			{
				in >> data[j];
				x[j] = j;
			}
			if ((brd == brd0) && (ch == ch0)) { flag = 1; } else { 
			  cout << "skip " << ev_n << " " << brd << " " << ch << " " << nsamples << endl;
			  flag = 0;
			}
		}
	}
	in.close();
	cout << ev_n << " : Got " << nsamples << " data form board: " << brd << " channel: " << ch << endl;
	if( gr != NULL )
		delete gr;
	gr = new TGraph (nsamples, x, data);
	return 1;
}


Int_t read_data(char *fnameprefix)
{
	char fname[256], fname_to_read[256];
	int i, j, tempdata;
	ifstream in;
	TStopwatch timer;

	timer.Start();
	read_data1(fnameprefix);
	timer.Stop();
	return 1;
}

void display_graph()
{
	gr->Draw("ALP");
	gr->SetMarkerStyle(6);
}

void process(char *fnameprefix, int logy = 0)
{
	read_data(fnameprefix);
	open_canvas();
	display_graph();
}

