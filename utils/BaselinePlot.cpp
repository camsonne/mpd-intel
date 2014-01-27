//
//
// Use process(..) to read histograms
//


#include <fstream>
#include "Riostream.h"
#include "TMath.h"
#include "TF1.h"
#include "TH1I.h"
#include "TH2I.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TStopwatch.h"
#include "TStyle.h"
#include "TLine.h"
#include "TFile.h"
#include "TTree.h"
#include "TGraphErrors.h"
#include "TMarker.h"
#include "TText.h"
#include "TTree.h"
#include "TCut.h"

using namespace std;

#define TEMP_FNAME	"datafile.dat"

//================================= GLOBAL VARIABLES


Int_t BP_THR;

const int nchannel = 16;
TH1I *h[nchannel];
TCanvas *c1 = NULL;

TGraph gth0(nchannel);
TGraph gth1(nchannel);
TGraph gratio(nchannel);
TGraph gfitratio(nchannel);
TGraph gsigma(nchannel);

Double_t kch,kth0,kth1,kratio,kfitratio,ksigma;
Double_t kboard;

TTree *fT = NULL;

TString ifile;

Double_t knrat = 1./35.; // nominal ratio 1 tick mark / 35 clock at 40 MHz

//
//================================================ Routines
//

void check_zip(char *fname_in, char *fname_out)
{
	char command[256];

	if( strstr(fname_in, ".gz") )	// Filename contains ".gz"
	{
		cout << "Unzipping " << fname_in << " ..." << endl;
		sprintf(command, "gzip -d -f -c %s > %s", fname_in, TEMP_FNAME);
		system(command);
		strcpy(fname_out, TEMP_FNAME);
	}
	else
		strcpy(fname_out, fname_in);
}

void open_canvas(int xy, TString title="Data Display")
{ 
	if( c1 != NULL )
		delete c1;
//	c1 = new TCanvas("c1", "Data Display", 0, 0, 1000, 1000);
	c1 = new TCanvas("c1", Form("c1: %s",title.Data()), 1000, 1000);
	c1->Divide(xy, xy);
}

Int_t read_data1(int ch, char *fnameprefix)
{
	char fname[256], fname_to_read[256];
	int j, tempdata;
	ifstream in;

	sprintf(fname, "%s_%d.dat", fnameprefix, ch);
	check_zip(fname, fname_to_read);
	in.open( fname_to_read );
	cout << "Reading '" << fname_to_read << "' data file" << endl;
	if( !in.good() )
	{
		cout << "Can't open input file " << fname_to_read << endl;
		return 0;
	}
	else
	{
		j = 0;
		while( in.good() && j < 4096 )
		{
			in >> tempdata;
			if (j>BP_THR) {
			  h[ch]->Fill(j, tempdata);
			}
			j++;
		}
		if( j < 4096 )
			cout << "Cannot reach EOF in file filling h[" << ch << "]" << endl;
	}
	in.close();
	return 1;
}


Int_t read_data(char *fnameprefix)
{
	char fname[256], fname_to_read[256];
	int i, j, tempdata;
	ifstream in;
	TStopwatch timer;

	timer.Start();
	for(i=0; i<nchannel; i++)
		read_data1(i, fnameprefix);
	timer.Stop();
	cout << endl << "Read 16 files in " << timer.RealTime() << " s" << endl;
	return 1;
}

void create_histo1(int ch)
{
	char id[100], title[100];

	if( h[ch] ) delete h[ch];

	sprintf(id, "h%d", ch);
	sprintf(title, "Histo of channel %d", ch);
	cout << "Creating h[" << ch << "] = '" << title << "'" << endl;
	h[ch] = new TH1I(Form("h%02d",ch), Form("Histo of channel %d", ch), 4096-BP_THR, BP_THR-0.5, 4095.5);
//	cout << "h[" << ch << "] created" << endl;
	if( h[ch] == NULL )
		cout << "Cannot create h[" << ch << "]" << endl;
}


void create_histo()
{
	for(int i=0; i<nchannel; i++)
		create_histo1(i);
}

