#include "Plotter.hh"
#include "Style.hh"
#include "../../../DataFormats/Math/interface/deltaPhi.h"
#include "mkPlotsLivia/CMS_lumi.C"

Plotter::Plotter( TString inName, TString outName, TString inSpecies, const DblVec puweights, const Double_t lumi, Bool_t Data, Bool_t Blind, TString type, Bool_t doMETCorr, const DblVec metCorr){

  fType = type;  
  isData = Data;
  doBlind = Blind;
  doMETcorr = doMETCorr;
  fMETCorr = metCorr;

  // Get ROOT file
  name = inName;
  species = inSpecies;
  inFile = TFile::Open(Form("%s%s.root",name.Data(),species.Data()));
  CheckValidFile(inFile,Form("%s%s.root",name.Data(),species.Data()));  
  // Open Tree from inFile
  tpho = (TTree*)inFile->Get("DiPhotonTree"); 
  CheckValidTree(tpho,"DiPhotonTree",Form("%s%s.root",name.Data(),species.Data()));

  fLumi = lumi;
  fPUWeights = puweights;

  fSelection.resize(8);
  TH1D *fSel = (TH1D*)inFile->Get("h_selection");
  CheckValidTH1D(fSel,"h_selection",Form("%s%s.root",name.Data(),species.Data()));
  //if (isData && doBlind){
  //  for (UInt_t i=0; i<6; i++){
  //    fSelection[i]=fSel->GetBinContent(i+1);
  //  }
  //  fSelection[6]=0;
  //  fSelection[7]=0;
  //}
  if (!(isData && doBlind)){
    for (UInt_t i=0; i<8; i++){ 
      // values of bin i correspond to passing (all cuts previous + one listed below):  
      // 1=trigger, 2=presel, 3=selection, 4=pt1>30,pt2>20, 5=pt1>mgg/3,pt2>mgg/4, 6=goodVtx, 7=mgg, 8=met
      fSelection[i]=fSel->GetBinContent(i+1);
    }
  }
  std::cout << "Finished getting the h_selection" << std::endl;  

  // Make TLorentzVector for the photons
  TLorentzVector *fLorenzVec1		= new TLorentzVector();
  TLorentzVector *fLorenzVec2		= new TLorentzVector();
  TLorentzVector *fLorenzVecgg		= new TLorentzVector();
  TLorentzVector *fLorenzVecJet1	= new TLorentzVector();
  TLorentzVector *fLorenzVecJet2	= new TLorentzVector();

  // Make output directory
  fName = outName;
  TString FullPath = fName.Data();
  FullPath+=species.Data();
  FullPath+="/";
  MakeOutDirectory(FullPath.Data());

  // Make output ROOT file
  outFile = new TFile(Form("%s/%s/plots_%s.root",fName.Data(),species.Data(),species.Data()),"RECREATE");
  CheckValidFile(outFile,Form("%s/%s/plots_%s.root",fName.Data(),species.Data(),species.Data()));

  // set all the branch addresses appropriately
  Plotter::SetBranchAddresses();

}// end Plotter::Plotter


Plotter::~Plotter(){
  std::cout << "Finished & Deleting" <<std::endl;
  std::cout << "Deleting inTree" <<std::endl;
  delete tpho;
  std::cout << "Deleting inFile" <<std::endl;
  delete inFile;
  // Write and Close output ROOT file
  //Plotter::DeleteBranches();
  std::cout << "Delete histos" <<std::endl;
  Plotter::DeleteHists();
  std::cout << "Deleting outFile" <<std::endl;
  delete outFile;
  std::cout << "Finished Deleting" <<std::endl;
}// end Plotter::~Plotter


