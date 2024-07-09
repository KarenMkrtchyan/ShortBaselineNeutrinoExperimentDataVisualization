void width() {
    // ROOT file name
    #define filename "Supp_Stage0_55.root"

    TFile *file = new TFile(filename, "READ");
    if (!file || file->IsZombie()) {
        std::cerr << "Error: Cannot open the file!" << std::endl;
        return;
    }

    // Subdirectory 
    TDirectoryFile *dir = (TDirectoryFile*)file->Get("hitttreemaker");
    if (!dir) {
        std::cerr << "Error: Cannot find the directory 'hitttreemaker'!" << std::endl;
        file->Close();
        return;
    }

    // Actual Data
    TTree *tree = (TTree*)dir->Get("gaushit1dTPCEE_MCStage0Var");
    if (!tree) {
        std::cerr << "Error: Cannot find the TTree 'gaushit1dTPCEE_MCStage0Var'" << std::endl;
        file->Close();
        return;
    }

    // Open output file in UPDATE mode
    TFile *outputFile = new TFile("out.root", "RECREATE");
    if (!outputFile->IsOpen()) {
        std::cerr << "Error: Cannot create or open output file!" << std::endl;
        file->Close();
        return;
    }

    // Automate the plotting of each plane and TPC
    const char *format = "TPC==%d&&Plane==%d";
    const char *formatTitle = "TPC %d, Plane %d";
    char formattedString[100];
    char formattedString2[100];
    string drawParam[]= {"Width", "Amplitude"};


    // Disable statistics box globally
    gStyle->SetOptStat(0);
    TCanvas *c1 = new TCanvas("c1", "Width Histograms", 800, 600);

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            snprintf(formattedString, sizeof(formattedString), format, i, j);
            snprintf(formattedString2, sizeof(formattedString2), formatTitle, i, j); // For title

            for(int k =0; k<2; k++){
            std::string param = drawParam[k];
            std::string drawCommand = param + ">>htemp";
            std::cout << "Drawing: " << drawCommand << " with selection: " << formattedString << std::endl;
            
            tree->Draw(drawCommand.c_str(), formattedString);
            TH1 *h = (TH1*)gDirectory->Get("htemp");

            // Handle the case where h is NULL
            if (h) {
                char name[100];
                snprintf(name, sizeof(name), "Width_TPC%d_Plane%d", i, j);
                h->SetName(name);
                h->SetTitle(formattedString2);
                h->GetXaxis()->SetTitle("Hit Width [ticks]");
                h->GetYaxis()->SetTitle("Hits");
                h->GetYaxis()->SetTitleOffset(1.3);
                h->Write();
            } else {
                std::cerr << "Warning: Histogram for TPC " << i << " Plane " << j << " not created (empty selection)." << std::endl;
            }

        }
    }

    // Close the files
    outputFile->Close();
    file->Close();
    }
}
