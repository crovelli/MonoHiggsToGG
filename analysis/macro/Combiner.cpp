#include "Combiner.hh"
#include <iostream>
#include <fstream>

Combiner::Combiner( SamplePairVec Samples, const Double_t inLumi, const ColorMap colorMap, const TString outdir, const Bool_t doNmin1, const Bool_t do_stack, const TString type, const Bool_t doQCDrescale){

  if (doNmin1) addText = "_n-1";
  else addText="";

  doStack = false;
  if (do_stack) doStack = true;

  doQCDscale = doQCDrescale;

  fType = type;
  lumi	= inLumi;
  fOutDir = outdir;
  TString fOut = "comb";

  MakeOutDirectory(Form("%s%s",fOutDir.Data(),fOut.Data()));
  fOutFile = new TFile(Form("%s%s/combplots%s.root",fOutDir.Data(),fOut.Data(),addText.Data()),"RECREATE");
  CheckValidFile(fOutFile, Form("%s%s/combplots%s.root",fOutDir.Data(),fOut.Data(),addText.Data())); 

  for (SamplePairVecIter iter = Samples.begin(); iter != Samples.end(); ++iter){
    if ( (*iter).second == 1 ) {fBkgNames.push_back((*iter).first);}      // background
    else if ( (*iter).second == 0 ) {fSigNames.push_back((*iter).first);} // signal
    else {fDataNames.push_back((*iter).first);}			          // data
  }
  
  fNData = fDataNames.size();
  fNBkg  = fBkgNames.size();
  fNSig  = fSigNames.size();
 
  std::cout << "DataSize = " << fNData << " BkgSize = " << fNBkg << " SigSize = " << fNSig << std::endl;

  Combiner::InitTH1DNames();
  fNTH1D = fTH1DNames.size(); 

  // define colorMap and title
  fColorMap = colorMap;
  
  fSampleTitleMap["DoubleEG"]		= "Data";
  fSampleTitleMap["QCD"] 		= "QCD";
  fSampleTitleMap["GJets"]		= "#gamma + Jets";
  fSampleTitleMap["VH"]			= "V + H";
  fSampleTitleMap["DYJetsToLL"]		= "Drell-Yan";
  fSampleTitleMap["GluGluHToGG"]	= "H #rightarrow #gamma#gamma (ggH)";
  fSampleTitleMap["DiPhoton"]		= "#gamma + #gamma";
  fSampleTitleMap["ttHJetToGG"]		= "tt + H #rightarrow #gamma#gamma";
  fSampleTitleMap["VBFHToGG"]		= "VBF H #rightarrow #gamma#gamma";
  //fSampleTitleMap["DMHtoGG_M1"]		= "m_{#chi} = 1 GeV";//#bar{#chi}#chi HH ,m_{#chi} = 1 GeV";
  //fSampleTitleMap["DMHtoGG_M10"]	= "m_{#chi} = 10 GeV";//#bar{#chi}#chi HH ,m_{#chi} = 10 GeV";
  //fSampleTitleMap["DMHtoGG_M100"]	= "m_{#chi} = 100 GeV";//#bar{#chi}#chi HH ,m_{#chi} = 100 GeV";
  //fSampleTitleMap["DMHtoGG_M1000"]	= "m_{#chi} = 1000 GeV";//#bar{#chi}#chi HH ,m_{#chi} = 1000 GeV";
  fSampleTitleMap["FakeData"]		= "FakeData";  
  fSampleTitleMap["FakeDataII"]		= "Test";
  fSampleTitleMap["2HDM_mZP600"]	= "m_{Z'} = 600 GeV";//#bar{#chi}#chi HH ,m_{#chi} = 1 GeV";
  fSampleTitleMap["2HDM_mZP800"]	= "m_{Z'} = 800 GeV";//#bar{#chi}#chi HH ,m_{#chi} = 1 GeV";
  fSampleTitleMap["2HDM_mZP1000"]	= "m_{Z'} = 1000 GeV";//#bar{#chi}#chi HH ,m_{#chi} = 1 GeV";
  fSampleTitleMap["2HDM_mZP1200"]	= "m_{Z'} = 1200 GeV";//#bar{#chi}#chi HH ,m_{#chi} = 10 GeV";
  fSampleTitleMap["2HDM_mZP1400"]	= "m_{Z'} = 1400 GeV";//#bar{#chi}#chi HH ,m_{#chi} = 10 GeV";
  fSampleTitleMap["2HDM_mZP1700"]	= "m_{Z'} = 1700 GeV";//#bar{#chi}#chi HH ,m_{#chi} = 100 GeV";
  //fSampleTitleMap["2HDM_mZP2000"]	= "m_{Z'} = 2000 GeV";//#bar{#chi}#chi HH ,m_{#chi} = 1000 GeV";
  fSampleTitleMap["2HDM_mZP2500"]	= "m_{Z'} = 2500 GeV";//#bar{#chi}#chi HH ,m_{#chi} = 1000 GeV";

  //for (std::map<TString,TString>::iterator iter = fSampleTitleMap.begin(); iter != fSampleTitleMap.end(); ++iter) {
  //  std::cout << (*iter).first << "  " << (*iter).second << std::endl;
  //}

  Combiner::InitCanvAndHists();

}// end Combiner::Combiner

Combiner::~Combiner(){
  std::cout << "Finished & Deleting" << std::endl;

 // delete all pointers
  for (UInt_t th1d = 0; th1d < fNTH1D; th1d++){
    //if (fNData > 0 ){
      delete fOutDataTH1DHists[th1d];
      delete fOutTH1DRatioPads[th1d];
      delete fOutTH1DRatioLines[th1d];
    //}
    delete GJetsClone[th1d];
    delete fOutBkgTH1DHists[th1d];
    delete fOutBkgTH1DStacks[th1d];
    delete fOutBkgTH1DStacksForUncer[th1d];
    delete fTH1DLegends[th1d];
    delete fOutTH1DStackPads[th1d];
    delete fOutTH1DCanvases[th1d];
    
    for (UInt_t data = 0; data < fNData; data++) { delete fInDataTH1DHists[th1d][data]; }
    for (UInt_t mc = 0; mc < fNBkg; mc++) { delete fInBkgTH1DHists[th1d][mc]; }
    for (UInt_t mc = 0; mc < fNSig; mc++) { delete fInSigTH1DHists[th1d][mc]; }
  }

  for (UInt_t data = 0; data < fNData; data++) { delete fDataFiles[data]; }
  for (UInt_t mc = 0; mc < fNBkg; mc++) { delete fBkgFiles[mc]; }
  for (UInt_t mc = 0; mc < fNSig; mc++) { delete fSigFiles[mc]; }

  delete fOutFile;

}// end Combiner::~Combiner

