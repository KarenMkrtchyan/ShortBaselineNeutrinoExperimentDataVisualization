void pdfMaker() {
    // Open the input file
    TFile *file = new TFile("E.root", "READ");
    if (!file || file->IsZombie()) {
        std::cerr << "Error: Cannot open the file!" << std::endl;
        return;
    }

    string drawParam[]= {"Width", "Amplitude", "Amplitude_Width"};
    for(int k =0; k<3; k++){ // Draw param 
        for(int i = 0; i < 3; i++) { // Loop planes
            for(int j = 0; j < 2; j++) { // Loop TPCs
                char name[100];
                snprintf(name, sizeof(name), "%s_TPC%d_Plane%d", drawParam[k].c_str(), j, i);             
                TCanvas *c1 = (TCanvas*)file->Get(name);
                std::string newName = std::string(name)+".pdf";
                gSystem->cd("nonTrack/");
                c1->SaveAs(newName.c_str()); //prints our plots as pdfs
            }
        }
    }
    file->Close();
}
