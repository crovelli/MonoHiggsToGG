#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/PatCandidates/interface/PackedGenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/Math/interface/deltaR.h"

//#include "DataFormats/EcalRecHit/interface/EcalRecHitCoanalysis/plugins/NewSinglePhoAnalyzer.ccllections.h"
//#include "RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h"

#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"

#include "flashgg/DataFormats/interface/Photon.h"
//#include "flashgg/DataFormats/interface/PhotonCandidate.h"
#include "flashgg/DataFormats/interface/GenPhotonExtra.h"
#include "flashgg/DataFormats/interface/Electron.h"
#include "flashgg/DataFormats/interface/Muon.h"
#include "flashgg/DataFormats/interface/Jet.h"

#include "flashgg/Taggers/interface/LeptonSelection.h"

#include "DataFormats/PatCandidates/interface/MET.h"

#include "DataFormats/Common/interface/TriggerResults.h"
#include "FWCore/Common/interface/TriggerNames.h"

#include "TRandom.h"
#include "TMath.h"
#include "TTree.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include <map>
#include <iostream>
#include <fstream>

#define MAX_PU_REWEIGHT 60

using namespace std;
using namespace edm;
using namespace flashgg;

using pat::PackedGenParticle;   

// curtesy of Y.Iiyama
typedef std::map<unsigned, std::set<unsigned> > EventList;   

EventList readEventListSinglePho(char const* _fileName) {
  
  EventList list;   
  ifstream listFile;
  listFile.open(_fileName); 
  if (!listFile.is_open())    
    throw std::runtime_error(_fileName);  

  unsigned iL(0);    
  std::string line;  
  while (true) {    
    std::getline(listFile, line);    
    if (!listFile.good())  
      break;

    if (line.find(":") == std::string::npos || line.find(":") == line.rfind(":"))  
      continue;

    unsigned run(std::atoi(line.substr(0, line.find(":")).c_str()));        
    unsigned event(std::atoi(line.substr(line.rfind(":") + 1).c_str()));       

    list[run].insert(event);      

    ++iL;   
  }

  std::cout << "Loaded " << iL << " events" << std::endl;      

  return list;        
}   

// diphoton tree
struct phoTree_struc_ {

  int hltPhoton165;
  int hltPhoton175;
  int run;
  int event;
  int lumi;
  int nvtx;
  float rho;
  int sampleID;
  float totXsec;
  float pu_weight;
  float pu_n;
  float sumDataset;
  float perEveW;
  float calomet;
  float calometPhi;
  float calometSumEt;
  float pfmet;
  float pfmetPhi;
  float pfmetSumEt;
  float t1pfmet;
  float t1p2pfmet;
  float t1pfmetJetEnUp ;            
  float t1pfmetJetEnDown ;          
  float t1pfmetJetResUp   ;         
  float t1pfmetJetResDown   ;       
  float t1pfmetMuonEnUp      ;      
  float t1pfmetMuonEnDown      ;      
  float t1pfmetElectronEnUp  ; 
  float t1pfmetElectronEnDown  ; 
  float t1pfmetTauEnUp        ;
  float t1pfmetTauEnDown        ;
  float t1pfmetPhotonEnUp     ;
  float t1pfmetPhotonEnDown     ;
  float t1pfmetUnclusteredEnUp;
  float t1pfmetUnclusteredEnDown;
  float t1pfmetPhi;
  float t1pfmetSumEt;
  float ptJet1;
  float etaJet1;
  float phiJet1;
  float massJet1;
  //  int eventClass;
  float pt1; 
  float ptUncorr1; 
  // float ptOverM1; 
  float eta1; 
  float phi1;
  float sceta1;
  float r91; 
  float sieie1; 
  float hoe1; 
  float scRawEne1;
  float chiso1; 
  float phoiso1; 
  float neuiso1;
  int eleveto1;
  int presel1;
  int sel1;
  int tightsel1;
  int loosesel1;
  // int genmatch1;   
  // float geniso1;   
  int passCHiso1;
  int passNHiso1; 
  int passPHiso1;
  int passSieie1;
  int passHoe1;
  int passTightCHiso1;
  int passTightNHiso1; 
  int passTightPHiso1;
  int passTightSieie1;
  int passTightHoe1;
  int passLooseCHiso1;
  int passLooseNHiso1; 
  int passLoosePHiso1;
  int passLooseSieie1;
  int passLooseHoe1;
  int metF_GV;
  int metF_HBHENoise;
  int metF_HBHENoiseIso;
  int metF_CSC;
  int metF_eeBadSC;
};


class NewPhoAnalyzer : public edm::EDAnalyzer {
  
public:
  
  explicit NewPhoAnalyzer(const edm::ParameterSet&);
  ~NewPhoAnalyzer();
  
private:
  
  edm::Service<TFileService> fs_;
  
  virtual void beginJob() override;
  virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
  virtual void endJob() override;
  
  void initTreeStructure();

  void SetPuWeights(std::string puWeightFile);
  float GetPUWeight(float pun);
  bool isGammaPresel( float sceta, float pt, float r9, float chiso, float hoe);
  bool rediscoveryHLT(float sceta, float pt, float r9,float sieie, float pfIso,float trkSum03 );
  bool isGammaSelected( float rho, float pt, float sceta, float r9, float chiso, float nhiso, float phoiso, float hoe, float sieie, bool passElectronVeto);
  int effectiveAreaRegion(float sceta);
  bool testPhotonIsolation(int passSieie, int passCHiso, int passNHiso, int passPHiso, int passHoe, int passEleVeto);
  //bool testPhotonIsolation(float rho,float pt, float sceta, float r9, float chiso, float nhiso, float phoiso , float hoe, float sieie, bool passElectronVeto);
  double getGammaEAForPhotonIso(float sceta);
  double getChargedHadronEAForPhotonIso(float sceta);
  double getNeutralHadronEAForPhotonIso(float sceta);
  int passSieieCuts(float sceta, float sieie);
  int passCHisoCuts(float sceta, float chiso, float pt);
  int passNHisoCuts(float sceta, float nhiso, float pt);
  int passPHisoCuts(float sceta, float phiso, float pt);
  int passHoeCuts(float sceta, float hoe);
  bool LeadPhoTriggerSel(float eta, float hoe, float r9, float sieie, float phoiso, float pt);
  bool SubLeadPhoTriggerSel(float eta, float hoe, float r9, float sieie, float phoiso, float chiso, float pt);
  int passTightSieieCuts(float sceta, float sieie);
  int passTightCHisoCuts(float sceta, float chiso, float pt);
  int passTightNHisoCuts(float sceta, float nhiso, float pt);
  int passTightPHisoCuts(float sceta, float phiso, float pt);
  int passTightHoeCuts(float sceta, float hoe);
  int passLooseSieieCuts(float sceta, float sieie);
  int passLooseCHisoCuts(float sceta, float chiso, float pt);
  int passLooseNHisoCuts(float sceta, float nhiso, float pt);
  int passLoosePHisoCuts(float sceta, float phiso, float pt);
  int passLooseHoeCuts(float sceta, float hoe);

  float getSmearingValue(float sceta, float r9, int syst);
  float getScalingValue(int sampleID, float sceta, float r9, int runNumber, int syst);
  float getPtCorrected(float pt, float sceta,float r9, int run, int sampleID);
  float applyEnergySmearing(float pt, float sceta,float r9, int run);
  float applyEnergyScaling(int sampleID, float pt, float sceta,float r9, int run);
  bool geometrical_acceptance(float eta1, float eta2);

  EDGetTokenT<View<reco::Vertex> > vertexToken_;
  edm::EDGetTokenT<edm::View<flashgg::Photon> > photonToken_;
  EDGetTokenT<edm::View<flashgg::DiPhotonCandidate> > diPhotonToken_; 
  EDGetTokenT<edm::View<PileupSummaryInfo> > PileUpToken_; 
  edm::InputTag rhoFixedGrid_;
  EDGetTokenT<vector<flashgg::GenPhotonExtra> > genPhotonExtraToken_;
  edm::InputTag genInfo_;
  EDGetTokenT<View<reco::GenParticle> > genPartToken_;
  std::vector<edm::InputTag> inputTagJets_;
  EDGetTokenT<View<flashgg::Electron> > electronToken_;   
  EDGetTokenT<View<flashgg::Muon> > muonToken_;        
  EDGetTokenT<View<pat::MET> > METToken_;

  EDGetTokenT<edm::TriggerResults> triggerBitsToken_;
  EDGetTokenT<edm::TriggerResults> triggerFlagsToken_;

  string bTag_;    

  typedef std::vector<edm::Handle<edm::View<flashgg::Jet> > > JetCollectionVector;

  // sample-dependent parameters needed for the analysis
  int dopureweight_;
  int sampleIndex_;
  string puWFileName_;
  float xsec_;    // pb
  float kfac_;
  float sumDataset_;

  // to compute weights for pileup
  std::vector<Double_t> puweights_;
  bool doOfficialPUrecipe = true;

  // output tree with several diphoton infos
  TTree *PhotonTree;
  phoTree_struc_ treepho_;

  // to keep track of the number of events
  TH1F *h_entries;

  // to keep track of the sum of weights
  TH1F *h_sumW;
  bool isFilled;

  // numbers to store the passing of cuts
  std::vector< int > numPassingCuts;
  int numCuts = 7;


  // events breakdown
  TH1F *h_selection;

  // counters to get eff
  Int_t eff_start = 0;
  Int_t eff_passingHLT = 0;
  Int_t eff_end = 0;

  //counters Livia
  Int_t totLivia = 0;
  Int_t trigLivia = 0;
  Int_t onerecoLivia = 0;
  Int_t notrigLivia = 0;
  Int_t nomasstrigLivia = 0;
  Int_t noleadtrigLivia = 0;
  Int_t nosubleadtrigLivia = 0;
  Int_t preselLivia = 0;
  Int_t preselHLTLivia = 0;
  Int_t preselAccLivia = 0;
  Int_t preselHoELivia = 0;
  Int_t preselIsoLivia = 0;
  Int_t preselIsoRelLivia = 0;
  Int_t preselR9Livia = 0;
  Int_t selLivia = 0;
  Int_t kinLivia = 0;
  Int_t kinScalLivia = 0;
  Int_t vtxLivia = 0;
  Int_t massLivia = 0;
  Int_t elvetoLivia = 0;

  // 74X only: met filters lists
  EventList listCSC, listEEbadSC;
};
   

NewPhoAnalyzer::NewPhoAnalyzer(const edm::ParameterSet& iConfig):
  // collections
  vertexToken_(consumes<View<reco::Vertex> >(iConfig.getUntrackedParameter<InputTag> ("VertexTag", InputTag("offlineSlimmedPrimaryVertices")))),
  photonToken_(consumes<View<flashgg::Photon> >(iConfig.getUntrackedParameter<InputTag> ("PhotonTag", InputTag("flashggRandomizedPhotons")))), 
  diPhotonToken_(consumes<View<flashgg::DiPhotonCandidate> >(iConfig.getUntrackedParameter<InputTag> ("DiPhotonTag", InputTag("flashggDiPhotons")))),
  PileUpToken_(consumes<View<PileupSummaryInfo> >(iConfig.getUntrackedParameter<InputTag> ("PileUpTag"))),
  genPhotonExtraToken_(mayConsume<vector<flashgg::GenPhotonExtra> >(iConfig.getParameter<InputTag>("genPhotonExtraTag"))),
  genPartToken_(consumes<View<reco::GenParticle> >(iConfig.getUntrackedParameter<InputTag> ("GenParticlesTag", InputTag("flashggPrunedGenParticles")))),
  inputTagJets_( iConfig.getParameter<std::vector<edm::InputTag> >( "inputTagJets" ) ),   
  electronToken_( consumes<View<flashgg::Electron> >( iConfig.getParameter<InputTag>( "ElectronTag" ) ) ),
  muonToken_( consumes<View<flashgg::Muon> >( iConfig.getParameter<InputTag>( "MuonTag" ) ) ), 
  METToken_( consumes<View<pat::MET> >( iConfig.getUntrackedParameter<InputTag> ( "METTag", InputTag( "slimmedMETs" ) ) ) ),
  triggerBitsToken_( consumes<edm::TriggerResults>( iConfig.getParameter<edm::InputTag>( "bits" ) ) ),
  triggerFlagsToken_( consumes<edm::TriggerResults>( iConfig.getParameter<edm::InputTag>( "flags" ) ) )
{ 
  numPassingCuts.resize(numCuts);
  for (int i=0; i<numCuts; i++) numPassingCuts[i]=0;

  dopureweight_ = iConfig.getUntrackedParameter<int>("dopureweight", 0);
  sampleIndex_  = iConfig.getUntrackedParameter<int>("sampleIndex",0);
  puWFileName_  = iConfig.getParameter<std::string>("puWFileName");   
  xsec_         = iConfig.getUntrackedParameter<double>("xsec",1.); 
  kfac_         = iConfig.getUntrackedParameter<double>("kfac",1.); 
  sumDataset_   = iConfig.getUntrackedParameter<double>("sumDataset",-999.);
  genInfo_      = iConfig.getParameter<edm::InputTag>("generatorInfo"); 

  bTag_ = iConfig.getUntrackedParameter<string> ( "bTag", "combinedInclusiveSecondaryVertexV2BJetTags" );   
};