void Combiner::DoComb(){
  // copy th1d plots into output hists/stacks
  for (UInt_t th1d = 0; th1d < fNTH1D; th1d++){

    // data : copy first histogram & add all others too it 
    //if (fNData > 0){
      for (UInt_t data = 0; data < fNData; data++){
        if (data == 0) fOutDataTH1DHists[th1d] = (TH1D*)fInDataTH1DHists[th1d][data]->Clone(); 
        else fOutDataTH1DHists[th1d]->Add(fInDataTH1DHists[th1d][data]);
      }
    //}// end if ndata>0

    
    // Because QCD has some events with very large weights
    // Copy the GJets histo, weight it by the QCD integral 
    // and use it for QCD distribution instead.
    Double_t qcd_integral = 0;
    Double_t gjets_integral = 0;
    Double_t qcd_integral_new = 0;
    for (UInt_t mc = 0; mc < fNBkg; mc++){
      if (fBkgNames[mc] == "QCD") qcd_integral = fInBkgTH1DHists[th1d][mc]->Integral();  
      if (fBkgNames[mc] == "GJets"){
         GJetsClone[th1d] = (TH1D*)fInBkgTH1DHists[th1d][mc]->Clone();
         GJetsClone[th1d]->SetFillColor(fColorMap["QCD"]);
         gjets_integral = GJetsClone[th1d]->Integral();
         if (gjets_integral > 0) GJetsClone[th1d]->Scale(qcd_integral/gjets_integral);
      }
    }
    qcd_integral_new = GJetsClone[th1d]->Integral();
    //if (qcd_integral_new != qcd_integral) std::cout << "New QCD is NOT the same: old = " << qcd_integral << " new = " << qcd_integral_new << " diff = " << qcd_integral_new-qcd_integral << std::endl;
    //std::cout << "QCD_Integral     = " << qcd_integral << std::endl;
    //std::cout << "QCD_Integral_New = " << qcd_integral_new << std::endl;

    // bkg : copy histos and add to stacks
    for (UInt_t mc = 0; mc < fNBkg; mc++){
      //fInBkgTH1DHists[th1d][mc]->Scale(lumi);
      if (doQCDscale && fBkgNames[mc] == "QCD"){
        fOutBkgTH1DStacks[th1d]->Add(GJetsClone[th1d]);
        fOutBkgTH1DStacksForUncer[th1d]->Add(GJetsClone[th1d]);
      }
      else{
        fOutBkgTH1DStacks[th1d]->Add(fInBkgTH1DHists[th1d][mc]);
        fOutBkgTH1DStacksForUncer[th1d]->Add(fInBkgTH1DHists[th1d][mc]);
      }
      // draw bkg in legend as box for stack plots, and line for overlay plot
      //if (doStack) fTH1DLegends[th1d]->AddEntry(fInBkgTH1DHists[th1d][mc],fSampleTitleMap[fBkgNames[mc]],"f");
      //else fTH1DLegends[th1d]->AddEntry(fInBkgTH1DHists[th1d][mc],fSampleTitleMap[fBkgNames[mc]],"l");
      if (mc == 0){
        fOutBkgTH1DHists[th1d] = (TH1D*)fInBkgTH1DHists[th1d][mc]->Clone();
      }
      else if (doQCDscale && fBkgNames[mc]=="QCD"){
        fOutBkgTH1DHists[th1d]->Add(GJetsClone[th1d]);
      }
      else{
        fOutBkgTH1DHists[th1d]->Add(fInBkgTH1DHists[th1d][mc]);
      }
    }
    if (th1d==fIndexMgg){
      for (UInt_t mc = 0; mc < fNBkg; mc++){// print out values of integrals
        if (addText!="_n-1"){
          std::cout << fBkgNames[mc] << " in " << fTH1DNames[th1d] << " = " << fInBkgTH1DHists[th1d][mc]->Integral() 
          << " %: " << fInBkgTH1DHists[th1d][mc]->Integral()/fOutBkgTH1DHists[th1d]->Integral() << std::endl;
        }
      }
    } 
    if (th1d==fIndexNvtx && addText!="_n-1"){
      std::cout << "DoubleEG" << " in " << fTH1DNames[th1d] << " = " << fOutDataTH1DHists[th1d]->Integral()  << std::endl;
      for (UInt_t mc = 0; mc < fNBkg; mc++){// print out values of integrals
        std::cout << fBkgNames[mc] << " in " << fTH1DNames[th1d] << " = " << fInBkgTH1DHists[th1d][mc]->Integral()  << std::endl;
      }
      for (UInt_t mc = 0; mc < fNSig; mc++){// print out values of integrals
        std::cout << fSigNames[mc] << " in " << fTH1DNames[th1d] << " = " << fInSigTH1DHists[th1d][mc]->Integral()  << std::endl;
      }
    } 
    //fOutBkgTH1DHists[th1d]->Sumw2();
    //std::cout << "histo# " << th1d << std::endl;
    //std::cout << "maxbin " << fOutBkgTH1DHists[th1d]->GetSize() << std::endl;
    //for (UInt_t bin = 1; bin < fOutBkgTH1DHists[th1d]->GetSize(); bin++){
    //  std::cout << "bin " << bin << " Val = " << fOutBkgTH1DHists[th1d]->GetBinContent(bin) << " Err = " << fOutBkgTH1DHists[th1d]->GetBinError(bin) << std::endl;
    //}
    fOutBkgTH1DHists[th1d]->SetFillColor(kGray+3);
    fOutBkgTH1DHists[th1d]->SetFillStyle(3003);
    fOutBkgTH1DHists[th1d]->SetMarkerSize(0);

    // LEGEND 
    // for sig and bkg 
    UInt_t fNMaxMC = 0;
    if (fNSig <= fNBkg) fNMaxMC = fNBkg;
    else fNMaxMC = fNSig;
    for (UInt_t mc = 0; mc < fNMaxMC; mc++){
      if (mc < fNBkg){
        if (doStack) fTH1DLegends[th1d]->AddEntry(fInBkgTH1DHists[th1d][mc],fSampleTitleMap[fBkgNames[mc]],"f");
        else fTH1DLegends[th1d]->AddEntry(fInBkgTH1DHists[th1d][mc],fSampleTitleMap[fBkgNames[mc]],"l");
      }
      if (mc < fNSig) fTH1DLegends[th1d]->AddEntry(fInSigTH1DHists[th1d][mc],fSampleTitleMap[fSigNames[mc]],"l");
    } 
    // add uncertainty in stack plot
    fOutBkgTH1DStacksForUncer[th1d]->Add(fOutBkgTH1DHists[th1d],"E2");
    if (doStack) fTH1DLegends[th1d]->AddEntry(fOutBkgTH1DHists[th1d],"Bkg Uncertainty","F");
    // add data to legend if int > 0
    Double_t dataInt = fOutDataTH1DHists[th1d]->Integral();
    if (fNData > 0 && doStack && dataInt > 0) fTH1DLegends[th1d]->AddEntry(fOutDataTH1DHists[th1d],"Data","pl");


    //// sig: just add to legend
    //for (UInt_t mc = 0; mc < fNSig; mc++){
    //  //fInSigTH1DHists[th1d][mc]->Scale(lumi);
    //  fTH1DLegends[th1d]->AddEntry(fInSigTH1DHists[th1d][mc],fSampleTitleMap[fSigNames[mc]],"l");
    //}
    //// bkg : copy histos and add to stacks
    //for (UInt_t mc = 0; mc < fNBkg; mc++){
    //  // draw bkg in legend as box for stack plots, and line for overlay plot
    //  if (doStack) fTH1DLegends[th1d]->AddEntry(fInBkgTH1DHists[th1d][mc],fSampleTitleMap[fBkgNames[mc]],"f");
    //  else fTH1DLegends[th1d]->AddEntry(fInBkgTH1DHists[th1d][mc],fSampleTitleMap[fBkgNames[mc]],"l");
    //}



  }// end loop over th1d histos

  //if (addText!="_n-1") Combiner::MakeEffPlots();
  if (addText!="_n-1") Combiner::FindMETEfficiencies();
  Combiner::MakeOutputCanvas();

}// end Combiner::DoComb