void Plotter::DoPlots(int prompt){
  Plotter::SetUpPlots();
 
  nentries = tpho->GetEntries(); 
  std::cout << "nentries = " << nentries << std::endl;
  
  Double_t effPUn[60]={0};
  Double_t effPUd[60]={0};
  Double_t effptn[60]={0};
  Double_t effptd[60]={0};

  fTH1DMap["hlt"]->Fill(0.5,nentries);
  // fSelection[i]-> 1=trigger, 2=presel, 3=selection, 4=pt1>30,pt2>20, 5=pt1>mgg/3,pt2>mgg/4, 6=goodVtx, 7=mgg, 8=met
  for (UInt_t i=0; i<7; i++){
    fTH1DMap["selection"]->Fill(i+0.5,fSelection[i]);
  }

  Int_t numFailingMETfil = 0;
  Int_t numOutOfMggRange = 0;
  Int_t numNegativeWeight = 0;
  Int_t numFailEV = 0;
  Int_t numDuplicateRemoved = 0;
  Int_t numPassingAll = 0;
 
  Int_t numRelFailingMETfil = 0;
  Int_t numRelOutOfMggRange = 0;
  Int_t numRelFailEV = 0;

  Int_t nRel0entries = nentries;
  Int_t nRel1entries = 0;
  Int_t nRel2entries = 0;
  Int_t nRel3entries = 0;
  
  for (UInt_t entry = 0; entry < nentries; entry++){
    tpho->GetEntry(entry);

    // Fill TLorentzVector
    fLorenzVec1.SetPtEtaPhiM(pt1,eta1,phi1,0.);
    fLorenzVec2.SetPtEtaPhiM(pt2,eta2,phi2,0.);
    fLorenzVecgg = fLorenzVec1 + fLorenzVec2;
    fLorenzVecJet1.SetPtEtaPhiM(ptJetLead,etaJetLead,phiJetLead,0.);
    fLorenzVecJet2.SetPtEtaPhiM(ptJetSubLead,etaJetSubLead,phiJetSubLead,0.);
 

    // calculate the weight
    Double_t Weight = (weight)*fPUWeights[nvtx];// PURW[0] corresponds to bin1=0vtx

    if (hltPhoton26Photon16Mass60 == 1) fTH1DMap["hlt"]->Fill(1.5,1);
    if (hltPhoton36Photon22Mass15 == 1) fTH1DMap["hlt"]->Fill(2.5,1);
    if (hltPhoton42Photon25Mass15 == 1) fTH1DMap["hlt"]->Fill(3.5,1);
    if (hltDiphoton30Mass95 == 1)   	fTH1DMap["hlt"]->Fill(4.5,1);
    if (hltDiphoton30Mass70 == 1)   	fTH1DMap["hlt"]->Fill(5.5,1);
    if (hltDiphoton30Mass55 == 1)   	fTH1DMap["hlt"]->Fill(6.5,1);
    if (hltDiphoton30Mass55PV == 1) 	fTH1DMap["hlt"]->Fill(7.5,1);
    if (hltDiphoton30Mass55EB == 1) 	fTH1DMap["hlt"]->Fill(8.5,1);

    fTH1DMap["eff_sel"]->Fill(0.5,Weight);

    Bool_t passCH1  = false;
    Bool_t passCH2  = false;
    Bool_t passNH1  = false;
    Bool_t passNH2  = false;
    Bool_t passPH1  = false;
    Bool_t passPH2  = false;
    Bool_t passS1   = false;
    Bool_t passS2   = false;
    Bool_t passHE1  = false;
    Bool_t passHE2  = false;
    Bool_t passAll1 = false;
    Bool_t passAll2 = false;
    Bool_t passBoth = false;
    Bool_t passEV1  = false; 
    Bool_t passEV2  = false; 

    // For LOOSE Photon ID Working Point
    // Can replace "Loose" with "Tight" 
    // OR remove "Loose" for Medium WP
    if (passLooseCHiso1==1) passCH1 = true; 
    if (passLooseCHiso2==1) passCH2 = true; 
    if (passLooseNHiso1==1) passNH1 = true;
    if (passLooseNHiso2==1) passNH2 = true;
    if (passLoosePHiso1==1) passPH1 = true;
    if (passLoosePHiso2==1) passPH2 = true;
    if (passLooseSieie1==1) passS1  = true;
    if (passLooseSieie2==1) passS2  = true;
    if (passLooseHoe1==1)   passHE1 = true; 
    if (passLooseHoe2==1)   passHE2 = true; 
    if (eleveto1==1)	    passEV1 = true;
    if (eleveto2==1)	    passEV2 = true; 

    //if (!passCH1) std::cout << "Fails CHIso1" << std::endl;
    //if (!passCH2) std::cout << "Fails CHIso2" << std::endl;
    //if (!passNH1) std::cout << "Fails NHIso1" << std::endl;
    //if (!passNH2) std::cout << "Fails NHIso2" << std::endl;
    //if (!passPH1) std::cout << "Fails PHIso1" << std::endl;
    //if (!passPH2) std::cout << "Fails PHIso2" << std::endl;
    //if (!passS1)  std::cout << "Fails SIEIE1" << std::endl;
    //if (!passS2)  std::cout << "Fails SIEIE2" << std::endl;
    //if (!passHE1) std::cout << "Fails HoE1"   << std::endl;
    //if (!passHE2) std::cout << "Fails HoE2"   << std::endl;

    if (passCH1 && passNH1 && passPH1 && passS1 && passHE1 && passEV1) passAll1 = true;
    if (passCH2 && passNH2 && passPH2 && passS2 && passHE2 && passEV2) passAll2 = true;
    if (passAll1 && passAll2) passBoth = true;


    Bool_t EB1, EB2, EE1, EE2, inEE, inEB;
    Bool_t hiR9, loR9;

    // Check if the Data passes MET filters
    Bool_t passMETfil = true;
    if (isData){
      if (metF_GV!=1 || metF_HBHENoise!=1 || metF_HBHENoiseIso!=1 || metF_CSC!=1 || metF_eeBadSC!=1 ) passMETfil = false; 
    }
    if (!passMETfil) numFailingMETfil++;
    if (!isData && !passMETfil) std::cout << "SOMETHING WRONG W/ MET FILTERS" << std::endl;

    // Check that the weight is not less than 0
    Bool_t weightNegative = false;
    if (Weight <= 0) weightNegative = true;

    if (mgg < 100 || mgg >= 180) numOutOfMggRange++;
    if (weightNegative) numNegativeWeight++;
    if (!passEV1 || !passEV2) numFailEV++;
    if (prompt==1 || prompt==2){
      if (genmatch1==1 && genmatch2==1) numDuplicateRemoved++;
    }

    if (mgg >= 100 && mgg < 180){
      nRel1entries++;
      if (!passBoth){
        numRelFailEV++;
      }
      else{
        nRel2entries++;
	if (!passMETfil){
	  numRelFailingMETfil++;
	}
	else nRel3entries++;
      }
    }  


    // START full selection for plots
    if (passMETfil && !weightNegative){ //Data passes MET filters && not a negativeWeight
      if (true /*pt1 > 0.65*mgg && pt2 > 0.25*mgg*/){
      //if (mgg >= 100 && mgg < 180 && passEV1 && passEV2 /*&&  pt1 > 0.65*mgg && pt2 > 0.25*mgg */ /*&& t1pfmet > 80*/ ){
        fTH1DMap["eff_sel"]->Fill(1.5,Weight);
        if (!isData || (isData && hltDiphoton30Mass95==1)){ // data has to pass trigger

          // to remove duplicate events 
	  // original implementation:
          if (prompt==1 && (genmatch1==1 && genmatch2==1)) continue;   // only PF and FF for gjets  
          if (prompt==2 && (genmatch1==1 && genmatch2==1)) continue;   // only PF and FF for gjets  
          //if (prompt==2 && (genmatch1==1 || genmatch2==1)) continue;   // only FF for QCD       

	  numPassingAll++;

	  // t1pfmet phi Correction
	  Double_t t1pfmetCorrX;
	  Double_t t1pfmetCorrY;
	  Double_t t1pfmetCorrE;
	  if (doMETcorr){
	    t1pfmetCorrX = t1pfmet*cos(t1pfmetPhi) - (fMETCorr[0] + fMETCorr[1]*t1pfmetSumEt);
	    t1pfmetCorrY = t1pfmet*sin(t1pfmetPhi) - (fMETCorr[2] + fMETCorr[3]*t1pfmetSumEt);
            //std::cout << "px = t1pfmet*cos(t1pfmetPhi) - (" << fMETCorr[0] << " + " << fMETCorr[1] << "*t1pfmetSumEt)" << std::endl;
            //std::cout << "py = t1pfmet*sin(t1pfmetPhi) - (" << fMETCorr[2] << " + " << fMETCorr[3] << "*t1pfmetSumEt)" << std::endl;
	    t1pfmetCorrE = sqrt(t1pfmetCorrX*t1pfmetCorrX + t1pfmetCorrY*t1pfmetCorrY);
          }
          else{ 
	    t1pfmetCorrX = 0; 
	    t1pfmetCorrY = 0; 
	    t1pfmetCorrE = 0;
	  } 
	  TLorentzVector correctedMet;
	  correctedMet.SetPxPyPzE(t1pfmetCorrX,t1pfmetCorrY,0,t1pfmetCorrE);
	  Double_t t1pfmetPhiCorr = correctedMet.Phi(); 
	  Double_t t1pfmetCorr = correctedMet.Pt();

          // split events by eta
          EB1 = false;
          EB2 = false;
          EE1 = false;
          EE2 = false;
          if (fabs(eta1)>1.566)  EE1=true;
          if (fabs(eta2)>1.566)  EE2=true;
          if (fabs(eta1)<1.4442) EB1=true;
          if (fabs(eta2)<1.4442) EB2=true; 
          inEE=false;
          inEB=false;
          if (EB1 && EB2) inEB=true;
          else if (EE1 || EE2) inEE=true;
          
          // split events by r9
          hiR9 = false;
          loR9 = false;
          if (r91 > 0.94 && r92 > 0.94) hiR9 = true;
          else if (r91 <= 0.94 || r92 <= 0.94) loR9 = true;

          //if (passEV1 && passEV2){
	  //  if (inEB && hiR9){  
	  //    if (isData && doBlind){
	  //      if (mgg<115 || mgg>135) fTH1DMap["EBHighR9_mgg"]->Fill(mgg,Weight);
	  //      fTH1DMap["EBHighR9_ptgg"]->Fill(ptgg,Weight);
	  //      if (t1pfmet<100) fTH1DMap["EBHighR9_t1pfmet"]->Fill(t1pfmet,Weight);
	  //    }
	  //    else{
	  //      fTH1DMap["EBHighR9_mgg"]->Fill(mgg,Weight);
	  //      fTH1DMap["EBHighR9_ptgg"]->Fill(ptgg,Weight);
	  //      fTH1DMap["EBHighR9_t1pfmet"]->Fill(t1pfmet,Weight);
	  //    }
          //  }
          //  if (inEB && loR9){
	  //    if (isData && doBlind){
	  //      if (mgg<115 || mgg>135) fTH1DMap["EBLowR9_mgg"]->Fill(mgg,Weight);
	  //      fTH1DMap["EBLowR9_ptgg"]->Fill(ptgg,Weight);
	  //      if (t1pfmet<100) fTH1DMap["EBLowR9_t1pfmet"]->Fill(t1pfmet,Weight);
	  //    }
	  //    else{
	  //      fTH1DMap["EBLowR9_mgg"]->Fill(mgg,Weight);
	  //      fTH1DMap["EBLowR9_ptgg"]->Fill(ptgg,Weight);
	  //      fTH1DMap["EBLowR9_t1pfmet"]->Fill(t1pfmet,Weight);
	  //    }
          //  }
          //  if (inEE && hiR9){
	  //    if (isData && doBlind){
	  //      if (mgg<115 || mgg>135) fTH1DMap["EEHighR9_mgg"]->Fill(mgg,Weight);
	  //      fTH1DMap["EEHighR9_ptgg"]->Fill(ptgg,Weight);
	  //      if (t1pfmet<100) fTH1DMap["EEHighR9_t1pfmet"]->Fill(t1pfmet,Weight);
	  //    }
	  //    else{
	  //      fTH1DMap["EEHighR9_mgg"]->Fill(mgg,Weight);
	  //      fTH1DMap["EEHighR9_ptgg"]->Fill(ptgg,Weight);
	  //      fTH1DMap["EEHighR9_t1pfmet"]->Fill(t1pfmet,Weight);
	  //    }
          //  }
          //  if (inEE && loR9){
	  //    if (isData && doBlind){
	  //      if (mgg<115 || mgg>135) fTH1DMap["EELowR9_mgg"]->Fill(mgg,Weight);
	  //      fTH1DMap["EELowR9_ptgg"]->Fill(ptgg,Weight);
	  //      if (t1pfmet<100) fTH1DMap["EELowR9_t1pfmet"]->Fill(t1pfmet,Weight);
	  //    }
	  //    else{
	  //      fTH1DMap["EELowR9_mgg"]->Fill(mgg,Weight);
	  //      fTH1DMap["EELowR9_ptgg"]->Fill(ptgg,Weight);
	  //      fTH1DMap["EELowR9_t1pfmet"]->Fill(t1pfmet,Weight);
	  //    }
          //  }
          //}


          fTH1DMap["eff_sel"]->Fill(2.5,Weight);
          //Fill histograms
          if (doBlind){
           if (mgg < 115 || mgg > 135){
             fTH1DMap["t1pfmet_partblind"]->Fill(t1pfmet,Weight);
             fTH1DMap["t1pfmetCorr_partblind"]->Fill(t1pfmetCorr,Weight);
           }
          }
          else{
            fTH1DMap["t1pfmet_partblind"]->Fill(t1pfmet,Weight);
            fTH1DMap["t1pfmetCorr_partblind"]->Fill(t1pfmetCorr,Weight);
          }

          if (isData && doBlind){ // BLIND THE DATA mgg and met distributions
            if (mgg < 115 || mgg > 135){
              if (t1pfmet < 100) fTH2DMap["t1pfmet_mgg"]->Fill(mgg,t1pfmet,Weight);
              fTH1DMap["t1pfmet_scaledipho"]->Fill(t1pfmet,Weight);
	      if (t1pfmet >= 80) fTH1DMap["mgg_selt1pfmet"]->Fill(mgg,Weight);  
              fTH1DMap["mgg"]->Fill(mgg,Weight);
              fTH2DMap["mgg_PU"]->Fill(nvtx,mgg,Weight);
              fTH2DMap["mgg_ptgg"]->Fill(ptgg,mgg,Weight);
              fTH2DMap["t1pfmet_ptgg"]->Fill(ptgg,t1pfmet,Weight);
              fTH2DMap["t1pfmet_dphi"]->Fill(deltaPhi(fLorenzVecgg.Phi(),t1pfmetPhi),t1pfmet,Weight);
            }
            //fTH1DMap["t1pfmet"]->Fill(t1pfmet,Weight);
            //fTH1DMap["t1pfmet_zoom"]->Fill(t1pfmet,Weight);
            if (t1pfmet < 100){
              fTH1DMap["t1pfmet"]->Fill(t1pfmet,Weight);
              fTH1DMap["t1pfmet_zoom"]->Fill(t1pfmet,Weight);
              fTH2DMap["t1pfmet_PU"]->Fill(nvtx,t1pfmet,Weight);
              fTH1DMap["t1pfmetCorr"]->Fill(t1pfmetCorr,Weight);
            }

            if (pfmet < 100) fTH1DMap["pfmet"]->Fill(pfmet,Weight);
            if (calomet < 100) fTH1DMap["calomet"]->Fill(calomet,Weight);
            /*if (ptgg<0) */ fTH1DMap["ptgg"]->Fill(ptgg,Weight);
          }
          else{
            fTH1DMap["mgg"]->Fill(mgg,Weight);
            fTH1DMap["ptgg"]->Fill(ptgg,Weight);
            fTH1DMap["t1pfmet"]->Fill(t1pfmet,Weight);
            fTH1DMap["t1pfmet_scaledipho"]->Fill(t1pfmet,Weight*1.5);
            fTH1DMap["t1pfmetCorr"]->Fill(t1pfmetCorr,Weight);
            fTH1DMap["t1pfmetJetEnUp"]->Fill(t1pfmetJetEnUp,Weight);
            fTH1DMap["t1pfmetJetEnDown"]->Fill(t1pfmetJetEnDown,Weight);
  	    fTH1DMap["t1pfmetJetResUp"]->Fill(t1pfmetJetResUp,Weight);
  	    fTH1DMap["t1pfmetJetResDown"]->Fill(t1pfmetJetResDown,Weight);
  	    fTH1DMap["t1pfmetMuonEnUp"]->Fill(t1pfmetMuonEnUp,Weight);
  	    fTH1DMap["t1pfmetMuonEnDown"]->Fill(t1pfmetMuonEnDown,Weight);
  	    fTH1DMap["t1pfmetEleEnUp"]->Fill(t1pfmetElectronEnUp,Weight);
  	    fTH1DMap["t1pfmetEleEnDown"]->Fill(t1pfmetElectronEnDown,Weight);
  	    fTH1DMap["t1pfmetTauEnUp"]->Fill(t1pfmetTauEnUp,Weight);
  	    fTH1DMap["t1pfmetTauEnDown"]->Fill(t1pfmetTauEnDown,Weight);
  	    fTH1DMap["t1pfmetPhoEnUp"]->Fill(t1pfmetPhotonEnUp,Weight);
  	    fTH1DMap["t1pfmetPhoEnDown"]->Fill(t1pfmetPhotonEnDown,Weight);
  	    fTH1DMap["t1pfmetUnclEnUp"]->Fill(t1pfmetUnclusteredEnUp,Weight);
  	    fTH1DMap["t1pfmetUnclEnDown"]->Fill(t1pfmetUnclusteredEnDown,Weight);
            if (mgg >= 110 && mgg <= 130) fTH1DMap["t1pfmet_selmgg"]->Fill(t1pfmet,Weight); 
            if (ptgg > 70) fTH1DMap["t1pfmet_selptgg"]->Fill(t1pfmet,Weight);
            if (t1pfmet >= 80) fTH1DMap["mgg_selt1pfmet"]->Fill(mgg,Weight); 
            if (t1pfmet >= 80) fTH1DMap["ptgg_selt1pfmet"]->Fill(ptgg,Weight);
            fTH1DMap["pfmet"]->Fill(pfmet,Weight);
            fTH1DMap["calomet"]->Fill(calomet,Weight);
            fTH1DMap["t1pfmet_zoom"]->Fill(t1pfmet,Weight);
            fTH2DMap["mgg_PU"]->Fill(nvtx,mgg,Weight);
            fTH2DMap["mgg_ptgg"]->Fill(ptgg,mgg,Weight);
            fTH2DMap["t1pfmet_PU"]->Fill(nvtx,t1pfmet,Weight);
            fTH2DMap["t1pfmet_ptgg"]->Fill(ptgg,t1pfmet,Weight);
            fTH2DMap["t1pfmet_ptgg"]->Fill(ptgg,t1pfmet,Weight);
          }

          fTH1DMap["nvtx"]->Fill(nvtx,Weight);
          fTH1DMap["pt1"]->Fill(pt1,Weight);
          fTH1DMap["pt2"]->Fill(pt2,Weight);
	  fTH1DMap["t1pfmetSumEt"]->Fill(t1pfmetSumEt,Weight);
          fTH1DMap["t1pfmetphi"]->Fill(t1pfmetPhi,Weight);
          fTH1DMap["t1pfmetphiCorr"]->Fill(t1pfmetPhiCorr,Weight);
          fTH1DMap["pfmetphi"]->Fill(pfmetphi,Weight);
          fTH1DMap["calometphi"]->Fill(calometphi,Weight);
          fTH1DMap["phi1"]->Fill(phi1,Weight);
          fTH1DMap["phi2"]->Fill(phi2,Weight);
          fTH1DMap["eta1"]->Fill(eta1,Weight);
          fTH1DMap["eta2"]->Fill(eta2,Weight);
          fTH1DMap["chiso1"]->Fill(chiso1,Weight);
          fTH1DMap["chiso2"]->Fill(chiso2,Weight);
          fTH1DMap["neuiso1"]->Fill(neuiso1,Weight);
          fTH1DMap["neuiso2"]->Fill(neuiso2,Weight);
          fTH1DMap["phoiso1"]->Fill(phoiso1,Weight);
          fTH1DMap["phoiso2"]->Fill(phoiso2,Weight);
          fTH1DMap["sieie1"]->Fill(sieie1,Weight);
          fTH1DMap["sieie2"]->Fill(sieie2,Weight);
          fTH1DMap["hoe1"]->Fill(hoe1,Weight);
          fTH1DMap["hoe2"]->Fill(hoe2,Weight);
          fTH1DMap["r91"]->Fill(r91,Weight);
          fTH1DMap["r92"]->Fill(r92,Weight);
          fTH1DMap["eleveto1"]->Fill(eleveto1,Weight);
          fTH1DMap["eleveto2"]->Fill(eleveto2,Weight);
	  fTH1DMap["ptJet1"]->Fill(ptJetLead,Weight);  
	  fTH1DMap["ptJet2"]->Fill(ptJetSubLead,Weight);   
	  fTH1DMap["phiJet1"]->Fill(phiJetLead,Weight);  
	  fTH1DMap["phiJet2"]->Fill(phiJetSubLead,Weight);  
	  fTH1DMap["etaJet1"]->Fill(etaJetLead,Weight);       
	  fTH1DMap["etaJet2"]->Fill(etaJetSubLead,Weight);    
	  fTH1DMap["dphiJet1MET"]->Fill(deltaPhi(fLorenzVecJet1.Phi(),t1pfmetPhi),Weight);
	  fTH1DMap["dphiJet2MET"]->Fill(deltaPhi(fLorenzVecJet2.Phi(),t1pfmetPhi),Weight);
	  fTH1DMap["absdphiJet1MET"]->Fill(TMath::Abs(deltaPhi(fLorenzVecJet1.Phi(),t1pfmetPhiCorr)),Weight);
	  fTH1DMap["absdphiJet2MET"]->Fill(TMath::Abs(deltaPhi(fLorenzVecJet2.Phi(),t1pfmetPhiCorr)),Weight);
	  fTH1DMap["absdphi_ggJet1"]->Fill(TMath::Abs(deltaPhi(fLorenzVecJet1.Phi(),fLorenzVecgg.Phi())),Weight);
          

	  // Study the effect of Jets on the MET distribution
	  // DeltaPhi between each Jet and the MET
          Double_t dphiJet1MET = 0;
          Double_t dphiJet2MET = 0;
	  if (ptJetLead > 50)    dphiJet1MET = TMath::Abs(deltaPhi(fLorenzVecJet1.Phi(),t1pfmetPhi));
	  if (ptJetSubLead > 50) dphiJet2MET = TMath::Abs(deltaPhi(fLorenzVecJet2.Phi(),t1pfmetPhi));
	  Bool_t dphiJet1METpass = false; // dphi Jet1-MET < 2.7
	  if ( dphiJet1MET < 2.7 ) dphiJet1METpass = true; 
	  Bool_t dphiJet2METpass = false; // dphi Jet2-MET < 2.7
	  if ( dphiJet2MET < 2.7 ) dphiJet2METpass = true; 

	  // Max DeltaPhi between Jet and MET
          Double_t maxJetMETphi = 0;
          if (dphiJet1MET > dphiJet2MET) maxJetMETphi = dphiJet1MET;         
	  else maxJetMETphi = dphiJet2MET;
	  Bool_t dphiJETMETpass = false; // max dphi Jet-MET < 2.7 
	  if ( maxJetMETphi < 2.7 ) dphiJETMETpass = true;
	  
	  // DeltaPhi between Jet1 and gg 
	  Double_t dphiJet1gg = 0;
	  dphiJet1gg = TMath::Abs(deltaPhi(fLorenzVecJet1.Phi(),fLorenzVecgg.Phi()));
	  Bool_t dphiJet1ggpass = false; // dphi gg-Jet1 < 2.7
	  if ( dphiJet1gg < 2.7 ) dphiJet1ggpass = true;

	  // DeltaPhi between gg and MET
	  Double_t dphiggMET = 0;
          dphiggMET = TMath::Abs(deltaPhi(fLorenzVecgg.Phi(),t1pfmetPhi));
	  Bool_t dphiggMETpass = false; // dphi gg-MET > 2.1
	  if ( dphiggMET > 2.1 ) dphiggMETpass = true;

	  // make a set of bools for easier comparison
	  Bool_t outsideMgg = false; // event lies outside mgg range
	  if ( mgg < 115 || mgg > 135 ) outsideMgg = true;

	  // plots
	  if ( (doBlind && outsideMgg) || !doBlind){
	    if ( dphiJet1ggpass ){ 
	      fTH1DMap["met_afterJetCut"]->Fill(t1pfmet,Weight); 
	      fTH1DMap["metCor_afterJetCut"]->Fill(t1pfmetCorr,Weight); 
	    }
	    if ( dphiggMETpass ){
	      fTH1DMap["met_afterggMETCut"]->Fill(t1pfmet,Weight); 
	      fTH1DMap["metCor_afterggMETCut"]->Fill(t1pfmetCorr,Weight); 
	      if ( dphiJet1ggpass ){ 
	        fTH1DMap["met_Isolategg"]->Fill(t1pfmet,Weight); 
	        fTH1DMap["metCor_Isolategg"]->Fill(t1pfmetCorr,Weight); 
	      } 
	      if ( dphiJet1METpass ){ 
	        fTH1DMap["met_IsolateMET"]->Fill(t1pfmet,Weight); 
	        fTH1DMap["metCor_IsolateMET"]->Fill(t1pfmetCorr,Weight); 
	      } 
	    } 
	    if ( dphiJet1METpass ){ 
	      fTH1DMap["met_afterJetMETCut"]->Fill(t1pfmet,Weight); 
	      fTH1DMap["metCor_afterJetMETCut"]->Fill(t1pfmetCorr,Weight); 
	      if ( dphiJet1ggpass ){ 
	        fTH1DMap["met_IsolateJET1"]->Fill(t1pfmet,Weight); 
	        fTH1DMap["metCor_IsolateJET1"]->Fill(t1pfmetCorr,Weight);
	      }
	    }
	    if ( dphiJETMETpass ){ 
	      fTH1DMap["met_afterJetMETPhiCut"]->Fill(t1pfmet,Weight); 
	      fTH1DMap["metCor_afterJetMETPhiCut"]->Fill(t1pfmetCorr,Weight); 
	      if ( dphiggMETpass ){
	        fTH1DMap["met_IsolateALL"]->Fill(t1pfmet,Weight); 
	        fTH1DMap["metCor_IsolateALL"]->Fill(t1pfmetCorr,Weight);
	      } 
	    }
	  }

	  if ( dphiJet1ggpass ){ 
            fTH1DMap["nvtx_afterJetCut"]->Fill(nvtx,Weight); 
            fTH1DMap["ptgg_afterJetCut"]->Fill(ptgg,Weight); 
            if (isData && doBlind && outsideMgg) fTH1DMap["mgg_afterJetCut"]->Fill(mgg,Weight);  
            else fTH1DMap["mgg_afterJetCut"]->Fill(mgg,Weight); 
	  }

          fTH1DMap["phigg"]->Fill(fLorenzVecgg.Phi(),Weight); 
          fTH1DMap["dphi_ggmet"]->Fill(deltaPhi(fLorenzVecgg.Phi(),t1pfmetPhi),Weight);
          fTH1DMap["absdphi_ggmet"]->Fill(TMath::Abs(deltaPhi(fLorenzVecgg.Phi(),t1pfmetPhi)),Weight);
          fTH1DMap["deta_gg"]->Fill((eta1-eta2),Weight);
          fTH1DMap["absdeta_gg"]->Fill(TMath::Abs(eta1-eta2),Weight);

          //std::cout << passCH1 <<" "<< passNH1 <<" "<< passPH1 <<" "<< passHE1 <<" "<< passS1 << std::endl; 
          //std::cout << passCH2 <<" "<< passNH2 <<" "<< passPH2 <<" "<< passHE2 <<" "<< passS2 << std::endl; 
          //std::cout << passAll1 <<" "<< passAll2 <<" "<< passBoth << std::endl;

          //fill n-1 plots for the photon ID selection variables
          //if (passCH1 && passNH1 && passPH1 && passS1)  fTH1DMap["hoe1_n-1"]->Fill(hoe1,Weight); 
          //if (passCH1 && passNH1 && passPH1 && passHE1) fTH1DMap["sieie1_n-1"]->Fill(sieie1,Weight);
          //if (passCH1 && passNH1 && passHE1 && passS1)  fTH1DMap["phoiso1_n-1"]->Fill(phoiso1,Weight);
          //if (passCH1 && passPH1 && passHE1 && passS1)  fTH1DMap["neuiso1_n-1"]->Fill(neuiso1,Weight);
          //if (passPH1 && passNH1 && passHE1 && passS1)  fTH1DMap["chiso1_n-1"]->Fill(chiso1,Weight);

          //if (passCH2 && passNH2 && passPH2 && passS2)  fTH1DMap["hoe2_n-1"]->Fill(hoe2,Weight); 
          //if (passCH2 && passNH2 && passPH2 && passHE2) fTH1DMap["sieie2_n-1"]->Fill(sieie2,Weight);
          //if (passCH2 && passNH2 && passHE2 && passS2)  fTH1DMap["phoiso2_n-1"]->Fill(phoiso2,Weight);
          //if (passCH2 && passPH2 && passHE2 && passS2)  fTH1DMap["neuiso2_n-1"]->Fill(neuiso2,Weight);
          //if (passPH2 && passNH2 && passHE2 && passS2)  fTH1DMap["chiso2_n-1"]->Fill(chiso2,Weight);

          //if (passAll1){// fill pho1 plots if these photons pass phoID
          //  fTH1DMap["pt1_n-1"]->Fill(pt1,Weight);
          //  fTH1DMap["r91_n-1"]->Fill(r91,Weight);
          //  fTH1DMap["phi1_n-1"]->Fill(phi1,Weight);
          //  fTH1DMap["eta1_n-1"]->Fill(eta1,Weight);
          //}
          //if (passAll2){// fill pho2 plots if these photons pass phoID
          //  fTH1DMap["pt2_n-1"]->Fill(pt2,Weight);
          //  fTH1DMap["r92_n-1"]->Fill(r92,Weight);
          //  fTH1DMap["phi2_n-1"]->Fill(phi2,Weight);
          //  fTH1DMap["eta2_n-1"]->Fill(eta2,Weight);
          //} 
          //if (passBoth){
          //  fTH1DMap["nvtx_n-1"]->Fill(nvtx,Weight);
          //  fTH1DMap["t1pfmetphi_n-1"]->Fill(t1pfmetPhi,Weight);  
          //  fTH1DMap["pfmetphi_n-1"]->Fill(pfmetphi,Weight);
          //  fTH1DMap["calometphi_n-1"]->Fill(calometphi,Weight);
          //  if (isData && doBlind){// BLIND THE DATA
          //    if (mgg < 115 || mgg > 135){
          //      fTH1DMap["mgg_n-1"]->Fill(mgg,Weight);  
          //      if (t1pfmet < 100) fTH2DMap["t1pfmet_mgg"]->Fill(mgg,t1pfmet,Weight);
          //    }
          //    if (t1pfmet < 100) fTH1DMap["t1pfmet_n-1"]->Fill(t1pfmet,Weight);  
          //    if (pfmet < 100)   fTH1DMap["pfmet_n-1"]->Fill(pfmet,Weight);
          //    if (calomet < 100) fTH1DMap["calomet_n-1"]->Fill(calomet,Weight);
          //    /*if (ptgg<0)*/ fTH1DMap["ptgg_n-1"]->Fill(ptgg,Weight);  
          //    //if (mgg >= 110 && mgg <= 130) fTH1DMap["t1pfmet_selmgg"]->Fill(t1pfmet,Weight); 
          //    if (t1pfmet >= 50 && ( mgg < 115 || mgg > 135)) fTH1DMap["mgg_selt1pfmet"]->Fill(mgg,Weight);  
          //  }
          //  else{
          //    fTH1DMap["mgg_n-1"]->Fill(mgg,Weight);  
          //    fTH2DMap["t1pfmet_mgg"]->Fill(mgg,t1pfmet,Weight);
          //    fTH1DMap["t1pfmet_n-1"]->Fill(t1pfmet,Weight);  
          //    fTH1DMap["pfmet_n-1"]->Fill(pfmet,Weight);
          //    fTH1DMap["calomet_n-1"]->Fill(calomet,Weight);
          //    fTH1DMap["ptgg_n-1"]->Fill(ptgg,Weight);  
	  


          if (passCH1 && passCH2){
            fTH1DMap["eff_sel"]->Fill(3.5,Weight);
            if (passNH1 && passNH2){
              fTH1DMap["eff_sel"]->Fill(4.5,Weight);
              if (passPH1 && passPH2){
                fTH1DMap["eff_sel"]->Fill(5.5,Weight);
                if (passS1 && passS2){ 
                  fTH1DMap["eff_sel"]->Fill(6.5,Weight);
           	if (passHE1 && passHE2){
                    fTH1DMap["eff_sel"]->Fill(7.5,Weight);
          	  if (!isData || !doBlind){// BLIND THE DATA
                      if (mgg >= 115 && mgg <= 135){
              	      fTH1DMap["eff_sel"]->Fill(8.5,Weight);
              	      if (t1pfmet >= 100){
              	        fTH1DMap["eff_sel"]->Fill(9.5,Weight);
              	      }
                      }
          	  }
                  }
                }
              }
            }
          }

          for (UInt_t i = 0; i < 60; i++){
            if (nvtx == i){
              effPUd[i]++;
              if (passBoth) effPUn[i]++;
            }
            if (ptgg >= 10*i && ptgg < 10*(i+1)){
              effptd[i]++;
              if (passBoth) effptn[i]++;
            }
          }
 
        }// end if passes trigger
      }// end if passes pt cuts 
      
      if (hltDiphoton30Mass95==1){ //passes trigger
        if(passAll2 && pt2 > mgg/4) fTH1DMap["phi1_pho2pass"]->Fill(phi1,Weight);
        if(passAll1 && pt1 > mgg/3) fTH1DMap["phi2_pho1pass"]->Fill(phi2,Weight);
      }
    }// end if passes MET filter
   
  }// end loop over entries in tree

  std::cout << "Number Events that have passed Analyzer: " << nentries << " events. " << std::endl;
  std::cout << "Number Events rejected by MET filters:   " << numFailingMETfil    << " out of " << nentries << " events. " << std::endl;
  
  std::cout << "nvtx   Int " << fTH1DMap["nvtx"]->Integral() << std::endl;
  std::cout << "ptJet1 Int " << fTH1DMap["ptJet1"]->Integral() << std::endl;

  std::cout << "======================================================" << std::endl;
  std::cout << "======================================================" << std::endl;
  UInt_t binMETlo = fTH1DMap["t1pfmetCorr_partblind"]->GetXaxis()->FindBin(80);
  UInt_t binMEThi = fTH1DMap["t1pfmetCorr_partblind"]->GetXaxis()->FindBin(298); 
  std::cout << "Events in MET tail of CorrMET			= " << fTH1DMap["t1pfmetCorr_partblind"]->Integral(binMETlo,binMEThi) << std::endl;
  std::cout << "======================================================" << std::endl;
  std::cout << "Events in MET tail of CorrMET + JetCut		= " << fTH1DMap["metCor_afterJetCut"]->Integral(binMETlo,binMEThi) << std::endl;
  std::cout << "Events in MET tail of CorrMET + ggMETCut	= " << fTH1DMap["metCor_afterggMETCut"]->Integral(binMETlo,binMEThi) << std::endl;
  std::cout << "Events in MET tail of CorrMET + JetMETCut	= " << fTH1DMap["metCor_afterJetMETCut"]->Integral(binMETlo,binMEThi) << std::endl;
  std::cout << "Events in MET tail of CorrMET + JetMETPhiCut	= " << fTH1DMap["metCor_afterJetMETPhiCut"]->Integral(binMETlo,binMEThi) << std::endl;
  std::cout << "======================================================" << std::endl;
  std::cout << "Events in MET tail of CorrMET + Jet1 Iso	= " << fTH1DMap["metCor_IsolateJET1"]->Integral(binMETlo,binMEThi) << std::endl;
  std::cout << "Events in MET tail of CorrMET + MET Iso		= " << fTH1DMap["metCor_IsolateMET"]->Integral(binMETlo,binMEThi) << std::endl;
  std::cout << "Events in MET tail of CorrMET + gg Iso		= " << fTH1DMap["metCor_Isolategg"]->Integral(binMETlo,binMEThi) << std::endl;
  std::cout << "======================================================" << std::endl;
  std::cout << "Events in MET tail of CorrMET + ALL Iso		= " << fTH1DMap["metCor_IsolateALL"]->Integral(binMETlo,binMEThi) << std::endl;
  std::cout << "======================================================" << std::endl;
  std::cout << "======================================================" << std::endl;

  //std::cout << "Number Events rejected by Mgg range:     " << numOutOfMggRange    << " out of " << nentries << " events. " << std::endl; 
  //std::cout << "Number Events rejected by Neg Weight:    " << numNegativeWeight   << " out of " << nentries << " events. " << std::endl; 
  //std::cout << "Number Events rejected by ElectronVeto:  " << numFailEV           << " out of " << nentries << " events. " << std::endl; 
  //std::cout << "Number Events rejected by DupRemoval:    " << numDuplicateRemoved << " out of " << nentries << " events. " << std::endl;  
  //std::cout << "Number Events PASSING all selection:     " << numPassingAll       << " out of " << nentries << " events. " << std::endl; 
  //
  //std::cout << "Number Events that have passed Analyzer: " << nentries << " events. " << std::endl;
  //std::cout << "Number Events rejected by Mgg range:     " << numOutOfMggRange       << " out of rel " << nRel0entries << " events. " << std::endl; 
  //std::cout << "Number Events rejected by ElectronVeto:  " << numRelFailEV           << " out of rel " << nRel1entries << " events. " << std::endl; 
  //std::cout << "Number Events rejected by MET filters:   " << numRelFailingMETfil    << " out of rel " << nRel2entries << " events. " << std::endl;
  //std::cout << "Number Events PASSING all selection:     " << numPassingAll          << " out of rel " << nRel3entries << " events. " << std::endl; 


  Double_t effPU = 0;
  Double_t effpt = 0;
  Double_t bin = 0;
  for (UInt_t i=0; i<60; i++){
    bin = (Double_t)i;
    if (effPUd[i] > 0) effPU = (Double_t)effPUn[i]/(Double_t)effPUd[i];
    if (effptd[i] > 0) effpt = (Double_t)effptn[i]/(Double_t)effptd[i];
    fTH1DMap["eff_PU"]->Fill(bin,effPU); 
    fTH1DMap["eff_pt"]->Fill(bin*10,effpt); 
  }

  fTH1DMap["eff_sel"]->GetXaxis()->SetBinLabel(1,"nentries");
  fTH1DMap["eff_sel"]->GetXaxis()->SetBinLabel(2,"passPt");
  fTH1DMap["eff_sel"]->GetXaxis()->SetBinLabel(3,"passTrigger");
  fTH1DMap["eff_sel"]->GetXaxis()->SetBinLabel(4,"passCHiso");
  fTH1DMap["eff_sel"]->GetXaxis()->SetBinLabel(5,"passNHiso");
  fTH1DMap["eff_sel"]->GetXaxis()->SetBinLabel(6,"passPHiso");
  fTH1DMap["eff_sel"]->GetXaxis()->SetBinLabel(7,"passSieie");
  fTH1DMap["eff_sel"]->GetXaxis()->SetBinLabel(8,"passHoe");
  fTH1DMap["eff_sel"]->GetXaxis()->SetBinLabel(9,"passMgg");
  fTH1DMap["eff_sel"]->GetXaxis()->SetBinLabel(10,"passMet");  

  fTH1DMap["hlt"]->GetXaxis()->SetBinLabel(1,"nentries");
  fTH1DMap["hlt"]->GetXaxis()->SetBinLabel(2,"Pho26Pho16M60");
  fTH1DMap["hlt"]->GetXaxis()->SetBinLabel(3,"Pho36Pho22M15");
  fTH1DMap["hlt"]->GetXaxis()->SetBinLabel(4,"Pho42Pho25M15");
  fTH1DMap["hlt"]->GetXaxis()->SetBinLabel(5,"Dipho30M95");
  fTH1DMap["hlt"]->GetXaxis()->SetBinLabel(6,"Dipho30M70");
  fTH1DMap["hlt"]->GetXaxis()->SetBinLabel(7,"Dipho30M55");
  fTH1DMap["hlt"]->GetXaxis()->SetBinLabel(8,"Dipho30M55PV");
  fTH1DMap["hlt"]->GetXaxis()->SetBinLabel(9,"Dipho30M55EB");

  //std::cout << "phi1 " << fTH1DMap["phi1_n-1"]->Integral() <<  " phi2 " << fTH1DMap["phi2_n-1"]->Integral() << std::endl;

  Plotter::SavePlots();

}// end Plotter::DoPlots