void display_histo1(int ch, int logy)
{


  Double_t sum;

  c1->cd(ch+1);
  Double_t total;

  total = h[ch]->Integral(0,4096,"width");
  cout << "Channel " << ch << " Integral = " << total << " norm. ratio= " << knrat << endl; 
	
  if (total > 0) {
    if( logy > 0)
      c1->GetPad(ch+1)->SetLogy(1);
    else
      c1->GetPad(ch+1)->SetLogy(0);
    h[ch]->Draw();
  }
  
  Double_t th0, th1, th2;
  Int_t fth0, fth1;
  Int_t b0,b1,b2;
  
  fth0 = 0;
  fth1 = 0;
  Int_t is = h[ch]->GetSize();
  
  b0=0;
  b1= 0;
  b2 = is;
    
  th0 = 0;
  th1 = (Double_t) is;
  th2 = th1;
 
  Double_t ratio, rat0;
  
  int i= is-1;

  do {
    
    Double_t b = h[ch]->GetBinLowEdge(i);

    Double_t v = h[ch]->GetBinContent(i);

    
    // cout << i << " : " << b << " # " << v << " " << th1 << " " << fth1<< " , " << th0 << endl;
    
    if ((v>0) && (th1==((Double_t) is))) {
      th2 = b;
      b2 = h[ch]->GetBinLowEdge(i);
    }

    if ((v>0) && (fth1==0)) {
      th1 = b;
      b1 = h[ch]->GetBinLowEdge(i);
    }

    sum= h[ch]->Integral(i,is-1, "width");
    ratio = sum/total;

    if ((v==0) && (th1 < ((Double_t) is)) && (ratio>knrat*.97)) {
      fth1 = 1;
    }

    i--;

  } while ((i>0) && (fth1==0));

  int j=0;

  do {

    Double_t bb = h[ch]->GetBinLowEdge(j);
    Double_t v0 = h[ch]->GetBinContent(j);

    rat0= h[ch]->Integral(0,j, "width")/total;

    if ((v0==0) && (rat0 > (1-knrat)*.97) && (rat0<(1-knrat)*1.03)) {
      th0 = bb;
    }

    j++;

  } while ((j<is) && (th0==0));
  

  TF1 *g = new TF1("g","gaus",th1-20,th2+20);

  g->SetParameter(0,sum/2.5);
  g->SetParameter(1,(th2+th1)/2);
  g->SetParameter(2,(th2-th1)/6);

  //	g->SetParLimits(0,sum/2.,sum*2);
  g->SetParLimits(1,th1,th2);
  //	g->SetParLimits(2,(th2-th1)/20,(th2-th1)/2);
  
  g->SetLineColor(2);
  h[ch]->Fit(g,"","",th1-20,th2+20);

  Double_t fsum = g->GetParameter(0)*g->GetParameter(2)*sqrt(2.*3.1415);

  cout << " ==== " << ch << " : " << th0 << " " << th1 << " " << th2 << " sum= " << sum << " ratio= " << sum/total << " fit/sum= " << fsum/sum << endl;

 gth0.SetPoint(ch, ch, th0);
 gth1.SetPoint(ch, ch, th1);
 gratio.SetPoint(ch, ch, sum/total);
 gfitratio.SetPoint(ch, ch, fsum/sum);
 gsigma.SetPoint(ch, ch, g->GetParameter(2));

 kch = (Double_t) ch;
 kth0 = th0;
 kth1 = th1;
 kratio = sum/total;
 kfitratio =fsum/sum;
 ksigma =  g->GetParameter(2);

 fT->Fill();

}


void display_histo(int logy)
{
  Float_t rms;
  rms=0;
  for(int i=0; i<nchannel; i++) {
		display_histo1(i, logy);
		rms += h[i]->GetRMS();
  }

  cout << " Average RMS = " << rms/((Float_t) nchannel) << endl;
}