void Combiner::FindMETEfficiencies(){
  // Finds the efficiency for each sample (integral of events passing 80 GeV cut over all selected events)
  // Integrals are computed also including the overflow bin
  // Also plots for each sample the MET shape for the different corrections to visualize the effect

  fNMETPlots = 15;
  UInt_t fNMETCat = fNMETPlots*2;

  DblVecVec fSigMETEff;
  DblVecVec fBkgMETEff;
  DblVec    fDataMETEff;
  fSigMETEff.resize(fNSig);
  fBkgMETEff.resize(fNBkg);

  DblVecVec fSigMET;
  DblVecVec fBkgMET;
  DblVec    fDataMET;
  fSigMET.resize(fNSig);
  fBkgMET.resize(fNBkg);
   

  for (UInt_t mc = 0; mc < fNSig; mc++){
    fSigMETEff[mc].resize(fNMETCat); 
    fSigMET[mc].resize(fNMETCat); 
    for (UInt_t th1d = fIndexMET; th1d < (fIndexMET+fNMETPlots); th1d++){
      UInt_t maxbin = fInSigTH1DHists[th1d][mc]->GetSize(); 
      UInt_t minbin = fInSigTH1DHists[th1d][mc]->GetXaxis()->FindBin(80.0);
      UInt_t zerbin = fInSigTH1DHists[th1d][mc]->GetXaxis()->FindBin(0.0);
      UInt_t effbin1 = th1d-fIndexMET;
      UInt_t effbin2 = th1d-fIndexMET+fNMETPlots;
      fSigMETEff[mc][th1d-fIndexMET+fNMETPlots] = fInSigTH1DHists[th1d][mc]->Integral(minbin,maxbin);// events above 80 GeV
      fSigMETEff[mc][th1d-fIndexMET] = fInSigTH1DHists[th1d][mc]->Integral(zerbin,maxbin);// all events 
      if (fSigMETEff[mc][th1d-fIndexMET] > 0) fSigMET[mc][th1d-fIndexMET] = (fSigMETEff[mc][th1d-fIndexMET+fNMETPlots]/fSigMETEff[mc][th1d-fIndexMET]);
      else fSigMET[mc][th1d-fIndexMET]=0; 
    }
  }  
  for (UInt_t mc = 0; mc < fNBkg; mc++){
    fBkgMETEff[mc].resize(fNMETCat); 
    fBkgMET[mc].resize(fNMETCat); 
    for (UInt_t th1d = fIndexMET; th1d < (fIndexMET+fNMETPlots); th1d++){
      UInt_t maxbin = fInBkgTH1DHists[th1d][mc]->GetSize(); 
      UInt_t minbin = fInBkgTH1DHists[th1d][mc]->GetXaxis()->FindBin(80.0);
      UInt_t effbin1 = th1d-fIndexMET;
      UInt_t effbin2 = th1d-fIndexMET+fNMETPlots;
      fBkgMETEff[mc][th1d-fIndexMET+fNMETPlots] = fInBkgTH1DHists[th1d][mc]->Integral(minbin,maxbin);// events above 80 GeV
      fBkgMETEff[mc][th1d-fIndexMET] = fInBkgTH1DHists[th1d][mc]->Integral();// all events
      if (fBkgMETEff[mc][th1d-fIndexMET] > 0) fBkgMET[mc][th1d-fIndexMET] = fBkgMETEff[mc][th1d-fIndexMET+fNMETPlots]/fBkgMETEff[mc][th1d-fIndexMET];
      else fBkgMET[mc][th1d-fIndexMET]=0; 
    }
  }  
  fDataMETEff.resize(fNMETCat);
  fDataMET.resize(fNMETCat);
  for (UInt_t th1d = fIndexMET; th1d < (fIndexMET+fNMETPlots); th1d++){
    UInt_t maxbin = fOutDataTH1DHists[th1d]->GetSize(); 
    UInt_t minbin = fOutDataTH1DHists[th1d]->GetXaxis()->FindBin(80.0);
    UInt_t effbin1 = th1d-fIndexMET;
    UInt_t effbin2 = th1d-fIndexMET+fNMETPlots;
    fDataMETEff[th1d-fIndexMET+fNMETPlots] = fOutDataTH1DHists[th1d]->Integral(minbin,maxbin);// events above 80 GeV
    fDataMETEff[th1d-fIndexMET] = fOutDataTH1DHists[th1d]->Integral();// all events
    if (fDataMETEff[th1d-fIndexMET] > 0) fDataMET[th1d-fIndexMET] = fDataMETEff[th1d-fIndexMET+fNMETPlots]/fDataMETEff[th1d-fIndexMET];
    else fDataMET[th1d-fIndexMET]=0; 
  }

  SystMET.push_back("Original");
  SystMET.push_back("JetEnUp");
  SystMET.push_back("JetEnDown");
  SystMET.push_back("JetResUp");
  SystMET.push_back("JetResDown");
  SystMET.push_back("MuonEnUp");
  SystMET.push_back("MuonEnDown");
  SystMET.push_back("EleEnUp");
  SystMET.push_back("EleEnDown");
  SystMET.push_back("TauEnUp");
  SystMET.push_back("TauEnDown");
  SystMET.push_back("PhoEnUp");
  SystMET.push_back("PhoEnDown");
  SystMET.push_back("JetUnclEnUp");
  SystMET.push_back("JetUnclEnDown");
 
  fSystMETTitleMap["Original"]		= "Original";  
  fSystMETTitleMap["JetEnUp"] 		= "Jet Energy Up"; 
  fSystMETTitleMap["JetEnDown"]		= "Jet Energy Down"; 
  fSystMETTitleMap["JetResUp"] 		= "Jet Res. Up"; 
  fSystMETTitleMap["JetResDown"]	= "Jet Res. Down"; 
  fSystMETTitleMap["MuonEnUp"]		= "Muon Energy Up"; 
  fSystMETTitleMap["MuonEnDown"]	= "Muon Energy Down"; 
  fSystMETTitleMap["EleEnUp"]		= "Electron Energy Up"; 
  fSystMETTitleMap["EleEnDown"]		= "Electron Energy Down"; 
  fSystMETTitleMap["TauEnUp"]		= "Tau Energy Up"; 
  fSystMETTitleMap["TauEnDown"]		= "Tau Energy Down"; 
  fSystMETTitleMap["PhoEnUp"]		= "Photon Energy Up"; 
  fSystMETTitleMap["PhoEnDown"] 	= "Photon Energy Down"; 
  fSystMETTitleMap["JetUnclEnUp"]	= "Unclust. Jet Energy Up"; 
  fSystMETTitleMap["JetUnclEnDown"] 	= "Unclust. Jet Energy Down"; 


  //std::cout << " ---------------------------------------------------------------- " << std::endl;
  //std::cout << " ---------------------------------------------------------------- " << std::endl;
  //std::cout << "MET Efficiencies for Systematics " << std::endl; 
  //std::cout << " ---------------------------------------------------------------- " << std::endl;
  //std::cout << " ---------------------------------------------------------------- " << std::endl;
  ////for (UInt_t i=0; i < fNMETPlots; i++){
  ////  std::cout << " Data		: " << SystMET[i] << "		= " << fDataMETEff[i+fNMETPlots] << " / " << fDataMETEff[i] << "	= " << fDataMET[i] << std::endl;
  ////}
  ////std::cout << " ---------------------------------------------------------------- " << std::endl;
  //for (UInt_t mc=0; mc < fNSig; mc++){
  //  for (UInt_t i=0; i < fNMETPlots; i++){
  //    std::cout << fSigNames[mc] << " 	: " << SystMET[i] << "		= " << fSigMETEff[mc][i+fNMETPlots] << " / " << fSigMETEff[mc][i] << "	= " << fSigMET[mc][i] << std::endl;
  //  }
  //  std::cout << " ---------------------------------------------------------------- " << std::endl;
  //}
  //for (UInt_t mc=0; mc < fNBkg; mc++){
  //  for (UInt_t i=0; i < fNMETPlots; i++){
  //    std::cout << fBkgNames[mc] << " 	: " << SystMET[i] << "		= " << fBkgMETEff[mc][i+fNMETPlots] << " / " << fBkgMETEff[mc][i] << "	= " << fBkgMET[mc][i] << std::endl;
  //  }
  //  std::cout << " ---------------------------------------------------------------- " << std::endl;
  //}
  //std::cout << " ---------------------------------------------------------------- " << std::endl;

  // write as a table for Latex
  std::ofstream	fOutTableTxtFile;
  fOutTableTxtFile.open(Form("%s/ResultsSystematicsForLatex.tex",fOutDir.Data()));

  TStrMap fSampleTitleMap;
  fSampleTitleMap["QCD"] 		= "QCD";
  fSampleTitleMap["GJets"]		= "$\\gamma$ + Jets";
  fSampleTitleMap["VH"]			= "V + H";
  fSampleTitleMap["DYJetsToLL"]		= "Drell-Yan";
  fSampleTitleMap["GluGluHToGG"]	= "$H \\rightarrow \\gamma \\gamma$ (ggH)";
  fSampleTitleMap["DiPhoton"]		= "$\\gamma\\gamma$";
  fSampleTitleMap["ttHJetToGG"]		= "tt $+ H \\rightarrow \\gamma\\gamma$";
  fSampleTitleMap["VBFHToGG"]		= "VBF $H \\rightarrow \\gamma\\gamma$";
  fSampleTitleMap["2HDM_mZP600"]	= "2HDM, $m_{Z'} = 600 GeV, m_{A0} = 300 GeV$";
  fSampleTitleMap["2HDM_mZP800"]	= "2HDM, $m_{Z'} = 800 GeV, m_{A0} = 300 GeV$";
  fSampleTitleMap["2HDM_mZP1000"]	= "2HDM, $m_{Z'} = 1000 GeV, m_{A0} = 300 GeV$";
  fSampleTitleMap["2HDM_mZP1200"]	= "2HDM, $m_{Z'} = 1200 GeV, m_{A0} = 300 GeV$";
  fSampleTitleMap["2HDM_mZP1400"]	= "2HDM, $m_{Z'} = 1400 GeV, m_{A0} = 300 GeV$";
  fSampleTitleMap["2HDM_mZP1700"]	= "2HDM, $m_{Z'} = 1700 GeV, m_{A0} = 300 GeV$";
  fSampleTitleMap["2HDM_mZP2000"]	= "2HDM, $m_{Z'} = 2000 GeV, m_{A0} = 300 GeV$";
  fSampleTitleMap["2HDM_mZP2500"]	= "2HDM, $m_{Z'} = 2500 GeV, m_{A0} = 300 GeV$";

  // find the biggest & smallest efficiencies
  // data
  Double_t maxData = 0;
  Double_t minData = 100; 
  for (UInt_t i=0; i < fNMETPlots; i++){
    if (maxData < fDataMET[i]) maxData = fDataMET[i];
    if (minData > fDataMET[i]) minData = fDataMET[i];
  }
  // sig
  DblVec   maxSig, minSig;
  maxSig.resize(fNSig);
  minSig.resize(fNSig);
  for (UInt_t mc=0; mc < fNSig; mc++){
    maxSig[mc]=0;
    minSig[mc]=100;
    for (UInt_t i=0; i < fNMETPlots; i++){
      if (maxSig[mc] <= fSigMET[mc][i]) maxSig[mc] = fSigMET[mc][i];
      if (minSig[mc] >= fSigMET[mc][i]) minSig[mc] = fSigMET[mc][i]; 
    } 
  } 
  // bkg
  DblVec   maxBkg, minBkg;
  maxBkg.resize(fNBkg);
  minBkg.resize(fNBkg); 
  for (UInt_t mc=0; mc < fNBkg; mc++){
    maxBkg[mc]=0;
    minBkg[mc]=100;
    for (UInt_t i=0; i < fNMETPlots; i++){
      if (maxBkg[mc] <= fBkgMET[mc][i]) maxBkg[mc] = fBkgMET[mc][i];
      if (minBkg[mc] >= fBkgMET[mc][i]) minBkg[mc] = fBkgMET[mc][i]; 
    } 
  } 


  if (fOutTableTxtFile.is_open()){
    //setup Latex doc
    fOutTableTxtFile << "\\documentclass[a4paper,landscape]{article}" << std::endl;
    fOutTableTxtFile << "\\usepackage[paperheight=15.0in,paperwidth=6.0in,margin=1.0in,headheight=0.0in,footskip=0.5in,includehead,includefoot]{geometry}" << std::endl;
    fOutTableTxtFile << "\\begin{document}" << std::endl;

    // ==========================================================
    // start summary of results table
    fOutTableTxtFile << "\% Summary of MET Systematics" << std::endl; 
    fOutTableTxtFile << "\\begin{table}[bthp]" <<std::endl;
    fOutTableTxtFile << "\\begin{tabular}{|l|c|c|}" <<std::endl;
    fOutTableTxtFile << "\\hline \\hline" <<std::endl;
    fOutTableTxtFile << "\\multicolumn{3}{|c|}{" << Form("$\\sqrt{s}$ = 13 TeV; L = %1.1f $fb^{-1}$",lumi) <<"} \\\\" <<std::endl;
    fOutTableTxtFile << "\\hline \\hline" << std::endl;
    fOutTableTxtFile << "Sample & Efficiency & Efficiency Range \\\\" << std::endl;
    fOutTableTxtFile << "\\hline" <<std::endl;
    for (UInt_t mc = 0; mc < fNSig; mc++){
      fOutTableTxtFile << fSampleTitleMap[fSigNames[mc]] << " & " << Form("%1.4f",fSigMET[mc][0]) << "& -" << Form("%1.4f",fSigMET[mc][0]-minSig[mc]) << ", +" << Form("%1.4f",maxSig[mc]-fSigMET[mc][0]) << " \\\\" << std::endl; 
    }
    fOutTableTxtFile << "\\hline" <<std::endl;
    for (UInt_t mc = 0; mc < fNBkg; mc++){
      fOutTableTxtFile << fSampleTitleMap[fBkgNames[mc]] << " & " << Form("%1.4f",fBkgMET[mc][0]) << "& -" << Form("%1.4f",fBkgMET[mc][0]-minBkg[mc]) << ", +" << Form("%1.4f",maxBkg[mc]-fBkgMET[mc][0]) << " \\\\" << std::endl; 
    }
    fOutTableTxtFile << "Data & " << Form("%1.4f",fDataMET[0]) << "& -" << Form("%1.4f",fDataMET[0]-minData) << ", +" << Form("%1.4f",maxData-fDataMET[0]) << " \\\\" << std::endl; 
    // end table
    fOutTableTxtFile << "\\hline \\hline" <<std::endl;
    fOutTableTxtFile << "\\end{tabular}" <<std::endl;
    fOutTableTxtFile << "\\end{table}" <<std::endl;
    // ==========================================================

    // ==========================================================
    // start list of full systematics table
    fOutTableTxtFile << "\% Full MET Systematics" << std::endl; 

    // signal
    for (UInt_t mc=0; mc < fNSig; mc++){
      fOutTableTxtFile << "\\begin{table}[bthp]" <<std::endl;
      fOutTableTxtFile << "\\begin{tabular}{|l|c|}" <<std::endl;
      fOutTableTxtFile << "\\hline \\hline" <<std::endl;
      fOutTableTxtFile << "\\multicolumn{2}{|c|}{" << fSampleTitleMap[fSigNames[mc]] << "} \\\\" << std::endl;
      fOutTableTxtFile << "\\multicolumn{2}{|c|}{" << Form("$\\sqrt{s}$ = 13 TeV; L = %1.1f $fb^{-1}$",lumi) <<"} \\\\" <<std::endl;
      fOutTableTxtFile << "\\hline" <<std::endl;
      fOutTableTxtFile << "Type1 PF MET Version & Efficiency \\\\" << std::endl;
      for (UInt_t i=0; i < fNMETPlots; i++){ 
        fOutTableTxtFile << fSystMETTitleMap[SystMET[i]] << " & " << Form("%1.4f",fSigMET[mc][i])  <<  " \\\\" << std::endl;
        if (i==0) fOutTableTxtFile << "\\hline" <<std::endl;
      }
      fOutTableTxtFile << "\\hline" << std::endl;
      fOutTableTxtFile << "\\end{tabular}" <<std::endl;
      fOutTableTxtFile << "\\end{table}" <<std::endl;
    }

    // background
    for (UInt_t mc=0; mc < fNBkg; mc++){
      fOutTableTxtFile << "\\begin{table}[bthp]" <<std::endl;
      fOutTableTxtFile << "\\begin{tabular}{|l|c|}" <<std::endl;
      fOutTableTxtFile << "\\hline \\hline" <<std::endl;
      fOutTableTxtFile << "\\multicolumn{2}{|c|}{" << fSampleTitleMap[fBkgNames[mc]] << "} \\\\" << std::endl;
      fOutTableTxtFile << "\\multicolumn{2}{|c|}{" << Form("$\\sqrt{s}$ = 13 TeV; L = %1.1f $fb^{-1}$",lumi) <<"} \\\\" <<std::endl;
      fOutTableTxtFile << "\\hline" <<std::endl;
      fOutTableTxtFile << "Type1 PF MET Version & Efficiency \\\\" << std::endl;
      for (UInt_t i=0; i < fNMETPlots; i++){ 
        fOutTableTxtFile << fSystMETTitleMap[SystMET[i]] << " & " << Form("%1.4f",fBkgMET[mc][i])  <<  " \\\\" << std::endl;
        if (i==0) fOutTableTxtFile << "\\hline" <<std::endl;
      }
      fOutTableTxtFile << "\\hline" << std::endl;
      fOutTableTxtFile << "\\end{tabular}" <<std::endl;
      fOutTableTxtFile << "\\end{table}" <<std::endl;
    }

    // data
    fOutTableTxtFile << "\\begin{table}[bthp]" <<std::endl;
    fOutTableTxtFile << "\\begin{tabular}{|l|c|}" <<std::endl;
    fOutTableTxtFile << "\\hline \\hline" <<std::endl;
    fOutTableTxtFile << "\\multicolumn{2}{|c|}{Data} \\\\" << std::endl;
    fOutTableTxtFile << "\\multicolumn{2}{|c|}{" << Form("$\\sqrt{s}$ = 13 TeV; L = %1.1f $fb^{-1}$",lumi) <<"} \\\\" <<std::endl;
    fOutTableTxtFile << "Type1 PF MET Version & Efficiency \\\\" << std::endl;
    for (UInt_t i=0; i < fNMETPlots; i++){ 
      fOutTableTxtFile << fSystMETTitleMap[SystMET[i]] << " & " << Form("%1.4f",fDataMET[i])  <<  " \\\\" << std::endl;
      if (i==0) fOutTableTxtFile << "\\hline" <<std::endl;
    }
    fOutTableTxtFile << "\\hline" << std::endl;
    fOutTableTxtFile << "\\end{tabular}" <<std::endl;
    fOutTableTxtFile << "\\end{table}" <<std::endl;


    // ==========================================================
    // end Latex doc
    fOutTableTxtFile << "\\end{document}" <<std::endl;
    std::cout << "Writing ResultsTable in " << Form("%s/ResultsTableForLatex.tex",fOutDir.Data()) << std::endl;
  }
  else std::cout << "File didn't Open" << std::endl;

  // close output text files
  fOutTableTxtFile.close();

  Combiner::MakeMETEffPlots();
}// end Combiner::FindMETEfficiencies