NewPhoAnalyzer::~NewPhoAnalyzer() { 

  std::cout<<"tot:    "<<totLivia<<std::endl;
  std::cout<<"trig:   "<<trigLivia<<std::endl;
  std::cout<<"onereco:   "<<onerecoLivia<<std::endl;
  /*  std::cout<<"notrig:   "<<notrigLivia<<std::endl;
  std::cout<<"nomasstrig:   "<<notrigLivia<<std::endl;
  std::cout<<"noleadtrig:   "<<notrigLivia<<std::endl;
  std::cout<<"nosubleadtrig:   "<<notrigLivia<<std::endl;*/
  std::cout<<"Acc: "<<preselAccLivia<<std::endl;
  std::cout<<"r9: "<<preselR9Livia<<std::endl;
  std::cout<<"Iso: "<<preselIsoLivia<<std::endl;
  std::cout<<"IsoRel: "<<preselIsoRelLivia<<std::endl;
  std::cout<<"HoE: "<<preselHoELivia<<std::endl;
  std::cout<<"presel + HLT:    "<<preselHLTLivia<<std::endl;
  std::cout<<"presel:    "<<preselLivia<<std::endl;
  std::cout<<"sel:    "<<selLivia<<std::endl;
  std::cout<<"elveto: "<<elvetoLivia<<std::endl;
  std::cout<<"kin:    "<<kinLivia<<std::endl;
  std::cout<<"kin_scaling:    "<<kinScalLivia<<std::endl;
  std::cout<<"vtx:    "<<vtxLivia<<std::endl;
  std::cout<<"mass:   "<<massLivia<<std::endl;

 };

void NewPhoAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup){

  // Sample index
  int sampleID = sampleIndex_;

  // access edm objects                                                                                    
  Handle<View<reco::Vertex> > primaryVertices;
  iEvent.getByToken(vertexToken_,primaryVertices);

  Handle<View<flashgg::Photon> > Photons;
  iEvent.getByToken(photonToken_,Photons);

 
  Handle<View< PileupSummaryInfo> > PileupInfos;
  iEvent.getByToken(PileUpToken_,PileupInfos);
  
  Handle<double> objs_rho;
  iEvent.getByLabel("fixedGridRhoAll",objs_rho);

  Handle<vector<flashgg::GenPhotonExtra> > genPhotonsHandle;
  edm::Handle<GenEventInfoProduct> genInfo;
  edm::Handle<View<reco::GenParticle> > genParticles;
 
  if (sampleID>0 && sampleID<10000) {     // MC
    iEvent.getByToken(genPhotonExtraToken_,genPhotonsHandle);
    iEvent.getByLabel(genInfo_,genInfo);   
    iEvent.getByToken( genPartToken_, genParticles );
  }

  // To keep track of the total number of events
  h_entries->Fill(5);

  totLivia++;
  eff_start++;

  Handle<View<pat::MET> > METs;
  iEvent.getByToken( METToken_, METs );

  Handle<edm::TriggerResults> triggerBits;
  iEvent.getByToken( triggerBitsToken_, triggerBits );

  Handle<edm::TriggerResults> triggerFlags;
  iEvent.getByToken( triggerFlagsToken_, triggerFlags );

  Handle<View<flashgg::Muon> > theMuons;           
  iEvent.getByToken( muonToken_, theMuons );   

  Handle<View<flashgg::Electron> > theElectrons;  
  iEvent.getByToken( electronToken_, theElectrons );    
  
 Handle<View<flashgg::DiPhotonCandidate> > diPhotons;
  iEvent.getByToken(diPhotonToken_,diPhotons);


  JetCollectionVector Jets( inputTagJets_.size() );         
  for( size_t j = 0; j < inputTagJets_.size(); ++j ) 
    iEvent.getByLabel( inputTagJets_[j], Jets[j] );

  // --------------------------------------------------
  //std::cout<<"------------------------------"<<std::endl;

  //Trigger info
 
  int hltPhoton22=-500;
  int hltPhoton30=-500;
  int hltPhoton36=-500;
  int hltPhoton50=-500;
  int hltPhoton175=-500;
  int hltPhoton165=-500;
   
  const edm::TriggerNames &triggerNames = iEvent.triggerNames( *triggerBits );
 
  for( unsigned index = 0; index < triggerNames.size(); ++index ) {
   
    // print out triggers that match "HLT_Photon or HLT_Diphoton" and have "Mass" as well
    //if( (TString::Format((triggerNames.triggerName( index )).c_str())).Contains("HLT_Photon") /*&& (TString::Format((triggerNames.triggerName( index )).c_str())).Contains("Mass")  */) cout << index << " " << triggerNames.triggerName( index ) << " " << triggerBits->accept( index ) << endl;
    

    if( (TString::Format((triggerNames.triggerName( index )).c_str())).Contains("HLT_Photon22_v") )hltPhoton22 = triggerBits->accept( index );
    if( (TString::Format((triggerNames.triggerName( index )).c_str())).Contains("HLT_Photon30_v") )hltPhoton30 = triggerBits->accept( index );
    if( (TString::Format((triggerNames.triggerName( index )).c_str())).Contains("HLT_Photon36_v") )hltPhoton36 = triggerBits->accept( index );
    if( (TString::Format((triggerNames.triggerName( index )).c_str())).Contains("HLT_Photon50_v") )hltPhoton50 = triggerBits->accept( index );
    if( (TString::Format((triggerNames.triggerName( index )).c_str())).Contains("HLT_Photon175_v") )hltPhoton175 = triggerBits->accept( index );
    if( (TString::Format((triggerNames.triggerName( index )).c_str())).Contains("HLT_Photon165_HE10_v") )hltPhoton165 = triggerBits->accept( index );
   
}

  int hltOk = hltPhoton175 || hltPhoton165 ||hltPhoton22 || hltPhoton30 ||hltPhoton36 || hltPhoton50 ;
  if (hltOk) eff_passingHLT++;

  // Event info
  int run   = iEvent.eventAuxiliary().run();
  int event = iEvent.eventAuxiliary().event();
  int lumi  = iEvent.eventAuxiliary().luminosityBlock(); 


  // MET flags
  int metF_GV = 1;
  int metF_HBHENoise =1;
  int metF_HBHENoiseIso =1;
  int metF_CSC =1;
  int metF_eeBadSC =1;

  // 76X: everything from miniAOD
  const edm::TriggerNames &flagsNames = iEvent.triggerNames( *triggerFlags );
  for( unsigned index = 0; index < flagsNames.size(); ++index ) {
    if (TString::Format((flagsNames.triggerName( index )).c_str())=="Flag_goodVertices" && !triggerFlags->accept( index )) metF_GV = 0;
    //Flag_HBHENoiseFilter 
    //Flag_HBHENoiseIsoFilter
    //Flag_CSCTightHalo2015Filter 
    if (TString::Format((flagsNames.triggerName( index )).c_str())=="Flag_eeBadScFilter" && !triggerFlags->accept( index )) metF_eeBadSC = 0;
  }

  // 74X: partially to be read from external lists
  EventList::iterator rItrCSC;
  rItrCSC = listCSC.find(run);
  if (rItrCSC != listCSC.end()) {     
    set<unsigned> eventSetCSC = rItrCSC->second;
    set<unsigned>::iterator eItrCSC;
    eItrCSC = eventSetCSC.find(event);
    if (eItrCSC != eventSetCSC.end()) metF_CSC = 0;     
  }
  //
  EventList::iterator rItrEEbadSC;         // this is to kill the 4th bad SC which is not included in the flags in trigger results
  rItrEEbadSC = listEEbadSC.find(run);
  if (rItrEEbadSC != listEEbadSC.end()) {     
    set<unsigned> eventSetEEbadSC = rItrEEbadSC->second;
    set<unsigned>::iterator eItrEEbadSC;
    eItrEEbadSC = eventSetEEbadSC.find(event);
    if (eItrEEbadSC != eventSetEEbadSC.end()) metF_eeBadSC = 0;     
  }

  
  // # Vertices
  int nvtx = primaryVertices->size(); 

  // Energy density
  float rho = *(objs_rho.product());

  // PU weight (for MC only and if requested)
  float pu_weight = 1.;
  float pu_n      = -1.;
  if (sampleID>0 && sampleID<10000) {     // MC
    pu_n = 0.;
    for( unsigned int PVI = 0; PVI < PileupInfos->size(); ++PVI ) {
      Int_t pu_bunchcrossing = PileupInfos->ptrAt( PVI )->getBunchCrossing();
      if( pu_bunchcrossing == 0 ) {
	pu_n = PileupInfos->ptrAt( PVI )->getTrueNumInteractions();
      }
    }
    if (dopureweight_){
        if (doOfficialPUrecipe) pu_weight = GetPUWeight(pu_n);// for Chiara's official PU recipe          
        else pu_weight = GetPUWeight(nvtx); 
    }
  }
 
 // x-sec * kFact for MC only 
  float totXsec = 1.;
  if (sampleID>0 && sampleID<10000) totXsec = xsec_ * kfac_;



  // other weights for the dataset
  float sumDataset = 1.;  
  float perEveW    = 1.;
  if (sampleID>0 && sampleID<10000) { 
    sumDataset = sumDataset_;
    const auto & eveWeights = genInfo->weights();
    if(!eveWeights.empty()) perEveW = eveWeights[0];
  }
 
  // To keep track of the sum of weights
  if (!isFilled) {
    h_sumW->Fill(5,sumDataset);
    isFilled = true;
  }
  if (sampleID>0 && sampleID<10000)hltOk=1;
  // Events breakdown
  if (hltOk){
    //std::cout<<"passing trigger"<<std::endl;
    trigLivia++;
    h_selection->Fill(0.,perEveW);
    numPassingCuts[0]++;
 
   
  // Setup bool to check that events in MC actually pass trigger requirements
 
    // bool passesLeadTrigSel = false;
    // bool passesTrigger = false;

  // Get MET
  if( METs->size() != 1 )
    { std::cout << "WARNING number of MET is not equal to 1" << std::endl; }
  Ptr<pat::MET> theMET = METs->ptrAt( 0 );

  vector<int> kinpho;
  // Loop over photon candidates and choose the most energetic one passing the id
  if (Photons->size()>0) {
    onerecoLivia++;
  
    // Diphoton candidates: preselection
    vector<int> preselpho;
    vector<int> preselHLTpho;
    vector<int> preselphoAcc;
    vector<int> preselphoR9;
    vector<int> preselphoIso;
    vector<int> preselphoIsoRel;
    vector<int> preselphoHoE;
    
    for( size_t photonlooper = 0; photonlooper < Photons->size() /*&& diphotonlooper < 1*/; photonlooper++ ) {

      Ptr<flashgg::Photon> phoPtr = Photons->ptrAt( photonlooper );      
      
      float leadScEta  = (phoPtr->superCluster())->eta();         
      float leadR9noZS = phoPtr->full5x5_r9(); 
      float leadPt     = getPtCorrected(phoPtr->et(), leadScEta,leadR9noZS, run, sampleID);
      float leadSieie  = phoPtr->full5x5_sigmaIetaIeta();
      float leadHoE    = phoPtr->hadTowOverEm();
      float leadChIso  = phoPtr->egChargedHadronIso()- rho * getChargedHadronEAForPhotonIso((phoPtr->superCluster())->eta());     
  
      bool leadPresel  = isGammaPresel( leadScEta, leadPt, leadR9noZS, leadChIso, leadHoE); 

      //rediscovery HLT
      float leadPfPhIso = phoPtr->pfPhoIso03();
      float leadTrkSum03 = phoPtr->trkSumPtHollowConeDR03();
     
      bool leadHLTok = rediscoveryHLT( leadScEta,leadPt, leadR9noZS,leadSieie,leadPfPhIso,leadTrkSum03 );

      //excercise for syncronyzation livia 
      if(fabs(leadScEta)<1.4442 || (fabs(leadScEta)>1.566 && fabs(leadScEta)<2.5)){
	preselphoAcc.push_back(photonlooper);
	if(leadR9noZS>0.8){
	  preselphoR9.push_back(photonlooper);
	  if(leadChIso < 20){
	    preselphoIso.push_back(photonlooper);
	    if( leadChIso/leadPt < 0.3){
	      preselphoIsoRel.push_back(photonlooper);
	      if(leadHoE< 0.08){
		preselphoHoE.push_back(photonlooper);
	      }
	    }
	  }
	}
      }
      if (!leadPresel ) continue;   
      preselpho.push_back(photonlooper);
      if(!leadHLTok )continue;
      preselHLTpho.push_back(photonlooper);
    }
     //excercise for synchronyzation livia 
    if (preselphoAcc.size()>0) {
      preselAccLivia++;
    }
    if (preselphoR9.size()>0) {
      preselR9Livia++;
    }
    if (preselphoIso.size()>0) {
      preselIsoLivia++;
    }
    if (preselphoIsoRel.size()>0) {
      preselIsoRelLivia++;
    }
    if (preselphoHoE.size()>0) {
      preselHoELivia++;
    }
    if (preselpho.size()>0) {
      preselLivia++;
    }
    if (preselHLTpho.size()>0) {
      preselHLTLivia++;
      h_selection->Fill(1.,perEveW);
      numPassingCuts[1]++;
     
      // photon candidates: Id/isolation selection
      vector<int> selectedpho;
      for( size_t photonlooper = 0; photonlooper < preselHLTpho.size(); photonlooper++ ) {

	int thephoton = preselHLTpho[photonlooper];
	Ptr<flashgg::Photon> phoPtr = Photons->ptrAt( thephoton );

	float leadR9noZS = phoPtr->full5x5_r9();
	float leadScEta  = (phoPtr->superCluster())->eta();   	
	float leadPt     = getPtCorrected(phoPtr->et(), leadScEta,leadR9noZS, run, sampleID);
        float leadSieienoZS = phoPtr->full5x5_sigmaIetaIeta();
	float leadHoE    = phoPtr->hadTowOverEm();	
	float leadChIso  = TMath::Max(phoPtr->egChargedHadronIso()- rho * getChargedHadronEAForPhotonIso((phoPtr->superCluster())->eta()),0.);
	float leadNeuIso = TMath::Max(phoPtr->egNeutralHadronIso()- rho * getNeutralHadronEAForPhotonIso((phoPtr->superCluster())->eta()),0.);
	float leadPhoIso = TMath::Max(phoPtr->egPhotonIso()- rho * getGammaEAForPhotonIso((phoPtr->superCluster())->eta()),0.);
	
        // medium working point selection
	int passLeadSieie = passSieieCuts( leadScEta, leadSieienoZS );
        int passLeadCHiso = passCHisoCuts( leadScEta, leadChIso, leadPt );
        int passLeadNHiso = passNHisoCuts( leadScEta, leadNeuIso, leadPt );
        int passLeadPHiso = passPHisoCuts( leadScEta, leadPhoIso, leadPt );
	int passLeadHoe   = passHoeCuts( leadScEta, leadHoE );
        // tight working point selection
	int passTightLeadSieie = passTightSieieCuts( leadScEta, leadSieienoZS );
        int passTightLeadCHiso = passTightCHisoCuts( leadScEta, leadChIso, leadPt );
        int passTightLeadNHiso = passTightNHisoCuts( leadScEta, leadNeuIso, leadPt );
        int passTightLeadPHiso = passTightPHisoCuts( leadScEta, leadPhoIso, leadPt );
	int passTightLeadHoe   = passTightHoeCuts( leadScEta, leadHoE );
	// loose working point selection
	int passLooseLeadSieie = passLooseSieieCuts( leadScEta, leadSieienoZS );
        int passLooseLeadCHiso = passLooseCHisoCuts( leadScEta, leadChIso, leadPt );
        int passLooseLeadNHiso = passLooseNHisoCuts( leadScEta, leadNeuIso, leadPt );
        int passLooseLeadPHiso = passLoosePHisoCuts( leadScEta, leadPhoIso, leadPt );
	int passLooseLeadHoe   = passLooseHoeCuts( leadScEta, leadHoE );
	//eleveto
        int passLeadElVeto = 0;
        int numberpassingEV1 = 0;
	if (phoPtr->passElectronVeto()) passLeadElVeto = 1;
        if (passLeadElVeto) numberpassingEV1++;
	bool leadSelel      = testPhotonIsolation( passLeadSieie, passLeadCHiso, passLeadNHiso, passLeadPHiso, passLeadHoe, 1);//passLeadElVeto);// FIXME 
        bool leadTightSelel = testPhotonIsolation( passTightLeadSieie, passTightLeadCHiso, passTightLeadNHiso, passTightLeadPHiso, passTightLeadHoe, 1); 
        bool leadLooseSelel = testPhotonIsolation( passLooseLeadSieie, passLooseLeadCHiso, passLooseLeadNHiso, passLooseLeadPHiso, passLooseLeadHoe, 1); 


        int numpassingmed = 0;
	int numpassing = 0;
        int numpassingloose = 0;
	if (leadSelel ) numpassingmed++;
	if (leadTightSelel ) numpassing++;
	if (leadLooseSelel ) numpassingloose++;

	if (!leadLooseSelel  ) continue; //loose cut based id
	selectedpho.push_back(thephoton); 
	
      }
     
      if (selectedpho.size()>0) {
	selLivia++;
	h_selection->Fill(2.,perEveW);
	numPassingCuts[2]++;
        
	vector<int> elvetopho;
	for( size_t photonlooper = 0; photonlooper < selectedpho.size(); photonlooper++ ) {
	int thephoton = selectedpho[photonlooper];
	Ptr<flashgg::Photon> phoPtr = Photons->ptrAt( thephoton );
	//eleveto
        int passLeadElVeto = 0;
       
	if (phoPtr->passElectronVeto()) passLeadElVeto = 1;
      	if(!passLeadElVeto)continue;
	elvetopho.push_back(thephoton);
      }
	if (elvetopho.size()>0) {
	  elvetoLivia++;
	  h_selection->Fill(3.,perEveW);
	  numPassingCuts[3]++;
	
	// photon candidates: pT cuts
	  
	  for( size_t photonlooper = 0; photonlooper < elvetopho.size(); photonlooper++ ) {
	    
	    int thephoton = elvetopho[photonlooper];
	    Ptr<flashgg::Photon> phoPtr = Photons->ptrAt( thephoton );
	    
	    float leadR9noZS = phoPtr->full5x5_r9();
	    float leadScEta  = (phoPtr->superCluster())->eta();   	
	    float leadPt     = getPtCorrected(phoPtr->et(), leadScEta,leadR9noZS, run, sampleID);
	     
	    if (leadPt<40) continue;      
	    
	    kinpho.push_back(thephoton);
	  }


     } // elveto
    } // selected
  } // preselected  
  }//one reco
 

  //among all the photons passing the selection i choose the most energetic one
  float candPhPtMax = 0;
  int candPhIndexMax= 999;
  for( size_t photonlooper = 0; photonlooper < kinpho.size(); photonlooper++ ) {
    int thephoton = kinpho[photonlooper];
    Ptr<flashgg::Photon> phoPtr = Photons->ptrAt( thephoton );
    float leadR9noZS = phoPtr->full5x5_r9();
    float leadScEta  = (phoPtr->superCluster())->eta();   	
    float leadPt     = getPtCorrected(phoPtr->et(), leadScEta,leadR9noZS, run, sampleID);
	  
    float pt = getPtCorrected(leadPt, leadScEta,leadR9noZS, run, sampleID);
    if(pt>candPhPtMax){
      candPhPtMax = pt;
      candPhIndexMax = thephoton;
    }
  }
  // if i found a good photon i start to look at jets
  if (candPhIndexMax<999) {
    kinLivia++;
    h_selection->Fill(4.,perEveW);
    numPassingCuts[4]++;
    

    vector<int> kinjet;
    vector<int> kindipho;//to record the dependence of the jet tot he diphoton pair
    float candJetPtMax = 0;
    int candJetIndexMax= 999;
    int candDiPhoIndexMax =999;
  
    Ptr<flashgg::Photon> phoPtr = Photons->ptrAt( candPhIndexMax );
    for( size_t diphotonlooper = 0; diphotonlooper < diPhotons->size() ; diphotonlooper++ ) {
      
      Ptr<flashgg::DiPhotonCandidate> diphoPtr = diPhotons->ptrAt( diphotonlooper );

      unsigned int jetCollectionIndex = diphoPtr->jetCollectionIndex();  
      for( unsigned int jetIndex = 0; jetIndex < Jets[jetCollectionIndex]->size() ; jetIndex++) {
      
	Ptr<flashgg::Jet> jetPtr = Jets[jetCollectionIndex]->ptrAt( jetIndex );     
	
	// jet selection: kinematics and id - hardcoded
	if( fabs( jetPtr->eta() ) > 2.4 ) continue;     // chiara: we only consider central jets 
	if( jetPtr->pt() < 30. ) continue;  
	//  if( !jetPtr->passesPuJetId( candDiphoPtr ) ) continue;   
	
	float dRPhoLeadJet    = deltaR( jetPtr->eta(), jetPtr->phi(), phoPtr->superCluster()->eta(), phoPtr->superCluster()->phi() ) ;
	if( dRPhoLeadJet < 0.3 ) continue;
	//among all the jets passing the selection i choose the most energetic one
	float pt =jetPtr->pt();
	if(pt>candJetPtMax){
	  candJetPtMax = pt;
	  candJetIndexMax = jetIndex;
	  candDiPhoIndexMax = diphotonlooper;
	}
	kinjet.push_back(jetIndex);
	kindipho.push_back(diphotonlooper);
      } // loop over jets
    }

   
  

    //if i found also an energetic jet i go ahead with the selection
    if (candJetIndexMax<999 && candDiPhoIndexMax<999) {
      kinScalLivia++;
      h_selection->Fill(5.,perEveW);
      numPassingCuts[5]++;
      std::cout<<candJetIndexMax<<"   "<<candDiPhoIndexMax<<std::endl;
	     	// to be kept in the tree
	
      //		int eventClass;
		float pt1,ptUncorr1,  eta1, phi1;
		float sceta1;
		float r91, sieie1, hoe1, scRawEne1;
		float chiso1, phoiso1, neuiso1;
		int presel1, sel1, tightsel1, loosesel1;
		//	int genmatch1;
		//	float geniso1;
		int eleveto1;
		float pfmet,pfmetPhi, pfmetSumEt,t1pfmet,t1pfmetPhi, t1pfmetSumEt,calomet,calometPhi, calometSumEt, t1p2pfmet;
		float t1pfmetJetEnUp ,t1pfmetJetEnDown ,t1pfmetJetResUp,t1pfmetJetResDown,t1pfmetMuonEnUp, t1pfmetMuonEnDown,t1pfmetElectronEnUp   ,t1pfmetElectronEnDown   ,t1pfmetTauEnUp,t1pfmetTauEnDown, t1pfmetPhotonEnUp, t1pfmetPhotonEnDown,t1pfmetUnclusteredEnUp,t1pfmetUnclusteredEnDown;
                int passCHiso1, passNHiso1,passPHiso1, passSieie1, passHoe1;
                int passTightCHiso1, passTightNHiso1, passTightPHiso1, passTightSieie1, passTightHoe1;
                int passLooseCHiso1,  passLooseNHiso1, passLoosePHiso1, passLooseSieie1, passLooseHoe1;
		

		float ptJet1, etaJet1, phiJet1, massJet1;
		Ptr<flashgg::DiPhotonCandidate> diphoPtr = diPhotons->ptrAt(candDiPhoIndexMax);
		unsigned int jetCollectionIndex = diphoPtr->jetCollectionIndex(); 
		Ptr<flashgg::Jet> jetPtr = Jets[jetCollectionIndex]->ptrAt( candJetIndexMax );
		ptJet1=jetPtr->pt();
		etaJet1=jetPtr->eta();
		phiJet1=jetPtr->phi();
		massJet1=jetPtr->mass();

		// fully selectedjetPtr event: tree re-initialization                                                                          
		initTreeStructure();        
		
		//met type1 corrected
		t1pfmet = theMET->pt();
		t1pfmetPhi = theMET->phi();
		t1pfmetSumEt = theMET->sumEt();


		//add MET systematic variables Livia
		t1pfmetJetEnUp           = theMET->shiftedPt(pat::MET::JetEnUp);
		t1pfmetJetEnDown         = theMET->shiftedPt(pat::MET::JetEnDown);
		t1pfmetJetResUp          = theMET->shiftedPt(pat::MET::JetResUp);
		t1pfmetJetResDown        = theMET->shiftedPt(pat::MET::JetResDown);
		t1pfmetMuonEnUp          = theMET->shiftedPt(pat::MET::MuonEnUp);
		t1pfmetMuonEnDown          = theMET->shiftedPt(pat::MET::MuonEnDown);
		t1pfmetElectronEnUp   = theMET->shiftedPt(pat::MET::ElectronEnUp);
		t1pfmetElectronEnDown    = theMET->shiftedPt(pat::MET::ElectronEnDown);
		t1pfmetTauEnUp         = theMET->shiftedPt(pat::MET::TauEnUp);
		t1pfmetTauEnDown         = theMET->shiftedPt(pat::MET::TauEnDown);
		t1pfmetPhotonEnUp      = theMET->shiftedPt(pat::MET::PhotonEnUp);
		t1pfmetPhotonEnDown      = theMET->shiftedPt(pat::MET::PhotonEnDown);
	  	t1pfmetUnclusteredEnUp = theMET->shiftedPt(pat::MET::UnclusteredEnUp);
		t1pfmetUnclusteredEnDown = theMET->shiftedPt(pat::MET::UnclusteredEnDown);

		//met correction type 1+2
		t1p2pfmet = theMET->corPt(pat::MET::Type1XY);

		//uncorrected met
		pfmet = theMET->uncorPt();
		pfmetPhi = theMET->uncorPhi();
		pfmetSumEt = theMET->uncorSumEt();
		calomet = theMET->caloMETPt();
		calometPhi = theMET->caloMETPhi();
		calometSumEt = theMET->caloMETSumEt();

	
		//-------> individual photon properties
		sceta1    = phoPtr->superCluster()->eta();
		r91	  =phoPtr->full5x5_r9();
		ptUncorr1       = phoPtr->et();
		pt1     = getPtCorrected(ptUncorr1, sceta1, r91, run, sampleID);
		eta1      = phoPtr->eta();
		phi1      = phoPtr->phi();
		sieie1	  = phoPtr->full5x5_sigmaIetaIeta();
		hoe1      = phoPtr->hadTowOverEm();
		scRawEne1 = phoPtr->superCluster()->rawEnergy();
		chiso1    = TMath::Max(phoPtr->egChargedHadronIso()- rho * getChargedHadronEAForPhotonIso((phoPtr->superCluster())->eta()),0.);
		neuiso1   = TMath::Max(phoPtr->egNeutralHadronIso()- rho * getNeutralHadronEAForPhotonIso((phoPtr->superCluster())->eta()),0.);
		phoiso1   = TMath::Max(phoPtr->egPhotonIso()- rho * getGammaEAForPhotonIso((phoPtr->superCluster())->eta()),0.);

		eleveto1  = 0;
		if (phoPtr->passElectronVeto()) eleveto1 = 1;
	
		//-------> photon selection (should be on, may be useful for extra studies
		presel1 = isGammaPresel( sceta1, pt1, r91, chiso1, hoe1 ); 
	

		//-------> pass each photon ID cut separately
		// medium working point selection
		passSieie1 = passSieieCuts( sceta1, sieie1);
		passCHiso1 = passCHisoCuts( sceta1, chiso1, pt1);
		passNHiso1 = passNHisoCuts( sceta1, neuiso1, pt1);
		passPHiso1 = passPHisoCuts( sceta1, phoiso1, pt1);
		passHoe1   = passHoeCuts( sceta1, hoe1);
	
		// tight working point selection
		passTightSieie1 = passTightSieieCuts( sceta1, sieie1);
		passTightCHiso1 = passTightCHisoCuts( sceta1, chiso1, pt1);
		passTightNHiso1 = passTightNHisoCuts( sceta1, neuiso1, pt1);
		passTightPHiso1 = passTightPHisoCuts( sceta1, phoiso1, pt1);
		passTightHoe1   = passTightHoeCuts( sceta1, hoe1);
		
		// loose working point selection
		passLooseSieie1 = passLooseSieieCuts( sceta1, sieie1);
		passLooseCHiso1 = passLooseCHisoCuts( sceta1, chiso1, pt1);
		passLooseNHiso1 = passLooseNHisoCuts( sceta1, neuiso1, pt1);
		passLoosePHiso1 = passLoosePHisoCuts( sceta1, phoiso1, pt1);
		passLooseHoe1   = passLooseHoeCuts( sceta1, hoe1);
	
 		//-------> pass all photon ID cuts above + electronVeto
		sel1 = testPhotonIsolation( passSieie1, passCHiso1, passNHiso1, passPHiso1, passHoe1, eleveto1 );
		tightsel1 = testPhotonIsolation( passTightSieie1, passTightCHiso1, passTightNHiso1, passTightPHiso1, passTightHoe1, eleveto1 );
		loosesel1 = testPhotonIsolation( passLooseSieie1, passLooseCHiso1, passLooseNHiso1, passLoosePHiso1, passLooseHoe1, eleveto1 );
	
		// Variables for the tree
		treepho_.hltPhoton175 = hltPhoton175;
		treepho_.hltPhoton165 = hltPhoton165;
		treepho_.run = run;
		treepho_.event = event;
		treepho_.lumi = lumi;
		treepho_.nvtx = nvtx;
		treepho_.rho = rho;
		treepho_.sampleID = sampleID;  
		treepho_.totXsec = totXsec;  
		treepho_.pu_weight = pu_weight;
		treepho_.pu_n = pu_n;
		treepho_.sumDataset = sumDataset;
		treepho_.perEveW = perEveW;
		treepho_.pfmet = pfmet;
		treepho_.pfmet = pfmetPhi;
		treepho_.pfmet = pfmetSumEt;
		treepho_.t1pfmet = t1pfmet;
		treepho_.t1p2pfmet = t1p2pfmet;
		treepho_.t1pfmetJetEnUp          = t1pfmetJetEnUp;           
		treepho_.t1pfmetJetEnDown        = t1pfmetJetEnDown        ;
		treepho_.t1pfmetJetResUp         = t1pfmetJetResUp         ;
		treepho_.t1pfmetJetResDown       = t1pfmetJetResDown       ;
		treepho_.t1pfmetMuonEnUp         = t1pfmetMuonEnUp         ;
		treepho_.t1pfmetMuonEnDown         = t1pfmetMuonEnDown         ;
		treepho_.t1pfmetElectronEnUp   = t1pfmetElectronEnUp   ;
		treepho_.t1pfmetElectronEnDown   = t1pfmetElectronEnDown   ;
		treepho_.t1pfmetTauEnUp        = t1pfmetTauEnUp        ;
		treepho_.t1pfmetTauEnDown        = t1pfmetTauEnDown        ;
		treepho_.t1pfmetPhotonEnUp     = t1pfmetPhotonEnUp     ;
		treepho_.t1pfmetPhotonEnDown     = t1pfmetPhotonEnDown     ;
		treepho_.t1pfmetUnclusteredEnUp= t1pfmetUnclusteredEnUp;
		treepho_.t1pfmetUnclusteredEnDown= t1pfmetUnclusteredEnDown;
		treepho_.t1pfmetPhi = t1pfmetPhi;
		treepho_.t1pfmetSumEt = t1pfmetSumEt;
		treepho_.calomet = calomet;
		treepho_.calometPhi = calometPhi;
		treepho_.calometSumEt = calometSumEt;

		//jet
		treepho_.ptJet1 = ptJet1;
		treepho_.etaJet1 = etaJet1;
		treepho_.phiJet1 = phiJet1;
		treepho_.massJet1 = massJet1;

		//		treepho_.eventClass = eventClass;
		treepho_.pt1 = pt1;
		treepho_.ptUncorr1 = ptUncorr1;
		//	treepho_.ptOverM1 = ptOverM1;
		treepho_.eta1 = eta1;
		treepho_.phi1 = phi1;
		treepho_.sceta1 = sceta1;
		treepho_.r91 = r91;
		treepho_.sieie1 = sieie1;
		treepho_.hoe1 = hoe1; 
		treepho_.scRawEne1 = scRawEne1;
		treepho_.chiso1 = chiso1; 
		treepho_.phoiso1 = phoiso1; 
		treepho_.neuiso1 = neuiso1;
		treepho_.eleveto1 = eleveto1;
		treepho_.presel1 = presel1;
		treepho_.sel1 = sel1;
		treepho_.tightsel1 = tightsel1;
		treepho_.loosesel1 = loosesel1;
		//	treepho_.genmatch1 = genmatch1; 
		//	treepho_.geniso1 = geniso1; 
		treepho_.passCHiso1 = passCHiso1;
		treepho_.passNHiso1 = passNHiso1;
		treepho_.passPHiso1 = passPHiso1;
		treepho_.passSieie1 = passSieie1;
		treepho_.passHoe1 = passHoe1;
		treepho_.passTightCHiso1 = passTightCHiso1;
		treepho_.passTightNHiso1 = passTightNHiso1;
		treepho_.passTightPHiso1 = passTightPHiso1;
		treepho_.passTightSieie1 = passTightSieie1;
		treepho_.passTightHoe1 = passTightHoe1;
		treepho_.passLooseCHiso1 = passLooseCHiso1;
		treepho_.passLooseNHiso1 = passLooseNHiso1;
		treepho_.passLoosePHiso1 = passLoosePHiso1;
		treepho_.passLooseSieie1 = passLooseSieie1;
		treepho_.passLooseHoe1 = passLooseHoe1;
		treepho_.metF_GV = metF_GV;
		treepho_.metF_HBHENoise = metF_HBHENoise;
		treepho_.metF_HBHENoiseIso = metF_HBHENoiseIso;
		treepho_.metF_CSC = metF_CSC;
		treepho_.metF_eeBadSC = metF_eeBadSC;
	
		// Filling the trees
		PhotonTree->Fill();
		
	      } // candPhIndex<999
	    } // candJetIndex<999
  }//hlt ok
}