void Plotter::SetUpPlots(){
  // fill all plots from tree
  fTH1DMap["nvtx"]		= Plotter::MakeTH1DPlot("nvtx","",40,0.,40.,"nvtx","");
  fTH1DMap["mgg"]		= Plotter::MakeTH1DPlot("mgg","",26,99.,151.,"m_{#gamma#gamma} (GeV)","");  
  fTH1DMap["ptgg"]		= Plotter::MakeTH1DPlot("ptgg","",60,0.,600.,"p_{T,#gamma#gamma} (GeV)","");
  fTH1DMap["t1pfmetphi"]	= Plotter::MakeTH1DPlot("t1pfmetphi","",20,-4.,4.,"E_{T}^{miss} #phi","");
  fTH1DMap["t1pfmetCorr"]	= Plotter::MakeTH1DPlot("t1pfmetCorr","",75,0.,900,"E_{T}^{miss} (GeV)",""); 
  fTH1DMap["t1pfmetphiCorr"]	= Plotter::MakeTH1DPlot("t1pfmetphiCorr","",20,-4.,4.,"E_{T}^{miss} #phi","");
  fTH1DMap["pfmet"]		= Plotter::MakeTH1DPlot("pfmet","",100,0.,1000,"PF MET (GeV)","");
  fTH1DMap["pfmetphi"]		= Plotter::MakeTH1DPlot("pfmetphi","",80,-4.,4.,"PF MET #phi","");
  fTH1DMap["calomet"]		= Plotter::MakeTH1DPlot("calomet","",100,0.,1000,"calo MET (GeV)","");
  fTH1DMap["calometphi"]	= Plotter::MakeTH1DPlot("calometphi","",80,-4.,4.,"calo MET #phi","");
  fTH1DMap["phi1"]		= Plotter::MakeTH1DPlot("phi1","",20,-4.,4.,"#phi(#gamma1)","");
  fTH1DMap["phi2"]		= Plotter::MakeTH1DPlot("phi2","",20,-4.,4.,"#phi(#gamma2)","");
  fTH1DMap["eta1"]		= Plotter::MakeTH1DPlot("eta1","",20,-3.,3.,"#eta(#gamma1)","");
  fTH1DMap["eta2"]		= Plotter::MakeTH1DPlot("eta2","",20,-3.,3.,"#eta(#gamma2)","");
  fTH1DMap["pt1"]		= Plotter::MakeTH1DPlot("pt1","",60,0.,600.,"p_{T,#gamma1} (GeV)","");
  fTH1DMap["pt2"]		= Plotter::MakeTH1DPlot("pt2","",40,0.,400.,"p_{T,#gamma2} (GeV)","");
  fTH1DMap["chiso1"]		= Plotter::MakeTH1DPlot("chiso1","",75,-5.,15.,"CHiso(#gamma1)","");
  fTH1DMap["chiso2"]		= Plotter::MakeTH1DPlot("chiso2","",75,-5.,15.,"CHiso(#gamma2)","");
  fTH1DMap["neuiso1"]		= Plotter::MakeTH1DPlot("neuiso1","",75,-5.,15.,"NHiso(#gamma1)","");
  fTH1DMap["neuiso2"]		= Plotter::MakeTH1DPlot("neuiso2","",75,-5.,15.,"NHiso(#gamma2)","");
  fTH1DMap["phoiso1"]		= Plotter::MakeTH1DPlot("phoiso1","",75,-5.,15.,"PHiso(#gamma1)",""); 
  fTH1DMap["phoiso2"]		= Plotter::MakeTH1DPlot("phoiso2","",75,-5.,15.,"PHiso(#gamma2)",""); 
  fTH1DMap["sieie1"]		= Plotter::MakeTH1DPlot("sieie1","",75,0.,0.03,"#sigma_{i#eta i#eta}(#gamma1)",""); 
  fTH1DMap["sieie2"]		= Plotter::MakeTH1DPlot("sieie2","",75,0.,0.03,"#sigma_{i#eta i#eta}(#gamma2)",""); 
  fTH1DMap["hoe1"]		= Plotter::MakeTH1DPlot("hoe1","",25,0.,0.025,"H/E(#gamma1)","");
  fTH1DMap["hoe2"]		= Plotter::MakeTH1DPlot("hoe2","",25,0.,0.025,"H/E(#gamma2)","");
  fTH1DMap["r91"]		= Plotter::MakeTH1DPlot("r91","",50,0.,1.1,"R9(#gamma1)","");
  fTH1DMap["r92"]		= Plotter::MakeTH1DPlot("r92","",50,0.,1.1,"R9(#gamma2)","");
  fTH1DMap["eleveto1"]		= Plotter::MakeTH1DPlot("eleveto1","",2,0,2.0,"Electron Veto(#gamma1)","");
  fTH1DMap["eleveto2"]		= Plotter::MakeTH1DPlot("eleveto2","",2,0,2.0,"Electron Veto(#gamma2)","");
  fTH1DMap["ptJet1"]		= Plotter::MakeTH1DPlot("ptJet1","",60,0.,600.,"p_{T,Jet1} (GeV)","");		
  fTH1DMap["ptJet2"]		= Plotter::MakeTH1DPlot("ptJet2","",60,0.,600.,"p_{T,Jet2} (GeV)","");		
  fTH1DMap["phiJet1"]		= Plotter::MakeTH1DPlot("phiJet1","",20,-4.,4.,"#phi(Jet1)","");
  fTH1DMap["phiJet2"]		= Plotter::MakeTH1DPlot("phiJet2","",20,-4.,4.,"#phi(Jet2)","");
  fTH1DMap["etaJet1"]		= Plotter::MakeTH1DPlot("etaJet1","",20,-3.,3.,"#eta(Jet1)","");
  fTH1DMap["etaJet2"]		= Plotter::MakeTH1DPlot("etaJet2","",20,-3.,3.,"#eta(Jet2)","");
  fTH1DMap["dphiJet1MET"]	= Plotter::MakeTH1DPlot("dphiJet1MET","",20,-4.,4.,"#Delta#phi(Jet1,E_{T}^{miss})","");
  fTH1DMap["dphiJet2MET"]	= Plotter::MakeTH1DPlot("dphiJet2MET","",20,-4.,4.,"#Delta#phi(Jet2,E_{T}^{miss})","");
  fTH1DMap["absdphiJet1MET"]	= Plotter::MakeTH1DPlot("absdphiJet1MET","",20,-4.,4.,"|#Delta#phi(Jet1,E_{T}^{miss})|","");
  fTH1DMap["absdphiJet2MET"]	= Plotter::MakeTH1DPlot("absdphiJet2MET","",20,-4.,4.,"|#Delta#phi(Jet2,E_{T}^{miss})|","");
  fTH1DMap["absdphi_ggJet1"]	= Plotter::MakeTH1DPlot("absdphi_ggJet1","",20,-4,4.,"|#Delta#phi(Jet1,E_{T}^{miss})|","");

  fTH1DMap["met_afterJetMETCut"]   = Plotter::MakeTH1DPlot("met_afterJetMETCut","",60,0.,300.,"E_{T}^{miss} (GeV)","");
  fTH1DMap["metCor_afterJetMETCut"]= Plotter::MakeTH1DPlot("metCorr_afterJetMETCut","",60,0.,300.,"E_{T}^{miss} (GeV)","");
  fTH1DMap["met_afterJetMETPhiCut"]   = Plotter::MakeTH1DPlot("met_afterJetMETPhiCut","",60,0.,300.,"E_{T}^{miss} (GeV)","");
  fTH1DMap["metCor_afterJetMETPhiCut"]= Plotter::MakeTH1DPlot("metCorr_afterJetMETPhiCut","",60,0.,300.,"E_{T}^{miss} (GeV)","");
  fTH1DMap["met_afterggMETCut"]	   = Plotter::MakeTH1DPlot("met_afterggMETCut","",60,0.,300.,"E_{T}^{miss} (GeV)","");
  fTH1DMap["metCor_afterggMETCut"] = Plotter::MakeTH1DPlot("metCorr_afterggMETCut","",60,0.,300.,"E_{T}^{miss} (GeV)","");
  fTH1DMap["met_afterJetCut"]	= Plotter::MakeTH1DPlot("met_afterJetCut","",60,0.,300.,"E_{T}^{miss} (GeV)","");
  fTH1DMap["metCor_afterJetCut"]= Plotter::MakeTH1DPlot("metCorr_afterJetCut","",60,0.,300.,"E_{T}^{miss} (GeV)","");

  fTH1DMap["met_IsolateMET"]	= Plotter::MakeTH1DPlot("met_IsolateMET","",60,0.,300.,"E_{T}^{miss} (GeV)","");
  fTH1DMap["metCor_IsolateMET"]	= Plotter::MakeTH1DPlot("metCorr_IsolateMET","",60,0.,300.,"E_{T}^{miss} (GeV)","");
  fTH1DMap["met_IsolateJET1"]	= Plotter::MakeTH1DPlot("met_IsolateJET1","",60,0.,300.,"E_{T}^{miss} (GeV)","");
  fTH1DMap["metCor_IsolateJET1"]= Plotter::MakeTH1DPlot("metCorr_IsolateJET1","",60,0.,300.,"E_{T}^{miss} (GeV)","");
  fTH1DMap["met_Isolategg"]	= Plotter::MakeTH1DPlot("met_Isolategg","",60,0.,300.,"E_{T}^{miss} (GeV)","");
  fTH1DMap["metCor_Isolategg"]	= Plotter::MakeTH1DPlot("metCorr_Isolategg","",60,0.,300.,"E_{T}^{miss} (GeV)","");
  fTH1DMap["met_IsolateALL"]	= Plotter::MakeTH1DPlot("met_IsolateALL","",60,0.,300.,"E_{T}^{miss} (GeV)","");
  fTH1DMap["metCor_IsolateALL"]	= Plotter::MakeTH1DPlot("metCorr_IsolateALL","",60,0.,300.,"E_{T}^{miss} (GeV)","");

  fTH1DMap["nvtx_afterJetCut"]	= Plotter::MakeTH1DPlot("nvtx_afterJetCut","",40,0.,40.,"nvtx","");
  fTH1DMap["mgg_afterJetCut"]	= Plotter::MakeTH1DPlot("mgg_afterJetCut","",26,99.,151.,"m_{#gamma#gamma} (GeV)","");  
  fTH1DMap["ptgg_afterJetCut"]	= Plotter::MakeTH1DPlot("ptgg_afterJetCut","",60,0.,600.,"p_{T,#gamma#gamma} (GeV)","");

  fTH1DMap["t1pfmet_partblind"]     = Plotter::MakeTH1DPlot("t1pfmet_partblind","",60,0.,300.,"E_{T}^{miss} (GeV)","");
  fTH1DMap["t1pfmetCorr_partblind"] = Plotter::MakeTH1DPlot("t1pfmetCorr_partblind","",60,0.,300.,"E_{T}^{miss} (GeV)","");
  fTH1DMap["t1pfmetSumEt"]	= Plotter::MakeTH1DPlot("t1pfmetSumEt","",75,0.,900.,"Sum E_{T} (GeV)","");
  fTH1DMap["t1pfmet_scaledipho"]= Plotter::MakeTH1DPlot("t1pfmet_scaledipho","",75,0.,900.,"E_{T}^{miss} (GeV)","");
  fTH1DMap["t1pfmet"]		= Plotter::MakeTH1DPlot("t1pfmet","",75,0.,900.,"E_{T}^{miss} (GeV)","");
  fTH1DMap["t1pfmetJetEnUp"]	= Plotter::MakeTH1DPlot("JetEnUp","",75,0.,900.,"E_{T}^{miss} (GeV)","");
  fTH1DMap["t1pfmetJetEnDown"]	= Plotter::MakeTH1DPlot("JetEnDown","",75,0.,900.,"E_{T}^{miss} (GeV)","");
  fTH1DMap["t1pfmetJetResUp"]	= Plotter::MakeTH1DPlot("JetResUp","",75,0.,900.,"E_{T}^{miss} (GeV)","");
  fTH1DMap["t1pfmetJetResDown"] = Plotter::MakeTH1DPlot("JetResDown","",75,0.,900.,"E_{T}^{miss} (GeV)","");
  fTH1DMap["t1pfmetMuonEnUp"]	= Plotter::MakeTH1DPlot("MuonEnUp","",75,0.,900.,"E_{T}^{miss} (GeV)","");
  fTH1DMap["t1pfmetMuonEnDown"]	= Plotter::MakeTH1DPlot("MuonEnDown","",75,0.,900.,"E_{T}^{miss} (GeV)","");
  fTH1DMap["t1pfmetEleEnUp"]	= Plotter::MakeTH1DPlot("EleEnUp","",75,0.,900.,"E_{T}^{miss} (GeV)","");
  fTH1DMap["t1pfmetEleEnDown"]	= Plotter::MakeTH1DPlot("EleEnDown","",75,0.,900.,"E_{T}^{miss} (GeV)","");
  fTH1DMap["t1pfmetTauEnUp"]	= Plotter::MakeTH1DPlot("TauEnUp","",75,0.,900.,"E_{T}^{miss} (GeV)","");
  fTH1DMap["t1pfmetTauEnDown"]	= Plotter::MakeTH1DPlot("TauEnDown","",75,0.,900.,"E_{T}^{miss} (GeV)","");
  fTH1DMap["t1pfmetPhoEnUp"]	= Plotter::MakeTH1DPlot("PhoEnUp","",75,0.,900.,"E_{T}^{miss} (GeV)","");
  fTH1DMap["t1pfmetPhoEnDown"]	= Plotter::MakeTH1DPlot("PhoEnDown","",75,0.,900.,"E_{T}^{miss} (GeV)","");
  fTH1DMap["t1pfmetUnclEnUp"]	= Plotter::MakeTH1DPlot("UnclEnUp","",75,0.,900.,"E_{T}^{miss} (GeV)","");
  fTH1DMap["t1pfmetUnclEnDown"]	= Plotter::MakeTH1DPlot("UnclEnDown","",75,0.,900.,"E_{T}^{miss} (GeV)","");

  // n minus 1 plots
  //fTH1DMap["nvtx_n-1"]		= Plotter::MakeTH1DPlot("nvtx_n-1","",40,0.,40.,"nvtx","");
  //fTH1DMap["mgg_n-1"]		= Plotter::MakeTH1DPlot("mgg_n-1","",26,99.,151.,"m_{#gamma#gamma} (GeV)","");  
  //fTH1DMap["ptgg_n-1"]		= Plotter::MakeTH1DPlot("ptgg_n-1","",60,0.,600.,"p_{T,#gamma#gamma} (GeV)","");
  //fTH1DMap["t1pfmet_n-1"]	= Plotter::MakeTH1DPlot("t1pfmet_n-1","",25,0.,200.,"E_{T}^{miss} (GeV)","");
  //fTH1DMap["t1pfmetphi_n-1"]	= Plotter::MakeTH1DPlot("t1pfmetphi_n-1","",20,-4.,4.,"E_{T}^{miss} #phi","");
  //fTH1DMap["pfmet_n-1"]		= Plotter::MakeTH1DPlot("pfmet_n-1","",100,0.,1000,"PF MET (GeV)","");
  //fTH1DMap["pfmetphi_n-1"]	= Plotter::MakeTH1DPlot("pfmetphi_n-1","",80,-4.,4.,"PF MET #phi","");
  //fTH1DMap["calomet_n-1"]	= Plotter::MakeTH1DPlot("calomet_n-1","",100,0.,1000,"calo MET (GeV)","");
  //fTH1DMap["calometphi_n-1"]	= Plotter::MakeTH1DPlot("calometphi_n-1","",80,-4.,4.,"calo MET #phi","");
  //fTH1DMap["phi1_n-1"]		= Plotter::MakeTH1DPlot("phi1_n-1","",20,-4.,4.,"#phi(#gamma1)","");
  //fTH1DMap["phi2_n-1"]		= Plotter::MakeTH1DPlot("phi2_n-1","",20,-4.,4.,"#phi(#gamma2)","");
  //fTH1DMap["eta1_n-1"]		= Plotter::MakeTH1DPlot("eta1_n-1","",20,-3.,3.,"#eta(#gamma1)","");
  //fTH1DMap["eta2_n-1"]		= Plotter::MakeTH1DPlot("eta2_n-1","",20,-3.,3.,"#eta(#gamma2)","");
  //fTH1DMap["pt1_n-1"]		= Plotter::MakeTH1DPlot("pt1_n-1","",60,0.,600.,"p_{T,#gamma1} (GeV)","");
  //fTH1DMap["pt2_n-1"]		= Plotter::MakeTH1DPlot("pt2_n-1","",60,0.,600.,"p_{T,#gamma2} (GeV)","");
  //fTH1DMap["chiso1_n-1"]	= Plotter::MakeTH1DPlot("chiso1_n-1","",75,-5.,15.,"CHiso(#gamma1)","");
  //fTH1DMap["chiso2_n-1"]	= Plotter::MakeTH1DPlot("chiso2_n-1","",75,-5.,15.,"CHiso(#gamma2)","");
  //fTH1DMap["neuiso1_n-1"]	= Plotter::MakeTH1DPlot("neuiso1_n-1","",75,-5.,15.,"NHiso(#gamma1)","");
  //fTH1DMap["neuiso2_n-1"]	= Plotter::MakeTH1DPlot("neuiso2_n-1","",75,-5.,15.,"NHiso(#gamma2)","");
  //fTH1DMap["phoiso1_n-1"]	= Plotter::MakeTH1DPlot("phoiso1_n-1","",75,-5.,15.,"PHiso(#gamma1)",""); 
  //fTH1DMap["phoiso2_n-1"]	= Plotter::MakeTH1DPlot("phoiso2_n-1","",75,-5.,15.,"PHiso(#gamma2)",""); 
  //fTH1DMap["sieie1_n-1"]	= Plotter::MakeTH1DPlot("sieie1_n-1","",75,0.,0.03,"#sigma_{i#eta i#eta}(#gamma1)",""); 
  //fTH1DMap["sieie2_n-1"]	= Plotter::MakeTH1DPlot("sieie2_n-1","",75,0.,0.03,"#sigma_{i#eta i#eta}(#gamma2)",""); 
  //fTH1DMap["hoe1_n-1"]		= Plotter::MakeTH1DPlot("hoe1_n-1","",25,0.,0.025,"H/E(#gamma1)","");
  //fTH1DMap["hoe2_n-1"]		= Plotter::MakeTH1DPlot("hoe2_n-1","",25,0.,0.025,"H/E(#gamma2)","");
  //fTH1DMap["r91_n-1"]		= Plotter::MakeTH1DPlot("r91_n-1","",50,0.,1.1,"R9(#gamma1)","");
  //fTH1DMap["r92_n-1"]		= Plotter::MakeTH1DPlot("r92_n-1","",50,0.,1.1,"R9(#gamma2)","");

  // special plots
  fTH1DMap["phigg"]		= Plotter::MakeTH1DPlot("phigg","",20,-4.,4.,"#phi(#gamma#gamma)","");
  fTH1DMap["dphi_ggmet"]	= Plotter::MakeTH1DPlot("dphi_ggmet","",20,-4.,4.,"#Delta#phi(#gamma#gamma,MET)","");
  fTH1DMap["absdphi_ggmet"]	= Plotter::MakeTH1DPlot("absdphi_ggmet","",20,0.,4.,"|#Delta#phi(#gamma#gamma,MET)|","");
  fTH1DMap["t1pfmet_selmgg"]	= Plotter::MakeTH1DPlot("t1pfmet_selmgg","",100,0.,1000.,"E_{T}^{miss} (GeV)","");
  fTH1DMap["mgg_selt1pfmet"]	= Plotter::MakeTH1DPlot("mgg_selt1pfmet","",26,99.,151.,"m_{#gamma#gamma} (GeV)","");
  fTH1DMap["phi1_pho2pass"]     = Plotter::MakeTH1DPlot("phi1_pho2pass","",80,-4.,4.,"","");
  fTH1DMap["phi2_pho1pass"]     = Plotter::MakeTH1DPlot("phi2_pho1pass","",80,-4.,4.,"","");
  fTH1DMap["t1pfmet_zoom"]	= Plotter::MakeTH1DPlot("t1pfmet_zoom","",60,0.,300.,"E_{T}^{miss} (GeV)","");
  fTH1DMap["deta_gg"]		= Plotter::MakeTH1DPlot("deta_gg","",20,-3.,3.,"#Delta#eta(#gamma#gamma)","");
  fTH1DMap["absdeta_gg"]	= Plotter::MakeTH1DPlot("absdeta_gg","",20,0.,3.,"|#Delta#eta(#gamma#gamma)|","");
  fTH1DMap["ptgg_selt1pfmet"]	= Plotter::MakeTH1DPlot("ptgg_selt1pfmet","",60,0.,600.,"p_{T,#gamma#gamma} (GeV)","");
  fTH1DMap["t1pfmet_selptgg"]	= Plotter::MakeTH1DPlot("t1pfmet_selptgg","",100,0.,1000.,"E_{T}^{miss} (GeV)","");

  //// pho cat plots
  //fTH1DMap["EBHighR9_mgg"]	= Plotter::MakeTH1DPlot("EBHighR9_mgg","",26,99.,151.,"m_{#gamma#gamma} (GeV)","");  
  //fTH1DMap["EBHighR9_ptgg"]	= Plotter::MakeTH1DPlot("EBHighR9_ptgg","",60,0.,600.,"p_{T,#gamma#gamma} (GeV)","");
  //fTH1DMap["EBHighR9_t1pfmet"]	= Plotter::MakeTH1DPlot("EBHighR9_t1pfmet","",75,0.,900,"t1PF MET (GeV)","");
  //fTH1DMap["EBLowR9_mgg"]	= Plotter::MakeTH1DPlot("EBLowR9_mgg","",26,99.,151.,"m_{#gamma#gamma} (GeV)","");  
  //fTH1DMap["EBLowR9_ptgg"]	= Plotter::MakeTH1DPlot("EBLowR9_ptgg","",60,0.,600.,"p_{T,#gamma#gamma} (GeV)","");
  //fTH1DMap["EBLowR9_t1pfmet"]	= Plotter::MakeTH1DPlot("EBLowR9_t1pfmet","",75,0.,900,"t1PF MET (GeV)","");
  //fTH1DMap["EEHighR9_mgg"]	= Plotter::MakeTH1DPlot("EEHighR9_mgg","",26,99.,151.,"m_{#gamma#gamma} (GeV)","");  
  //fTH1DMap["EEHighR9_ptgg"]	= Plotter::MakeTH1DPlot("EEHighR9_ptgg","",60,0.,600.,"p_{T,#gamma#gamma} (GeV)","");
  //fTH1DMap["EEHighR9_t1pfmet"]	= Plotter::MakeTH1DPlot("EEHighR9_t1pfmet","",75,0.,900,"t1PF MET (GeV)","");
  //fTH1DMap["EELowR9_mgg"]	= Plotter::MakeTH1DPlot("EELowR9_mgg","",26,99.,151.,"m_{#gamma#gamma} (GeV)","");  
  //fTH1DMap["EELowR9_ptgg"]	= Plotter::MakeTH1DPlot("EELowR9_ptgg","",60,0.,600.,"p_{T,#gamma#gamma} (GeV)","");
  //fTH1DMap["EELowR9_t1pfmet"]	= Plotter::MakeTH1DPlot("EELowR9_t1pfmet","",75,0.,900,"t1PF MET (GeV)","");

  // efficiency plots
  fTH1DMap["eff_sel"]		= Plotter::MakeTH1DPlot("eff_sel","",10,0.,10.,"","");
  fTH1DMap["selection"]		= Plotter::MakeTH1DPlot("selection","",6,0.,6.,"","");
  fTH1DMap["eff_PU"]		= Plotter::MakeTH1DPlot("eff_PU","",60,0.,60.,"","");
  fTH1DMap["eff_pt"]		= Plotter::MakeTH1DPlot("eff_pt","",60,0.,600.,"","");
  fTH1DMap["hlt"]		= Plotter::MakeTH1DPlot("hlt","",10,0.,10,"","");
  fTH1DMap["sel_ptgg"]		= Plotter::MakeTH1DPlot("sel_ptgg","",200,0,200,"","");
  fTH1DMap["sel_dphi"]		= Plotter::MakeTH1DPlot("sel_dphi","",20,0,20,"","");
  fTH1DMap["sel_deta"]		= Plotter::MakeTH1DPlot("sel_deta","",20,0,20,"","");

  // 2D plots
  fTH2DMap["mgg_PU"]		= Plotter::MakeTH2DPlot("mgg_PU","",60,0.,60.,40,100.,300.,"nvtx","m_{#gamma#gamma} (GeV)");
  fTH2DMap["mgg_ptgg"] 		= Plotter::MakeTH2DPlot("mgg_ptgg","",50,0.,500.,40,100.,300.,"p_{T,#gamma#gamma} (GeV)","m_{#gamma#gamma}");
  fTH2DMap["t1pfmet_PU"]	= Plotter::MakeTH2DPlot("t1pfmet_PU","",60,50.,300.,100,0.,1000.,"nvtx","E_{T}^{miss} (GeV)");
  fTH2DMap["t1pfmet_ptgg"]	= Plotter::MakeTH2DPlot("t1pfmet_ptgg","",40,0.,400.,25,0.,250.,"p_{T,#gamma#gamma} (GeV)","E_{T}^{miss} (GeV)");
  fTH2DMap["t1pfmet_mgg"]	= Plotter::MakeTH2DPlot("t1pfmet_mgg","",800,100.,300.,4000,0.,1000,"m_{#gamma#gamma} (GeV)","E_{T}^{miss} (GeV)");
  fTH2DMap["t1pfmet_dphi"]	= Plotter::MakeTH2DPlot("t1pfmet_dphi","",20,-4.,4.,25,0.,250.,"#Delta#phi(#gamma#gamma,E_{T}^{miss})","E_{T}^{miss} (GeV)");

}// end Plotter::SetUpPlots