void Combiner::MakeMETEffPlots(){

  // Make comparison plots of MET shapes after the various corrections
  // go into the output file
  fOutFile->cd();

  // setup color map for different MET shapes
  fColorMapMETEff["Original"]		= kBlack;
  fColorMapMETEff["JetEnUp"]		= kRed;
  fColorMapMETEff["JetEnDown"]		= kMagenta;
  fColorMapMETEff["JetResUp"]		= kOrange-3;
  fColorMapMETEff["JetResDown"]		= kOrange-3;
  fColorMapMETEff["MuonEnUp"]		= kTeal-1;
  fColorMapMETEff["MuonEnDown"]		= kTeal-1;
  fColorMapMETEff["EleEnUp"]		= kYellow;
  fColorMapMETEff["EleEnDown"]		= kYellow;
  fColorMapMETEff["TauEnUp"]		= kRed;
  fColorMapMETEff["TauEnDown"]		= kRed;
  fColorMapMETEff["PhoEnUp"]		= kGreen;
  fColorMapMETEff["PhoEnDown"]		= kTeal;
  fColorMapMETEff["JetUnclEnUp"]	= kBlue;
  fColorMapMETEff["JetUnclEnDown"]	= kBlue;

  // setup plot legend for signal
  std::vector<TLegend* > fSigMETEffLegend;
  fSigMETEffLegend.resize(fNSig*2);
  for (UInt_t mc = 0; mc < fNSig*2; mc++){
    fSigMETEffLegend[mc] = new TLegend(0.32,0.7,0.9,0.934); // (x1,y1,x2,y2)
    fSigMETEffLegend[mc]->SetNColumns(2);
    fSigMETEffLegend[mc]->SetBorderSize(4);
    fSigMETEffLegend[mc]->SetLineColor(kBlack);
    fSigMETEffLegend[mc]->SetTextSize(0.03);//0.035
    fSigMETEffLegend[mc]->SetLineWidth(2);
  }
  // setup plot legend for background 
  std::vector<TLegend* > fBkgMETEffLegend;
  fBkgMETEffLegend.resize(fNBkg*2);
  for (UInt_t mc = 0; mc < fNBkg*2; mc++){
    fBkgMETEffLegend[mc] = new TLegend(0.32,0.7,0.9,0.934); // (x1,y1,x2,y2)
    fBkgMETEffLegend[mc]->SetNColumns(2);
    fBkgMETEffLegend[mc]->SetBorderSize(4);
    fBkgMETEffLegend[mc]->SetLineColor(kBlack);
    fBkgMETEffLegend[mc]->SetTextSize(0.03);//0.035
    fBkgMETEffLegend[mc]->SetLineWidth(2);
  }

  // copy the plots and normalize them
  fOutSigMETEffTH1DHists.resize(fNMETPlots);
  fOutBkgMETEffTH1DHists.resize(fNMETPlots);
  for (UInt_t th1d = 0; th1d < fNMETPlots; th1d++){
    // sig
    fOutSigMETEffTH1DHists[th1d].resize(fNSig);
    for (UInt_t mc = 0; mc < fNSig; mc++){
      fOutSigMETEffTH1DHists[th1d][mc] = (TH1D*) fInSigTH1DHists[th1d+fIndexMET][mc]->Clone();
      //if (mc==0 || mc==1) fOutSigMETEffTH1DHists[th1d][mc]->GetXaxis()->SetMaximum(500);
      //else fOutSigMETEffTH1DHists[th1d][mc]->GetXaxis()->SetMaximum(900);
      if (fOutSigMETEffTH1DHists[th1d][mc]->Integral() > 0){
        fOutSigMETEffTH1DHists[th1d][mc]->Scale(1.0/fOutSigMETEffTH1DHists[th1d][mc]->Integral());
      } 
      fOutSigMETEffTH1DHists[th1d][mc]->SetFillColor(0);
      fOutSigMETEffTH1DHists[th1d][mc]->SetLineColor(fColorMapMETEff[SystMET[th1d]]);
      fOutSigMETEffTH1DHists[th1d][mc]->SetTitle(fSigNames[mc]);
      fOutSigMETEffTH1DHists[th1d][mc]->GetYaxis()->SetTitle("");
      if (th1d!=0) fSigMETEffLegend[mc]->AddEntry(fOutSigMETEffTH1DHists[th1d][mc],SystMET[th1d],"l");
      if (th1d==1 || th1d==2 || th1d==11 || th1d==12) fSigMETEffLegend[mc+fNSig]->AddEntry(fOutSigMETEffTH1DHists[th1d][mc],SystMET[th1d],"l");
    }
    // bkg
    fOutBkgMETEffTH1DHists[th1d].resize(fNBkg);
    for (UInt_t mc = 0; mc < fNBkg; mc++){
      fOutBkgMETEffTH1DHists[th1d][mc] = (TH1D*) fInBkgTH1DHists[th1d+fIndexMET][mc]->Clone();
      //fOutBkgMETEffTH1DHists[th1d][mc]->GetXaxis()->SetMaximum(350);
      fOutBkgMETEffTH1DHists[th1d][mc]->GetXaxis()->SetRangeUser(10,100);
      if (fOutBkgMETEffTH1DHists[th1d][mc]->Integral() > 0){
        fOutBkgMETEffTH1DHists[th1d][mc]->Scale(1.0/fOutBkgMETEffTH1DHists[th1d][mc]->Integral());
      } 
      fOutBkgMETEffTH1DHists[th1d][mc]->SetFillColor(0);
      fOutBkgMETEffTH1DHists[th1d][mc]->SetLineColor(fColorMapMETEff[SystMET[th1d]]);
      fOutBkgMETEffTH1DHists[th1d][mc]->SetTitle(fBkgNames[mc]);
      fOutBkgMETEffTH1DHists[th1d][mc]->GetYaxis()->SetTitle("");
      if (th1d!=0) fBkgMETEffLegend[mc]->AddEntry(fOutBkgMETEffTH1DHists[th1d][mc],SystMET[th1d],"l");
      if (th1d==1 || th1d==2 || th1d==11 || th1d==12) fBkgMETEffLegend[mc+fNBkg]->AddEntry(fOutBkgMETEffTH1DHists[th1d][mc],SystMET[th1d],"l");
    }
  }
  // add original MET distribution last in legend
  for (UInt_t mc = 0; mc < fNSig*2; mc++){
    if (mc < fNSig) fSigMETEffLegend[mc]->AddEntry(fOutSigMETEffTH1DHists[0][mc],SystMET[0],"l"); 
    else fSigMETEffLegend[mc]->AddEntry(fOutSigMETEffTH1DHists[0][mc-fNSig],SystMET[0],"l"); 
  }  
  for (UInt_t mc = 0; mc < fNBkg*2; mc++){
    if (mc < fNBkg) fBkgMETEffLegend[mc]->AddEntry(fOutBkgMETEffTH1DHists[0][mc],SystMET[0],"l"); 
    else fBkgMETEffLegend[mc]->AddEntry(fOutBkgMETEffTH1DHists[0][mc-fNBkg],SystMET[0],"l"); 
  }  

  // Make signal plots
  TCanvVec fOutSigMETEffCanvas;
  fOutSigMETEffCanvas.resize(fNSig*2); 
  std::vector<TPad* > fOutSigMETEffPad;
  fOutSigMETEffPad.resize(fNSig*2); 

  for (UInt_t mc = 0; mc < fNSig*2; mc++){
    gStyle->SetOptStat(0);
    // setup canvases
    if (mc < fNSig) fOutSigMETEffCanvas[mc] = new TCanvas(fSigNames[mc].Data(),"");
    else fOutSigMETEffCanvas[mc] = new TCanvas(Form("%s_v2",fSigNames[mc-fNSig].Data()),"");
    if (fOutSigMETEffCanvas[mc] == (TCanvas*)"NULL") std::cout << "CANVAS IS NULL" << std::endl;
    fOutSigMETEffCanvas[mc]->cd();
    fOutSigMETEffCanvas[mc]->Draw();
    // setup pads
    fOutSigMETEffPad[mc] = new TPad("","",0.01,0.2,0.99,1.);
    fOutSigMETEffPad[mc]->SetBottomMargin(0);
    fOutSigMETEffPad[mc]->SetRightMargin(0.06); 
    fOutSigMETEffPad[mc]->SetLeftMargin(0.12); 
    fOutSigMETEffPad[mc]->Draw(); 
    fOutSigMETEffPad[mc]->cd(); 

    Double_t maxval = 1; 
    if (mc < fNSig) maxval = fOutSigMETEffTH1DHists[0][mc]->GetMaximum();
    else maxval = fOutSigMETEffTH1DHists[0][mc-fNSig]->GetMaximum();
    // plots with all met shapes
    if (mc < fNSig){
      for (UInt_t th1d = 0; th1d < fNMETPlots; th1d++){
        if (th1d == 0){
          fOutSigMETEffTH1DHists[th1d][mc]->SetMaximum(maxval*10);
          fOutSigMETEffTH1DHists[th1d][mc]->SetMinimum(1E-3);
          fOutSigMETEffTH1DHists[th1d][mc]->Draw("HIST"); 
          if (mc == 0) fOutSigMETEffTH1DHists[th1d][mc]->GetXaxis()->SetRangeUser(0,400); 
          if (mc == 1) fOutSigMETEffTH1DHists[th1d][mc]->GetXaxis()->SetRangeUser(0,500); 
        }
        else fOutSigMETEffTH1DHists[th1d][mc]->Draw("HIST SAME");
      }
      fOutSigMETEffTH1DHists[0][mc]->SetLineWidth(2); 
      fOutSigMETEffTH1DHists[0][mc]->Draw("HIST SAME"); 
    }
    // these have the reduced plots
    else {
      for (UInt_t th1d = 0; th1d < fNMETPlots; th1d++){
        if (th1d == 0){
          fOutSigMETEffTH1DHists[th1d][mc-fNSig]->SetMaximum(maxval);
          fOutSigMETEffTH1DHists[th1d][mc-fNSig]->SetMinimum(1E-3);
          fOutSigMETEffTH1DHists[th1d][mc-fNSig]->Draw("HIST"); 
        }
        else if (th1d == 1 || th1d == 2 || th1d == 11 || th1d == 12 ) fOutSigMETEffTH1DHists[th1d][mc-fNSig]->Draw("HIST SAME");
      }
      fOutSigMETEffTH1DHists[0][mc-fNSig]->SetLineWidth(2); 
      fOutSigMETEffTH1DHists[0][mc-fNSig]->Draw("HIST SAME"); 
    }

    fSigMETEffLegend[mc]->Draw("SAME");
    // make right format for output plots & save them
    fOutSigMETEffPad[mc]->SetLogy(1); 
    CMSLumi(fOutSigMETEffCanvas[mc],11,lumi);
    if (mc < fNSig) fOutSigMETEffCanvas[mc]->SaveAs(Form("%scomb/METEff_%s.%s",fOutDir.Data(),fSigNames[mc].Data(),fType.Data()));
    else fOutSigMETEffCanvas[mc]->SaveAs(Form("%scomb/METEff_%s_Reduced.%s",fOutDir.Data(),fSigNames[mc-fNSig].Data(),fType.Data()));
    fOutFile->cd();
    if (mc < fNSig) fOutSigMETEffCanvas[mc]->Write(Form("METEff_%s",fSigNames[mc].Data()));
    else fOutSigMETEffCanvas[mc]->Write(Form("METEff_%s_Reduced",fSigNames[mc-fNSig].Data()));

    delete fSigMETEffLegend[mc];
    delete fOutSigMETEffPad[mc];
    delete fOutSigMETEffCanvas[mc];
  }
  for (UInt_t mc = 0; mc < fNSig; mc++){
    for (UInt_t th1d = 0; th1d < fNMETPlots; th1d++){ 
      delete fOutSigMETEffTH1DHists[th1d][mc]; 
    }
  }
  
  TCanvVec fOutBkgMETEffCanvas;
  fOutBkgMETEffCanvas.resize(fNBkg*2); 
  std::vector<TPad* > fOutBkgMETEffPad;
  fOutBkgMETEffPad.resize(fNBkg*2); 

  for (UInt_t mc = 0; mc < fNBkg*2; mc++){
    gStyle->SetOptStat(0);
    // setup canvases
    if (mc < fNBkg) fOutBkgMETEffCanvas[mc] = new TCanvas(fBkgNames[mc].Data(),"");
    else fOutBkgMETEffCanvas[mc] = new TCanvas(Form("%s_v2",fBkgNames[mc-fNBkg].Data()),"");
    if (fOutBkgMETEffCanvas[mc] == (TCanvas*)"NULL") std::cout << "CANVAS IS NULL" << std::endl;
    fOutBkgMETEffCanvas[mc]->cd();
    fOutBkgMETEffCanvas[mc]->Draw();
    // setup pads
    fOutBkgMETEffPad[mc] = new TPad("","",0.01,0.2,0.99,1.);
    fOutBkgMETEffPad[mc]->SetBottomMargin(0);
    fOutBkgMETEffPad[mc]->SetRightMargin(0.06); 
    fOutBkgMETEffPad[mc]->SetLeftMargin(0.12); 
    fOutBkgMETEffPad[mc]->Draw(); 
    fOutBkgMETEffPad[mc]->cd(); 

    Double_t maxval = 1;
    if (mc < fNBkg) maxval = fOutBkgMETEffTH1DHists[0][mc]->GetMaximum();
    else maxval = fOutBkgMETEffTH1DHists[0][mc-fNBkg]->GetMaximum();

    // plots with all met shapes
    if (mc < fNBkg){
      for (UInt_t th1d = 0; th1d < fNMETPlots; th1d++){
        if (th1d == 0){
          fOutBkgMETEffTH1DHists[th1d][mc]->SetMaximum(maxval*10);
          fOutBkgMETEffTH1DHists[th1d][mc]->SetMinimum(1E-3);
          fOutBkgMETEffTH1DHists[th1d][mc]->Draw("HIST"); 
        }
        else fOutBkgMETEffTH1DHists[th1d][mc]->Draw("HIST SAME");
        fOutBkgMETEffTH1DHists[th1d][mc]->GetXaxis()->SetRangeUser(0,350); 
      }
      fOutBkgMETEffTH1DHists[0][mc]->SetLineWidth(2); 
      fOutBkgMETEffTH1DHists[0][mc]->Draw("HIST SAME"); 
    }
    // these have the reduced plots
    else {
      for (UInt_t th1d = 0; th1d < fNMETPlots; th1d++){
        if (th1d == 0){ 
          fOutBkgMETEffTH1DHists[th1d][mc-fNBkg]->SetMaximum(maxval);
          fOutBkgMETEffTH1DHists[th1d][mc-fNBkg]->SetMinimum(1E-3);
          fOutBkgMETEffTH1DHists[th1d][mc-fNBkg]->Draw("HIST"); 
        }
        else if (th1d == 1 || th1d == 2 || th1d == 11 || th1d == 12 ) fOutBkgMETEffTH1DHists[th1d][mc-fNBkg]->Draw("HIST SAME");
      }
      fOutBkgMETEffTH1DHists[0][mc-fNBkg]->SetLineWidth(2); 
      fOutBkgMETEffTH1DHists[0][mc-fNBkg]->Draw("HIST SAME"); 
    }

    fBkgMETEffLegend[mc]->Draw("SAME");
    // make right format for output plots & save them
    fOutBkgMETEffPad[mc]->SetLogy(1); 
    CMSLumi(fOutBkgMETEffCanvas[mc],11,lumi);
    if (mc < fNBkg) fOutBkgMETEffCanvas[mc]->SaveAs(Form("%scomb/METEff_%s.%s",fOutDir.Data(),fBkgNames[mc].Data(),fType.Data()));
    else fOutBkgMETEffCanvas[mc]->SaveAs(Form("%scomb/METEff_%s_Reduced.%s",fOutDir.Data(),fBkgNames[mc-fNBkg].Data(),fType.Data()));
    fOutFile->cd();
    if (mc < fNBkg) fOutBkgMETEffCanvas[mc]->Write(Form("METEff_%s",fBkgNames[mc].Data()));
    else fOutBkgMETEffCanvas[mc]->Write(Form("METEff_%s_Reduced",fBkgNames[mc-fNBkg].Data()));

    delete fBkgMETEffLegend[mc];
    delete fOutBkgMETEffPad[mc];
    delete fOutBkgMETEffCanvas[mc];
  }

  for (UInt_t mc = 0; mc < fNBkg; mc++){
    for (UInt_t th1d = 0; th1d < fNMETPlots; th1d++){ 
      if (mc < fNBkg) delete fOutBkgMETEffTH1DHists[th1d][mc]; 
    }
  }

}// end Combiner::MakeMETEffPlots



