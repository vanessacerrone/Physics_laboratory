#include <iostream>
#include <string>
#include "../../macros/RootStyle.cc"

/*requested files: histogramOfData.root, histogramOfBackground.root, channel*/
using namespace std;

void peaksearch(string dataFileName, short chan, double conversion_factor_BG)
{
    set_root_style(1);

    /*Reading histogram file from the Root file*/

    //data
    const string bgFileName = "histo_bkg2500.root";

    TFile *dataFile = new TFile(dataFileName.c_str());
    TH1F *hdata = (TH1F*)dataFile->Get(Form("ch%i",chan));
    
    TFile *bgFile = new TFile(bgFileName.c_str());
    TH1F *hbg = (TH1F*)bgFile->Get(Form("ch%i",chan));


    TH1F* hbgConverted = (TH1F*)hbg->Clone("h");
    double temp;

    for (int i = 1; i<=hbg->GetXaxis()->GetNbins(); i++) 
    {
        temp = conversion_factor_BG * hbg->GetBinContent(i); 
        hbgConverted->SetBinContent(i, temp);
    }

    // set calibration function parameters
    Double_t a;
    Double_t b;

    //different calibrations according to the parameters needed for the 2 detectors


     if (chan == 0) //NaI
    {
        a = -9.25518;
        b = 0.0878425;

    }

    if (chan == 1) //HPGe
    {
        a = 1.44349;
        b = 0.15422;
    }

    //calibrating x-axis for data spectrum
    int max_binData = hdata->GetNbinsX(); 
	float max_kevData = hdata->GetBinCenter(max_binData)*b + a;
	hdata->GetXaxis()->SetLimits(a,max_kevData);


    //same for background
    int max_binBg = hbgConverted->GetNbinsX(); 
	float max_kevBg = hbgConverted->GetBinCenter(max_binBg)*b + a;
    hbgConverted->GetXaxis()->SetLimits(a,max_kevBg);

    //subtracting bin-to-bin the histogram of bg from the data one
    hdata->Add(hbgConverted, -1.);
    hdata->SetTitle("ZrO_{2} - HPGe detector");


    TH1F *h_peaks = (TH1F*)hdata->Clone();


	TSpectrum *s = new TSpectrum(30);
	int nPeaks;
	double *xPeaks;
	int sigma = 2;
	double minratio = 0.02;	// minimum ratio between a peak and the main peak

	nPeaks = s->Search(h_peaks,sigma,"nodraw",minratio);

	xPeaks = s->GetPositionX();

    vector<double> peakvec;

    for (int p = 0; p<nPeaks; p++){

        peakvec.push_back(xPeaks[p]);
    }
   
    // Sorting the  vector
    sort(peakvec.begin(), peakvec.end());

	for (int p = 0; p<nPeaks; p++) {
		cout << "Peak #" << p << " @ energy " << peakvec[p] << endl;	
	}
	
    
    h_peaks->GetXaxis()->SetRangeUser(0, 2000);

    float w;
    w = h_peaks->GetXaxis()->GetBinWidth(0);
    hdata->SetMinimum(0);
    hdata->GetXaxis()->SetTitle("Energy [keV]");
    hdata->GetYaxis()->SetTitle(Form("Counts/%0.1f keV",w));
    hdata->SetStats(kFALSE);
    hdata->GetYaxis()->SetMaxDigits(3);
    //h_peaks->Draw();

    Double_t peaks[17] = {76.3794,187.396,241.053, 296.562,352.07,463.086,511.193,583.353,609.257, 766.53,909.001,933.054, 966.359,1118.08,1234.65,1375.27,1760.13};
    Double_t min[17];
    Double_t max[17];
    Double_t tolerance = 0.014;
    
    // Set fit range 
    for (int i = 3; i<17; i++){

        min[i] = peaks[i]*(1-tolerance);
        max[i] = peaks[i]*(1+tolerance);

    }

    min[0] = peaks[0]*(1-0.05);
    max[0] = peaks[0]*(1+0.05);

    for (int i = 1; i<4; i++){

        min[i] = peaks[i]*(1-0.04);
        max[i] = peaks[i]*(1+0.04);

    }
   

    ofstream f;
    f.open ("ZrO2_HPGe.txt", std::ofstream::out | std::ofstream::app);
    f << "Mean "  << "\t" << "\t" << "\t"  << "StdDev " << "\t" << "Resolution[%]" << "\t"  << "\n";

    

    vector<double> mean;
    vector<double> stdev;
    vector<double> mean_err;
    vector<double> stdev_err;
    vector<double> res_v;
      vector<double> counts;
    Double_t res;
    Double_t integral;

    TF1** fit = new TF1*[17]; 
    for (unsigned int i=0;i < 17;i++) { 
        
        fit[i] = new TF1(Form("f%d",i), "gaus(0)+pol1(3)",min[i],max[i]);
        fit[i]->SetParameter(1,peaks[i]);
        fit[i]->SetParameter(2,7);
        fit[i]->SetParLimits(2,0,20);
        fit[i]->SetLineStyle(1);
	    fit[i]->SetLineWidth(1);
        hdata->Fit(fit[i],"R+");
        fit[i]->Draw("same");

        

        mean.push_back(fit[i]->GetParameter(1));
        mean_err.push_back(fit[i]->GetParError(1));
        stdev.push_back(fit[i]->GetParameter(2));
        stdev_err.push_back(fit[i]->GetParError(2));

        integral = (fit[i]->Integral(min[i],max[i]))/hdata->GetBinWidth(0);
        
        res = 2*sqrt(2*log(2))*stdev[i] / mean[i];
        res_v.push_back(res*100);
        counts.push_back(integral);

        f << mean[i] << "\t" << mean_err[i] << "\t" << stdev[i]
                << "\t" << stdev_err[i] << "\t" << res_v[i] << "\t" << counts[i] << "\t" << sqrt(counts[i]) << '\n'; 

   } 

    f.close();

}



