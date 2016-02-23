#ifndef _plottertools_
#define _plottertools_

#include "Style.hh"

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TString.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TROOT.h"
#include "TMath.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TDirectory.h"
#include "TLegend.h"
#include "TPaveText.h"
#include "TLatex.h"
#include "TLorentzVector.h"

#include <iostream>
#include <cmath>

#define nvar 30 //nvar has to be equal to NVARIABLES

struct PlotOptStruct{
public:
  Int_t		nbins;
  Double_t	xmin;
  Double_t	xmax; 
};

typedef std::map<TString,TH1D*>		TH1DMap;
typedef TH1DMap::iterator		TH1DMapIter;
typedef std::map<TString,TH2D*>		TH2DMap;
typedef TH2DMap::iterator		TH2DMapIter;


class Plotter{
public:
  Plotter(const TString inName, const TString outName, const TString inSpecies, const DblVec puweights, const Double_t lumi, Bool_t Data, Bool_t Blind, const TString type, Bool_t doMETCorr, const DblVec metcorr);
  ~Plotter();

  void DoPlots(int prompt);  
  void FindMinAndMax(TH1F *& h, int plotLog);
  TH1D * DrawOverflowBin(const TH1D * h);

  void SetBranchAddresses();
  void SetUpPlots();
  TH1D * MakeTH1DPlot(const TString hname, const TString htitle, const Int_t nbins, const Double_t xlow, const Double_t xhigh, const TString xtitle, const TString ytitle);
  TH2D * MakeTH2DPlot(const TString hname, const TString htitle, const Int_t xnbins, const Double_t xlow, const Double_t xhigh, const Int_t ynbins, const Double_t ylow, const Double_t yhigh, const TString xtitle, const TString ytitle);
  void SavePlots(); 

  void DeleteBranches();  
  void DeleteHists();

private:
  TString 		name;
  TString 		fName;
  TString 		species;
  TString		fType;
  Bool_t		isSigMC;
  Bool_t		isData;
  Bool_t		doBlind;
  TFile * 		inFile;
  TFile * 		outFile;

  TLorentzVector	fLorenzVec1;
  TLorentzVector	fLorenzVec2;
  TLorentzVector	fLorenzVecgg;
  TLorentzVector	fLorenzVecJet1;
  TLorentzVector	fLorenzVecJet2;

  Bool_t		doMETcorr;
  DblVec		fMETCorr;

  DblVec		fPUWeights;
  Double_t 		fLumi;
  DblVec		fSelection;

  TTree * 		tpho;
  Int_t			nentries;

  TH1DMap		fTH1DMap;
  TH1DMap		fTH1DNewMap; //these histos have overflow bin
  TH2DMap		fTH2DMap;