TH1D * Plotter::MakeTH1DPlot(const TString hname, const TString htitle, const Int_t nbins, const Double_t xlow, const Double_t xhigh, const TString xtitle, const TString ytitle){
  TString ytitleNew;
  Float_t binwidth = (xhigh-xlow)/nbins;
  //std::cout << "binwidth " <<  binwidth << std::endl;
  if (ytitle=="") ytitleNew = Form("Events/(%2.2f)",binwidth);
  else ytitleNew = ytitle;
  //std::cout << "yTitle = " << ytitleNew.Data() << std::endl;
 
  TH1D * hist = new TH1D(hname.Data(),htitle.Data(),nbins,xlow,xhigh);
  hist->GetXaxis()->SetTitle(xtitle.Data());
  hist->GetYaxis()->SetTitle(ytitleNew.Data());
  hist->Sumw2();
  gStyle->SetOptStat(1111111);
  return hist;
}// end Plotter::MakeTH1DPlot

TH2D * Plotter::MakeTH2DPlot(const TString hname, const TString htitle, const Int_t xnbins, const Double_t xlow, const Double_t xhigh, const Int_t ynbins, const Double_t ylow, const Double_t yhigh, const TString xtitle, const TString ytitle){
  TH2D * hist = new TH2D(hname.Data(),htitle.Data(),xnbins,xlow,xhigh,ynbins,ylow,yhigh);
  hist->GetXaxis()->SetTitle(xtitle.Data());
  hist->GetYaxis()->SetTitle(ytitle.Data());
  gStyle->SetOptStat(0);
  return hist;
}// end Plotter::MakeTH2DPlot

