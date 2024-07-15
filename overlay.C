void overlay() {
    // Input file
    #define filename "out.root" //output of visualize.C
    TFile *file = new TFile(filename, "READ");
    if (!file || file->IsZombie()) {
        std::cerr << "Error: Cannot open the file!" << std::endl;
        return;
    }
    // Output file
    #define outFile "15.root" //make this match the direcotry used
    TFile *outputFile = new TFile(outFile, "RECREATE");
    if (!outputFile->IsOpen()) {
        std::cerr << "Error: Cannot create or open output file!" << std::endl;
        return;
    }

    //no stat box 
    gStyle->SetOptStat(0);

    for(int i = 0; i < 3; i++) { // Loop planes
        for(int j = 0; j < 2; j++) { // Loop TPCs
            char simName[100];
            char simModName[100];
            char dataName[100];
            snprintf(simName, sizeof(simName), "Width_TPC%d_Plane%d", j, i); 
            snprintf(simModName, sizeof(simModName), "MOD_Width_TPC%d_Plane%d", j, i);
            snprintf(dataName, sizeof(dataName), "DATA_Width_TPC%d_Plane%d", j, i);

            std::cout << "Overlaying TPC" << i << " && Plane" << j << std::endl;
            TH1F *sim = (TH1F*)file->Get(simName);
            if (!sim) {
                std::cerr << "Error: Cannot find the histogram " << simName << std::endl;
                file->Close();
                return;
            }
            TH1F *simMod = (TH1F*)file->Get(simModName);
            if (!simMod) {
                std::cerr << "Error: Cannot find the histogram " << simModName << std::endl;
                file->Close();
                return;
            }
            TH1F *data = (TH1F*)file->Get(dataName);
            if (!data) {
                std::cerr << "Error: Cannot find the histogram " << dataName << std::endl;
                file->Close();
                return;
            }
            //Normalizing histograms to the data file
            double dataInt = (double)data->Integral();
            double simInt = sim->Integral();
            double simModInt = simMod->Integral();
            std::cout << dataInt/simInt << std::endl << dataInt/simModInt << std::endl;
            sim->Scale(dataInt/simInt);
            sim->Sumw2(0);
            simMod->Scale(dataInt/simModInt);
            simMod->Sumw2(0);

            //Style plots
            simMod->SetLineColor(kGreen);
            sim->SetLineColor(kRed);

            auto cs = new TCanvas(simName, "15 E");
            auto legend = new TLegend(0.5, 0.7, 0.9, 0.9);

            legend->SetTextSize(0.04); 
            legend->SetFillStyle(0);
            legend->AddEntry(data, "Detector Data");
            legend->AddEntry(sim, "Simulated Data");
            legend->AddEntry(simMod, "Modified Simulation Data");

            simMod->Draw();
            sim->Draw("same");
            data->Draw("same");
            legend->Draw();

            cs->Write();
        }
    }
    #undef filename
    #undef outFile
}