bool NewPhoAnalyzer::geometrical_acceptance(float eta1, float eta2)
{
  float ae1(fabs(eta1));
  float ae2(fabs(eta2));
        return     (ae1 < 1.4442 || (ae1 > 1.566 && ae1 < 2.5))
                && (ae2 < 1.4442 || (ae2 > 1.566 && ae2 < 2.5));
}

void NewPhoAnalyzer::beginJob() {

  // loading weights for pileup if needed
  if (dopureweight_) 
    SetPuWeights(puWFileName_);
  
  // to keep track of the original number of events
  h_entries = fs_->make<TH1F>("h_entries", "h_entries", 10,  0., 10.);
  h_entries->Sumw2();

  // to keep track of the sum of weights
  h_sumW = fs_->make<TH1F>("h_sumW", "h_sumW", 10,  0., 10.);
  h_sumW->Sumw2();
  isFilled = false;
  
  // for the event breakdown
  h_selection = fs_->make<TH1F>("h_selection", "h_selection", 8, -0.5, 7.5);
  h_selection->Sumw2();

  // For 74X only: met filters event lists 
  cout << "now reading met filters lists" << endl;
  listCSC     = readEventListSinglePho("/afs/cern.ch/user/c/crovelli/public/monoH/metFilters/csc2015_Dec01.txt");
  listEEbadSC = readEventListSinglePho("/afs/cern.ch/user/c/crovelli/public/monoH/metFilters/ecalscn1043093_Dec01.txt");
  cout << "met filters lists read" << endl;

  // Trees
  PhotonTree = fs_->make<TTree>("PhotonTree","di-photon tree");

  // with all infos
  PhotonTree->Branch("hltPhoton175",&(treepho_.hltPhoton175),"hltPhoton175/I");
  PhotonTree->Branch("hltPhoton165",&(treepho_.hltPhoton165),"hltPhoton165/I");
  PhotonTree->Branch("run",&(treepho_.run),"run/I");
  PhotonTree->Branch("event",&(treepho_.event),"event/I");
  PhotonTree->Branch("lumi",&(treepho_.lumi),"lumi/I");
  PhotonTree->Branch("nvtx",&(treepho_.nvtx),"nvtx/I");
  PhotonTree->Branch("rho",&(treepho_.rho),"rho/F");
  PhotonTree->Branch("sampleID",&(treepho_.sampleID),"sampleID/I");
  PhotonTree->Branch("totXsec",&(treepho_.totXsec),"totXsec/F");
  PhotonTree->Branch("pu_weight",&(treepho_.pu_weight),"pu_weight/F");
  PhotonTree->Branch("pu_n",&(treepho_.pu_n),"pu_n/F");
  PhotonTree->Branch("sumDataset",&(treepho_.sumDataset),"sumDataset/F");
  PhotonTree->Branch("perEveW",&(treepho_.perEveW),"perEveW/F");
  PhotonTree->Branch("pfmet",&(treepho_.pfmet),"pfmet/F");
  PhotonTree->Branch("pfmetPhi",&(treepho_.pfmetPhi),"pfmetPhi/F");
  PhotonTree->Branch("pfmetSumEt",&(treepho_.pfmetSumEt),"pfmetSumEt/F");
  PhotonTree->Branch("t1pfmet",&(treepho_.t1pfmet),"t1pfmet/F");
  PhotonTree->Branch("t1p2pfmet",&(treepho_.t1p2pfmet),"t1p2pfmet/F");
  PhotonTree->Branch("t1pfmetJetEnUp",&(treepho_.t1pfmetJetEnUp),"t1pfmetJetEnUp/F");         
  PhotonTree->Branch("t1pfmetJetEnDown",&(treepho_.t1pfmetJetEnDown),"t1pfmetJetEnDown/F");        
  PhotonTree->Branch("t1pfmetJetResUp",&(treepho_.t1pfmetJetResUp),"t1pfmetJetResUp/F");         
  PhotonTree->Branch("t1pfmetJetResDown",&(treepho_.t1pfmetJetResDown),"t1pfmetJetResDown/F");       
  PhotonTree->Branch("t1pfmetMuonEnUp",&(treepho_.t1pfmetMuonEnUp),"t1pfmetMuonEnUp/F");         
  PhotonTree->Branch("t1pfmetMuonEnDown",&(treepho_.t1pfmetMuonEnDown),"t1pfmetMuonEnDown/F");         
  PhotonTree->Branch("t1pfmetElectronEnUp",&(treepho_.t1pfmetElectronEnUp),"t1pfmetElectronEnUp/F");   
  PhotonTree->Branch("t1pfmetElectronEnDown",&(treepho_.t1pfmetElectronEnDown),"t1pfmetElectronEnDown/F");   
  PhotonTree->Branch("t1pfmetTauEnUp",&(treepho_.t1pfmetTauEnUp),"t1pfmetTauEnUp/F");        
  PhotonTree->Branch("t1pfmetTauEnDown",&(treepho_.t1pfmetTauEnDown),"t1pfmetTauEnDown/F");        
  PhotonTree->Branch("t1pfmetPhotonEnUp",&(treepho_.t1pfmetPhotonEnUp),"t1pfmetPhotonEnUp/F");     
  PhotonTree->Branch("t1pfmetPhotonEnDown",&(treepho_.t1pfmetPhotonEnDown),"t1pfmetPhotonEnDown/F");     
  PhotonTree->Branch("t1pfmetUnclusteredEnUp",&(treepho_.t1pfmetUnclusteredEnUp),"t1pfmetUnclusteredEnUp/F");
  PhotonTree->Branch("t1pfmetUnclusteredEnDown",&(treepho_.t1pfmetUnclusteredEnDown),"t1pfmetUnclusteredEnDown/F");
  PhotonTree->Branch("t1pfmetPhi",&(treepho_.t1pfmetPhi),"t1pfmetPhi/F");
  PhotonTree->Branch("t1pfmetSumEt",&(treepho_.t1pfmetSumEt),"t1pfmetSumEt/F");
  PhotonTree->Branch("calomet",&(treepho_.calomet),"calomet/F");
  PhotonTree->Branch("calometPhi",&(treepho_.calometPhi),"calometPhi/F");
  PhotonTree->Branch("calometSumEt",&(treepho_.calometSumEt),"calometSumEt/F");
  //
  PhotonTree->Branch("ptJet1",&(treepho_.ptJet1),"ptJet1/F");
  PhotonTree->Branch("etaJet1",&(treepho_.etaJet1),"etaJet1/F");
  PhotonTree->Branch("phiJet1",&(treepho_.phiJet1),"phiJet1/F");
  PhotonTree->Branch("massJet1",&(treepho_.massJet1),"massJet1/F");

  PhotonTree->Branch("pt1",&(treepho_.pt1),"pt1/F");
  PhotonTree->Branch("ptUncorr1",&(treepho_.ptUncorr1),"ptUncorr1/F");
  //  PhotonTree->Branch("ptOverM1",&(treepho_.ptOverM1),"ptOverM1/F");
  PhotonTree->Branch("eta1",&(treepho_.eta1),"eta1/F");
  PhotonTree->Branch("phi1",&(treepho_.phi1),"phi1/F");
  PhotonTree->Branch("sceta1",&(treepho_.sceta1),"sceta1/F");
  PhotonTree->Branch("r91",&(treepho_.r91),"r91/F");
  PhotonTree->Branch("sieie1",&(treepho_.sieie1),"sieie1/F");
  PhotonTree->Branch("hoe1",&(treepho_.hoe1),"hoe1/F");
  PhotonTree->Branch("scRawEne1",&(treepho_.scRawEne1),"scRawEne1/F");
  PhotonTree->Branch("chiso1",&(treepho_.chiso1),"chiso1/F");
  PhotonTree->Branch("phoiso1",&(treepho_.phoiso1),"phoiso1/F");
  PhotonTree->Branch("neuiso1",&(treepho_.neuiso1),"neuiso1/F");
  PhotonTree->Branch("eleveto1",&(treepho_.eleveto1),"eleveto1/I");
  PhotonTree->Branch("presel1",&(treepho_.presel1),"presel1/I");
  PhotonTree->Branch("sel1",&(treepho_.sel1),"sel1/I");
  PhotonTree->Branch("tightsel1",&(treepho_.tightsel1),"tightsel1/I");
  PhotonTree->Branch("loosesel1",&(treepho_.loosesel1),"loosesel1/I");
  // PhotonTree->Branch("genmatch1",&(treepho_.genmatch1),"genmatch1/I");
  // PhotonTree->Branch("geniso1",&(treepho_.geniso1),"geniso1/F");
  PhotonTree->Branch("passCHiso1",&(treepho_.passCHiso1),"passCHiso1/I");
  PhotonTree->Branch("passNHiso1",&(treepho_.passNHiso1),"passNHiso1/I");
  PhotonTree->Branch("passPHiso1",&(treepho_.passPHiso1),"passPHiso1/I");
   PhotonTree->Branch("passSieie1",&(treepho_.passSieie1),"passSieie1/I");
  PhotonTree->Branch("passHoe1",&(treepho_.passHoe1),"passHoe1/I");
  PhotonTree->Branch("passTightCHiso1",&(treepho_.passTightCHiso1),"passTightCHiso1/I");
  PhotonTree->Branch("passTightNHiso1",&(treepho_.passTightNHiso1),"passTightNHiso1/I");
  PhotonTree->Branch("passTightPHiso1",&(treepho_.passTightPHiso1),"passTightPHiso1/I");
  PhotonTree->Branch("passTightSieie1",&(treepho_.passTightSieie1),"passTightSieie1/I");
  PhotonTree->Branch("passTightHoe1",&(treepho_.passTightHoe1),"passTightHoe1/I");
  PhotonTree->Branch("passLooseCHiso1",&(treepho_.passLooseCHiso1),"passLooseCHiso1/I");
  PhotonTree->Branch("passLooseNHiso1",&(treepho_.passLooseNHiso1),"passLooseNHiso1/I");
  PhotonTree->Branch("passLoosePHiso1",&(treepho_.passLoosePHiso1),"passLoosePHiso1/I");
  PhotonTree->Branch("passLooseSieie1",&(treepho_.passLooseSieie1),"passLooseSieie1/I");
  PhotonTree->Branch("passLooseHoe1",&(treepho_.passLooseHoe1),"passLooseHoe1/I");
  PhotonTree->Branch("metF_GV",&(treepho_.metF_GV),"metF_GV/I");
  PhotonTree->Branch("metF_HBHENoise",&(treepho_.metF_HBHENoise),"metF_HBHENoise/I");
  PhotonTree->Branch("metF_HBHENoiseIso",&(treepho_.metF_HBHENoiseIso),"metF_HBHENoiseIso/I");
  PhotonTree->Branch("metF_CSC",&(treepho_.metF_CSC),"metF_CSC/I");
  PhotonTree->Branch("metF_eeBadSC",&(treepho_.metF_eeBadSC),"metF_eeBadSC/I");
 }