  // variables for branches
  Int_t 	nvtx;
  Float_t	weight;
  Float_t	mgg;
  Float_t	ptgg;
  Float_t	t1pfmet; 
  Float_t       t1p2pfmet;
  Float_t       t1pfmetJetEnUp;
  Float_t       t1pfmetJetEnDown;
  Float_t       t1pfmetJetResUp;
  Float_t       t1pfmetJetResDown;
  Float_t       t1pfmetMuonEnUp;
  Float_t       t1pfmetMuonEnDown;
  Float_t       t1pfmetElectronEnUp;
  Float_t       t1pfmetElectronEnDown;
  Float_t       t1pfmetTauEnUp;
  Float_t       t1pfmetTauEnDown;
  Float_t       t1pfmetPhotonEnUp;
  Float_t       t1pfmetPhotonEnDown;
  Float_t       t1pfmetUnclusteredEnUp;
  Float_t       t1pfmetUnclusteredEnDown;
  Float_t	t1pfmetPhi; 
  Float_t	t1pfmetSumEt; 
  Float_t	pfmet; 
  Float_t	pfmetphi; 
  Float_t	pfmetSumEt; 
  Float_t	calomet; 
  Float_t	calometphi; 
  Float_t	calometSumEt; 
  Int_t         genmatch1;
  Int_t         genmatch2;
  Float_t	pt1;
  Float_t	pt2;
  Float_t	phi1;
  Float_t	phi2;
  Float_t	eta1;
  Float_t	eta2;
  Float_t	r91;
  Float_t	r92;
  Float_t	phoiso1;
  Float_t	phoiso2;
  Float_t	chiso1;
  Float_t	chiso2;
  Float_t	neuiso1;
  Float_t	neuiso2;
  Float_t	sieie1;
  Float_t	sieie2;
  Float_t	hoe1;
  Float_t	hoe2;
  Int_t		passCHiso1;
  Int_t		passCHiso2;
  Int_t		passNHiso1;
  Int_t		passNHiso2;
  Int_t		passPHiso1;
  Int_t		passPHiso2;
  Int_t		passSieie1;
  Int_t		passSieie2;
  Int_t		passHoe1;
  Int_t		passHoe2;
  Int_t         passLooseCHiso1;
  Int_t         passLooseCHiso2;
  Int_t         passLooseNHiso1;
  Int_t         passLooseNHiso2;
  Int_t         passLoosePHiso1;
  Int_t         passLoosePHiso2;
  Int_t         passLooseSieie1;
  Int_t         passLooseSieie2;
  Int_t         passLooseHoe1;
  Int_t         passLooseHoe2;
  Int_t         passTightCHiso1;
  Int_t         passTightCHiso2;
  Int_t         passTightNHiso1;
  Int_t         passTightNHiso2;
  Int_t         passTightPHiso1;
  Int_t         passTightPHiso2;
  Int_t         passTightSieie1;
  Int_t         passTightSieie2;
  Int_t         passTightHoe1;
  Int_t         passTightHoe2;
  Int_t 	eleveto1;
  Int_t 	eleveto2;
  Int_t		sel1;
  Int_t		sel2;
  Int_t		presel1;
  Int_t		presel2;
  Int_t		hltPhoton26Photon16Mass60;
  Int_t		hltPhoton36Photon22Mass15;
  Int_t		hltPhoton42Photon25Mass15;
  Int_t		hltDiphoton30Mass95;
  Int_t		hltDiphoton30Mass70;
  Int_t		hltDiphoton30Mass55;
  Int_t		hltDiphoton30Mass55PV;
  Int_t		hltDiphoton30Mass55EB;
  Float_t       ptJetLead;
  Float_t       etaJetLead;
  Float_t       phiJetLead;
  Float_t       massJetLead;
  Int_t         indexJetLead;
  Float_t       ptJetSubLead;
  Float_t       etaJetSubLead;
  Float_t       phiJetSubLead;
  Float_t       massJetSubLead;
  Int_t         indexJetSubLead;
  Int_t         nEle;
  Int_t         nMuons;
  Int_t         nJets;
  Int_t         nLooseBjets;
  Int_t         nMediumBjets;
  Int_t         vhtruth;
  Int_t         metF_GV;
  Int_t         metF_HBHENoise;
  Int_t         metF_HBHENoiseIso;
  Int_t         metF_CSC;
  Int_t         metF_eeBadSC;
  Float_t       higgsVtxX;
  Float_t       higgsVtxY;
  Float_t       higgsVtxZ;
  Float_t       massCorrSmear;
  Float_t       massCorrSmearUp;
  Float_t       massCorrSmearDown;
  Float_t       massCorrScale;
  Float_t       massCorrScaleUp;
  Float_t       massCorrScaleDown;
  Float_t       massRaw;
  Float_t       mva1;
  Float_t       mva2;
  Int_t         genZ;
  Float_t       ptZ;
  Float_t       etaZ;
  Float_t       phiZ;