void Combiner::MakeEffPlots(){

  fOutFile->cd();
  TCanvas *c = new TCanvas();
  c->cd();

  TH1D *eff_mDM = new TH1D("eff_mDM","",1000,500,1500);

  Double_t eff_val = 0.;
  Double_t eff_num = 0.;
  Double_t eff_den = 0.;
  Double_t num_err = 0.;
  Double_t den_err = 0.;
  Double_t eff_err = 0.;
  std::vector<Double_t> numer = {15634, 37582, 41561, 21185, 22538};
  std::vector<Double_t> denom = {33854, 70376, 72970, 36018, 37448};
  //std::vector<Double_t> numer = {4864,6780,10451,18428};
  //std::vector<Double_t> denom = {40918,57500,55092,56656};
  std::vector<Double_t> value = {0,0,0,0};
  std::vector<Double_t> error = {0,0,0,0};
  std::vector<Double_t> mass =  {600,800,1000,1200,1400};//{1,10,100,1000}; 
  Int_t binForMass = 0;

  for (UInt_t mc = 0; mc < fNSig; mc++){
    //eff_num = fInSigTH1DHists[fIndexEff][mc]->GetBinContent(8); // events passing sel,mgg,met
    //eff_den = fInSigTH1DHists[fIndexEff][mc]->GetBinContent(1);  // events only require pass presel
    //num_err = TMath::Sqrt(eff_num);
    //den_err = TMath::Sqrt(eff_den);
    //if (eff_den > 0) eff_val = eff_num/eff_den;
    //eff_err = TMath::Sqrt(eff_val*(1.0-eff_val)/eff_den); 
    binForMass = eff_mDM->FindBin(mass[mc]);
    eff_mDM->SetBinContent(binForMass,eff_val);
    eff_mDM->SetBinError(binForMass,eff_err);
    value[mc]  = numer[mc]/denom[mc];
    error[mc]  = TMath::Sqrt(value[mc]*(1.0-value[mc])/denom[mc]);
    eff_mDM->SetBinContent(binForMass,value[mc]);
    eff_mDM->SetBinError(binForMass,error[mc]);
  }

  eff_mDM->SetMaximum(1.0);
  eff_mDM->SetMinimum(0.0);
  eff_mDM->GetXaxis()->SetTitle("m_{Z'} [GeV]");
  eff_mDM->GetYaxis()->SetTitle("Efficiency");
  eff_mDM->Draw("PE");
  eff_mDM->Write();
  
  CMSLumi(c,11,lumi);
  c->SetLogx();
  c->SaveAs(Form("%scomb/eff_mDM.%s",fOutDir.Data(),fType.Data()));
  delete c; 

  //// make significance plots for ptgg, absdphi, deta
  //UInt_t num_sel = 20;
  //TH1D *signif_ptgg = new TH1D("signif_ptgg","",num_sel,-0.5,19.5);
  //TH1D *signif_dphi = new TH1D("signif_dphi","",num_sel,-0.5,19.5);
  //TH1D *signif_deta = new TH1D("signif_deta","",num_sel,-0.5,19.5);

  //DblVecVec signal_ptgg, signal_dphi, signal_deta;
  //DblVec    totbkg_ptgg, totbkg_dphi, totbkg_deta;
  //signal_ptgg.resize(fNSig);
  //signal_dphi.resize(fNSig);
  //signal_deta.resize(fNSig);
  //
  //for (UInt_t mc = 0; mc < fNSig; mc++){
  //  signal_ptgg[mc].resize(num_sel); 
  //  signal_dphi[mc].resize(num_sel); 
  //  signal_deta[mc].resize(num_sel); 
  //  for (UInt_t cut = 0; cut < num_sel; cut++){
  //     //fInSigTH1DHists[fIndexPtgg][mc]->Integral();  
  //     //fInSigTH1DHists[fIndexDphi][mc]->Integral();  
  //     //fInSigTH1DHists[fIndexDeta][mc]->Integral();  
  //  }    
  //} 

  





}// end Combiner::MakeEffPlots