void NewPhoAnalyzer::endJob() { }

void NewPhoAnalyzer::initTreeStructure() {
  treepho_.hltPhoton175=-500;
  treepho_.hltPhoton165=-500;
 
  treepho_.run   = -500;
  treepho_.event = -500;
  treepho_.lumi  = -500;
  treepho_.nvtx  = -500;
  treepho_.rho   = -500.;
  treepho_.sampleID  = -500;
  treepho_.totXsec   = -500.;
  treepho_.pu_weight = -500.; 
  treepho_.pu_n = -500.;
  treepho_.sumDataset = -500.;
  treepho_.perEveW = -500.;
  treepho_.pfmet = -500.;
  treepho_.pfmetPhi = -500.;
  treepho_.pfmetSumEt = -500.;
  treepho_.t1pfmet = -500.;
  treepho_.t1pfmetPhi = -500.;
  treepho_.t1pfmetSumEt = -500.;
  treepho_.calomet = -500.;
  treepho_.calometPhi = -500.;
  treepho_.calometSumEt = -500.;
  //  treepho_.eventClass  = -500;
  //
  treepho_.ptJet1  = -500.;
  treepho_.etaJet1  = -500.;
  treepho_.phiJet1  = -500.;
  treepho_.massJet1  = -500.;
  treepho_.pt1  = -500.;
  // treepho_.ptOverM1 = -500.;
  treepho_.eta1 = -500.;
  treepho_.phi1 = -500.;
  treepho_.sceta1 = -500.;
  treepho_.r91  = -500.;
  treepho_.sieie1 = -500.;
  treepho_.hoe1   = -500.;
  treepho_.scRawEne1 = -500.;
  treepho_.chiso1  = -500.;
  treepho_.phoiso1 = -500.;
  treepho_.neuiso1 = -500.;
  treepho_.eleveto1 = -500;
  treepho_.presel1 = -500;
  treepho_.sel1 = -500;
  treepho_.tightsel1 = -500;
  treepho_.loosesel1 = -500;
  // treepho_.genmatch1 = -500;
  // treepho_.geniso1 = -500.;
  treepho_.passCHiso1 = -500;
  treepho_.passNHiso1 = -500;
  treepho_.passPHiso1 = -500;
  treepho_.passSieie1 = -500;
  treepho_.passHoe1 = -500;
  treepho_.passTightCHiso1 = -500;
  treepho_.passTightNHiso1 = -500;
  treepho_.passTightPHiso1 = -500;
  treepho_.passTightSieie1 = -500;
  treepho_.passTightHoe1 = -500;
  treepho_.passLooseCHiso1 = -500;
  treepho_.passLooseNHiso1 = -500;
  treepho_.passLoosePHiso1 = -500;
  treepho_.passLooseSieie1 = -500;
  treepho_.passLooseHoe1 = -500;
  treepho_.metF_GV = -500;
  treepho_.metF_HBHENoise = -500;
  treepho_.metF_HBHENoiseIso = -500;
  treepho_.metF_CSC = -500;
  treepho_.metF_eeBadSC = -500;
}

void NewPhoAnalyzer::SetPuWeights(std::string puWeightFile) {

  if (puWeightFile == "") {
    std::cout << "you need a weights file to use this function" << std::endl;
    return;
  }
  std::cout << "PU REWEIGHTING:: Using file " << puWeightFile << std::endl;

  TFile *f_pu  = new TFile(puWeightFile.c_str(),"READ");
  f_pu->cd();

  TH1D *puweights = 0;
  TH1D *gen_pu = 0; 
  if (doOfficialPUrecipe){ 
    gen_pu    = (TH1D*) f_pu->Get("generated_pu");// for Chiara's offical PU recipe
    puweights = (TH1D*) f_pu->Get("weights");// for Chiara's official PU recipe
  }
  else puweights = (TH1D*) f_pu->Get("nvtx_dataOverMC");// for weighting with nvtx
  //puweights = (TH1D*) f_pu->Get("puhist");// for Livia's old PU file

  if (!puweights){
    std::cout << "puweights histogram not found in file " << puWeightFile << std::endl;
    if (doOfficialPUrecipe && !gen_pu) {
      std::cout << "gen_pu histograms  not found in file " << puWeightFile << std::endl;
      return;
    }
    return;
  }
  
  if (doOfficialPUrecipe){
    TH1D* weightedPU= (TH1D*)gen_pu->Clone("weightedPU");
    weightedPU->Multiply(puweights);
    
    // Rescaling weights in order to preserve same integral of events                               
    TH1D* weights = (TH1D*)puweights->Clone("rescaledWeights");
    weights->Scale( gen_pu->Integral(1,MAX_PU_REWEIGHT) / weightedPU->Integral(1,MAX_PU_REWEIGHT) );
    for (int i = 0; i<MAX_PU_REWEIGHT; i++) {
      float weight=1.;
      weight=weights->GetBinContent(i+1);
      puweights_.push_back(weight);
      //std::cout << "i= " << i << " & has weight = " << weight << std::endl;
    }

  } else {

    float sumPuWeights=0.;
    for (int i = 0; i<MAX_PU_REWEIGHT; i++) {
      float weight=1.;
      weight=puweights->GetBinContent(i+1);
      sumPuWeights+=weight;
      puweights_.push_back(weight);
      //std::cout << "i= " << i << " & has weight = " << weight << std::endl;
    }
  }
}

float NewPhoAnalyzer::GetPUWeight(float pun) {
  
  float weight=1;
  if (sampleIndex_!=0 && pun<MAX_PU_REWEIGHT && puweights_.size()>0 && dopureweight_) 
    weight = puweights_[pun];
  return weight;
}

// miniAOD preselection + ECAL acceptance
bool NewPhoAnalyzer::isGammaPresel( float sceta, float pt, float r9, float chiso, float hoe) {

  bool isPresel = false;

  // ECAL good acceptance
  if (fabs(sceta)<1.4442 || (fabs(sceta)>1.566 && fabs(sceta)<2.5)){
    // miniAOD preselection
    if(r9>0.8 || chiso<20 || (chiso/pt)<0.3){      
      if (hoe<0.08)        isPresel=true;
    } 
  }
  return isPresel;
}

bool NewPhoAnalyzer::rediscoveryHLT(float sceta, float pt, float r9,float sieie, float pfIso,float trkSum03 ){
  bool isEB=false;
  bool isEE=false;
  bool HLTok=false;
  if (fabs(sceta)<1.4442) isEB=true;
  if(fabs(sceta)>1.566 && fabs(sceta)<2.5)isEE=true;
  if(isEB && r9>0.85){
    if(pfIso<100000 && trkSum03<100000&&sieie<100000&& r9>0.5) HLTok = true;
  }else if(isEE && r9>0.9){
    if(pfIso<100000 && trkSum03<100000&&sieie<100000&& r9>0.8) HLTok = true;
  }else if(isEB && r9<=0.85){
    if(pfIso<4 && trkSum03<6&&sieie<0.015&& r9>0.5) HLTok = true;
  }else if(isEE && r9<=0.9){
    if(pfIso<4 && trkSum03<6&&sieie<0.035&& r9>0.8) HLTok = true;
  }

  return HLTok;
}

