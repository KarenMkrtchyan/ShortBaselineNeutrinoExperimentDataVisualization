void binCounter(){
     // Open the ROOT file
    TFile *file = new TFile("E.root", "READ");
    if (!file || file->IsZombie()) {
        std::cerr << "Error: Cannot open the file!" << std::endl;
        return;
    }
    char graph[] = "Amplitude_TPC0_Plane2";
    // Retrieve the canvas
    TCanvas *c1 = (TCanvas*)file->Get(graph);
    if (!c1) {
        std::cerr << "Error: Cannot find the canvas Width_TPC1_Plane0" << std::endl;
        file->Close();
        return;
    }

    // Retrieve the histogram from the canvas
    TH1F *hist = (TH1F*)c1->GetPrimitive(graph);
    if (!hist) {
        std::cerr << "Error: Cannot find the histogram Width_TPC1_Plane0 in the canvas" << std::endl;
        file->Close();
        return;
    }
    for (int bin = 1; bin <= hist->GetXaxis()->GetNbins(); ++bin) {
        double binContent = hist->GetBinContent(bin);
        std::cout << "Bin " << bin << " integral is " << binContent << "\n";
    }
    file->Close();
}