TH1D * DrawOverflowBin(const TH1D * h){
    Int_t nbin = h->GetNbinsX()+1;
    Double_t overflow = h->GetBinContent(nbin); 

}


void Plotter::SavePlots(){
  outFile->cd();

  TCanvas * canv = new TCanvas();

  for (TH1DMapIter mapiter = fTH1DMap.begin(); mapiter != fTH1DMap.end(); mapiter++){
    canv->Clear();

    if ((*mapiter).second == (TH1D*) NULL)	{std::cout << "TH1D Null" << std::endl;}
    if (outFile == (TFile*) NULL)		{std::cout << "OutFile Null" << std::endl;}
    if (canv == (TCanvas*) NULL)		{std::cout << "Canvas Null" << std::endl;}

    //fTH1DNewMap[(*mapiter).first].second = DrawOverflowBin( (*mapiter).second );   

    (*mapiter).second->Write(); // save histos to root file 
    canv->cd();
    (*mapiter).second->Draw("HIST");

    CMSLumi(canv,0,fLumi);

    //// UNCOMMENT THESE LINES IF WANT TO MAKE OUTPUT FILES OF ALL PLOTS
    //canv->SetLogy(0);
    //canv->SaveAs(Form("%s%s/%s.%s",fName.Data(),species.Data(),(*mapiter).first.Data(),fType.Data()));

    //canv->SetLogy(1);
    //canv->SaveAs(Form("%s%s/%s_log.%s",fName.Data(),species.Data(),(*mapiter).first.Data(),fType.Data())); 

  }// end of loop over mapiter for 1d plots
  delete canv;

  TCanvas * canv2d = new TCanvas();

  for (TH2DMapIter mapiter = fTH2DMap.begin(); mapiter != fTH2DMap.end(); mapiter++){
    //canv->Clear();

    if ((*mapiter).second == (TH2D*) NULL)	{std::cout << "TH2D Null" << std::endl;}
    if (outFile == (TFile*) NULL)		{std::cout << "OutFile Null" << std::endl;}
    if (canv == (TCanvas*) NULL)		{std::cout << "Canvas Null" << std::endl;}

    (*mapiter).second->Write(); // save histos to root file 
    canv2d->cd();
    (*mapiter).second->Draw("colz");

    CMSLumi(canv2d,0,fLumi);

    canv2d->SetLogy(0);
    canv2d->SaveAs(Form("%s%s/%s.%s",fName.Data(),species.Data(),(*mapiter).first.Data(),fType.Data()));
  }// end of loop over mapiter for 2d plots
  delete canv2d;

}// end Plotter::SavePlots