double NewPhoAnalyzer::getChargedHadronEAForPhotonIso(float eta){
// There is NO correction of EffArea for the CH iso in Spr15 25ns ID
if (fabs(eta) < 1.0) return 0.0;
else if (fabs(eta) >= 1.0 && fabs(eta) < 1.479)  return 0.0;
else if (fabs(eta) >= 1.479 && fabs(eta) < 2.0 ) return 0.0;
else if (fabs(eta) >= 2.0 && fabs(eta) < 2.2 )   return 0.0;
else if (fabs(eta) >= 2.2 && fabs(eta) < 2.3 )   return 0.0;
else if (fabs(eta) >= 2.3 && fabs(eta) < 2.4 )   return 0.0;
else if (fabs(eta) >= 2.4) return 0.0;
else return 0.;
}
double NewPhoAnalyzer::getNeutralHadronEAForPhotonIso(float eta) {
if (fabs(eta) < 1.0) return 0.0599;
else if (fabs(eta) >= 1.0 && fabs(eta) < 1.479)  return 0.0819;
else if (fabs(eta) >= 1.479 && fabs(eta) < 2.0 ) return 0.0696;
else if (fabs(eta) >= 2.0 && fabs(eta) < 2.2 )   return 0.0360;
else if (fabs(eta) >= 2.2 && fabs(eta) < 2.3 )   return 0.0360;
else if (fabs(eta) >= 2.3 && fabs(eta) < 2.4 )   return 0.0462;
else if (fabs(eta) >= 2.4) return 0.0656;
else return 0.;
}

double NewPhoAnalyzer::getGammaEAForPhotonIso(float eta) {
if (fabs(eta) < 1.0) return 0.1271;
else if (fabs(eta) >= 1.0 && fabs(eta) < 1.479)  return 0.1101;
else if (fabs(eta) >= 1.479 && fabs(eta) < 2.0 ) return 0.0756;
else if (fabs(eta) >= 2.0 && fabs(eta) < 2.2 )   return 0.1175;
else if (fabs(eta) >= 2.2 && fabs(eta) < 2.3 )   return 0.1498;
else if (fabs(eta) >= 2.3 && fabs(eta) < 2.4 )   return 0.1857;
else if (fabs(eta) >= 2.4) return 0.2183;
else return 0.;
}

bool NewPhoAnalyzer::LeadPhoTriggerSel(float eta, float hoe, float r9, float sieie, float phoiso, float pt){
  bool passes = false;
  if (fabs(eta)<1.4442 && hoe < 0.1){
    if (r9 > 0.85 || (sieie < 0.015 && phoiso < (6 + 0.12*pt))){
      passes = true;
    }
  }
  else if (fabs(eta)>1.566 && hoe < 0.1){
    if (r9 > 0.9 || (sieie < 0.035 && phoiso < (6 + 0.12*pt))){
      passes = true;
    }
  }  
  return passes;
}

bool NewPhoAnalyzer::SubLeadPhoTriggerSel(float eta, float hoe, float r9, float sieie, float phoiso, float chiso, float pt){
  bool passes = false;
  if (fabs(eta)<1.4442 && hoe < 0.1){
    if (r9 > 0.85 || (sieie < 0.015 && phoiso < (6 + 0.12*pt) && chiso < (6 + 0.002*pt))){
      passes = true;
    }
  }
  else if (fabs(eta)>1.566 && hoe < 0.1){
    if (r9 > 0.9 || (sieie < 0.035 && phoiso < (6 + 0.12*pt) && chiso < (6 + 0.002*pt))){
      passes = true;
    }
  }  
  return passes;
}




// selection for 25ns implemented from EGamma suggestions
// https://twiki.cern.ch/twiki/bin/viewauth/CMS/CutBasedPhotonIdentificationRun2


// medium working point
int NewPhoAnalyzer::passSieieCuts(float sceta, float sieie){
  int passes = 1;
  if (fabs(sceta)<1.4442 && sieie>0.0102) passes = 0;
  if ((fabs(sceta)>1.566 && fabs(sceta)<2.5) && sieie>0.0268) passes = 0;
  return passes;
}
int NewPhoAnalyzer::passCHisoCuts(float sceta, float chiso, float pt){
  int passes = 1;
  if (fabs(sceta)<1.4442 && chiso>1.37) passes = 0;
  if ((fabs(sceta)>1.566 && fabs(sceta)<2.5) && chiso>1.10) passes = 0;
  return passes;
}
int NewPhoAnalyzer::passNHisoCuts(float sceta, float nhiso, float pt){
  int passes = 1;
  if (fabs(sceta)<1.4442 && nhiso > (1.06 + 0.014*pt + 0.000019*pt*pt)) passes = 0;
  if ((fabs(sceta)>1.566 && fabs(sceta)<2.5) && nhiso > (2.69 + 0.0139*pt + 0.000025*pt*pt)) passes = 0;
  return passes;
}
int NewPhoAnalyzer::passPHisoCuts(float sceta, float phiso, float pt){
  int passes = 1;
  if (fabs(sceta)<1.4442 && phiso > (0.28 + 0.0053*pt)) passes = 0;
  if ((fabs(sceta)>1.566 && fabs(sceta)<2.5) && phiso > (0.39 + 0.0034*pt)) passes = 0;
  return passes;
}
int NewPhoAnalyzer::passHoeCuts(float sceta, float hoe){
  int passes = 1;
  if (fabs(sceta)<1.4442 && hoe>0.05) passes = 0;
  if ((fabs(sceta)>1.566 && fabs(sceta)<2.5) && hoe>0.05) passes = 0;
  return passes;
}

// tight working point
int NewPhoAnalyzer::passTightSieieCuts(float sceta, float sieie){
  int passes = 1;
  if (fabs(sceta)<1.4442 && sieie>0.0100) passes = 0;
  if ((fabs(sceta)>1.566 && fabs(sceta)<2.5) && sieie>0.0268) passes = 0;
  return passes;
}
int NewPhoAnalyzer::passTightCHisoCuts(float sceta, float chiso, float pt){
  int passes = 1;
  if (fabs(sceta)<1.4442 && chiso>0.76) passes = 0;
  if ((fabs(sceta)>1.566 && fabs(sceta)<2.5) && chiso>0.56) passes = 0;
  return passes;
}
int NewPhoAnalyzer::passTightNHisoCuts(float sceta, float nhiso, float pt){
  int passes = 1;
  if (fabs(sceta)<1.4442 && nhiso > (0.97 + 0.014*pt + 0.000019*pt*pt)) passes = 0;
  if ((fabs(sceta)>1.566 && fabs(sceta)<2.5) &&  nhiso > (2.09 + 0.0139*pt + 0.000025*pt*pt)) passes = 0;
  return passes;
}
int NewPhoAnalyzer::passTightPHisoCuts(float sceta, float phiso, float pt){
  int passes = 1;
  if (fabs(sceta)<1.4442 && phiso > (0.08 + 0.0053*pt)) passes = 0;
  if ((fabs(sceta)>1.566 && fabs(sceta)<2.5) &&  phiso > (0.16 + 0.0034*pt)) passes = 0;
  return passes;
}
int NewPhoAnalyzer::passTightHoeCuts(float sceta, float hoe){
  int passes = 1;
  if (fabs(sceta)<1.4442 && hoe>0.05) passes = 0;
  if ((fabs(sceta)>1.566 && fabs(sceta)<2.5) &&hoe>0.05) passes = 0;
  return passes;
}

// loose working point
int NewPhoAnalyzer::passLooseSieieCuts(float sceta, float sieie){
  int passes = 1;
  if (fabs(sceta)<1.4442 && sieie>0.0102) passes = 0;
  if ((fabs(sceta)>1.566 && fabs(sceta)<2.5) && sieie>0.0274) passes = 0;
  return passes;
}
int NewPhoAnalyzer::passLooseCHisoCuts(float sceta, float chiso, float pt){
  int passes = 1;
  if (fabs(sceta)<1.4442 && chiso>3.32) passes = 0;
  if ((fabs(sceta)>1.566 && fabs(sceta)<2.5) && chiso>1.97) passes = 0;
  return passes;
}
int NewPhoAnalyzer::passLooseNHisoCuts(float sceta, float nhiso, float pt){
  int passes = 1;
  if (fabs(sceta)<1.4442 && nhiso > (1.92 + 0.014*pt + 0.000019*pt*pt)) passes = 0;
  if ((fabs(sceta)>1.566 && fabs(sceta)<2.5) &&  nhiso > (11.86 + 0.0139*pt + 0.000025*pt*pt)) passes = 0;
  return passes;
}
int NewPhoAnalyzer::passLoosePHisoCuts(float sceta, float phiso, float pt){
  int passes = 1;
  if (fabs(sceta)<1.4442 && phiso > (0.81 + 0.0053*pt)) passes = 0;
  if ((fabs(sceta)>1.566 && fabs(sceta)<2.5) &&  phiso > (0.83 + 0.0034*pt)) passes = 0;
  return passes;
}
int NewPhoAnalyzer::passLooseHoeCuts(float sceta, float hoe){
  int passes = 1;
  if (fabs(sceta)<1.4442 && hoe>0.05) passes = 0;
  if ((fabs(sceta)>1.566 && fabs(sceta)<2.5) &&hoe>0.05) passes = 0;
  return passes;
}

// PhotonID: if all true then valid photonID
bool NewPhoAnalyzer::testPhotonIsolation(int passSieie, int passCHiso, int passNHiso, int passPHiso, int passHoe, int passEleVeto){
  if (passSieie == 1 && passHoe == 1 && passCHiso == 1 && passNHiso == 1 && passPHiso == 1 && passEleVeto == 1) return true; //passes all selection
  else return false;
}



bool NewPhoAnalyzer::isGammaSelected( float rho, float pt, float sceta, float r9, float chiso, float nhiso, float phoiso, float hoe, float sieie, bool passElectronVeto) {
  std::cout<<rho<<" "<<pt<<" "<<sceta<<" "<<r9<<" "<<chiso<<" "<<nhiso<<" "<<phoiso<<" "<<hoe<<" "<<sieie<<" "<<passElectronVeto<<std::endl;
  // classes: 0 = EB highR9, 1 = EB low R9, 2 = EE high R9, 3 = EE lowR9
  int etaclass = fabs(sceta)>1.5;
  int r9class  = r9<0.94;                   
  int theclass = 2.*etaclass + r9class;                  

  // cuts - hardcoded
  float chiso_cut[4]  = { 5.95, 7.08, 6.10, 5.07 };     
  float phoiso_cut[4] = { 2.87, 5.47, 5.98, 3.44 };  
  // float nhiso_cut[4]  = { 27.4, 30.0, 30.0, 15.0 };  
  float sieie_cut[4]  = { 1.05e-02, 1.05e-02, 2.82e-02, 2.8e-02 };
  float hoe_cut[4]    = { 4.53e-01, 2.12e-01, 6.3e-02, 7.8e-02 };
  
  // effective areas - hardcoded 
  float chIsoAE[5] = { 0.00,0.000,0.00,0.00,0.00 };
  float phIsoAE[5] = { 0.21,0.200,0.14,0.22,0.31 };
  // float nhIsoAE[5] = { 0.04,0.059,0.05,0.05,0.15 };

  // EA corrections 
  int theEAregion = effectiveAreaRegion(sceta);
  float corrChIso = chiso - rho*chIsoAE[theEAregion];
  float corrPhIso = phoiso - rho*phIsoAE[theEAregion];
  //float corrChIso = std::max(chiso - rho*chIsoAE[theEAregion],0.);
  //float corrPhIso = std::max(phoiso - rho*phIsoAE[theEAregion],0.);
  // float corrNhIso = nhiso - rho*nhIsoAE[theEAregion];   

  if (corrChIso > chiso_cut[theclass])  return false;
  if (corrPhIso > phoiso_cut[theclass]) return false;
  // if (corrNhIso > nhiso_cut[theclass])  return false;
  if (sieie > sieie_cut[theclass])      return false;
  if (hoe> hoe_cut[theclass])           return false;

  // electron veto 
//  if (!passElectronVeto) return false;//livia

  return true;
} 

int NewPhoAnalyzer::effectiveAreaRegion(float sceta) {

  int theEAregion = 999;
  if (fabs(sceta)<=0.9) theEAregion = 0;
  if (fabs(sceta)<=1.5 && fabs(sceta)>0.9)  theEAregion = 1;
  if (fabs(sceta)<=2.0 && fabs(sceta)>1.5)  theEAregion = 2;
  if (fabs(sceta)<=2.2 && fabs(sceta)>2.0)  theEAregion = 3;
  if (fabs(sceta)<=2.5 && fabs(sceta)>2.2)  theEAregion = 4;
  return theEAregion;
}

float NewPhoAnalyzer::getSmearingValue(float sceta, float r9, int syst){
  float smearingValue = 1.0;
  float smearingError = 1.0;

  // Smearing values below taken from Smearing.txt which comes from:
  // https://gfasanel.web.cern.ch/gfasanel/RUN2_ECAL_Calibration/December2015_Rereco_C_D/step4/outFile-step4-invMass_SC_corr-loose-Et_20-noPF-HggRunEtaR9-smearEle_err.dat

  if (fabs(sceta)<=1.0){
    if (r9 >= 0.94){
      smearingValue = 0.0079581;
      smearingError = 0.00033622;
    }else {
      smearingValue = 0.0093795;
      smearingError = 0.00036528;
  }
  }
  if (fabs(sceta)>1.0 && fabs(sceta)<=1.4442){
    if (r9 >= 0.94){
      smearingValue = 0.011545;
      smearingError = 0.0015758;
    }else{
      smearingValue = 0.018267;
      smearingError = 0.0004384;
    }  
  }
  if (fabs(sceta)>1.4442 && fabs(sceta)<=2.0){
    if (r9 >= 0.94){
      smearingValue = 0.020104;
      smearingError = 0.00092618;
    }else{
      smearingValue = 0.022131;
      smearingError = 0.00064383;
    }
  }
  if (fabs(sceta)>2.0 && fabs(sceta)<=2.5){
    if (r9 >= 0.94){
      smearingValue = 0.022984;
      smearingError = 0.0005424;
    }else {
      smearingValue = 0.026830;
      smearingError = 0.00069514;
    }
  }
  return smearingValue+syst*smearingError;
}

