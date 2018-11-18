#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>

#include <TH1F.h>
#include <TF1.h>

#include <TSpectrum.h>
#include <TString.h>

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using std::cout;
using std::endl;

TH1F* analysis(TString const& filename, TString const& label){
	
        bool do_fit = true;

	TFile *file = new TFile(filename,"read");
   	TTree* t1 = (TTree*)file->Get("t1");

	TBranch *b_event;
   	TBranch *b_counts;
   	TBranch *b_charge;

        int event, counts;
        double charge;
	t1->SetBranchAddress("event", &event, &b_event);
  	t1->SetBranchAddress("counts", &counts, &b_counts);
  	t1->SetBranchAddress("charge", &charge, &b_charge);

        double res = 0.025; // pC
        double hist_xmin =  0.;
        double hist_xmax =  100.; // pC
        int n_bins = ( hist_xmax - hist_xmin )/res;
        TString hist_name = "histQDC"; hist_name += "_"; hist_name += label;
	TH1F* hist = new TH1F(hist_name, "", n_bins, hist_xmin, hist_xmax);
        hist->Sumw2(true);

	Long64_t nentries = t1->GetEntriesFast();

        int n_events = 0;
	for (Long64_t i=0; i < nentries; ++i) {
    	   t1->GetEntry(i);
    	   hist->Fill(charge);
           ++n_events;
        }

        hist->Scale( 1./n_events );

        TString canvas_name = "canvas"; canvas_name += "_"; canvas_name += label;
        TCanvas* canvas = new TCanvas(canvas_name);
 	hist->GetYaxis()->SetTitle("Normalized");
   	hist->GetXaxis()->SetTitle("Charge (pC)");
   	hist->Draw(); 

        if(do_fit){
	   int n_sigmas = 3;
	   double mean = hist->GetMean();
	   double sigma = hist->GetStdDev();
	   double x_min = mean - n_sigmas*sigma; 
	   double x_max = mean + n_sigmas*sigma;
    
	   TF1* fit_function0 = new TF1("fit_function","gaus",x_min,x_max);
	   hist->Fit(fit_function0,"R","");
	   
	   double chi2 = fit_function0->GetChisquare(); 
	   int ndf = fit_function0->GetNDF();
	   double par0 = fit_function0->GetParameter(0);
	   double err_par0 = fit_function0->GetParError(0);
	   double par1 = fit_function0->GetParameter(1);
	   double err_par1 = fit_function0->GetParError(1);
	   double par2 = fit_function0->GetParameter(2);
	   double err_par2 = fit_function0->GetParError(2);
        }
 
        //TSpectrum spectrum;
        //spectrum.Search(hist,5.,"",0.5);

        return hist;
}

void analysis(){

   /*TString filename_pedestal = "analysis_QDC_pedestal_241016-02.root";
   TString filename_data_1 = "analysis_QDC_241016-01.root";
   TString filename_data_2 = "analysis_QDC_241016-02.root";*/
   TString filename_data_1 = "analysis_QDC_071116-01.root";
   TString filename_data_2 = "analysis_QDC_071116-02.root";
   TString filename_data_3 = "analysis_QDC_071116-03.root";
   TString filename_data_4 = "analysis_QDC_071116-04.root";
   TString filename_data_5 = "analysis_QDC_071116-05.root";
   TString filename_pedestal_1 = "analysis_QDC_pedestal_071116-01.root";

   /*TH1F* hist_pedestal = analysis(filename_pedestal,"pedestal");
   TH1F* hist_data_1 = analysis(filename_data_1,"data_1");
   TH1F* hist_data_2 = analysis(filename_data_2,"data_2");*/
   TH1F* hist_data_1 = analysis(filename_data_1,"data_1");
   TH1F* hist_data_2 = analysis(filename_data_2,"data_2");
   TH1F* hist_data_3 = analysis(filename_data_3,"data_3");
   TH1F* hist_data_4 = analysis(filename_data_4,"data_4");
   TH1F* hist_data_5 = analysis(filename_data_5,"data_5");
   TH1F* hist_pedestal_1 = analysis(filename_pedestal_1,"pedestal");

   TCanvas* canvas = new TCanvas("canvas_all");
   canvas->cd();

   hist_pedestal_1->SetLineWidth(2);
   hist_pedestal_1->SetLineStyle(2);
   hist_pedestal_1->SetLineColor(kRed);

   hist_data_1->SetLineWidth(2);
   hist_data_1->SetLineStyle(1);
   hist_data_1->SetLineColor(38);
   hist_data_2->SetLineWidth(2);
   hist_data_2->SetLineStyle(1);
   hist_data_2->SetLineColor(46);
   hist_data_3->SetLineWidth(2);
   hist_data_3->SetLineStyle(1);
   hist_data_3->SetLineColor(38);
   hist_data_4->SetLineWidth(2);
   hist_data_4->SetLineStyle(1);
   hist_data_4->SetLineColor(46);
   hist_data_5->SetLineWidth(2);
   hist_data_5->SetLineStyle(1);
   hist_data_5->SetLineColor(38);
  
   hist_pedestal_1->SetStats(false);
   hist_data_1->SetStats(false);
   hist_data_2->SetStats(false);
   hist_data_3->SetStats(false);
   hist_data_4->SetStats(false);
   hist_data_5->SetStats(false);

   hist_pedestal_1->Draw("HISTE1");
   hist_data_1->Draw("HISTE1SAME");
   hist_data_2->Draw("HISTE1SAME");
   hist_data_3->Draw("HISTE1SAME");
   hist_data_4->Draw("HISTE1SAME");
   hist_data_5->Draw("HISTE1SAME");
} 