void Combiner::MakeOutputCanvas(){
  for (UInt_t th1d = 0; th1d < fNTH1D; th1d++){

/*
     for (UInt_t mc = 0; mc < fNBkg; mc++){
      std::cout << th1d << " before " << fInBkgTH1DHists[th1d][mc]->GetMaximum() << std::endl;
      fInBkgTH1DHists[th1d][mc]->Scale(lumi);
    }
    if (fNData > 0) fOutDataTH1DHists[th1d]->Scale(lumi);
*/
    Bool_t isLogY = false;
    if (doStack){// do Stack plots
      Combiner::DrawCanvasStack(th1d,isLogY);
      isLogY = true;
      Combiner::DrawCanvasStack(th1d,isLogY);
      if (fNData > 0) fOutTH1DRatioPads[th1d]->Clear(); //delete the ratio plot for overlay plots
    }
    else{// do overlay next 
      Combiner::DrawCanvasOverlay(th1d,isLogY);
      isLogY = true;
      Combiner::DrawCanvasOverlay(th1d,isLogY);
    }
  }
}// end Combiner::MakeOutputCanvas


void Combiner::DrawCanvasOverlay(const UInt_t th1d, const Bool_t isLogY){
  gStyle->SetOptStat(0);
  fOutTH1DCanvases[th1d]->cd();
  fOutTH1DStackPads[th1d]->Draw();
  fOutTH1DStackPads[th1d]->cd();
   
  //if (fOutDataTH1DHists[th1d]->Integral() > 0) fOutDataTH1DHists[th1d]->Scale(1.0/fOutDataTH1DHists[th1d]->Integral());

  for (UInt_t mc = 0; mc < fNSig; mc++){
    if (fInSigTH1DHists[th1d][mc]->Integral() > 0){
      fInSigTH1DHists[th1d][mc]->Scale(1.0/fInSigTH1DHists[th1d][mc]->Integral());
    }
  }
  for (UInt_t mc = 0; mc < fNBkg; mc++){
    if (fInBkgTH1DHists[th1d][mc]->Integral() > 0 ){
      fInBkgTH1DHists[th1d][mc]->Scale(1.0/fInBkgTH1DHists[th1d][mc]->Integral());
    }
    fInBkgTH1DHists[th1d][mc]->SetFillColor(0);
    fInBkgTH1DHists[th1d][mc]->SetLineColor(fColorMap[fBkgNames[mc]]);
  }

  Double_t maxOverlay = -100;
  maxOverlay = Combiner::GetMaximum(th1d, false);
  //Double_t minOverlay = 1e9;
  //minOverlay = Combiner::GetMinimum(th1d, false);  

  // start by drawing the sig first
  if (isLogY) fInSigTH1DHists[th1d][0]->SetMaximum(maxOverlay*1E3);
  else fInSigTH1DHists[th1d][0]->SetMaximum(maxOverlay*1.1);

  //fInSigTH1DHists[th1d][0]->SetMinimum(0.0);
  //if (fNData > 0) fInSigTH1DHists[th1d][0]->SetMinimum(minOverlay*0.9);
  //if (th1d==fIndexMgg){ 
  //  //fInSigTH1DHists[th1d][0]->SetMinimum(0.001); 
  //  fInSigTH1DHists[th1d][0]->SetMaximum(10);
  //}

  fInSigTH1DHists[th1d][0]->SetTitle("");
  fInSigTH1DHists[th1d][0]->GetYaxis()->SetTitle("");
  fInSigTH1DHists[th1d][0]->Draw("hist");

  for (UInt_t mc = 0; mc < fNBkg; mc++){
    fInBkgTH1DHists[th1d][mc]->Draw("HIST SAME");
  }
  for (UInt_t mc = 0; mc < fNSig; mc++){
    fInSigTH1DHists[th1d][mc]->Draw("HIST SAME");
  }
  //if (fNData > 0) fOutDataTH1DHists[th1d]->Draw("PE SAME");

  fTH1DLegends[th1d]->Draw("SAME"); 

  TString suffix = "";
  if (isLogY) suffix="_log";

  fOutTH1DStackPads[th1d]->SetLogy(isLogY);
  fOutTH1DCanvases[th1d]->cd();

  CMSLumi(fOutTH1DCanvases[th1d],11,lumi);

  fOutTH1DCanvases[th1d]->SaveAs(Form("%scomb/%s_comb%s%s.%s",fOutDir.Data(),fTH1DNames[th1d].Data(),addText.Data(),suffix.Data(),fType.Data()));  
  fOutFile->cd();
  fOutTH1DCanvases[th1d]->Write(Form("%s%s_comb%s",fTH1DNames[th1d].Data(),suffix.Data(),addText.Data()));


}// end Combiner::DrawCanvasOverlay

void Combiner::DrawCanvasStack(const UInt_t th1d, const Bool_t isLogY){
  gStyle->SetOptStat(0);
  fOutTH1DCanvases[th1d]->cd();
  fOutTH1DStackPads[th1d]->Draw();
  fOutTH1DStackPads[th1d]->cd();





 /* for (UInt_t mc = 0; mc < fNSig; mc++){
    fInSigTH1DHists[th1d][mc]->Scale(lumi);
  }
  for (UInt_t data = 0; data < fNData; data++){
    //std::cout << fDataNames[data].Data() << std::endl;
//    if (Form("%s",fDataNames[data].Data()) == "FakeData"){
      if ( fDataNames[data].Data() == "FakeData" ) std::cout << "worked!" << std::endl;
      else std::cout << "hahahaha"<< std::endl;
      fOutDataTH1DHists[th1d]->Scale(lumi);
   // }
  }*/

  Double_t maxval = -100;
  maxval = Combiner::GetMaximum(th1d, true);
  //Double_t minval = 1E20;
  //minval = Combiner::GetMinimum(th1d, true);

  // start by drawing the sig first
  fInSigTH1DHists[th1d][0]->SetTitle("");
  fInSigTH1DHists[th1d][0]->GetXaxis()->SetTitleOffset(999);
  fInSigTH1DHists[th1d][0]->GetXaxis()->SetLabelSize(0);
  if (isLogY){
    fInSigTH1DHists[th1d][0]->SetMaximum(maxval*1E3);
    fInSigTH1DHists[th1d][0]->SetMinimum(1E-3);
  }
  else {
    fInSigTH1DHists[th1d][0]->SetMaximum(maxval*1.5);
    fInSigTH1DHists[th1d][0]->SetMinimum(0);
  }
  fInSigTH1DHists[th1d][0]->Draw("HIST");

  fOutBkgTH1DStacks[th1d]->Draw("HIST SAME");
  //fOutBkgTH1DHists[th1d]->Draw("E2 SAME");//E2 draws error as rectangle
  //fOutBkgTH1DStacksForUncer[th1d]->Draw("nostack E2 SAME");

  // check that the blinding does not completely cover the plot
  // this could be problematic in the future
  Double_t dataInt = fOutDataTH1DHists[th1d]->Integral();
  if (fNData > 0 && dataInt > 0){
    fOutDataTH1DHists[th1d]->Draw("PE SAME");
  }

  for (UInt_t mc = 0; mc < fNSig; mc++){
    fInSigTH1DHists[th1d][mc]->Draw("HIST SAME");
    if ( mc == fNSig-1 ){ // on last draw redraw axis because they are overwritten by stack
      fInSigTH1DHists[th1d][mc]->Draw("AXIS SAME");
    }
  }

  //fInSigTH1DHists[th1d][0]->Draw("AXIS SAME");

  fOutBkgTH1DHists[th1d]->Draw("E2 SAME");//E2 draws error as rectangle
  fTH1DLegends[th1d]->Draw("SAME"); 

  TString suffix = "";
  if (isLogY) suffix="_log";

  fOutTH1DStackPads[th1d]->SetLogy(isLogY);

  fOutTH1DCanvases[th1d]->cd();

  if (fNData > 0 && dataInt > 0){ // make & draw ratio plots
    Combiner::MakeRatioPlots();
    Combiner::MakeRatioLine(th1d);

    fOutTH1DRatioPads[th1d]->Draw();
    fOutTH1DRatioPads[th1d]->cd();

    fOutRatioTH1DHists[th1d]->Draw("EP"); // draw first so line can appear
    fOutTH1DRatioLines[th1d]->Draw("SAME");

    fOutRatioTH1DHists[th1d]->GetYaxis()->SetNdivisions(505);

    fOutRatioTH1DHists[th1d]->GetXaxis()->SetLabelSize(0.11);
    fOutRatioTH1DHists[th1d]->GetXaxis()->SetTitleSize(0.15);
    fOutRatioTH1DHists[th1d]->GetXaxis()->SetTickSize(0.11);

    fOutRatioTH1DHists[th1d]->GetYaxis()->SetLabelSize(0.11);
    fOutRatioTH1DHists[th1d]->GetYaxis()->SetTitleSize(0.15);
    fOutRatioTH1DHists[th1d]->GetYaxis()->SetTitleOffset(0.35);

    fOutRatioTH1DHistsCopy[th1d]->Draw("E2 SAME");
    fOutRatioTH1DHists[th1d]->Draw("EP SAME");
  } 

  fOutTH1DCanvases[th1d]->Update();

  CMSLumi(fOutTH1DCanvases[th1d],11,lumi);

  fOutTH1DCanvases[th1d]->SaveAs(Form("%scomb/%s_stack%s%s.%s",fOutDir.Data(),fTH1DNames[th1d].Data(),addText.Data(),suffix.Data(),fType.Data()));  
  fOutFile->cd();
  fOutTH1DCanvases[th1d]->Write(Form("%s%s_stack%s",fTH1DNames[th1d].Data(),suffix.Data(),addText.Data()));

  /*
  for (UInt_t mc = 0; mc < fNSig; mc++){
    fInSigTH1DHists[th1d][mc]->Scale(1/lumi);
  }
  for (UInt_t data = 0; data < fNData; data++){
    //std::cout << fDataNames[data].Data() << std::endl;
//    if (Form("%s",fDataNames[data].Data()) == "FakeData"){
      if ( fDataNames[data].Data() == "FakeData" ) std::cout << "worked!" << std::endl;
      else std::cout << "hahahaha"<< std::endl;
      fOutDataTH1DHists[th1d]->Scale(1/lumi);
   // }
  }
  */

}// end Combiner::DrawCanvasStack

