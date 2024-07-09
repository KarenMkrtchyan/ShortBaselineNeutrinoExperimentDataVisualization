#include "TFile.h"
#include "TH1F.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TPad.h"

void widthVamp() {
    // Open the ROOT file
    TFile *file = new TFile("Supp_Stage1_55_track.root", "read");
    if (!file || file->IsZombie()) {
        std::cerr << "Error: Cannot open the file!" << std::endl;
        return;
    }
    TDirectoryFile *dir = (TDirectoryFile*)file->Get("hitttreemaker");
    if (!dir) {
        std::cerr << "Error: Cannot find the directory 'hitttreemaker'!" << std::endl;
        return;
    }
    TTree *tree = (TTree*)dir->Get("pandoraTrackGausCryoE_MCstage1Var");
    if (!tree) {
        std::cerr << "Error: Cannot find the TTree 'pandoraTrackGausCryoE_MCstage1Var'!" << std::endl;
        return;
    }

    // Create a canvas with 2x2 pads
    TCanvas *c1 = new TCanvas("c1", "Width vs Amplitude for Planes 0-3", 1200, 900);
    c1->Divide(2, 2);

    // Remove stat box for all plots
    gStyle->SetOptStat(0);

    // Loop over planes 0 through 3
    for (int plane = 0; plane <= 3; plane++) {
        // Select the appropriate pad for each plane
        c1->cd(plane + 1);
        
        // Create the condition string for each plane
        TString condition = Form("TPC==0 && Plane==%d", plane);
        
        // Draw the plot for each plane
        tree->Draw("Amplitude:Width", condition.Data(), "colz");
        
        // Get the histogram and set labels
        TH1 *hist = (TH1*)gPad->GetPrimitive("htemp");
        if (hist) {
            hist->SetTitle(Form("Plane %d", plane));
            hist->GetXaxis()->SetTitle("Hit Width [ticks]");
            hist->GetYaxis()->SetTitle("Amplitude [ADC]");
        }
        gPad->Modified();
        gPad->Update();
    }

    // Update the canvas to show all plots
    c1->Update();

    file->Close();
}