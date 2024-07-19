#include <glob.h>
#include <vector>

std::string expandWildcard(const std::string& pattern) {
    glob_t glob_result;
    glob(pattern.c_str(), GLOB_TILDE, nullptr, &glob_result);
    std::string file;
    if (glob_result.gl_pathc > 0) {
        file = std::string(glob_result.gl_pathv[0]); // Use the first match
    }
    globfree(&glob_result);
    return file;
}

void visualize() {
    //add in every file you want visualized 
    std::vector<std::string> run{"10",  "16",  "235", "317", "504", "999", "12", "20", "245", "37", "550", "123", "201", "25", "390", "64", "140", "215", "282", "420", "732", "15", "231", "29", "450", "888"}; // Directory being visualized

    std::string haddCommand = "hadd -f combinedNotTrack.root";
    for(int m =0; m <run.size(); m++){
        const std::string outFile = "hist_" + run[m] + ".root"; 
        haddCommand+= " " + outFile;
        //tracks, modified tracks, data
        //execute this file from the user directory (karenm)
        std::string files[3];

         //TRACK 
        // files[0] = run[m] + "/Stage1_" + run[m] + "_track/Supp_Stage1_" + run[m] + "_track.root";
        // files[1] = run[m] + "/Stage1_Modified_" + run[m] + "_track/Supp_Stage1_Modified_" + run[m] + "_track.root";
        // files[2] = expandWildcard(run[m] + "/data_*_hitTTree_track/Supp_data_*.root");
        

        files[0] = run[m] + "/Stage0_" + run[m] + "/Supp_Stage0_" + run[m] + ".root";
        files[1] = run[m] + "/Stage0_Modified_" + run[m] + "/Supp_Stage0_Modified_" + run[m] + ".root";
        files[2] = expandWildcard(run[m] + "/data_*_hitTTree/Supp_data_*.root");
        
        // std::cout << files[2] << std::endl;
        //------------CHANGE OUTPUT FILE NAME------------------//
        //RECREATE will overwrite the file, use this the first time executing
        //UPDATE will add on to the file, use this ever subsequent run 
        
        TFile *outputFile = new TFile(outFile.c_str(), "RECREATE");
        if (!outputFile->IsOpen()) {
            std::cerr << "Error: Cannot create or open output file!" << std::endl;
            return;
        }
        outputFile->Close();
        
        // Automate the plotting of each plane and TPC
        const char *format = "TPC==%d && Plane==%d";
        const char *formatTitle = "TPC %d, Plane %d";
        char formattedString[100];
        char formattedString2[100];

        //needed for historgrams to merge together propely
        int nbinsx = 50;
        int xlow = 2;
        int xlowAmp = 5;
        int xup = 20; //for 2D hist
        int xupWidth = 8;
        int xupAmp = 40;

        // Disable statistics box globally
        gStyle->SetOptStat(0);
        string drawParam[]= {"Width", "Amplitude", "Amplitude:Width"};
        TCanvas *c1 = new TCanvas("c1", "Histograms", 800, 600); //needed so h can be properly selected later on

      
        for(int l =0; l<3; l++){ //File     
            TFile *file = new TFile(files[l].c_str(), "READ");
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

            //---------------CHANGE DIR---------------//
            // Dir names for different files are not the same
            //Unmodifed hits
            /*
            gaushit1dTPCEE_MCStage0Var
            gaushit1dTPCEW_MCStage0Var
            gaushit1dTPCWE_MCStage0Var
            gaushit1dTPCWW_MCStage0Var
            */
            //Modified hits
            /*
            pandoraTrackGausCryoE_MCstage1Var
            pandoraTrackGausCryoW_MCstage1Var
            */
            //Data file
            /*
            pandoraTrackGausCryoE_stage1
            pandoraTrackGausCryoW_stage1
            */
            TTree *tree;
            if(l==0 || l==1){
                tree = (TTree*)dir->Get("gaushit1dTPCEE_MCStage0Var");
                if (!tree) {
                    std::cerr << "Error: Cannot find the TTree" << std::endl;
                    file->Close();
                    return;
                }
            }else{ 
                tree = (TTree*)dir->Get("gaushit1dTPCEE_stage0");
                if (!tree) {
                    std::cerr << "Error: Cannot find the TTree" << std::endl;
                    file->Close();
                    return;
                }
            }

            TFile *outputFileReal  = new TFile(outFile.c_str(), "UPDATE"); //even Lisan Al Gaib doesn't know why this line is neccessary 
            for(int k =0; k<3; k++){ // Draw param 
                for (int i = 0; i < 2; i++) { // TPC
                    for (int j = 0; j < 3; j++) { // Plane         
                        snprintf(formattedString, sizeof(formattedString), format, i, j); // for logical selection
                        snprintf(formattedString2, sizeof(formattedString2), formatTitle, i, j); // For title
                        std::string param = drawParam[k];
                        std::string drawCommand;           
                        if(k==0){
                            drawCommand = param + ">>htemp(" + std::to_string(nbinsx) + "," + std::to_string(xlow) + "," + std::to_string(xupWidth) + ")";
                        }
                        else if(k==1){
                            drawCommand = param + ">>htemp(" + std::to_string(nbinsx) + "," + std::to_string(xlowAmp) + "," + std::to_string(xupAmp) + ")";
                        }
                        else{
                            drawCommand = "Amplitude:Width>>htemp(" + std::to_string(nbinsx) + "," + std::to_string(xlow) + "," + std::to_string(xup) + "," + std::to_string(nbinsx) + "," + std::to_string(xlowAmp) + "," + std::to_string(xupAmp) + ")";
                        }
                        std::cout << "Drawing: " << run[m] << " " << drawCommand << " with selection: " << formattedString << std::endl; 
                        tree->Draw(drawCommand.c_str(), formattedString);             
                        
                        TH1 *h = (TH1*)gDirectory->Get("htemp");
                        // Handle the case where h is NULL 
                        if (h) {
                            char name[100];
                            if(l==0){ // tracks
                                snprintf(name, sizeof(name), "%s_TPC%d_Plane%d", param.c_str(), i, j);
                            }
                            else if(l==1){ // modified tracks
                                snprintf(name, sizeof(name), "MOD_%s_TPC%d_Plane%d", param.c_str(), i, j);
                            }
                            else{ // data tracks
                                snprintf(name, sizeof(name), "DATA_%s_TPC%d_Plane%d", param.c_str(), i, j);
                            }
                            h->SetName(name);
                            h->SetTitle(formattedString2);
                            if(k==0){
                                h->GetXaxis()->SetTitle("Hit Width [ticks]");
                                h->GetYaxis()->SetTitle("Hits");
                            }else if(k==1){
                                h->GetXaxis()->SetTitle("Amplitude [ADC]");
                                h->GetYaxis()->SetTitle("Hits");
                            }else{
                                if(l==0){ // tracks
                                    snprintf(name, sizeof(name), "Amplitude_Width_TPC%d_Plane%d",  i, j);
                                }
                                else if(l==1){ // modified tracks
                                    snprintf(name, sizeof(name), "MOD_Amplitude_Width_TPC%d_Plane%d",  i, j);
                                }
                                else{ // data tracks
                                    // std::cout<<"Data time\n";
                                    snprintf(name, sizeof(name), "DATA_Amplitude_Width_TPC%d_Plane%d",  i, j);
                                }
                                h->SetName(name);
                                h->SetTitle(formattedString2);
                                h->GetXaxis()->SetTitle("Hit Width [ticks]");
                                h->GetYaxis()->SetTitle("Amplitude [ADC]"); 
                            }                      
                        h->GetYaxis()->SetTitleOffset(1.3);
                        // std::string newName = std::string(name)+".pdf";
                        // c1->SaveAs(newName.c_str()); //prints our plots as pdfs
                        h->Write();
                    } else {
                        std::cerr << "Warning: Histogram for TPC " << i << " Plane " << j << " not created (empty selection)." << std::endl;
                    }
                }
            }
        }
        file->Close();
        outputFileReal->Close();
        }
    }
    std::cout << haddCommand << std::endl;
    gSystem->Exec(haddCommand.c_str());
}