void Combiner::MakeRatioLine(const UInt_t th1d){
  // have line held at ratio of 1.0 over whole x range
  fOutTH1DRatioLines[th1d]->SetX1(fOutRatioTH1DHists[th1d]->GetXaxis()->GetXmin());
  fOutTH1DRatioLines[th1d]->SetY1(1.0);
  fOutTH1DRatioLines[th1d]->SetX2(fOutRatioTH1DHists[th1d]->GetXaxis()->GetXmax());
  fOutTH1DRatioLines[th1d]->SetY2(1.0);

  // customize appearance
  fOutTH1DRatioLines[th1d]->SetLineColor(kRed);
  fOutTH1DRatioLines[th1d]->SetLineWidth(2);

}// end Combiner::MakeRatioLine

void Combiner::MakeRatioPlots(){

Double_t bkg = 0.;
Double_t dat = 0.;
Double_t bkgErr = 0.;
Double_t datErr = 0.;
Double_t ratErr = 0.;

  for (UInt_t th1d = 0; th1d < fNTH1D; th1d++){ // double hists
    if (fNData > 0) fOutRatioTH1DHists[th1d] = (TH1D*)fOutDataTH1DHists[th1d]->Clone();
    else fOutRatioTH1DHists[th1d] = (TH1D*)fOutBkgTH1DHists[th1d]->Clone();

    fOutRatioTH1DHists[th1d]->SetTitle("");
    fOutRatioTH1DHists[th1d]->Divide(fOutBkgTH1DHists[th1d]);  

    fOutRatioTH1DHistsCopy[th1d] = (TH1D*)fOutRatioTH1DHists[th1d]->Clone();

    fOutRatioTH1DHists[th1d]->SetLineColor(kBlack);
    fOutRatioTH1DHists[th1d]->SetMinimum(-0.1);  // Define Y ..
    fOutRatioTH1DHists[th1d]->SetMaximum(2.1);   // .. range
    fOutRatioTH1DHists[th1d]->SetStats(0);       // No statistics on lower plot
    fOutRatioTH1DHists[th1d]->GetYaxis()->SetTitle("Data/MC");
    for (UInt_t bin=0; bin<=fOutRatioTH1DHists[th1d]->GetNbinsX();bin++){
      if (fOutBkgTH1DHists[th1d]->GetBinContent(bin)){
        bkg = fOutBkgTH1DHists[th1d]->GetBinContent(bin);
        dat = fOutDataTH1DHists[th1d]->GetBinContent(bin);
        bkgErr = fOutBkgTH1DHists[th1d]->GetBinError(bin);
        datErr = fOutDataTH1DHists[th1d]->GetBinError(bin);
        ratErr = TMath::Sqrt(TMath::Power(datErr/bkg,2)+TMath::Power(dat*bkgErr/(bkg*bkg),2)); 
      }
      else ratErr = 0.;
      //std::cout << "RatioErr1 = " << ratErr << std::endl;
      fOutRatioTH1DHists[th1d]->SetBinError(bin,ratErr);
    }
    for (UInt_t bin=0; bin<=fOutRatioTH1DHistsCopy[th1d]->GetNbinsX();bin++){
      if (fOutBkgTH1DHists[th1d]->GetBinContent(bin)){
        bkg = fOutBkgTH1DHists[th1d]->GetBinContent(bin);
        dat = fOutDataTH1DHists[th1d]->GetBinContent(bin);
        bkgErr = fOutBkgTH1DHists[th1d]->GetBinError(bin);
        datErr = fOutDataTH1DHists[th1d]->GetBinError(bin);
        ratErr = bkgErr/bkg; 
      }
      else ratErr = 0.;
      //std::cout << "RatioErr2 = " << ratErr << std::endl;
      fOutRatioTH1DHistsCopy[th1d]->SetFillColor(kGray+3);
      fOutRatioTH1DHistsCopy[th1d]->SetFillStyle(3003);
      fOutRatioTH1DHistsCopy[th1d]->SetMarkerSize(0);
      fOutRatioTH1DHistsCopy[th1d]->SetBinContent(bin,1.0);
      fOutRatioTH1DHistsCopy[th1d]->SetBinError(bin,ratErr);
    }
  }// end loop over hists

}// end Combiner::MakeRatioPlots

Double_t Combiner::GetMaximum(const UInt_t th1d, const Bool_t stack) {
  Double_t max = -100;

  std::vector<Double_t> tmpmax;
  for (UInt_t mc = 0; mc < fNSig; mc++){
    tmpmax.push_back( fInSigTH1DHists[th1d][mc]->GetBinContent(fInSigTH1DHists[th1d][mc]->GetMaximumBin()));
  }
  if (fNData > 0 && stack) tmpmax.push_back(fOutDataTH1DHists[th1d]->GetBinContent(fOutDataTH1DHists[th1d]->GetMaximumBin()));
  if (stack) tmpmax.push_back(fOutBkgTH1DStacks[th1d]->GetMaximum());
  else{
    for (UInt_t mc = 0; mc < fNBkg; mc++){
      tmpmax.push_back(fInBkgTH1DHists[th1d][mc]->GetBinContent(fInBkgTH1DHists[th1d][mc]->GetMaximumBin()));
    }
  }

  for (UInt_t i = 0; i < tmpmax.size(); i++){
    if ( tmpmax[i] > max ) max = tmpmax[i];
  }

  return max;
}// end Combiner::GetMaximum

Double_t Combiner::GetMinimum(const UInt_t th1d, const Bool_t stack) {
  // need to loop through to check bin != 0
  Double_t datamin  = 1e20;

  if (fNData > 0){ 
    for (Int_t bin = 1; bin <= fOutDataTH1DHists[th1d]->GetNbinsX(); bin++){
      Float_t tmpmin = fOutDataTH1DHists[th1d]->GetBinContent(bin);
      if ((tmpmin < datamin) && (tmpmin > 0)) {
        datamin    = tmpmin;
      }
    } 
  }
  return datamin;
}// end Combiner::GetMinimum

