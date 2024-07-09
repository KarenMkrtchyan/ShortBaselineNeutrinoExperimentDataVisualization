void width() {
    // ROOT file name
    //--------------CHANGE FILE NAME--------------//
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
        std::cerr << "Error: Cannot find the TTree" << std::endl;
        file->Close();
        return;
    }

    //------------CHANGE OUTPUT FILE NAME------------------//
    //RECREATE will overwrite the file, use this the first time executing
    //UPDATE will add on to the file, use this ever subsequent run 
    TFile *outputFile = new TFile("out.root", "RECREATE");
    if (!outputFile->IsOpen()) {
        std::cerr << "Error: Cannot create or open output file!" << std::endl;
        file->Close();
        return;
    }

    // Automate the plotting of each plane and TPC
    const char *format = "TPC==%d && Plane==%d";
    const char *formatTitle = "TPC %d, Plane %d";
    char formattedString[100];
    char formattedString2[100];

    // Disable statistics box globally
    gStyle->SetOptStat(0);
    string drawParam[]= {"Width", "Amplitude", "Amplitude:Width"};
    TCanvas *c1 = new TCanvas("c1", "Histograms", 800, 600); //needed so h can be properly selected later on

for(int k =0; k<3; k++){//Draw param (do not ask why it didn't work being the 3rd loop in the nest)
    for (int i = 0; i < 3; i++) { //TPC
        for (int j = 0; j < 3; j++) { //Plane         
            snprintf(formattedString, sizeof(formattedString), format, i, j); // for logical selection
            snprintf(formattedString2, sizeof(formattedString2), formatTitle, i, j); // For title
                std::string param = drawParam[k];
                std::string drawCommand = param + ">>htemp";
                std::cout << "Drawing: " << drawCommand << " with selection: " << formattedString << std::endl; 
                if(k==2){
                    tree->Draw(drawCommand.c_str(), formattedString, "colz");
                }else{
                    tree->Draw(drawCommand.c_str(), formattedString);
                }              
                TH1 *h = (TH1*)gDirectory->Get("htemp");

                // Handle the case where h is NULL (some files are missing TPC 3)
                if (h) {
                    char name[100];
                    snprintf(name, sizeof(name), "%s_TPC%d_Plane%d", param.c_str(), i, j);
                    h->SetName(name);
                    h->SetTitle(formattedString2);
                    if(k==0){
                        h->GetXaxis()->SetTitle("Hit Width [ticks]");
                         h->GetYaxis()->SetTitle("Hits");
                    }else if(k==1){
                        h->GetXaxis()->SetTitle("Amplitude [ADC]");
                        h->GetYaxis()->SetTitle("Hits");
                    }else{
                        snprintf(name, sizeof(name), "Amplitude_Width_TPC%d_Plane%d",  i, j);
                        h->SetName(name);
                        h->SetTitle(formattedString2);
                        h->GetXaxis()->SetTitle("Hit Width [ticks]");
                        h->GetYaxis()->SetTitle("Amplitude [ADC]"); 
                    }                      
                    h->GetYaxis()->SetTitleOffset(1.3);
                    std::string newName = std::string(name)+".pdf";
                    // c1->SaveAs(newName.c_str()); //prints our plots as pdfs
                    h->Write();
                } else {
                    std::cerr << "Warning: Histogram for TPC " << i << " Plane " << j << " not created (empty selection)." << std::endl;
                }
            }
    }
}
    // Close the files
    outputFile->Close();
    file->Close();
}
