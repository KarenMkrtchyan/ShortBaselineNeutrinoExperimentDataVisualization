void overlay() {
    // Input file
    #define filename "combined.root" //output of visualize.C
    TFile *file = new TFile(filename, "READ");
    if (!file || file->IsZombie()) {
        std::cerr << "Error: Cannot open the file!" << std::endl;
        return;
    }
    // Output file
    #define outFile "overlayed.root" //make this match the direcotry used
    TFile *outputFile = new TFile(outFile, "RECREATE");
    if (!outputFile->IsOpen()) {
        std::cerr << "Error: Cannot create or open output file!" << std::endl;
        return;
    }

    //no stat box 
    gStyle->SetOptStat(0);

    string drawParam[]= {"Width", "Amplitude", "Amplitude_Width"};

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
                //Normalizing histograms to the data file DO ONLY ONCE TO COMBINED HISTOGRAM
                double dataInt = (double)data->Integral();
                double simInt = sim->Integral();
                double simModInt = simMod->Integral();
               // std::cout << dataInt/simInt << std::endl << dataInt/simModInt << std::endl;
                sim->Scale(dataInt/simInt);
                //sim->Sumw2(0); //removes the error bars to look like a normal histogram 
                simMod->Scale(dataInt/simModInt);
                //simMod->Sumw2(0);

                //Style plots
                simMod->SetLineColor(kRed);
                sim->SetLineColor(kBlue);
                data->SetLineColor(kBlack);
                auto cs = new TCanvas(simName, "15 E");
                if(k != 2){           
                    auto legend = new TLegend(0.5, 0.7, 0.9, 0.9);

                    legend->SetTextSize(0.04); 
                    legend->SetFillStyle(0);
                    legend->AddEntry(data, "Data");
                    legend->AddEntry(sim, "Simulation");
                    legend->AddEntry(simMod, "Modified Simulation");

                    simMod->Draw();
                    sim->Draw("same");
                    data->Draw("same");
                    legend->Draw();

                    double goodnessOfFit = data->Chi2Test(sim, "UW");
                    double goodnessOfFitMod = data->Chi2Test(simMod, "UW");

                    char annotation[100];
                    snprintf(annotation, sizeof(annotation), "P val sim:%lf\nmod sim:%lf", goodnessOfFit, goodnessOfFitMod); 
                    std::cout << annotation << std::endl;
                    // auto text = new TLatex(.1,.92,annotation);
                    // text->Draw();
                    cs->Write();
                }else{
                    cs->Divide(3,1);
                    cs->cd(1);
                    sim->Draw("colz");
                    cs->cd(2);
                    simMod->Draw("colz");
                    cs->cd(3);
                    data->Draw("colz");

                    cs->Write();
                }
            }
        }
    }
    #undef filename
    #undef outFile
}