void Combiner::InitCanvAndHists(){
  // open input files into TFileVec for data
  fDataFiles.resize(fNData);
  for (UInt_t data = 0; data < fNData; data++) {
    TString datafile = Form("%s%s/plots_%s.root",fOutDir.Data(),fDataNames[data].Data(),fDataNames[data].Data());
    fDataFiles[data] = TFile::Open(datafile.Data());
    CheckValidFile(fDataFiles[data],datafile);
  }

  // open input files into TFileVec for bkg
  fBkgFiles.resize(fNBkg);
  for (UInt_t mc = 0; mc < fNBkg; mc++) {
    TString bkgfile = Form("%s%s/plots_%s.root",fOutDir.Data(),fBkgNames[mc].Data(),fBkgNames[mc].Data());
    fBkgFiles[mc] = TFile::Open(bkgfile.Data());
    CheckValidFile(fBkgFiles[mc],bkgfile);
  }

  // open input files into TFileVec for bkg
  fSigFiles.resize(fNSig);
  for (UInt_t mc = 0; mc < fNSig; mc++) {
    TString sigfile = Form("%s%s/plots_%s.root",fOutDir.Data(),fSigNames[mc].Data(),fSigNames[mc].Data());
    fSigFiles[mc] = TFile::Open(sigfile.Data());
    CheckValidFile(fSigFiles[mc],sigfile);
  }

  fInDataTH1DHists.resize(fNTH1D);
  fInBkgTH1DHists.resize(fNTH1D);
  fInSigTH1DHists.resize(fNTH1D);

  for (UInt_t th1d = 0; th1d < fNTH1D; th1d++){ // loop over double hists
    fInDataTH1DHists[th1d].resize(fNData); 
    for (UInt_t data = 0; data < fNData; data++) { // init data double hists
      fInDataTH1DHists[th1d][data] = (TH1D*)fDataFiles[data]->Get(Form("%s%s",fTH1DNames[th1d].Data(),addText.Data()));
      CheckValidTH1D(fInDataTH1DHists[th1d][data],fTH1DNames[th1d],fDataFiles[data]->GetName());
    }
    fInBkgTH1DHists[th1d].resize(fNBkg); 
    for (UInt_t mc = 0; mc < fNBkg; mc++) { // init bkg double hists
      fInBkgTH1DHists[th1d][mc] = (TH1D*)fBkgFiles[mc]->Get(Form("%s%s",fTH1DNames[th1d].Data(),addText.Data()));
      CheckValidTH1D(fInBkgTH1DHists[th1d][mc],fTH1DNames[th1d],fBkgFiles[mc]->GetName());
      fInBkgTH1DHists[th1d][mc]->SetFillColor(fColorMap[fBkgNames[mc]]);
      fInBkgTH1DHists[th1d][mc]->SetLineColor(kBlack);
    }
    fInSigTH1DHists[th1d].resize(fNSig); 
    for (UInt_t mc = 0; mc < fNSig; mc++) { // init sig double hists
      fInSigTH1DHists[th1d][mc] = (TH1D*)fSigFiles[mc]->Get(Form("%s%s",fTH1DNames[th1d].Data(),addText.Data()));
      CheckValidTH1D(fInSigTH1DHists[th1d][mc],fTH1DNames[th1d],fSigFiles[mc]->GetName());
      fInSigTH1DHists[th1d][mc]->SetLineColor(fColorMap[fSigNames[mc]]);
    }
  }

  // output histos
  fOutDataTH1DHists.resize(fNTH1D);
  fOutBkgTH1DStacks.resize(fNTH1D);
  fOutBkgTH1DStacksForUncer.resize(fNTH1D);
  for (UInt_t th1d = 0; th1d < fNTH1D; th1d++){
    fOutBkgTH1DStacks[th1d] = new THStack("","");
    fOutBkgTH1DStacksForUncer[th1d] = new THStack("","");
  }

  fTH1DLegends.resize(fNTH1D);
  for (UInt_t th1d = 0; th1d < fNTH1D; th1d++){
    //fTH1DLegends[th1d] = new TLegend(0.6075,0.5,0.9,0.934); // (x1,y1,x2,y2)
    fTH1DLegends[th1d] = new TLegend(0.32,0.7,0.9,0.934); // (x1,y1,x2,y2)
    fTH1DLegends[th1d]->SetNColumns(2);
    fTH1DLegends[th1d]->SetBorderSize(4);
    fTH1DLegends[th1d]->SetLineColor(kBlack);
    fTH1DLegends[th1d]->SetTextSize(0.03);//0.035
    fTH1DLegends[th1d]->SetLineWidth(2);
  }


  GJetsClone.resize(fNTH1D);
  fOutTH1DCanvases.resize(fNTH1D);
  fOutTH1DStackPads.resize(fNTH1D);
  fOutBkgTH1DHists.resize(fNTH1D);
  //if (fNData > 0){
    fOutRatioTH1DHists.resize(fNTH1D); 
    fOutRatioTH1DHistsCopy.resize(fNTH1D); 
    fOutTH1DRatioLines.resize(fNTH1D);
    fOutTH1DRatioPads.resize(fNTH1D);
  //} 

  for (UInt_t th1d = 0; th1d < fNTH1D; th1d++){
    fOutTH1DCanvases[th1d] = new TCanvas(fTH1DNames[th1d].Data(),"");
    fOutTH1DCanvases[th1d]->cd();

    //fOutTH1DStackPads[th1d] = new TPad("","",0,0.3,1.0,0.99);
    fOutTH1DStackPads[th1d] = new TPad("","",0.01,0.2,0.99,1.);//x1,y1,x2,y2
    fOutTH1DStackPads[th1d]->SetBottomMargin(0); // upper and lower pad are joined
    fOutTH1DStackPads[th1d]->SetRightMargin(0.06); 
    fOutTH1DStackPads[th1d]->SetLeftMargin(0.12); 

    //if (fNData > 0){// for lower pad with ratio plot
      //fOutTH1DRatioPads[th1d] = new TPad("","",0,0.05,1.0,0.3);
      fOutTH1DRatioPads[th1d] = new TPad("","",0.01,0.001,0.99,0.19);//x1,y1,x2,y2
      fOutTH1DRatioPads[th1d]->SetTopMargin(0.01);
      fOutTH1DRatioPads[th1d]->SetRightMargin(0.06);
      fOutTH1DRatioPads[th1d]->SetLeftMargin(0.12);
      fOutTH1DRatioPads[th1d]->SetBottomMargin(0.4);
      fOutTH1DRatioLines[th1d] = new TLine();
    //}
  }
}// end Combiner::InitCanvAndHists



void Combiner::InitTH1DNames(){
  
  // higgs & met variables
  fTH1DNames.push_back("mgg");
  fIndexMgg = fTH1DNames.size()-1;
  fTH1DNames.push_back("ptgg");
  fIndexPtgg = fTH1DNames.size()-1;
  fTH1DNames.push_back("nvtx"); 
  fIndexNvtx = fTH1DNames.size()-1;
  fTH1DNames.push_back("t1pfmetphi");
  //fTH1DNames.push_back("pfmetphi");
  //fTH1DNames.push_back("pfmet");
  //fTH1DNames.push_back("calometphi");
  //fTH1DNames.push_back("calomet");

  //// photon variables
  fTH1DNames.push_back("pt1");
  fTH1DNames.push_back("pt2");     
  fTH1DNames.push_back("eta1");
  fTH1DNames.push_back("eta2");
  fTH1DNames.push_back("phi1");
  fTH1DNames.push_back("phi2");
  fTH1DNames.push_back("r91");
  fTH1DNames.push_back("r92");

  // photon ID variables
  fTH1DNames.push_back("hoe1");
  fTH1DNames.push_back("hoe2");
  fTH1DNames.push_back("sieie1");
  fTH1DNames.push_back("sieie2");
  fTH1DNames.push_back("phoiso1");
  fTH1DNames.push_back("phoiso2");
  fTH1DNames.push_back("chiso1");
  fTH1DNames.push_back("chiso2");
  fTH1DNames.push_back("neuiso1");
  fTH1DNames.push_back("neuiso2");

  if (addText!="_n-1"){ // plots that don't have n-1 versions 
    //fTH1DNames.push_back("eleveto1");
    //fTH1DNames.push_back("eleveto2");
    //fTH1DNames.push_back("phi1_pho2pass");
    //fTH1DNames.push_back("phi2_pho1pass");
    fTH1DNames.push_back("t1pfmetCorr");
    fTH1DNames.push_back("t1pfmetphiCorr");
    fTH1DNames.push_back("t1pfmet_zoom");
    fTH1DNames.push_back("t1pfmetSumEt");
    fTH1DNames.push_back("t1pfmet_partblind");
    fTH1DNames.push_back("t1pfmetCorr_partblind");
    
    fTH1DNames.push_back("t1pfmet");
    fIndexMET = fTH1DNames.size()-1;
    fTH1DNames.push_back("JetEnUp");
    fTH1DNames.push_back("JetEnDown");
    fTH1DNames.push_back("JetResUp");
    fTH1DNames.push_back("JetResDown");
    fTH1DNames.push_back("MuonEnUp");
    fTH1DNames.push_back("MuonEnDown");
    fTH1DNames.push_back("EleEnUp");
    fTH1DNames.push_back("EleEnDown");
    fTH1DNames.push_back("TauEnUp");
    fTH1DNames.push_back("TauEnDown");
    fTH1DNames.push_back("PhoEnUp");
    fTH1DNames.push_back("PhoEnDown");
    fTH1DNames.push_back("UnclEnUp");
    fTH1DNames.push_back("UnclEnDown");

    fTH1DNames.push_back("ptJet1");
    fTH1DNames.push_back("ptJet2");
    fTH1DNames.push_back("phiJet1");
    fTH1DNames.push_back("phiJet2");
    fTH1DNames.push_back("etaJet1");
    fTH1DNames.push_back("etaJet2");
    fTH1DNames.push_back("dphiJet1MET");
    fTH1DNames.push_back("dphiJet2MET");
    fTH1DNames.push_back("absdphiJet1MET");
    fTH1DNames.push_back("absdphiJet2MET");
    fTH1DNames.push_back("nvtx_afterJetCut");
    fTH1DNames.push_back("ptgg_afterJetCut");
    fTH1DNames.push_back("mgg_afterJetCut");

    fTH1DNames.push_back("met_afterJetCut");
    fTH1DNames.push_back("metCorr_afterJetCut");
    fTH1DNames.push_back("met_afterggMETCut");
    fTH1DNames.push_back("metCorr_afterggMETCut");
    fTH1DNames.push_back("met_afterJetMETCut");
    fTH1DNames.push_back("metCorr_afterJetMETCut");
    fTH1DNames.push_back("met_afterJetMETPhiCut");
    fTH1DNames.push_back("metCorr_afterJetMETPhiCut");

    fTH1DNames.push_back("met_IsolateMET");
    fTH1DNames.push_back("metCorr_IsolateMET");
    fTH1DNames.push_back("met_Isolategg");
    fTH1DNames.push_back("metCorr_Isolategg");
    fTH1DNames.push_back("met_IsolateJET1");
    fTH1DNames.push_back("metCorr_IsolateJET1");

    fTH1DNames.push_back("met_IsolateALL");
    fTH1DNames.push_back("metCorr_IsolateALL");

    //fTH1DNames.push_back("t1pfmet_zoom_wofil");
    fTH1DNames.push_back("mgg_selt1pfmet");
    fTH1DNames.push_back("t1pfmet_selmgg");
    fTH1DNames.push_back("phigg");
    fTH1DNames.push_back("dphi_ggmet");
    fTH1DNames.push_back("absdphi_ggJet1");
    fTH1DNames.push_back("absdphi_ggmet");
    fIndexDphi = fTH1DNames.size()-1;
    fTH1DNames.push_back("deta_gg");
    fTH1DNames.push_back("absdeta_gg");
    fIndexDeta = fTH1DNames.size()-1;
    fTH1DNames.push_back("selection");
    fIndexEff = fTH1DNames.size()-1;
    fTH1DNames.push_back("eff_sel");

    //fTH1DNames.push_back("EBHighR9_mgg");
    //fTH1DNames.push_back("EBHighR9_ptgg");
    //fTH1DNames.push_back("EBHighR9_t1pfmet");
    //fTH1DNames.push_back("EBLowR9_mgg");
    //fTH1DNames.push_back("EBLowR9_ptgg");
    //fTH1DNames.push_back("EBLowR9_t1pfmet");
    //fTH1DNames.push_back("EEHighR9_mgg");
    //fTH1DNames.push_back("EEHighR9_ptgg");
    //fTH1DNames.push_back("EEHighR9_t1pfmet");
    //fTH1DNames.push_back("EELowR9_mgg");
    //fTH1DNames.push_back("EELowR9_ptgg");
    //fTH1DNames.push_back("EELowR9_t1pfmet");

  }
}// end Combiner::InitTH1DNames
