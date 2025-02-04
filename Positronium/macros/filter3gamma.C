#include "gethisto.C"
#include <vector>


/*-- Filter the spectra for the 3 photons decay *--/

/*
 * Author  : Vanessa
 * Example : Raw data in input root file is processed and stored
             histograms, then a filtering procedure is performed:
             - sum of D1, D2 and D3 events has to be (1022 ± 10 %) keV
             - D1 D2 and D3 energy must be in [0,450] keV
             

 * Usage   : $ cd /path/to/root/file
 *           $ root
 *           # .L filter3gamma.C
 *           # filter("file.root", 1000, 0,2000) // histograms with 1000 bins for x in [0,2000] 
 */

void filter(const string name_file, int numBins, double minX, double maxX) {

	// process raw data
	slimport_data_t indata1,indata2,indata3,indata4;
	TFile *infile = new TFile(name_file.c_str());
	TTree *intree = (TTree*)infile->Get("acq_tree_0");

	TBranch *inbranch1 = intree->GetBranch("acq_ch0");
    TBranch *inbranch2 = intree->GetBranch("acq_ch1");
    TBranch *inbranch3 = intree->GetBranch("acq_ch2");
    TBranch *inbranch4 = intree->GetBranch("acq_ch3");

	inbranch1->SetAddress(&indata1.timetag);
    inbranch2->SetAddress(&indata2.timetag);
    inbranch3->SetAddress(&indata3.timetag);
    inbranch4->SetAddress(&indata4.timetag);

	TH1F *h_spectrum1 = new TH1F("h1","Detector 1",numBins,minX,maxX);
    TH1F *h_spectrum2 = new TH1F("h2","Detector 2",numBins,minX,maxX);
    TH1F *h_spectrum3 = new TH1F("h3","Detector 3",numBins,minX,maxX);
    TH1F *h_fil1 = new TH1F("h1f","3 photons decay - Filtered spectra",250,0,1000);
    TH1F *h_fil2 = new TH1F("h2f","Detector 2 filtered",250,0,1000);
    TH1F *h_fil3 = new TH1F("h3f","Detector 3 filtered",250,0,1000);
    TH1F *h_sum = new TH1F("hsum","3 photons decay - Sum spectrum",numBins,minX,maxX);
    TH1F *h_tac = new TH1F("htac","3 photons decay - TAC spectrum",1000,0,1000);
    

	// histogram filling
    double entry1,entry2,entry3,entry4;
    
	for (int i=0; i<inbranch3->GetEntries(); i++) {
        inbranch2->GetEntry(i);
        entry2 =  -21.7267 + indata2.qlong*0.114383  ;
		inbranch1->GetEntry(i);
        entry1 = -17.7045 + indata1.qlong*0.115652;
        inbranch3->GetEntry(i);
        entry3 = -22.9221 + indata3.qlong*0.110317;
        inbranch4->GetEntry(i);
        entry4 = -0.0201002 + 0.0116122*indata4.qlong;

        h_spectrum1->Fill(entry1);
        h_spectrum2->Fill(entry2);
        h_spectrum3->Fill(entry3);
        double sum = entry1+entry2+entry3;

        h_sum->Fill(sum);
        
        // conditions
        
		if ((entry1 + entry2 + entry3) < 1124.2 && (entry1 + entry2 + entry3) > 919.8 ) {
            if ( (entry1 < 450 && entry1 > 0) && (entry2 < 450 && entry2 > 0) && (entry3 < 450 && entry3 > 0)){

			 h_fil1->Fill(entry1);
             h_fil2->Fill(entry2);
             h_fil3->Fill(entry3);
             h_tac->Fill(entry4);
             
		}
        }

	}


    TCanvas* c1 = new TCanvas("c1","D1",1080,1020);
    gPad->SetLeftMargin(0.12);
    gStyle->SetOptStat(0000);
    gStyle->SetTitleY(0.975);
    h_fil1->SetMinimum(1);
    h_fil1->GetXaxis()->SetTitle("Energy [keV]");
    h_fil1->GetXaxis()->SetLabelOffset(0.01);
    h_fil1->GetXaxis()->SetLabelSize(0.035);
    h_fil1->GetXaxis()->SetTitleSize(0.04);
    h_fil1->GetXaxis()->SetTitleOffset(1.15);
    h_fil1->GetYaxis()->SetTitle("Counts");
    h_fil1->GetYaxis()->SetLabelOffset(0.008);
    h_fil1->GetYaxis()->SetLabelSize(0.035);
    h_fil1->GetYaxis()->SetTitleSize(0.04);
    h_fil1->GetYaxis()->SetTitleOffset(1.3);
    h_fil1->GetXaxis()->SetRangeUser(0, 600);
    h_fil1->GetYaxis()->SetRangeUser(1, 2000);
    h_fil1->SetLineWidth(1);
    h_fil1->Draw();

    auto legend = new TLegend(0.67,0.75,0.85,0.87);
    legend->AddEntry(h_fil1,"Filtered data","l");
    legend->Draw();



    TCanvas* c2 = new TCanvas("c2","D2",1080,1020);
    gPad->SetLeftMargin(0.12);
    gStyle->SetOptStat(0000);

    h_fil2->SetMinimum(1);
    h_fil2->GetXaxis()->SetTitle("Energy [keV]");
    h_fil2->GetXaxis()->SetLabelOffset(0.01);
    h_fil2->GetXaxis()->SetLabelSize(0.035);
    h_fil2->GetXaxis()->SetTitleSize(0.04);
    h_fil2->GetXaxis()->SetTitleOffset(1.15);
    h_fil2->GetYaxis()->SetTitle("Counts");
    h_fil2->GetYaxis()->SetLabelOffset(0.008);
    h_fil2->GetYaxis()->SetLabelSize(0.035);
    h_fil2->GetYaxis()->SetTitleSize(0.04);
    h_fil2->GetYaxis()->SetTitleOffset(1.3);
    h_fil2->GetXaxis()->SetRangeUser(0, 500);
    h_fil2->GetYaxis()->SetRangeUser(1, 2000);
    h_fil2->SetLineWidth(1);
    h_fil2->Draw();
    

    auto legend2 = new TLegend(0.67,0.75,0.85,0.87);
    legend2->AddEntry(h_fil2,"Filtered data","l");
    legend2->Draw();



    TCanvas* c3 = new TCanvas("c3","D3",1080,1020);
    gPad->SetLeftMargin(0.12);
    gStyle->SetOptStat(0000);

    h_fil3->SetMinimum(0);
    h_fil3->GetXaxis()->SetTitle("Energy [keV]");
    h_fil3->GetXaxis()->SetLabelOffset(0.01);
    h_fil3->GetXaxis()->SetLabelSize(0.035);
    h_fil3->GetXaxis()->SetTitleSize(0.04);
    h_fil3->GetXaxis()->SetTitleOffset(1.15);
    h_fil3->GetYaxis()->SetTitle("Counts");
    h_fil3->GetYaxis()->SetLabelOffset(0.008);
    h_fil3->GetYaxis()->SetLabelSize(0.035);
    h_fil3->GetYaxis()->SetTitleSize(0.04);
    h_fil3->GetYaxis()->SetTitleOffset(1.3);
    h_fil3->GetXaxis()->SetRangeUser(0, 500);
    h_fil3->GetYaxis()->SetRangeUser(0, 2000);
    h_fil3->SetLineWidth(1);
    h_fil3->Draw();

    auto legend3 = new TLegend(0.67,0.75,0.85,0.87);
    legend3->AddEntry(h_fil3,"Filtered data","l");
    legend3->Draw();

    TCanvas* c4 = new TCanvas("c4","",1080,1020);
    gPad->SetLeftMargin(0.12);
    gStyle->SetOptStat(0000);
   
    h_tac->SetMinimum(1);
    h_tac->GetXaxis()->SetTitle("time [ns], arbitrary zero");
    h_tac->GetXaxis()->SetLabelOffset(0.01);
    h_tac->GetXaxis()->SetLabelSize(0.035);
    h_tac->GetXaxis()->SetTitleSize(0.04);
    h_tac->GetXaxis()->SetTitleOffset(1.15);
    h_tac->GetYaxis()->SetTitle("Counts/1 ns");
    h_tac->GetYaxis()->SetLabelOffset(0.008);
    h_tac->GetYaxis()->SetLabelSize(0.035);
    h_tac->GetYaxis()->SetTitleSize(0.04);
    h_tac->GetYaxis()->SetTitleOffset(1.3);
    h_tac->GetXaxis()->SetRangeUser(0, 1000);
    h_tac->GetYaxis()->SetRangeUser(0, 5500);
    h_tac->SetFillColorAlpha(kAzure-9,0.5);
    h_tac->SetLineWidth(1);
    h_tac->Draw();

    //TLine *line = new TLine(1022, 0, 1022, 5000);
    //line->SetLineStyle(2);
    //line->SetLineColor(kRed);
    //line->Draw("same");

    //TPaveText *pt = new TPaveText(0.2,0.1,0.5,0.5,"blNDC");
    //pt->AddText("1022 keV");
    //pt->Draw("same");


    TCanvas* c5 = new TCanvas("c5","Sum",1080,1020);
    gPad->SetLeftMargin(0.12);
    gStyle->SetOptStat(0000);

    h_fil1->SetLineColor(kRed);
    h_fil1->SetLineWidth(1);
    h_fil1->Draw();

    h_fil2->SetLineColor(kBlue);
    h_fil2->SetLineWidth(1);
    h_fil2->Draw("same");

    h_fil3->SetLineColor(kGreen-3);
    h_fil3->SetLineWidth(1);
    h_fil3->Draw("same");


    auto legend4 = new TLegend(0.67,0.75,0.85,0.87);
    legend4->AddEntry(h_fil1,"D1","l");
    legend4->AddEntry(h_fil2,"D2","l");
    legend4->AddEntry(h_fil3,"D3","l");
    legend4->Draw();

    

}