void draw_quality_indeces(int b0=0, int db=1) {

  TCanvas *c2 = new TCanvas("c2",Form("Graphs: %s",ifile.Data()));
  c2->Divide(2,3);

  TCut good = Form("(abs(ratio-%f)<0.03)&&(abs(fitratio-1.)<0.1)&&((th1-th0)>1500.)",knrat);
  good="(1==1)";
  cout << " Good Cut = " << good.GetTitle() << endl;

  c2->cd(1); 

  TH2F *frame = new TH2F("frame","Thresholds",db*16,16*b0-0.5,16*(b0+db)-0.5,10,0., 4000.);
  frame->Draw();

  // thresholds
  fT->SetMarkerStyle(26);
  fT->SetMarkerColor(2);
  fT->Draw("th0:ch+16*board","","same"); 
  c2->Update();
  
  fT->SetMarkerStyle(25);
  fT->SetMarkerColor(3);
  fT->Draw("th1:ch+16*board","","same"); 
  c2->Update();

  fT->SetMarkerStyle(21);
  fT->SetMarkerColor(2);
  fT->Draw("th0:ch+16*board",good,"same"); 
  c2->Update();

  Int_t nn = fT->GetEntries();
  
  fT->SetMarkerStyle(20);
  fT->SetMarkerColor(3);
  fT->Draw("th1:ch+16*board",good,"same"); 
  c2->Update();
  
  c2->cd(2);
  fT->Draw("th0",good);
  TH1F *ht0 = (TH1F*) gPad->GetPrimitive("htemp");
  c2->Update();

  c2->cd(4);
  fT->Draw("th1-th0",good);
  TH1F *ht1 = (TH1F*) gPad->GetPrimitive("htemp");
  c2->Update();

  // integral ratio
  c2->cd(3); 

  TH2F *frame1 = new TH2F("frame1","Ratios",db*16,16*b0-0.5,16*(b0+db)-0.5,10,0., 1.);
  frame1->Draw();

  fT->SetMarkerStyle(26);
  fT->SetMarkerColor(2);
  fT->Draw("ratio:ch+16*board","","same");
  c2->Update();
  
  fT->SetMarkerStyle(25);
  fT->SetMarkerColor(3);
  fT->Draw("fitratio:ch+16*board","","same");
  c2->Update();

  fT->SetMarkerStyle(21);
  fT->SetMarkerColor(2);
  fT->Draw("ratio:ch+16*board",good,"same");
  c2->Update();

  fT->SetMarkerStyle(20);
  fT->SetMarkerColor(3);
  fT->Draw("fitratio:ch+16*board",good,"same");
  c2->Update();

  // sigma
  
  c2->cd(5); 
  fT->SetMarkerStyle(25);
  fT->Draw("sigma:ch+16*board");
  c2->Update();

  fT->SetMarkerStyle(20);
  fT->Draw("sigma:ch+16*board",good,"same");
  c2->Update();

  c2->cd(6);
  fT->Draw("sigma",good);
  TH1F *hsi = (TH1F*) gPad->GetPrimitive("htemp");
  c2->Update();

  c2->SaveAs(Form("%s.eps",ifile.Data()));
  c2->SaveAs(Form("%s.jpg",ifile.Data()));

  cout << " *** 0-thr / delta / sigma *** " << endl;

  cout << "OUT_BP: " << ht0->GetMean() << " " << ht0->GetRMS() << " " << ht1->GetMean() << " " << ht1->GetRMS() << " " << hsi->GetMean() << " " << hsi->GetRMS() << " " << hsi->GetEntries() << endl;
  
}

//
//
//

void process(const char *fnameprefix, int board0=0, int dboard=1, int logy = 1, Int_t chthr=0)
{

  if (fT != NULL) { delete fT; }

  fT = new TTree("ft","Results");

  fT->Branch("ch", &kch, "ch/D");
  fT->Branch("th0", &kth0, "th0/D");
  fT->Branch("th1", &kth1, "th1/D");
  fT->Branch("ratio", &kratio, "ratio/D");
  fT->Branch("fitratio", &kfitratio, "fitratio/D");
  fT->Branch("sigma", &ksigma, "sigma/D");
  fT->Branch("board", &kboard, "board/D");

  ifile = fnameprefix;

  for (int ib=board0; ib<(board0+dboard); ib++) {
    BP_THR=chthr;
    create_histo();
    kboard= (Double_t) ib;
    read_data(Form("%s_%d",fnameprefix, ib));
    open_canvas(4,ifile);
    display_histo(logy);

    c1->SaveAs(Form("%s_%d.eps",ifile.Data(),ib));
    c1->SaveAs(Form("%s_%d.jpg",ifile.Data(),ib));

  }

  draw_quality_indeces(board0, dboard);

}

// 
void process_loop(const char * fnameprefix) {

  for (int i=0;i<30;i+=1) { 

    TString pippo = Form("%s_%04d_histo",fnameprefix, i+327);

    process(pippo.Data(), 0, 1);
    
  }

}

void process1(char *fnameprefix, int logy = 0, int ch = 0)
{
	create_histo1(ch);
	read_data1(ch,fnameprefix);
	open_canvas(1);
	display_histo1(ch, logy);

	TCanvas *c2 = new TCanvas("c2","Graphs");
	c2->Divide(1,5);


	c2->cd(1); 
	gth0.Draw("AP"); c2->Update();
	
	c2->cd(2); 
	gth1.Draw("AP"); c2->Update();
	
	c2->cd(3); 
	gratio.Draw("AP"); c2->Update();
	
	c2->cd(4); 
	gfitratio.Draw("AP"); c2->Update();
	
	c2->cd(5); 
	gsigma.Draw("AP"); c2->Update();
}

