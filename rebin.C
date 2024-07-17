void rebin() {
    // Open the input file
    TFile *file = new TFile("combined.root", "READ");
    if (!file || file->IsZombie()) {
        std::cerr << "Error: Cannot open the file!" << std::endl;
        return;
    }
    #define outFile "rebined.root" //make this match the direcotry used
    TFile *outputFile = new TFile(outFile, "RECREATE");
    if (!outputFile->IsOpen()) {
        std::cerr << "Error: Cannot create or open output file!" << std::endl;
        return;
    }


    string drawParam[]= {"Width", "Amplitude", "Amplitude_Width"};
    int rebinFactor = 5; // Merge 5 bins into 1
    for(int k =0; k<3; k++){ // Draw param 
        for(int i = 0; i < 3; i++) { // Loop planes
            for(int j = 0; j < 2; j++) { // Loop TPCs
                char simName[100];
                char simModName[100];
                char dataName[100];
 
                snprintf(simName, sizeof(simName), "%s_TPC%d_Plane%d", drawParam[k].c_str(), j, i); 
                snprintf(simModName, sizeof(simModName), "MOD_%s_TPC%d_Plane%d", drawParam[k].c_str(), j, i);
                snprintf(dataName, sizeof(dataName), "DATA_%s_TPC%d_Plane%d", drawParam[k].c_str(), j, i);

                std::cout << "Overlaying TPC" << i << " && Plane" << j <<" for " << drawParam[k] << std::endl;
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
                TH1 *rebinnedSim = sim->Rebin(rebinFactor);
                TH1 *rebinnedSimMod = simMod->Rebin(rebinFactor);
                TH1 *rebinnedData = data->Rebin(rebinFactor);

                rebinnedSim->Write();
                rebinnedSimMod->Write();
                rebinnedData->Write();


            }
        }
    }
    file->Close();
}
