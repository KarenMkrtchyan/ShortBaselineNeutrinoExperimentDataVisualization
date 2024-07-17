void visualize() {
    //--------------CHANGE FILE NAMES--------------//
    //tracks, modified tracks, data
    //execute this file from the user directory (karenm)
    string files[] = {"15/Stage1_15_track/Supp_Stage1_15_track.root", 
    "15/Stage1_Modified_15_track/Supp_Stage1_Modified_15_track.root", 
    "15/data_run11971_EventBuilder9_art1_33_fstrmBNBMAJORITY_20240605T142228_20240606T040954-stage0_20240607T000132-stage1-3d8ced9a-efa9-44e4-bec4-96c02037b64f_hitTTree_track/Supp_data_run11971_EventBuilder9_art1_33_fstrmBNBMAJORITY_20240605T142228_20240606T040954-stage0_20240607T000132-stage1-3d8ced9a-efa9-44e4-bec4-96c02037b64f_hitTTree_track.root"};

    //------------CHANGE OUTPUT FILE NAME------------------//
    //RECREATE will overwrite the file, use this the first time executing
    //UPDATE will add on to the file, use this ever subsequent run 
    #define outFile "out.root"
    TFile *outputFile = new TFile(outFile, "RECREATE");
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
    int nbinsx = 150;
    int xlow = 0;
    int xup = 15;

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
        tree = (TTree*)dir->Get("pandoraTrackGausCryoE_MCstage1Var");
        if (!tree) {
            std::cerr << "Error: Cannot find the TTree" << std::endl;
            file->Close();
            return;
        }
       }else{ 
        tree = (TTree*)dir->Get("pandoraTrackGausCryoE_stage1");
        if (!tree) {
            std::cerr << "Error: Cannot find the TTree" << std::endl;
            file->Close();
            return;
        }
       }
       //change directory into output file 
    //    gSystem->cd(outFile);
       TFile *outputFileReal  = new TFile(outFile, "UPDATE"); //even Lisan Al Gaib doesn't know why this line is neccessary 
       for(int k =0; k<3; k++){ // Draw param 
           for (int i = 0; i < 2; i++) { // TPC
               for (int j = 0; j < 3; j++) { // Plane         
                   snprintf(formattedString, sizeof(formattedString), format, i, j); // for logical selection
                   snprintf(formattedString2, sizeof(formattedString2), formatTitle, i, j); // For title
                   std::string param = drawParam[k];
                   std::string drawCommand = param + ">>htemp(" + std::to_string(nbinsx) + "," + std::to_string(xlow) + "," + std::to_string(xup) + ")";
                   std::cout << "Drawing: " << drawCommand << " with selection: " << formattedString << std::endl; 
                   if(k==2){
                       drawCommand = "Amplitude:Width>>htemp(" + std::to_string(nbinsx) + "," + std::to_string(xlow) + "," + std::to_string(xup) + "," + std::to_string(nbinsx) + "," + std::to_string(xlow) + "," + std::to_string(xup) + ")";
                       tree->Draw(drawCommand.c_str(), formattedString, "colz");
                   }else{
                       tree->Draw(drawCommand.c_str(), formattedString);
                   }              
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
   #undef outFile
}