float NewPhoAnalyzer::getScalingValue(int sampleID, float sceta, float r9, int runNumber, int syst){
  float scalingValue = 1.0;
  float scalingError = 1.0;
 
  bool loR9 = false;
  bool hiR9 = false;
  if (r9 >= 0.94) hiR9 = true;
  if (r9  < 0.94) loR9 = true;

  if (sampleID < 10000){ // if MC use the average of scaling for testing
    if (fabs(sceta)<=1.0){
      scalingValue = 0.998825;
      scalingError = 0.0002428;  
    }    
    if (fabs(sceta)>1.0 || fabs(sceta)<=1.442){
      scalingValue = 1.001543;
      scalingError = 0.001057;
    }    
    if (fabs(sceta)>=1.566 || fabs(sceta)<=2.0){
      scalingValue = 1.005211;
      scalingError = 0.001289;
    }    
    if (fabs(sceta)>2.0 || fabs(sceta)<=2.5){
      scalingValue = 1.004596;
      scalingError = 0.001182;
    }    
  }

  else{ // if Data
    // Scaling values below taken from Scaling.txt which comes from:
    // https://gfasanel.web.cern.ch/gfasanel/RUN2_ECAL_Calibration/December2015_Rereco_C_D/step2/step2-invMass_SC_corr-loose-Et_20-noPF-HggRunEtaR9.dat

    if (fabs(sceta)<=1.0){
      if (runNumber >= 254790 && runNumber <= 256629 && loR9) scalingValue = 1.0028;
      if (runNumber >= 254790 && runNumber <= 256629 && hiR9) scalingValue = 0.9987;
      if (runNumber >= 256630 && runNumber <= 257613 && loR9) scalingValue = 0.9993;
      if (runNumber >= 256630 && runNumber <= 257613 && hiR9) scalingValue = 0.9953;
      if (runNumber >= 257614 && runNumber <= 257818 && loR9) scalingValue = 1.0007;
      if (runNumber >= 257614 && runNumber <= 257818 && hiR9) scalingValue = 0.9966;
      if (runNumber >= 257819 && runNumber <= 258158 && loR9) scalingValue = 1.0010;
      if (runNumber >= 257819 && runNumber <= 258158 && hiR9) scalingValue = 0.9970;
      if (runNumber >= 258159 && runNumber <= 258213 && loR9) scalingValue = 1.0006;
      if (runNumber >= 258159 && runNumber <= 258213 && hiR9) scalingValue = 0.9966;
      if (runNumber >= 258214 && runNumber <= 258443 && loR9) scalingValue = 1.0013;
      if (runNumber >= 258214 && runNumber <= 258443 && hiR9) scalingValue = 0.9972;
      if (runNumber >= 258444 && runNumber <= 258704 && loR9) scalingValue = 1.0005;
      if (runNumber >= 258444 && runNumber <= 258704 && hiR9) scalingValue = 0.9965;
      if (runNumber >= 258705 && runNumber <= 258744 && loR9) scalingValue = 1.0011;
      if (runNumber >= 258705 && runNumber <= 258744 && hiR9) scalingValue = 0.9971;
      if (runNumber >= 258745 && runNumber <= 259625 && loR9) scalingValue = 1.0007;
      if (runNumber >= 258745 && runNumber <= 259625 && hiR9) scalingValue = 0.9966;
      if (runNumber >= 259626 && runNumber <= 259810 && loR9) scalingValue = 1.0010;
      if (runNumber >= 259626 && runNumber <= 259810 && hiR9) scalingValue = 0.9970;
      if (runNumber >= 259811 && runNumber <= 259890 && loR9) scalingValue = 1.0011;
      if (runNumber >= 259811 && runNumber <= 259890 && hiR9) scalingValue = 0.9970;
      if (runNumber >= 259891 && runNumber <= 260426 && loR9) scalingValue = 1.0003;
      if (runNumber >= 259891 && runNumber <= 260426 && hiR9) scalingValue = 0.9962;
      if (runNumber >= 260427 && runNumber <= 260535 && loR9) scalingValue = 1.0002;
      if (runNumber >= 260427 && runNumber <= 260535 && hiR9) scalingValue = 0.9962;
      if (runNumber >= 260536 && runNumber <= 260627 && loR9) scalingValue = 1.0013;
      if (runNumber >= 260536 && runNumber <= 260627 && hiR9) scalingValue = 0.9972;
    }
    if (fabs(sceta)>1.0 && fabs(sceta)<=1.4442){
      if (runNumber >= 254790 && runNumber <= 256629 && loR9) scalingValue = 1.0036;
      if (runNumber >= 254790 && runNumber <= 256629 && hiR9) scalingValue = 0.9923;
      if (runNumber >= 256630 && runNumber <= 257613 && loR9) scalingValue = 1.0036;
      if (runNumber >= 256630 && runNumber <= 257613 && hiR9) scalingValue = 0.9922;
      if (runNumber >= 257614 && runNumber <= 257818 && loR9) scalingValue = 1.0078;
      if (runNumber >= 257614 && runNumber <= 257818 && hiR9) scalingValue = 0.9964;
      if (runNumber >= 257819 && runNumber <= 258158 && loR9) scalingValue = 1.0066;
      if (runNumber >= 257819 && runNumber <= 258158 && hiR9) scalingValue = 0.9952;
      if (runNumber >= 258159 && runNumber <= 258213 && loR9) scalingValue = 1.0064;
      if (runNumber >= 258159 && runNumber <= 258213 && hiR9) scalingValue = 0.9951;
      if (runNumber >= 258214 && runNumber <= 258443 && loR9) scalingValue = 1.0072;
      if (runNumber >= 258214 && runNumber <= 258443 && hiR9) scalingValue = 0.9958;
      if (runNumber >= 258444 && runNumber <= 258704 && loR9) scalingValue = 1.0068;
      if (runNumber >= 258444 && runNumber <= 258704 && hiR9) scalingValue = 0.9954;
      if (runNumber >= 258705 && runNumber <= 258744 && loR9) scalingValue = 1.0072;
      if (runNumber >= 258705 && runNumber <= 258744 && hiR9) scalingValue = 0.9958;
      if (runNumber >= 258745 && runNumber <= 259625 && loR9) scalingValue = 1.0075;
      if (runNumber >= 258745 && runNumber <= 259625 && hiR9) scalingValue = 0.9961;
      if (runNumber >= 259626 && runNumber <= 259810 && loR9) scalingValue = 1.0072;
      if (runNumber >= 259626 && runNumber <= 259810 && hiR9) scalingValue = 0.9958;
      if (runNumber >= 259811 && runNumber <= 259890 && loR9) scalingValue = 1.0090;
      if (runNumber >= 259811 && runNumber <= 259890 && hiR9) scalingValue = 0.9976;
      if (runNumber >= 259891 && runNumber <= 260426 && loR9) scalingValue = 1.0096;
      if (runNumber >= 259891 && runNumber <= 260426 && hiR9) scalingValue = 0.9982;
      if (runNumber >= 260427 && runNumber <= 260535 && loR9) scalingValue = 1.0083;
      if (runNumber >= 260427 && runNumber <= 260535 && hiR9) scalingValue = 0.9969;
      if (runNumber >= 260536 && runNumber <= 260627 && loR9) scalingValue = 1.0105;
      if (runNumber >= 260536 && runNumber <= 260627 && hiR9) scalingValue = 0.9991;
    }
    if (fabs(sceta)>=1.566 && fabs(sceta)<=2.0){
      if (runNumber >= 254790 && runNumber <= 256629 && loR9) scalingValue = 1.0138;
      if (runNumber >= 254790 && runNumber <= 256629 && hiR9) scalingValue = 1.0055;
      if (runNumber >= 256630 && runNumber <= 257613 && loR9) scalingValue = 1.0177;
      if (runNumber >= 256630 && runNumber <= 257613 && hiR9) scalingValue = 1.0094;
      if (runNumber >= 257614 && runNumber <= 257818 && loR9) scalingValue = 1.0101;
      if (runNumber >= 257614 && runNumber <= 257818 && hiR9) scalingValue = 1.0018;
      if (runNumber >= 257819 && runNumber <= 258158 && loR9) scalingValue = 1.0102;
      if (runNumber >= 257819 && runNumber <= 258158 && hiR9) scalingValue = 1.0019;
      if (runNumber >= 258159 && runNumber <= 258213 && loR9) scalingValue = 1.0087;
      if (runNumber >= 258159 && runNumber <= 258213 && hiR9) scalingValue = 1.0004;
      if (runNumber >= 258214 && runNumber <= 258443 && loR9) scalingValue = 1.0078;
      if (runNumber >= 258214 && runNumber <= 258443 && hiR9) scalingValue = 0.9995;
      if (runNumber >= 258444 && runNumber <= 258704 && loR9) scalingValue = 1.0080;
      if (runNumber >= 258444 && runNumber <= 258704 && hiR9) scalingValue = 0.9998;
      if (runNumber >= 258705 && runNumber <= 258744 && loR9) scalingValue = 1.0080;
      if (runNumber >= 258705 && runNumber <= 258744 && hiR9) scalingValue = 0.9998;
      if (runNumber >= 258745 && runNumber <= 259625 && loR9) scalingValue = 1.0096;
      if (runNumber >= 258745 && runNumber <= 259625 && hiR9) scalingValue = 1.0013;
      if (runNumber >= 259626 && runNumber <= 259810 && loR9) scalingValue = 1.0092;
      if (runNumber >= 259626 && runNumber <= 259810 && hiR9) scalingValue = 1.0009;
      if (runNumber >= 259811 && runNumber <= 259890 && loR9) scalingValue = 1.0061;
      if (runNumber >= 259811 && runNumber <= 259890 && hiR9) scalingValue = 0.9978;
      if (runNumber >= 259891 && runNumber <= 260426 && loR9) scalingValue = 1.0062;
      if (runNumber >= 259891 && runNumber <= 260426 && hiR9) scalingValue = 0.9980;
      if (runNumber >= 260427 && runNumber <= 260535 && loR9) scalingValue = 1.0061;
      if (runNumber >= 260427 && runNumber <= 260535 && hiR9) scalingValue = 0.9978;
      if (runNumber >= 260536 && runNumber <= 260627 && loR9) scalingValue = 1.0095;
      if (runNumber >= 260536 && runNumber <= 260627 && hiR9) scalingValue = 1.0012;
    }
    if (fabs(sceta)>2.0 && fabs(sceta)<=2.5){
      if (runNumber >= 254790 && runNumber <= 256629 && hiR9) scalingValue = 1.0194;
      if (runNumber >= 254790 && runNumber <= 256629 && loR9) scalingValue = 1.0281;
      if (runNumber >= 256630 && runNumber <= 257613 && hiR9) scalingValue = 1.0096;
      if (runNumber >= 256630 && runNumber <= 257613 && loR9) scalingValue = 1.0182;
      if (runNumber >= 257614 && runNumber <= 257818 && hiR9) scalingValue = 1.0017;
      if (runNumber >= 257614 && runNumber <= 257818 && loR9) scalingValue = 1.0102;
      if (runNumber >= 257819 && runNumber <= 258158 && hiR9) scalingValue = 1.0015;
      if (runNumber >= 257819 && runNumber <= 258158 && loR9) scalingValue = 1.0100;
      if (runNumber >= 258159 && runNumber <= 258213 && hiR9) scalingValue = 0.9987;
      if (runNumber >= 258159 && runNumber <= 258213 && loR9) scalingValue = 1.0072;
      if (runNumber >= 258214 && runNumber <= 258443 && hiR9) scalingValue = 0.9969;
      if (runNumber >= 258214 && runNumber <= 258443 && loR9) scalingValue = 1.0054;
      if (runNumber >= 258444 && runNumber <= 258704 && hiR9) scalingValue = 0.9986;
      if (runNumber >= 258444 && runNumber <= 258704 && loR9) scalingValue = 1.0070;
      if (runNumber >= 258705 && runNumber <= 258744 && hiR9) scalingValue = 0.9990;
      if (runNumber >= 258705 && runNumber <= 258744 && loR9) scalingValue = 1.0075;
      if (runNumber >= 258745 && runNumber <= 259625 && hiR9) scalingValue = 0.9979;
      if (runNumber >= 258745 && runNumber <= 259625 && loR9) scalingValue = 1.0064;
      if (runNumber >= 259626 && runNumber <= 259810 && hiR9) scalingValue = 0.9987;
      if (runNumber >= 259626 && runNumber <= 259810 && loR9) scalingValue = 1.0072;
      if (runNumber >= 259811 && runNumber <= 259890 && hiR9) scalingValue = 0.9964;
      if (runNumber >= 259811 && runNumber <= 259890 && loR9) scalingValue = 1.0049;
      if (runNumber >= 259891 && runNumber <= 260426 && hiR9) scalingValue = 0.9942;
      if (runNumber >= 259891 && runNumber <= 260426 && loR9) scalingValue = 1.0026;
      if (runNumber >= 260427 && runNumber <= 260535 && hiR9) scalingValue = 0.9961;
      if (runNumber >= 260427 && runNumber <= 260535 && loR9) scalingValue = 1.0046;
      if (runNumber >= 260536 && runNumber <= 260627 && hiR9) scalingValue = 0.9961;
      if (runNumber >= 260536 && runNumber <= 260627 && loR9) scalingValue = 1.0046;
    }

    if (fabs(sceta)<=1.0){
      if (runNumber >= 254790 && runNumber <= 256629 && loR9) scalingError = 0.0007;
      if (runNumber >= 254790 && runNumber <= 256629 && hiR9) scalingError = 0.0007;
      if (runNumber >= 256630 && runNumber <= 257613 && loR9) scalingError = 0.0002;
      if (runNumber >= 256630 && runNumber <= 257613 && hiR9) scalingError = 0.0002;
      if (runNumber >= 257614 && runNumber <= 257818 && loR9) scalingError = 0.0002;
      if (runNumber >= 257614 && runNumber <= 257818 && hiR9) scalingError = 0.0002;
      if (runNumber >= 257819 && runNumber <= 258158 && loR9) scalingError = 0.0002;
      if (runNumber >= 257819 && runNumber <= 258158 && hiR9) scalingError = 0.0002;
      if (runNumber >= 258159 && runNumber <= 258213 && loR9) scalingError = 0.0002;
      if (runNumber >= 258159 && runNumber <= 258213 && hiR9) scalingError = 0.0002;
      if (runNumber >= 258214 && runNumber <= 258443 && loR9) scalingError = 0.0002;
      if (runNumber >= 258214 && runNumber <= 258443 && hiR9) scalingError = 0.0002;
      if (runNumber >= 258444 && runNumber <= 258704 && loR9) scalingError = 0.0002;
      if (runNumber >= 258444 && runNumber <= 258704 && hiR9) scalingError = 0.0002;
      if (runNumber >= 258705 && runNumber <= 258744 && loR9) scalingError = 0.0002;
      if (runNumber >= 258705 && runNumber <= 258744 && hiR9) scalingError = 0.0002;
      if (runNumber >= 258745 && runNumber <= 259625 && loR9) scalingError = 0.0003;
      if (runNumber >= 258745 && runNumber <= 259625 && hiR9) scalingError = 0.0003;
      if (runNumber >= 259626 && runNumber <= 259810 && loR9) scalingError = 0.0002;
      if (runNumber >= 259626 && runNumber <= 259810 && hiR9) scalingError = 0.0002;
      if (runNumber >= 259811 && runNumber <= 259890 && loR9) scalingError = 0.0002;
      if (runNumber >= 259811 && runNumber <= 259890 && hiR9) scalingError = 0.0002;
      if (runNumber >= 259891 && runNumber <= 260426 && loR9) scalingError = 0.0002;
      if (runNumber >= 259891 && runNumber <= 260426 && hiR9) scalingError = 0.0002;
      if (runNumber >= 260427 && runNumber <= 260535 && loR9) scalingError = 0.0002;
      if (runNumber >= 260427 && runNumber <= 260535 && hiR9) scalingError = 0.0002;
      if (runNumber >= 260536 && runNumber <= 260627 && loR9) scalingError = 0.0002;
      if (runNumber >= 260536 && runNumber <= 260627 && hiR9) scalingError = 0.0002;
    }									   
    if (fabs(sceta)>1.0 && fabs(sceta)<=1.4442){				   
      if (runNumber >= 254790 && runNumber <= 256629 && loR9) scalingError = 0.0031;
      if (runNumber >= 254790 && runNumber <= 256629 && hiR9) scalingError = 0.0031;
      if (runNumber >= 256630 && runNumber <= 257613 && loR9) scalingError = 0.0007;
      if (runNumber >= 256630 && runNumber <= 257613 && hiR9) scalingError = 0.0008;
      if (runNumber >= 257614 && runNumber <= 257818 && loR9) scalingError = 0.0009;
      if (runNumber >= 257614 && runNumber <= 257818 && hiR9) scalingError = 0.0010;
      if (runNumber >= 257819 && runNumber <= 258158 && loR9) scalingError = 0.0008;
      if (runNumber >= 257819 && runNumber <= 258158 && hiR9) scalingError = 0.0009;
      if (runNumber >= 258159 && runNumber <= 258213 && loR9) scalingError = 0.0009;
      if (runNumber >= 258159 && runNumber <= 258213 && hiR9) scalingError = 0.0010;
      if (runNumber >= 258214 && runNumber <= 258443 && loR9) scalingError = 0.0008;
      if (runNumber >= 258214 && runNumber <= 258443 && hiR9) scalingError = 0.0009;
      if (runNumber >= 258444 && runNumber <= 258704 && loR9) scalingError = 0.0008;
      if (runNumber >= 258444 && runNumber <= 258704 && hiR9) scalingError = 0.0009;
      if (runNumber >= 258705 && runNumber <= 258744 && loR9) scalingError = 0.0008;
      if (runNumber >= 258705 && runNumber <= 258744 && hiR9) scalingError = 0.0009;
      if (runNumber >= 258745 && runNumber <= 259625 && loR9) scalingError = 0.0011;
      if (runNumber >= 258745 && runNumber <= 259625 && hiR9) scalingError = 0.0012;
      if (runNumber >= 259626 && runNumber <= 259810 && loR9) scalingError = 0.0009;
      if (runNumber >= 259626 && runNumber <= 259810 && hiR9) scalingError = 0.0010;
      if (runNumber >= 259811 && runNumber <= 259890 && loR9) scalingError = 0.0009;
      if (runNumber >= 259811 && runNumber <= 259890 && hiR9) scalingError = 0.0010;
      if (runNumber >= 259891 && runNumber <= 260426 && loR9) scalingError = 0.0009;
      if (runNumber >= 259891 && runNumber <= 260426 && hiR9) scalingError = 0.0010;
      if (runNumber >= 260427 && runNumber <= 260535 && loR9) scalingError = 0.0009;
      if (runNumber >= 260427 && runNumber <= 260535 && hiR9) scalingError = 0.0009;
      if (runNumber >= 260536 && runNumber <= 260627 && loR9) scalingError = 0.0007;
      if (runNumber >= 260536 && runNumber <= 260627 && hiR9) scalingError = 0.0008;
    }									   
    if (fabs(sceta)>=1.566 && fabs(sceta)<=2.0){				   
      if (runNumber >= 254790 && runNumber <= 256629 && loR9) scalingError = 0.0034;
      if (runNumber >= 254790 && runNumber <= 256629 && hiR9) scalingError = 0.0034;
      if (runNumber >= 256630 && runNumber <= 257613 && loR9) scalingError = 0.0010;
      if (runNumber >= 256630 && runNumber <= 257613 && hiR9) scalingError = 0.0011;
      if (runNumber >= 257614 && runNumber <= 257818 && loR9) scalingError = 0.0011;
      if (runNumber >= 257614 && runNumber <= 257818 && hiR9) scalingError = 0.0011;
      if (runNumber >= 257819 && runNumber <= 258158 && loR9) scalingError = 0.0010;
      if (runNumber >= 257819 && runNumber <= 258158 && hiR9) scalingError = 0.0011;
      if (runNumber >= 258159 && runNumber <= 258213 && loR9) scalingError = 0.0011;
      if (runNumber >= 258159 && runNumber <= 258213 && hiR9) scalingError = 0.0011;
      if (runNumber >= 258214 && runNumber <= 258443 && loR9) scalingError = 0.0011;
      if (runNumber >= 258214 && runNumber <= 258443 && hiR9) scalingError = 0.0012;
      if (runNumber >= 258444 && runNumber <= 258704 && loR9) scalingError = 0.0010;
      if (runNumber >= 258444 && runNumber <= 258704 && hiR9) scalingError = 0.0011;
      if (runNumber >= 258705 && runNumber <= 258744 && loR9) scalingError = 0.0010;
      if (runNumber >= 258705 && runNumber <= 258744 && hiR9) scalingError = 0.0011;
      if (runNumber >= 258745 && runNumber <= 259625 && loR9) scalingError = 0.0016;
      if (runNumber >= 258745 && runNumber <= 259625 && hiR9) scalingError = 0.0016;
      if (runNumber >= 259626 && runNumber <= 259810 && loR9) scalingError = 0.0011;
      if (runNumber >= 259626 && runNumber <= 259810 && hiR9) scalingError = 0.0012;
      if (runNumber >= 259811 && runNumber <= 259890 && loR9) scalingError = 0.0011;
      if (runNumber >= 259811 && runNumber <= 259890 && hiR9) scalingError = 0.0012;
      if (runNumber >= 259891 && runNumber <= 260426 && loR9) scalingError = 0.0011;
      if (runNumber >= 259891 && runNumber <= 260426 && hiR9) scalingError = 0.0012;
      if (runNumber >= 260427 && runNumber <= 260535 && loR9) scalingError = 0.0011;
      if (runNumber >= 260427 && runNumber <= 260535 && hiR9) scalingError = 0.0012;
      if (runNumber >= 260536 && runNumber <= 260627 && loR9) scalingError = 0.0009;
      if (runNumber >= 260536 && runNumber <= 260627 && hiR9) scalingError = 0.0009;
    }									   
    if (fabs(sceta)>2.0 && fabs(sceta)<=2.5){				   
      if (runNumber >= 254790 && runNumber <= 256629 && hiR9) scalingError = 0.0033;
      if (runNumber >= 254790 && runNumber <= 256629 && loR9) scalingError = 0.0033;
      if (runNumber >= 256630 && runNumber <= 257613 && hiR9) scalingError = 0.0009;
      if (runNumber >= 256630 && runNumber <= 257613 && loR9) scalingError = 0.0009;
      if (runNumber >= 257614 && runNumber <= 257818 && hiR9) scalingError = 0.0010;
      if (runNumber >= 257614 && runNumber <= 257818 && loR9) scalingError = 0.0010;
      if (runNumber >= 257819 && runNumber <= 258158 && hiR9) scalingError = 0.0009;
      if (runNumber >= 257819 && runNumber <= 258158 && loR9) scalingError = 0.0009;
      if (runNumber >= 258159 && runNumber <= 258213 && hiR9) scalingError = 0.0010;
      if (runNumber >= 258159 && runNumber <= 258213 && loR9) scalingError = 0.0010;
      if (runNumber >= 258214 && runNumber <= 258443 && hiR9) scalingError = 0.0010;
      if (runNumber >= 258214 && runNumber <= 258443 && loR9) scalingError = 0.0010;
      if (runNumber >= 258444 && runNumber <= 258704 && hiR9) scalingError = 0.0009;
      if (runNumber >= 258444 && runNumber <= 258704 && loR9) scalingError = 0.0010;
      if (runNumber >= 258705 && runNumber <= 258744 && hiR9) scalingError = 0.0010;
      if (runNumber >= 258705 && runNumber <= 258744 && loR9) scalingError = 0.0010;
      if (runNumber >= 258745 && runNumber <= 259625 && hiR9) scalingError = 0.0014;
      if (runNumber >= 258745 && runNumber <= 259625 && loR9) scalingError = 0.0015;
      if (runNumber >= 259626 && runNumber <= 259810 && hiR9) scalingError = 0.0010;
      if (runNumber >= 259626 && runNumber <= 259810 && loR9) scalingError = 0.0010;
      if (runNumber >= 259811 && runNumber <= 259890 && hiR9) scalingError = 0.0010;
      if (runNumber >= 259811 && runNumber <= 259890 && loR9) scalingError = 0.0011;
      if (runNumber >= 259891 && runNumber <= 260426 && hiR9) scalingError = 0.0011;
      if (runNumber >= 259891 && runNumber <= 260426 && loR9) scalingError = 0.0011;
      if (runNumber >= 260427 && runNumber <= 260535 && hiR9) scalingError = 0.0010;
      if (runNumber >= 260427 && runNumber <= 260535 && loR9) scalingError = 0.0011;
      if (runNumber >= 260536 && runNumber <= 260627 && hiR9) scalingError = 0.0008;
      if (runNumber >= 260536 && runNumber <= 260627 && loR9) scalingError = 0.0009;
    }

  }


  return scalingValue+syst*scalingError;
}


float NewPhoAnalyzer::applyEnergySmearing(float ptUncorr, float sceta, float r9, int run){
  float Smearing	= getSmearingValue( sceta, r9, 0 );   
  TRandom Rand(run);
  float Smear 		= Rand.Gaus(1,Smearing);
  float pt = ptUncorr*Smear;
  return pt;
}

float NewPhoAnalyzer::applyEnergyScaling(int sampleID, float ptUncorr, float sceta, float r9, int run){
    float Scaling	= getScalingValue(sampleID, sceta, r9, run, 0 );
    float pt = ptUncorr*Scaling; 
    return pt;
}

 float NewPhoAnalyzer::getPtCorrected(float ptUncorr, float sceta, float r9, int run, int sampleID){
   float pt;
   if(sampleID>=10000) pt = applyEnergyScaling(sampleID, ptUncorr,sceta,r9, run);// is data
   else pt = applyEnergySmearing(ptUncorr,sceta,r9, run);// is MC
   return pt;
 }

DEFINE_FWK_MODULE(NewPhoAnalyzer);
