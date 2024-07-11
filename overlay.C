void overlay(){
    #define filename "out.root"

    TFile *file = new TFile(filename, "READ");
    if (!file || file->IsZombie()) {
        std::cerr << "Error: Cannot open the file!" << std::endl;
        return;
    }
    TH1F *sim = (TH1F*)file->Get("Width_TPC0_Plane0");
    if (!sim) {
        std::cerr << "Error: Cannot find the TTree" << std::endl;
        file->Close();
        return;
    }
    TH1F *simMod = (TH1F*)file->Get("MOD_Width_TPC0_Plane0");
    if (!simMod) {
        std::cerr << "Error: Cannot find the TTree" << std::endl;
        file->Close();
        return;
    }
    TH1F *data = (TH1F*)file->Get("DATA_Width_TPC0_Plane0");
    if (!data) {
        std::cerr << "Error: Cannot find the TTree" << std::endl;
        file->Close();
        return;
    }

    auto hs = new THStack("hs", "");
    hs->Add(data);
    simMod->SetFillColor(kGreen);
    hs->Add(simMod);
    sim->SetFillColor(kRed);
    hs->Add(sim);
    auto cs = new TCanvas("cs", "cs", 10, 10, 700, 900);
    hs->Draw();
}