void Plotter::DeleteHists(){
  for (TH1DMapIter mapiter = fTH1DMap.begin(); mapiter != fTH1DMap.end(); mapiter++){
    delete ((*mapiter).second);
  }
  fTH1DMap.clear();

  for (TH2DMapIter mapiter = fTH2DMap.begin(); mapiter != fTH2DMap.end(); mapiter++){
    delete ((*mapiter).second);
  }
  fTH2DMap.clear();

}// end Plotter::DeleteHists

void Plotter::SetBranchAddresses(){
  tpho->SetBranchAddress("weight", &weight,  &b_weight);
  tpho->SetBranchAddress("nvtx",   &nvtx,    &b_nvtx);
  tpho->SetBranchAddress("mgg",    &mgg,     &b_mgg);
  tpho->SetBranchAddress("ptgg",   &ptgg,    &b_ptgg);
  tpho->SetBranchAddress("t1pfmet", &t1pfmet, &b_t1pfmet);   
  tpho->SetBranchAddress("t1p2pfmet", &t1p2pfmet, &b_t1p2pfmet);
  tpho->SetBranchAddress("t1pfmetJetEnUp", &t1pfmetJetEnUp, &b_t1pfmetJetEnUp);
  tpho->SetBranchAddress("t1pfmetJetEnDown", &t1pfmetJetEnDown, &b_t1pfmetJetEnDown);
  tpho->SetBranchAddress("t1pfmetJetResUp", &t1pfmetJetResUp, &b_t1pfmetJetResUp);
  tpho->SetBranchAddress("t1pfmetJetResDown", &t1pfmetJetResDown, &b_t1pfmetJetResDown);
  tpho->SetBranchAddress("t1pfmetMuonEnUp", &t1pfmetMuonEnUp, &b_t1pfmetMuonEnUp);
  tpho->SetBranchAddress("t1pfmetMuonEnDown", &t1pfmetMuonEnDown, &b_t1pfmetMuonEnDown);
  tpho->SetBranchAddress("t1pfmetElectronEnUp", &t1pfmetElectronEnUp, &b_t1pfmetElectronEnUp);
  tpho->SetBranchAddress("t1pfmetElectronEnDown", &t1pfmetElectronEnDown, &b_t1pfmetElectronEnDown);
  tpho->SetBranchAddress("t1pfmetTauEnUp", &t1pfmetTauEnUp, &b_t1pfmetTauEnUp);
  tpho->SetBranchAddress("t1pfmetTauEnDown", &t1pfmetTauEnDown, &b_t1pfmetTauEnDown);
  tpho->SetBranchAddress("t1pfmetPhotonEnUp", &t1pfmetPhotonEnUp, &b_t1pfmetPhotonEnUp);
  tpho->SetBranchAddress("t1pfmetPhotonEnDown", &t1pfmetPhotonEnDown, &b_t1pfmetPhotonEnDown);
  tpho->SetBranchAddress("t1pfmetUnclusteredEnUp", &t1pfmetUnclusteredEnUp, &b_t1pfmetUnclusteredEnUp);
  tpho->SetBranchAddress("t1pfmetUnclusteredEnDown", &t1pfmetUnclusteredEnDown, &b_t1pfmetUnclusteredEnDown);
  tpho->SetBranchAddress("t1pfmetPhi", &t1pfmetPhi, &b_t1pfmetPhi);
  tpho->SetBranchAddress("t1pfmetSumEt", &t1pfmetSumEt, &b_t1pfmetSumEt);   
  tpho->SetBranchAddress("pfmet", &pfmet, &b_pfmet);   
  tpho->SetBranchAddress("pfmetPhi", &pfmetphi, &b_pfmetPhi);   
  tpho->SetBranchAddress("pfmetSumEt", &pfmetSumEt, &b_pfmetSumEt);   
  tpho->SetBranchAddress("calomet", &calomet, &b_calomet);   
  tpho->SetBranchAddress("calometPhi", &calometphi, &b_calometPhi);   
  tpho->SetBranchAddress("calometSumEt", &calometSumEt, &b_calometSumEt);   
  tpho->SetBranchAddress("genmatch1", &genmatch1, &b_genmatch1);  
  tpho->SetBranchAddress("genmatch2", &genmatch2, &b_genmatch2);   
  tpho->SetBranchAddress("pt1", &pt1, &b_pt1);   
  tpho->SetBranchAddress("pt2", &pt2, &b_pt2);   
  tpho->SetBranchAddress("chiso1", &chiso1, &b_chiso1);   
  tpho->SetBranchAddress("chiso2", &chiso2, &b_chiso2);   
  tpho->SetBranchAddress("neuiso1", &neuiso1, &b_neuiso1);   
  tpho->SetBranchAddress("neuiso2", &neuiso2, &b_neuiso2);   
  tpho->SetBranchAddress("phoiso1", &phoiso1, &b_phoiso1);   
  tpho->SetBranchAddress("phoiso2", &phoiso2, &b_phoiso2);   
  tpho->SetBranchAddress("sieie1", &sieie1, &b_sieie1);   
  tpho->SetBranchAddress("sieie2", &sieie2, &b_sieie2);   
  tpho->SetBranchAddress("hoe1", &hoe1, &b_hoe1);   
  tpho->SetBranchAddress("hoe2", &hoe2, &b_hoe2);   
  tpho->SetBranchAddress("r91", &r91, &b_r91);   
  tpho->SetBranchAddress("r92", &r92, &b_r92);   
  tpho->SetBranchAddress("phi1", &phi1, &b_phi1);   
  tpho->SetBranchAddress("phi2", &phi2, &b_phi2);   
  tpho->SetBranchAddress("eta1", &eta1, &b_eta1);   
  tpho->SetBranchAddress("eta2", &eta2, &b_eta2);   
  tpho->SetBranchAddress("eleveto1", &eleveto1, &b_eleveto1);   
  tpho->SetBranchAddress("eleveto2", &eleveto2, &b_eleveto2);  
  tpho->SetBranchAddress("presel1", &presel1, &b_presel1); 
  tpho->SetBranchAddress("presel2", &presel2, &b_presel2); 
  tpho->SetBranchAddress("sel1", &sel1, &b_sel1); 
  tpho->SetBranchAddress("sel2", &sel2, &b_sel2); 
  tpho->SetBranchAddress("passCHiso1", &passCHiso1, &b_passCHiso1);   
  tpho->SetBranchAddress("passCHiso2", &passCHiso2, &b_passCHiso2);   
  tpho->SetBranchAddress("passNHiso1", &passNHiso1, &b_passNHiso1);   
  tpho->SetBranchAddress("passNHiso2", &passNHiso2, &b_passNHiso2);   
  tpho->SetBranchAddress("passPHiso1", &passPHiso1, &b_passNHiso1);   
  tpho->SetBranchAddress("passPHiso2", &passPHiso2, &b_passNHiso2);   
  tpho->SetBranchAddress("passSieie1", &passSieie1, &b_passSieie1);
  tpho->SetBranchAddress("passSieie2", &passSieie2, &b_passSieie2);
  tpho->SetBranchAddress("passHoe1", &passHoe1, &b_passHoe1);
  tpho->SetBranchAddress("passHoe2", &passHoe2, &b_passHoe2);
  tpho->SetBranchAddress("passLooseCHiso1", &passLooseCHiso1, &b_passLooseCHiso1);
  tpho->SetBranchAddress("passLooseCHiso2", &passLooseCHiso2, &b_passLooseCHiso2);
  tpho->SetBranchAddress("passLooseNHiso1", &passLooseNHiso1, &b_passLooseNHiso1);
  tpho->SetBranchAddress("passLooseNHiso2", &passLooseNHiso2, &b_passLooseNHiso2);
  tpho->SetBranchAddress("passLoosePHiso1", &passLoosePHiso1, &b_passLoosePHiso1);
  tpho->SetBranchAddress("passLoosePHiso2", &passLoosePHiso2, &b_passLoosePHiso2);
  tpho->SetBranchAddress("passLooseSieie1", &passLooseSieie1, &b_passLooseSieie1);
  tpho->SetBranchAddress("passLooseSieie2", &passLooseSieie2, &b_passLooseSieie2);
  tpho->SetBranchAddress("passLooseHoe1", &passLooseHoe1, &b_passLooseHoe1);
  tpho->SetBranchAddress("passLooseHoe2", &passLooseHoe2, &b_passLooseHoe2);
  tpho->SetBranchAddress("passTightCHiso1", &passTightCHiso1, &b_passTightCHiso1);
  tpho->SetBranchAddress("passTightCHiso2", &passTightCHiso2, &b_passTightCHiso2);
  tpho->SetBranchAddress("passTightNHiso1", &passTightNHiso1, &b_passTightNHiso1);
  tpho->SetBranchAddress("passTightNHiso2", &passTightNHiso2, &b_passTightNHiso2);
  tpho->SetBranchAddress("passTightPHiso1", &passTightPHiso1, &b_passTightPHiso1);
  tpho->SetBranchAddress("passTightPHiso2", &passTightPHiso2, &b_passTightPHiso2);
  tpho->SetBranchAddress("passTightSieie1", &passTightSieie1, &b_passTightSieie1);
  tpho->SetBranchAddress("passTightSieie2", &passTightSieie2, &b_passTightSieie2);
  tpho->SetBranchAddress("passTightHoe1", &passTightHoe1, &b_passTightHoe1);
  tpho->SetBranchAddress("passTightHoe2", &passTightHoe2, &b_passTightHoe2);
  tpho->SetBranchAddress("hltPhoton26Photon16Mass60", &hltPhoton26Photon16Mass60, &b_hltPhoton26Photon16Mass60);
  tpho->SetBranchAddress("hltPhoton36Photon22Mass15", &hltPhoton36Photon22Mass15, &b_hltPhoton36Photon22Mass15);
  tpho->SetBranchAddress("hltPhoton42Photon25Mass15", &hltPhoton42Photon25Mass15, &b_hltPhoton42Photon25Mass15);
  tpho->SetBranchAddress("hltDiphoton30Mass95", &hltDiphoton30Mass95, &b_hltDiphoton30Mass95);
  tpho->SetBranchAddress("hltDiphoton30Mass70", &hltDiphoton30Mass70, &b_hltDiphoton30Mass70);
  tpho->SetBranchAddress("hltDiphoton30Mass55", &hltDiphoton30Mass55, &b_hltDiphoton30Mass55);
  tpho->SetBranchAddress("hltDiphoton30Mass55PV", &hltDiphoton30Mass55PV, &b_hltDiphoton30Mass55PV);
  tpho->SetBranchAddress("hltDiphoton30Mass55EB", &hltDiphoton30Mass55EB, &b_hltDiphoton30Mass55EB);
  tpho->SetBranchAddress("nEle", &nEle, &b_nEle);
  tpho->SetBranchAddress("nMuons", &nMuons, &b_nMuons);
  tpho->SetBranchAddress("nJets", &nJets, &b_nJets);
  tpho->SetBranchAddress("nLooseBjets", &nLooseBjets, &b_nLooseBjets);
  tpho->SetBranchAddress("nMediumBjets", &nMediumBjets, &b_nMediumBjets);
  tpho->SetBranchAddress("ptJetLead", &ptJetLead, &b_ptJetLead);
  tpho->SetBranchAddress("etaJetLead", &etaJetLead, &b_etaJetLead);
  tpho->SetBranchAddress("phiJetLead", &phiJetLead, &b_phiJetLead);
  tpho->SetBranchAddress("massJetLead", &massJetLead, &b_massJetLead);
  tpho->SetBranchAddress("indexJetLead", &indexJetLead, &b_indexJetLead);
  tpho->SetBranchAddress("ptJetSubLead", &ptJetSubLead, &b_ptJetSubLead);
  tpho->SetBranchAddress("etaJetSubLead", &etaJetSubLead, &b_etaJetSubLead);
  tpho->SetBranchAddress("phiJetSubLead", &phiJetSubLead, &b_phiJetSubLead);
  tpho->SetBranchAddress("massJetSubLead", &massJetSubLead, &b_massJetSubLead);
  tpho->SetBranchAddress("indexJetSubLead", &indexJetSubLead, &b_indexJetSubLead);
  tpho->SetBranchAddress("vhtruth", &vhtruth, &b_vhtruth);
  tpho->SetBranchAddress("metF_GV", &metF_GV, &b_metF_GV);
  tpho->SetBranchAddress("metF_HBHENoise", &metF_HBHENoise, &b_metF_HBHENoise);
  tpho->SetBranchAddress("metF_HBHENoiseIso", &metF_HBHENoiseIso, &b_metF_HBHENoiseIso);
  tpho->SetBranchAddress("metF_CSC", &metF_CSC, &b_metF_CSC);
  tpho->SetBranchAddress("metF_eeBadSC", &metF_eeBadSC, &b_metF_eeBadSC);
  tpho->SetBranchAddress("higgsVtxX", &higgsVtxX, &b_higgsVtxX);
  tpho->SetBranchAddress("higgsVtxY", &higgsVtxY, &b_higgsVtxY);
  tpho->SetBranchAddress("higgsVtxZ", &higgsVtxZ, &b_higgsVtxZ);
  tpho->SetBranchAddress("massCorrSmear", &massCorrSmear, &b_massCorrSmear);
  tpho->SetBranchAddress("massCorrSmearUp", &massCorrSmearUp, &b_massCorrSmearUp);
  tpho->SetBranchAddress("massCorrSmearDown", &massCorrSmearDown, &b_massCorrSmearDown);
  tpho->SetBranchAddress("massCorrScale", &massCorrScale, &b_massCorrScale);
  tpho->SetBranchAddress("massCorrScaleUp", &massCorrScaleUp, &b_massCorrScaleUp);
  tpho->SetBranchAddress("massCorrScaleDown", &massCorrScaleDown, &b_massCorrScaleDown);
  tpho->SetBranchAddress("massRaw", &massRaw, &b_massRaw);
  tpho->SetBranchAddress("mva1", &mva1, &b_mva1);
  tpho->SetBranchAddress("mva2", &mva2, &b_mva2);
  tpho->SetBranchAddress("genZ", &genZ, &b_b_genZ);
  tpho->SetBranchAddress("ptZ",  &ptZ,  &b_b_ptZ);
  tpho->SetBranchAddress("etaZ", &etaZ, &b_b_etaZ);
  tpho->SetBranchAddress("phiZ", &phiZ, &b_b_phiZ);

  //tpho->SetBranchAddress("", &, &b_);
  
}// end Plotter::SetBranchAddresses


void Plotter::DeleteBranches(){
  delete b_weight;
  delete b_nvtx;
  delete b_mgg;
  delete b_ptgg;
  delete b_pt1;
  delete b_pt2;
}// end Plotter::DeleteBranches


void Plotter::FindMinAndMax(TH1F *& h, int plotLog){
  Float_t max = h->GetMaximum();
  if (plotLog==1) h->SetMaximum(10*max);
  if (plotLog==0) h->SetMaximum(2*max);

  Float_t min = 1000;
  Bool_t newmin = false;

  for (Int_t bin=1; bin <= h->GetNbinsX(); bin++){
    Float_t tmpmin = h->GetBinContent(bin);
    if ((tmpmin < min) && (tmpmin > 0)){
      min = tmpmin;
      newmin = true;
    }
  }

  if (newmin){
    h->SetMinimum(0.90*min);
  }
}// end Plotter::FindMinAndMax