  // branches
  TBranch 	*b_nvtx;
  TBranch	*b_weight;
  TBranch	*b_mgg;
  TBranch	*b_ptgg;
  TBranch	*b_t1pfmet;
  TBranch       *b_t1p2pfmet;   //!
  TBranch       *b_t1pfmetJetEnUp;   //!
  TBranch       *b_t1pfmetJetEnDown;   //!
  TBranch       *b_t1pfmetJetResUp;   //!
  TBranch       *b_t1pfmetJetResDown;   //!
  TBranch       *b_t1pfmetMuonEnUp;   //!
  TBranch       *b_t1pfmetMuonEnDown;   //!
  TBranch       *b_t1pfmetElectronEnUp;   //!
  TBranch       *b_t1pfmetElectronEnDown;   //!
  TBranch       *b_t1pfmetTauEnUp;   //!
  TBranch       *b_t1pfmetTauEnDown;   //!
  TBranch       *b_t1pfmetPhotonEnUp;   //!
  TBranch       *b_t1pfmetPhotonEnDown;   //!
  TBranch       *b_t1pfmetUnclusteredEnUp;   //!
  TBranch       *b_t1pfmetUnclusteredEnDown;   //!
  TBranch	*b_t1pfmetPhi;
  TBranch	*b_t1pfmetSumEt;
  TBranch	*b_pfmet;
  TBranch	*b_pfmetPhi;
  TBranch	*b_pfmetSumEt;
  TBranch	*b_calomet;
  TBranch	*b_calometPhi;
  TBranch	*b_calometSumEt;
  TBranch	*b_genmatch1;
  TBranch	*b_genmatch2;
  TBranch	*b_pt1;
  TBranch	*b_pt2;
  TBranch	*b_phi1;
  TBranch	*b_phi2;
  TBranch	*b_eta1;
  TBranch	*b_eta2;
  TBranch	*b_r91;
  TBranch	*b_r92;
  TBranch	*b_phoiso1;
  TBranch	*b_phoiso2;
  TBranch	*b_chiso1;
  TBranch	*b_chiso2;
  TBranch	*b_neuiso1;
  TBranch	*b_neuiso2;
  TBranch	*b_sieie1;
  TBranch	*b_sieie2;
  TBranch	*b_hoe1;
  TBranch	*b_hoe2;
  TBranch	*b_passCHiso1;
  TBranch	*b_passCHiso2;
  TBranch	*b_passNHiso1;
  TBranch	*b_passNHiso2;
  TBranch	*b_passPHiso1;
  TBranch	*b_passPHiso2;
  TBranch	*b_passSieie1;
  TBranch	*b_passSieie2;
  TBranch	*b_passHoe1;
  TBranch	*b_passHoe2;
  TBranch       *b_passLooseCHiso1;   //!
  TBranch       *b_passLooseCHiso2;   //!
  TBranch       *b_passLooseNHiso1;   //!
  TBranch       *b_passLooseNHiso2;   //!
  TBranch       *b_passLoosePHiso1;   //!
  TBranch       *b_passLoosePHiso2;   //!
  TBranch       *b_passLooseSieie1;   //!
  TBranch       *b_passLooseSieie2;   //!
  TBranch       *b_passLooseHoe1;   //!
  TBranch       *b_passLooseHoe2;   //!
  TBranch       *b_passTightCHiso1;   //!
  TBranch       *b_passTightCHiso2;   //!
  TBranch       *b_passTightNHiso1;   //!
  TBranch       *b_passTightNHiso2;   //!
  TBranch       *b_passTightPHiso1;   //!
  TBranch       *b_passTightPHiso2;   //!
  TBranch       *b_passTightSieie1;   //!
  TBranch       *b_passTightSieie2;   //!
  TBranch       *b_passTightHoe1;   //!
  TBranch       *b_passTightHoe2;   //!
  TBranch	*b_eleveto1;
  TBranch	*b_eleveto2;
  TBranch	*b_sel1;
  TBranch	*b_sel2;
  TBranch	*b_presel1;
  TBranch	*b_presel2;
  TBranch	*b_hltPhoton26Photon16Mass60;
  TBranch	*b_hltPhoton36Photon22Mass15;
  TBranch	*b_hltPhoton42Photon25Mass15;
  TBranch	*b_hltDiphoton30Mass95;
  TBranch	*b_hltDiphoton30Mass70;
  TBranch	*b_hltDiphoton30Mass55;
  TBranch	*b_hltDiphoton30Mass55PV;
  TBranch	*b_hltDiphoton30Mass55EB;
  TBranch       *b_nEle;   //!
  TBranch       *b_nMuons;   //!
  TBranch       *b_nJets;   //!
  TBranch       *b_nLooseBjets;   //!
  TBranch       *b_nMediumBjets;   //!
  TBranch       *b_ptJetLead;   //!
  TBranch       *b_etaJetLead;   //!
  TBranch       *b_phiJetLead;   //!
  TBranch       *b_massJetLead;   //!
  TBranch       *b_indexJetLead;   //!
  TBranch       *b_ptJetSubLead;   //!
  TBranch       *b_etaJetSubLead;   //!
  TBranch       *b_phiJetSubLead;   //!
  TBranch       *b_massJetSubLead;   //!
  TBranch       *b_indexJetSubLead;   //!
  TBranch       *b_vhtruth;   //!
  TBranch       *b_metF_GV;   //!
  TBranch       *b_metF_HBHENoise;   //!
  TBranch       *b_metF_HBHENoiseIso;   //!
  TBranch       *b_metF_CSC;   //!
  TBranch       *b_metF_eeBadSC;   //!
  TBranch       *b_higgsVtxX;   //!
  TBranch       *b_higgsVtxY;   //!
  TBranch       *b_higgsVtxZ;   //!
  TBranch       *b_massCorrSmear;   //!
  TBranch       *b_massCorrSmearUp;   //!
  TBranch       *b_massCorrSmearDown;   //!
  TBranch       *b_massCorrScale;   //!
  TBranch       *b_massCorrScaleUp;   //!
  TBranch       *b_massCorrScaleDown;   //!
  TBranch       *b_massRaw;   //!
  TBranch       *b_mva1;   //!
  TBranch       *b_mva2;   //!
  TBranch       *b_b_genZ;   //!
  TBranch       *b_b_ptZ;   //!
  TBranch       *b_b_etaZ;   //!
  TBranch       *b_b_phiZ;   //!

};

#endif
