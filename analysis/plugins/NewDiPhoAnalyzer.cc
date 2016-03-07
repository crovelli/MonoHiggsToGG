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


#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"

#include "flashgg/DataFormats/interface/Photon.h"
#include "flashgg/DataFormats/interface/DiPhotonCandidate.h"
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

EventList readEventList(char const* _fileName) {
  
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
struct diphoTree_struc_ {

  int hltPhoton26Photon16Mass60;
  int hltPhoton36Photon22Mass15;
  int hltPhoton42Photon25Mass15;
  int hltDiphoton30Mass95;
  int hltDiphoton30Mass70;
  int hltDiphoton30Mass55;
  int hltDiphoton30Mass55PV;
  int hltDiphoton30Mass55EB;
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
  float ptgg;
  float mgg;
  int eventClass;
  float pt1; 
  float ptUncorr1; 
  float ptOverM1; 
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
  float pt2;
  float ptUncorr2;  
  float ptOverM2; 
  float eta2; 
  float phi2;
  float sceta2;
  float r92; 
  float sieie2; 
  float hoe2; 
  float scRawEne2;
  float chiso2; 
  float phoiso2; 
  float neuiso2;
  int eleveto2;
  int presel1;
  int presel2;
  int sel1;
  int sel2;
  int tightsel1;
  int tightsel2;
  int loosesel1;
  int loosesel2;
  float ptJetLead;
  float etaJetLead;
  float phiJetLead;
  float massJetLead;
  int indexJetLead;
  float CHfracJet1;
  float NHfracJet1;
  float NEMfracJet1;
  float CEMfracJet1;
  float ELfracJet1;
  float PHfracJet1;
  float MUfracJet1;
  int CHmultJet1;
  int NEmultJet1;
  float ptJetSubLead;
  float etaJetSubLead;
  float phiJetSubLead;
  float massJetSubLead;
  int indexJetSubLead;
  float CHfracJet2;
  float NHfracJet2;
  float NEMfracJet2;
  float CEMfracJet2;
  float ELfracJet2;
  float PHfracJet2;
  float MUfracJet2;
  int CHmultJet2;
  int NEmultJet2;
  float ptJet3;
  float etaJet3;
  float phiJet3;
  float massJet3;
  int indexJet3;
  float CHfracJet3;
  float NHfracJet3;
  float NEMfracJet3;
  float CEMfracJet3;
  float ELfracJet3;
  float PHfracJet3;
  float MUfracJet3;
  int CHmultJet3;
  int NEmultJet3;
  float ptJet4;
  float etaJet4;
  float phiJet4;
  float massJet4;
  int indexJet4;
  float CHfracJet4;
  float NHfracJet4;
  float NEMfracJet4;
  float CEMfracJet4;
  float ELfracJet4;
  float PHfracJet4;
  float MUfracJet4;
  int CHmultJet4;
  int NEmultJet4;
  int vtxIndex;
  float vtxX; 
  float vtxY; 
  float vtxZ;
  int genmatch1;   
  int genmatch2;
  float genmgg;
  float geniso1;   
  float geniso2;
  float higgsVtxX; 
  float higgsVtxY; 
  float higgsVtxZ; 
  float genVtxX; 
  float genVtxY; 
  float genVtxZ;
  int passCHiso1;
  int passCHiso2;
  int passNHiso1; 
  int passNHiso2;
  int passPHiso1;
  int passPHiso2;
  int passSieie1;
  int passSieie2;
  int passHoe1;
  int passHoe2;
  int passTightCHiso1;
  int passTightCHiso2;
  int passTightNHiso1; 
  int passTightNHiso2;
  int passTightPHiso1;
  int passTightPHiso2;
  int passTightSieie1;
  int passTightSieie2;
  int passTightHoe1;
  int passTightHoe2;
  int passLooseCHiso1;
  int passLooseCHiso2;
  int passLooseNHiso1; 
  int passLooseNHiso2;
  int passLoosePHiso1;
  int passLoosePHiso2;
  int passLooseSieie1;
  int passLooseSieie2;
  int passLooseHoe1;
  int passLooseHoe2;
  int nEle;
  int nMuons;
  int nJets;
  int nLooseBjets;
  int nMediumBjets;
  int vhtruth;
  int metF_GV;
  int metF_HBHENoise;
  int metF_HBHENoiseIso;
  int metF_CSC;
  int metF_eeBadSC;
  int metF_HadronTrackRes;
  int metF_MuonBadTrack;

  float massCorrSmear; 
  float massCorrSmearUp; 
  float massCorrSmearDown; 
  float massCorrScale;
  float massCorrScaleUp;
  float massCorrScaleDown;
  float massRaw;
  int genZ;
  float ptZ;
  float etaZ;
  float phiZ;
  float mva1;
  float mva2;
};


class NewDiPhoAnalyzer : public edm::EDAnalyzer {
  
public:
  
  explicit NewDiPhoAnalyzer(const edm::ParameterSet&);
  ~NewDiPhoAnalyzer();
  
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


  std::vector<edm::EDGetTokenT<View<flashgg::Jet> > > tokenJets_;
  
  EDGetTokenT<View<reco::Vertex> > vertexToken_;
  EDGetTokenT<edm::View<flashgg::DiPhotonCandidate> > diPhotonToken_; 
  EDGetTokenT<edm::View<PileupSummaryInfo> > PileUpToken_;
  EDGetTokenT<double> rhoToken_;
  EDGetTokenT<vector<flashgg::GenPhotonExtra> > genPhotonExtraToken_;
  EDGetTokenT<GenEventInfoProduct> genInfoToken_;
  EDGetTokenT<View<reco::GenParticle> > genPartToken_;
  std::vector<edm::InputTag> inputTagJets_;     
  EDGetTokenT<View<Electron> > electronToken_;   
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
  TTree *DiPhotonTree;
  diphoTree_struc_ treeDipho_;

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
  //EventList listCSC, listEEbadSC, listHadronTrackRes, listMuonBadTrack;
};
   

NewDiPhoAnalyzer::NewDiPhoAnalyzer(const edm::ParameterSet& iConfig):
  // collections
  vertexToken_(consumes<View<reco::Vertex> >(iConfig.getUntrackedParameter<InputTag> ("VertexTag", InputTag("offlineSlimmedPrimaryVertices")))),
  diPhotonToken_(consumes<View<flashgg::DiPhotonCandidate> >(iConfig.getUntrackedParameter<InputTag> ("DiPhotonTag", InputTag("flashggDiPhotons")))),
  PileUpToken_(consumes<View<PileupSummaryInfo> >(iConfig.getUntrackedParameter<InputTag> ("PileUpTag"))),
  rhoToken_(consumes<double>(iConfig.getParameter<InputTag>("RhoTag"))),
  genPhotonExtraToken_(mayConsume<vector<flashgg::GenPhotonExtra> >(iConfig.getParameter<InputTag>("genPhotonExtraTag"))),
  genInfoToken_(consumes<GenEventInfoProduct>(iConfig.getParameter<InputTag>("generatorInfo"))),
  genPartToken_(consumes<View<reco::GenParticle> >(iConfig.getUntrackedParameter<InputTag> ("GenParticlesTag", InputTag("flashggPrunedGenParticles")))),
  inputTagJets_( iConfig.getParameter<std::vector<edm::InputTag> >( "inputTagJets" ) ),   
  electronToken_( consumes<View<flashgg::Electron> >( iConfig.getParameter<InputTag>( "ElectronTag" ) ) ),
  muonToken_( consumes<View<flashgg::Muon> >( iConfig.getParameter<InputTag>( "MuonTag" ) ) ), 
  METToken_( consumes<View<pat::MET> >( iConfig.getUntrackedParameter<InputTag> ( "METTag" ) ) ),
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

  for ( unsigned i = 0 ; i < inputTagJets_.size() ; i++) {
    auto token = consumes<View<flashgg::Jet> >(inputTagJets_[i]);
    tokenJets_.push_back(token);
  }

  bTag_ = iConfig.getUntrackedParameter<string> ( "bTag", "combinedInclusiveSecondaryVertexV2BJetTags" );   
};

inline bool SortByJetPT(const edm::Ptr<flashgg::Jet> & jet1, const edm::Ptr<flashgg::Jet> & jet2){
  return jet1->pt() > jet2->pt();
} 


NewDiPhoAnalyzer::~NewDiPhoAnalyzer() { 

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
  std::cout<<"presel:    "<<preselLivia<<std::endl;
  std::cout<<"presel + HLT:    "<<preselHLTLivia<<std::endl;
  std::cout<<"sel:    "<<selLivia<<std::endl;
  std::cout<<"elveto: "<<elvetoLivia<<std::endl;
  std::cout<<"kin:    "<<kinLivia<<std::endl;
  std::cout<<"kin_scaling:    "<<kinScalLivia<<std::endl;
  std::cout<<"vtx:    "<<vtxLivia<<std::endl;
  std::cout<<"mass:   "<<massLivia<<std::endl;

 
  // std::cout << "Number of Initial Events = " << eff_start << std::endl;
  // std::cout << "Number of Events Passing HLT = " << eff_passingHLT << std::endl;
  //std::cout << "Number Events After Sel. = " << eff_end   << std::endl;
  //std::cout << "passing cuts" << std::endl;
  //for (int i=0; i<numCuts; i++) std::cout << "number passing " << i << " is " << numPassingCuts[i] << std::endl;
};

void NewDiPhoAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup){

  // Sample index
  int sampleID = sampleIndex_;

  // access edm objects                                                                                    
  Handle<View<reco::Vertex> > primaryVertices;
  iEvent.getByToken(vertexToken_,primaryVertices);

  Handle<View<flashgg::DiPhotonCandidate> > diPhotons;
  iEvent.getByToken(diPhotonToken_,diPhotons);

  Handle<View< PileupSummaryInfo> > PileupInfos;
  iEvent.getByToken(PileUpToken_,PileupInfos);
  
  Handle<double> objs_rho;
  iEvent.getByToken(rhoToken_, objs_rho);
  //iEvent.getByLabel("fixedGridRhoAll",objs_rho);

  Handle<vector<flashgg::GenPhotonExtra> > genPhotonsHandle;
  edm::Handle<GenEventInfoProduct> genInfo;
  edm::Handle<View<reco::GenParticle> > genParticles;
 
  if (sampleID>0 && sampleID<10000) {     // MC
    iEvent.getByToken(genPhotonExtraToken_,genPhotonsHandle);
    iEvent.getByToken(genInfoToken_,genInfo);   
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

  
  JetCollectionVector Jets( inputTagJets_.size() );
  for( size_t j = 0; j < inputTagJets_.size(); ++j ) {
    iEvent.getByToken( tokenJets_[j], Jets[j] );
  }
 
  Handle<View<flashgg::Muon> > theMuons;           
  iEvent.getByToken( muonToken_, theMuons );   

  Handle<View<flashgg::Electron> > theElectrons;  
  iEvent.getByToken( electronToken_, theElectrons );    


  // --------------------------------------------------
  // std::cout<<"------------------------------ "<<diPhotons->size()<<" ------------------------------ "<<std::endl;

  //Trigger info
 
  //HLT_Photon26_R9Id85_OR_CaloId24b40e_Iso50T80L_Photon16_AND_HE10_R9Id65_Eta2_Mass60_v2 
  //HLT_Photon36_R9Id85_OR_CaloId24b40e_Iso50T80L_Photon22_AND_HE10_R9Id65_Eta2_Mass15_v2
  //HLT_Photon42_R9Id85_OR_CaloId24b40e_Iso50T80L_Photon25_AND_HE10_R9Id65_Eta2_Mass15_v1
  //HLT_Diphoton30_18_R9Id_OR_IsoCaloId_AND_HE_R9Id_Mass95_v1
  //HLT_Diphoton30_18_R9Id_OR_IsoCaloId_AND_HE_R9Id_DoublePixelSeedMatch_Mass70_v1 
  //HLT_Diphoton30PV_18PV_R9Id_AND_IsoCaloId_AND_HE_R9Id_DoublePixelVeto_Mass55_v1
  //HLT_Diphoton30_18_Solid_R9Id_AND_IsoCaloId_AND_HE_R9Id_Mass55_v1
  //HLT_Diphoton30EB_18EB_R9Id_OR_IsoCaloId_AND_HE_R9Id_DoublePixelVeto_Mass55_v1

  int hltPhoton26Photon16Mass60=-500;
  int hltPhoton36Photon22Mass15=-500;
  int hltPhoton42Photon25Mass15=-500;
  int hltDiphoton30Mass95=-500;
  int hltDiphoton30Mass70=-500;
  int hltDiphoton30Mass55=-500;
  int hltDiphoton30Mass55EB=-500;  
  int hltDiphoton30Mass55PV=-500;

  const edm::TriggerNames &triggerNames = iEvent.triggerNames( *triggerBits );
  //  vector<std::string> const &names = triggerNames.triggerNames();  
  for( unsigned index = 0; index < triggerNames.size(); ++index ) {
    // print out triggers that match "HLT_Photon or HLT_Diphoton" and have "Mass" as well
    //if( (TString::Format((triggerNames.triggerName( index )).c_str())).Contains("HLT_Photon") /*&& (TString::Format((triggerNames.triggerName( index )).c_str())).Contains("Mass") */ ) cout << index << " " << triggerNames.triggerName( index ) << " " << triggerBits->accept( index ) << endl;

    //if( (TString::Format((triggerNames.triggerName( index )).c_str())).Contains("HLT_Diphoton") && (TString::Format((triggerNames.triggerName( index )).c_str())).Contains("Mass")  ) cout << index << " " << triggerNames.triggerName( index ) << " " << triggerBits->accept( index ) << endl;
    //print ALL HLT triggers: 
    //if( (TString::Format((triggerNames.triggerName( index )).c_str())).Contains("HLT") ) cout << index << " " << triggerNames.triggerName( index ) << " " << triggerBits->accept( index ) << endl;

    // store trigger bits of interest
    if( (TString::Format((triggerNames.triggerName( index )).c_str())).Contains("HLT_Photon26") && (TString::Format((triggerNames.triggerName( index )).c_str())).Contains("Photon16")&& (TString::Format((triggerNames.triggerName( index )).c_str())).Contains("Mass60")  )hltPhoton26Photon16Mass60 = triggerBits->accept( index );
    if( (TString::Format((triggerNames.triggerName( index )).c_str())).Contains("HLT_Photon36") && (TString::Format((triggerNames.triggerName( index )).c_str())).Contains("Photon22")&& (TString::Format((triggerNames.triggerName( index )).c_str())).Contains("Mass15")  )hltPhoton36Photon22Mass15 = triggerBits->accept( index );
    if( (TString::Format((triggerNames.triggerName( index )).c_str())).Contains("HLT_Photon42") && (TString::Format((triggerNames.triggerName( index )).c_str())).Contains("Photon25")&& (TString::Format((triggerNames.triggerName( index )).c_str())).Contains("Mass15")  )hltPhoton42Photon25Mass15 = triggerBits->accept( index );
    if( (TString::Format((triggerNames.triggerName( index )).c_str())).Contains("HLT_Diphoton30") && (TString::Format((triggerNames.triggerName( index )).c_str())).Contains("Mass95")  )hltDiphoton30Mass95 = triggerBits->accept( index );
    if( (TString::Format((triggerNames.triggerName( index )).c_str())).Contains("HLT_Diphoton30") && (TString::Format((triggerNames.triggerName( index )).c_str())).Contains("Mass70")  )hltDiphoton30Mass70 = triggerBits->accept( index );
    if( (TString::Format((triggerNames.triggerName( index )).c_str())).Contains("HLT_Diphoton30PV") && (TString::Format((triggerNames.triggerName( index )).c_str())).Contains("DoublePixelVeto_Mass55")  )hltDiphoton30Mass55PV = triggerBits->accept( index );
    if( (TString::Format((triggerNames.triggerName( index )).c_str())).Contains("HLT_Diphoton30") && (TString::Format((triggerNames.triggerName( index )).c_str())).Contains("R9Id_Mass55")  )hltDiphoton30Mass55 = triggerBits->accept( index );
    if( (TString::Format((triggerNames.triggerName( index )).c_str())).Contains("HLT_Diphoton30EB") && (TString::Format((triggerNames.triggerName( index )).c_str())).Contains("DoublePixelVeto_Mass55")  )hltDiphoton30Mass55EB = triggerBits->accept( index );

  }

  if (hltDiphoton30Mass95) eff_passingHLT++;

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
  int metF_HadronTrackRes =1;
  int metF_MuonBadTrack =1;

  // 76X: everything from miniAOD
  const edm::TriggerNames &flagsNames = iEvent.triggerNames( *triggerFlags );
  for( unsigned index = 0; index < flagsNames.size(); ++index ) {
    if (TString::Format((flagsNames.triggerName( index )).c_str())=="Flag_goodVertices" && !triggerFlags->accept( index )) metF_GV = 0;
    if (TString::Format((flagsNames.triggerName( index )).c_str())=="Flag_HBHENoiseFilter" && !triggerFlags->accept( index )) metF_HBHENoise = 0;
    if (TString::Format((flagsNames.triggerName( index )).c_str())=="Flag_HBHENoiseFilterIso" && !triggerFlags->accept( index )) metF_HBHENoiseIso = 0;
    if (TString::Format((flagsNames.triggerName( index )).c_str())=="Flag_CSCTightHalo2015Filter" && !triggerFlags->accept( index )) metF_CSC = 0;
    if (TString::Format((flagsNames.triggerName( index )).c_str())=="Flag_eeBadScFilter" && !triggerFlags->accept( index )) metF_eeBadSC = 0;
  }

  //// 74X: partially to be read from external lists
  //EventList::iterator rItrCSC;
  //rItrCSC = listCSC.find(run);
  //if (rItrCSC != listCSC.end()) {     
  //  set<unsigned> eventSetCSC = rItrCSC->second;
  //  set<unsigned>::iterator eItrCSC;
  //  eItrCSC = eventSetCSC.find(event);
  //  if (eItrCSC != eventSetCSC.end()) metF_CSC = 0;     
  //}
  ////
  //EventList::iterator rItrEEbadSC;         // this is to kill the 4th bad SC which is not included in the flags in trigger results
  //rItrEEbadSC = listEEbadSC.find(run);
  //if (rItrEEbadSC != listEEbadSC.end()) {     
  //  set<unsigned> eventSetEEbadSC = rItrEEbadSC->second;
  //  set<unsigned>::iterator eItrEEbadSC;
  //  eItrEEbadSC = eventSetEEbadSC.find(event);
  //  if (eItrEEbadSC != eventSetEEbadSC.end()) metF_eeBadSC = 0;     
  //}
  ////two new additional filters 74X
  //EventList::iterator rItrHadronTrackRes;         
  //rItrHadronTrackRes = listHadronTrackRes.find(run);
  //if (rItrHadronTrackRes != listHadronTrackRes.end()) {     
  //  set<unsigned> eventSetHadronTrackRes = rItrHadronTrackRes->second;
  //  set<unsigned>::iterator eItrHadronTrackRes;
  //  eItrHadronTrackRes = eventSetHadronTrackRes.find(event);
  //  if (eItrHadronTrackRes != eventSetHadronTrackRes.end()) metF_HadronTrackRes = 0;     
  //}
  //EventList::iterator rItrMuonBadTrack;        
  //rItrMuonBadTrack = listMuonBadTrack.find(run);
  //if (rItrMuonBadTrack != listMuonBadTrack.end()) {     
  //  set<unsigned> eventSetMuonBadTrack = rItrMuonBadTrack->second;
  //  set<unsigned>::iterator eItrMuonBadTrack;
  //  eItrMuonBadTrack = eventSetMuonBadTrack.find(event);
  //  if (eItrMuonBadTrack != eventSetMuonBadTrack.end()) metF_MuonBadTrack = 0;     
  //}

  //if ( metF_CSC == 0) std::cout << "FAILS MET FILTER : CSC " << std::endl;
  //if ( metF_eeBadSC == 0) std::cout << "FAILS MET FILTER : eeBadSC " << std::endl;
  //if ( metF_HadronTrackRes == 0) std::cout << "FAILS MET FILTER : Hadron Track Res " << std::endl;
  //if ( metF_MuonBadTrack == 0) std::cout << "FAILS MET FILTER : Muon Bad Track " << std::endl;
  //if ( metF_GV == 0) std::cout << "FAILS MET FILTER : GV " << std::endl;

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
  if (sampleID>0 && sampleID<10000)hltDiphoton30Mass95=1;
  // Events breakdown
  if (hltDiphoton30Mass95){
    //std::cout<<"passing trigger"<<std::endl;
    trigLivia++;
    h_selection->Fill(0.,perEveW);
    numPassingCuts[0]++;
 
    //if (hltDiphoton30Mass95) std::cout << "  MADE IT PASSED HLT !!!! " << std::endl; 
 
  
    // Get MET
    if( METs->size() != 1 )
      { std::cout << "WARNING number of MET is not equal to 1" << std::endl; }
    Ptr<pat::MET> theMET = METs->ptrAt( 0 );


    // Loop over diphoton candidates
    if (diPhotons->size()>0) {
      onerecoLivia++;
  
      // Diphoton candidates: preselection
      vector<int> preselDipho;
      vector<int> preselHLTDipho;
      vector<int> preselDiphoAcc;
      vector<int> preselDiphoR9;
      vector<int> preselDiphoIso;
      vector<int> preselDiphoIsoRel;
      vector<int> preselDiphoHoE;
    
      for( size_t diphotonlooper = 0; diphotonlooper < diPhotons->size() /*&& diphotonlooper < 1*/; diphotonlooper++ ) {

	Ptr<flashgg::DiPhotonCandidate> diphoPtr = diPhotons->ptrAt( diphotonlooper );      
      
	float leadScEta  = (diphoPtr->leadingPhoton()->superCluster())->eta();         
	float leadR9noZS = diphoPtr->leadingPhoton()->full5x5_r9(); 
	float leadPt     = getPtCorrected(diphoPtr->leadingPhoton()->et(), leadScEta,leadR9noZS, run, sampleID);
	float leadSieie  = diphoPtr->leadingPhoton()->full5x5_sigmaIetaIeta();
	float leadHoE    = diphoPtr->leadingPhoton()->hadTowOverEm();
	float leadChIso  = diphoPtr->leadingPhoton()->egChargedHadronIso()- rho * getChargedHadronEAForPhotonIso((diphoPtr->leadingPhoton()->superCluster())->eta());
     
	bool leadPresel  = isGammaPresel( leadScEta, leadPt, leadR9noZS, leadChIso, leadHoE); 

	float subleadScEta  = (diphoPtr->subLeadingPhoton()->superCluster())->eta(); 
	float subleadR9noZS = diphoPtr->subLeadingPhoton()->full5x5_r9();              
	float subleadPt     = getPtCorrected(diphoPtr->subLeadingPhoton()->et(), leadScEta, subleadR9noZS,run, sampleID);
	float subleadSieie  = diphoPtr->subLeadingPhoton()->full5x5_sigmaIetaIeta(); 
	float subleadHoE    = diphoPtr->subLeadingPhoton()->hadTowOverEm();
	float subleadChIso  = diphoPtr->subLeadingPhoton()->egChargedHadronIso()- rho * getChargedHadronEAForPhotonIso((diphoPtr->subLeadingPhoton()->superCluster())->eta());      
	bool subleadPresel  = isGammaPresel( subleadScEta, subleadPt, subleadR9noZS, subleadChIso, subleadHoE); 

	//rediscovery HLT
	float leadPfPhIso = diphoPtr->leadingPhoton()->pfPhoIso03();
	float leadTrkSum03 = diphoPtr->leadingPhoton()->trkSumPtHollowConeDR03();
	float subleadPfPhIso = diphoPtr->leadingPhoton()->pfPhoIso03();
	float subleadTrkSum03 = diphoPtr->leadingPhoton()->trkSumPtHollowConeDR03();
     
	bool leadHLTok = rediscoveryHLT( leadScEta,leadPt, leadR9noZS,leadSieie,leadPfPhIso,leadTrkSum03 );
	bool subleadHLTok = rediscoveryHLT( subleadScEta,subleadPt, subleadR9noZS,subleadSieie,subleadPfPhIso,subleadTrkSum03 );
      
	//excercise for synchronyzation livia 
	if(geometrical_acceptance(leadScEta,subleadScEta)){
	  preselDiphoAcc.push_back(diphotonlooper);
	  if(subleadR9noZS>0.8 && leadR9noZS>0.8){
	    preselDiphoR9.push_back(diphotonlooper);
	    if(subleadChIso<20 && leadChIso < 20){
	      preselDiphoIso.push_back(diphotonlooper);
	      if(subleadChIso/subleadPt < 0.3 && leadChIso/leadPt < 0.3){
		preselDiphoIsoRel.push_back(diphotonlooper);
		if(subleadHoE <0.08 && leadHoE< 0.08){
		  preselDiphoHoE.push_back(diphotonlooper);
		}
	      }
	    }
	  }
	}
	if (/*!passesTrigger ||*/ !leadPresel || !subleadPresel) continue;   
	preselDipho.push_back(diphotonlooper);
	if(!leadHLTok || !subleadHLTok)continue;
	preselHLTDipho.push_back(diphotonlooper);
      }
      //excercise for synchronyzation livia 
      if (preselDiphoAcc.size()>0) {
	preselAccLivia++;
      }
      if (preselDiphoR9.size()>0) {
	preselR9Livia++;
      }
      if (preselDiphoIso.size()>0) {
	preselIsoLivia++;
      }
      if (preselDiphoIsoRel.size()>0) {
	preselIsoRelLivia++;
      }
      if (preselDiphoHoE.size()>0) {
	preselHoELivia++;
      }
      if (preselDipho.size()>0) {
	preselLivia++;
      }
    
      if (preselHLTDipho.size()>0) {
	preselHLTLivia++;
	h_selection->Fill(1.,perEveW);
	numPassingCuts[1]++;
     
	// Diphoton candidates: Id/isolation selection
	vector<int> selectedDipho;
	for( size_t diphotonlooper = 0; diphotonlooper < preselHLTDipho.size(); diphotonlooper++ ) {

	  int theDiphoton = preselHLTDipho[diphotonlooper];
	  Ptr<flashgg::DiPhotonCandidate> diphoPtr = diPhotons->ptrAt( theDiphoton );

	  float leadR9noZS = diphoPtr->leadingPhoton()->full5x5_r9();
	  float leadScEta  = (diphoPtr->leadingPhoton()->superCluster())->eta();   	
	  float leadPt     = getPtCorrected(diphoPtr->leadingPhoton()->et(), leadScEta,leadR9noZS, run, sampleID);
	  float leadSieienoZS = diphoPtr->leadingPhoton()->full5x5_sigmaIetaIeta();
	  float leadHoE    = diphoPtr->leadingPhoton()->hadTowOverEm();	
	  float leadChIso  = TMath::Max(diphoPtr->leadingPhoton()->egChargedHadronIso()- rho * getChargedHadronEAForPhotonIso((diphoPtr->leadingPhoton()->superCluster())->eta()),0.);
	  float leadNeuIso = TMath::Max(diphoPtr->leadingPhoton()->egNeutralHadronIso()- rho * getNeutralHadronEAForPhotonIso((diphoPtr->leadingPhoton()->superCluster())->eta()),0.);
	  float leadPhoIso = TMath::Max(diphoPtr->leadingPhoton()->egPhotonIso()- rho * getGammaEAForPhotonIso((diphoPtr->leadingPhoton()->superCluster())->eta()),0.);
	
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
	  if (diphoPtr->leadingPhoton()->passElectronVeto()) passLeadElVeto = 1;
	  if (passLeadElVeto) numberpassingEV1++;
	  bool leadSelel      = testPhotonIsolation( passLeadSieie, passLeadCHiso, passLeadNHiso, passLeadPHiso, passLeadHoe, 1);//passLeadElVeto);// FIXME 
	  bool leadTightSelel = testPhotonIsolation( passTightLeadSieie, passTightLeadCHiso, passTightLeadNHiso, passTightLeadPHiso, passTightLeadHoe, 1); 
	  bool leadLooseSelel = testPhotonIsolation( passLooseLeadSieie, passLooseLeadCHiso, passLooseLeadNHiso, passLooseLeadPHiso, passLooseLeadHoe, 1); 

	  //look at subleading
	  float subleadR9noZS = diphoPtr->subLeadingPhoton()->full5x5_r9();
	  float subleadScEta  = (diphoPtr->subLeadingPhoton()->superCluster())->eta();   	
	  float subleadPt     = getPtCorrected(diphoPtr->subLeadingPhoton()->et(), subleadScEta,subleadR9noZS, run, sampleID);
	  float subleadSieienoZS = diphoPtr->subLeadingPhoton()->full5x5_sigmaIetaIeta();
	  float subleadHoE    = diphoPtr->subLeadingPhoton()->hadTowOverEm();
	  float subleadChIso  = TMath::Max(diphoPtr->subLeadingPhoton()->egChargedHadronIso()- rho * getChargedHadronEAForPhotonIso((diphoPtr->subLeadingPhoton()->superCluster())->eta()),0.);
	  float subleadNeuIso = TMath::Max(diphoPtr->subLeadingPhoton()->egNeutralHadronIso()- rho * getNeutralHadronEAForPhotonIso((diphoPtr->subLeadingPhoton()->superCluster())->eta()),0.);
	  float subleadPhoIso = TMath::Max(diphoPtr->subLeadingPhoton()->egPhotonIso()- rho * getGammaEAForPhotonIso((diphoPtr->subLeadingPhoton()->superCluster())->eta()),0.);
       
	
	  // medium working point selection
	  int passSubLeadSieie = passSieieCuts( subleadScEta, subleadSieienoZS );
	  int passSubLeadCHiso = passCHisoCuts( subleadScEta, subleadChIso, subleadPt );
	  int passSubLeadNHiso = passNHisoCuts( subleadScEta, subleadNeuIso, subleadPt );
	  int passSubLeadPHiso = passPHisoCuts( subleadScEta, subleadPhoIso, subleadPt );
	  int passSubLeadHoe   = passHoeCuts( subleadScEta, subleadHoE );
	  // tight working point selection
	  int passTightSubLeadSieie = passTightSieieCuts( subleadScEta, subleadSieienoZS );
	  int passTightSubLeadCHiso = passTightCHisoCuts( subleadScEta, subleadChIso, subleadPt );
	  int passTightSubLeadNHiso = passTightNHisoCuts( subleadScEta, subleadNeuIso, subleadPt );
	  int passTightSubLeadPHiso = passTightPHisoCuts( subleadScEta, subleadPhoIso, subleadPt );
	  int passTightSubLeadHoe   = passTightHoeCuts( subleadScEta, subleadHoE );
	  // loose working point selection
	  int passLooseSubLeadSieie = passLooseSieieCuts( subleadScEta, subleadSieienoZS );
	  int passLooseSubLeadCHiso = passLooseCHisoCuts( subleadScEta, subleadChIso, subleadPt );
	  int passLooseSubLeadNHiso = passLooseNHisoCuts( subleadScEta, subleadNeuIso, subleadPt );
	  int passLooseSubLeadPHiso = passLoosePHisoCuts( subleadScEta, subleadPhoIso, subleadPt );
	  int passLooseSubLeadHoe   = passLooseHoeCuts( subleadScEta, subleadHoE );

      
	  //int passSubLeadElVeto = 0;
	  //int numberpassingEV2 = 0;
	  //if (diphoPtr->subLeadingPhoton()->passElectronVeto()) passSubLeadElVeto = 1;
	  //if (passSubLeadElVeto) numberpassingEV2++;
	  bool subleadSelel      = testPhotonIsolation( passSubLeadSieie, passSubLeadCHiso, passSubLeadNHiso, passSubLeadPHiso, passSubLeadHoe, 1);// passSubLeadElVeto);// FIXME
	  bool subleadTightSelel = testPhotonIsolation( passTightSubLeadSieie, passTightSubLeadCHiso, passTightSubLeadNHiso, passTightSubLeadPHiso, passTightSubLeadHoe, 1);
	  bool subleadLooseSelel = testPhotonIsolation( passLooseSubLeadSieie, passLooseSubLeadCHiso, passLooseSubLeadNHiso, passLooseSubLeadPHiso, passLooseSubLeadHoe, 1);

	  int numpassingmed = 0;
	  int numpassing = 0;
	  int numpassingloose = 0;
	  if (leadSelel || subleadSelel) numpassingmed++;
	  if (leadTightSelel || subleadTightSelel) numpassing++;
	  if (leadLooseSelel || subleadLooseSelel) numpassingloose++;

	  if (!leadLooseSelel || !subleadLooseSelel ) continue; //loose cut based id
	  selectedDipho.push_back(theDiphoton); 
	
	  //// ADDED MVA PHOTON SELECTION
	  //// MVA values come from FLASHgg and replace the Cut-Based Photon ID	
	  //float leadMVA     = diphoPtr->leadingPhoton()->phoIdMvaDWrtVtx(diphoPtr->vtx());
	  //float subleadMVA     = diphoPtr->subLeadingPhoton()->phoIdMvaDWrtVtx(diphoPtr->vtx());
	  //
	  //bool leadMVASel = false;
	  //if (leadMVA > -0.9) leadMVASel = true;
	  //bool subleadMVASel = false;
	  //if (subleadMVA > -0.9) subleadMVASel = true;
	  //if (!leadMVASel || !subleadMVASel) continue;
	  //selectedDipho.push_back(theDiphoton); 
	
	}
     
	if (selectedDipho.size()>0) {
	  selLivia++;
	  h_selection->Fill(2.,perEveW);
	  numPassingCuts[2]++;
        
	  vector<int> elvetoDipho;
	  for( size_t diphotonlooper = 0; diphotonlooper < selectedDipho.size(); diphotonlooper++ ) {
	    int theDiphoton = selectedDipho[diphotonlooper];
	    Ptr<flashgg::DiPhotonCandidate> diphoPtr = diPhotons->ptrAt( theDiphoton );
	    int passSubLeadElVeto = 0;
	    int numberpassingEV2 = 0;
	    if (diphoPtr->subLeadingPhoton()->passElectronVeto()) passSubLeadElVeto = 1;
	    if (passSubLeadElVeto) numberpassingEV2++;
	    //eleveto
	    int passLeadElVeto = 0;
	    int numberpassingEV1 = 0;
	    if (diphoPtr->leadingPhoton()->passElectronVeto()) passLeadElVeto = 1;
	    if (passLeadElVeto) numberpassingEV1++;
	    if(!passLeadElVeto || !passSubLeadElVeto)continue;
	    elvetoDipho.push_back(theDiphoton);
	  }
	  if (elvetoDipho.size()>0) {
	    elvetoLivia++;
	    h_selection->Fill(3.,perEveW);
	    numPassingCuts[3]++;
	
	    // Diphoton candidates: pT cuts
	    vector<int> kineDipho;
	    for( size_t diphotonlooper = 0; diphotonlooper < elvetoDipho.size(); diphotonlooper++ ) {
	    
	      int theDiphoton = elvetoDipho[diphotonlooper];
	      Ptr<flashgg::DiPhotonCandidate> diphoPtr = diPhotons->ptrAt( theDiphoton );
	    
	      float leadR9noZS = diphoPtr->leadingPhoton()->full5x5_r9();
	      float leadScEta  = (diphoPtr->leadingPhoton()->superCluster())->eta();   	
	      float leadPt     = getPtCorrected(diphoPtr->leadingPhoton()->et(), leadScEta,leadR9noZS, run, sampleID);
	    
	      float subleadR9noZS = diphoPtr->subLeadingPhoton()->full5x5_r9();
	      float subleadScEta  = (diphoPtr->subLeadingPhoton()->superCluster())->eta();   	
	      float subleadPt     = getPtCorrected(diphoPtr->subLeadingPhoton()->et(), subleadScEta,subleadR9noZS, run, sampleID);
	    
	      if (leadPt<30 || subleadPt<20) continue;      
	    
	      kineDipho.push_back(theDiphoton);
	    }

	    if (kineDipho.size()>0) {
	      kinLivia++;
	      h_selection->Fill(4.,perEveW);
	      numPassingCuts[4]++;
         
	      // Diphoton candidates: mgg cut
	      vector<int> kinScalDipho;
	      for( size_t diphotonlooper = 0; diphotonlooper < kineDipho.size(); diphotonlooper++ ) {

		int theDiphoton = kineDipho[diphotonlooper];
		Ptr<flashgg::DiPhotonCandidate> diphoPtr = diPhotons->ptrAt( theDiphoton );
	    
		//float thisSystemMgg = diphoPtr->mass();

	   
		float leadR9noZS = diphoPtr->leadingPhoton()->full5x5_r9();
		float leadScEta  = (diphoPtr->leadingPhoton()->superCluster())->eta();   	
		float leadPhi  = diphoPtr->leadingPhoton()->phi();   	
		float leadPt     = getPtCorrected(diphoPtr->leadingPhoton()->et(), leadScEta,leadR9noZS, run, sampleID);
	 
		float subleadR9noZS = diphoPtr->subLeadingPhoton()->full5x5_r9();
		float subleadScEta  = (diphoPtr->subLeadingPhoton()->superCluster())->eta();   	
		float subleadPhi  = diphoPtr->subLeadingPhoton()->phi();   	
		float subleadPt     = getPtCorrected(diphoPtr->subLeadingPhoton()->et(), subleadScEta,subleadR9noZS, run, sampleID);
      
		TLorentzVector* p1=new TLorentzVector(0,0,0,0);;
		TLorentzVector* p2=new TLorentzVector(0,0,0,0);;
		p1->SetPtEtaPhiM(leadPt,diphoPtr->leadingPhoton()->eta() , leadPhi, 0);
		p2->SetPtEtaPhiM(subleadPt, diphoPtr->subLeadingPhoton()->eta(), subleadPhi, 0);
		float thisSystemMggCorr = (*p1+*p2).M();

	
		// if (thisSystemMggCorr<50 ) continue; 
		if (leadPt< thisSystemMggCorr/3 || subleadPt<thisSystemMggCorr/4) continue; //Livia correction: add scaling pt cuts

		kinScalDipho.push_back(theDiphoton);
	      }

	      if (kinScalDipho.size()>0) {
		kinScalLivia++;
		h_selection->Fill(5.,perEveW);
		numPassingCuts[5]++;
            
		vector<int> vtxDipho;
		for( size_t diphotonlooper = 0; diphotonlooper < kinScalDipho.size(); diphotonlooper++ ) {  
		  int theDiphoton = kinScalDipho[diphotonlooper];
		  Ptr<flashgg::DiPhotonCandidate> candDiphoPtr = diPhotons->ptrAt( theDiphoton );
		  bool goodVtx = true;
		  int theVertex = candDiphoPtr->vertexIndex();
		  float vtxX = (primaryVertices->ptrAt(theVertex))->position().x();
		  float vtxY = (primaryVertices->ptrAt(theVertex))->position().y();
		  float d0vtx = sqrt( vtxX*vtxX + vtxY*vtxY );
		  if ( (primaryVertices->ptrAt(theVertex))->ndof()<=4 )  goodVtx = false;
		  if (fabs(d0vtx)>2) goodVtx = false;
		  if (fabs((primaryVertices->ptrAt(theVertex))->position().z())>=24) goodVtx = false;
		  bool isVtxFake = ((primaryVertices->ptrAt(theVertex))->ndof()==0) && ((primaryVertices->ptrAt(theVertex))->chi2()==0);   // chiara: also && tracks.empty, but can not be used here
		  if (isVtxFake) goodVtx = false;
		  if(!goodVtx)continue;
		  vtxDipho.push_back(theDiphoton);
		}
	    
		if(vtxDipho.size()>0){
		  vtxLivia++;
		  h_selection->Fill(6.,perEveW);
		  numPassingCuts[6]++;
		  vector<int> massDipho;
		  for( size_t diphotonlooper = 0; diphotonlooper < vtxDipho.size(); diphotonlooper++ ) {  
		    int theDiphoton = vtxDipho[diphotonlooper];
		    Ptr<flashgg::DiPhotonCandidate> diphoPtr = diPhotons->ptrAt( theDiphoton );
		    float theMass = diphoPtr->mass();
		
		    //correct mass for smearing and scaling
		    float leadR9noZS = diphoPtr->leadingPhoton()->full5x5_r9();
		    float leadScEta  = (diphoPtr->leadingPhoton()->superCluster())->eta();   	
		    float subleadR9noZS = diphoPtr->subLeadingPhoton()->full5x5_r9();
		    float subleadScEta  = (diphoPtr->subLeadingPhoton()->superCluster())->eta();   	
	      
		    float leadSmearing	= getSmearingValue( leadScEta, leadR9noZS,0);
		    float subleadSmearing	= getSmearingValue( subleadScEta,subleadR9noZS  ,0);

		    float gaussMean		= 1.0;
              	
		    TRandom Rand1(event);
		    float Smear1 		= Rand1.Gaus(gaussMean,leadSmearing);
		    TRandom Rand2(event+83941);
		    float Smear2 		= Rand2.Gaus(gaussMean,subleadSmearing);
		    float massCorrSmear	= theMass*sqrt(Smear1*Smear2);
		
		    // scaling of Data
		    float leadScaling	= getScalingValue( sampleID, leadScEta, leadR9noZS , run, 0);
		    float subleadScaling	= getScalingValue( sampleID, subleadScEta, subleadR9noZS, run, 0);
		    float Scaling		= leadScaling*subleadScaling;
		    float massCorrScale	= theMass*sqrt(Scaling);

		    float theMassCorr = theMass;
	
		    // final theMassCorr (has Smearing or Scaling applied)
		    if (sampleID>0 && sampleID<10000){
		      theMassCorr = massCorrSmear;	  // smear mass for MC
		    }
		    else theMassCorr = massCorrScale; // scale mass for Data
		
		    if (theMassCorr <= 100 || theMassCorr >= 180) continue;
		    massDipho.push_back(theDiphoton);
		  }

		  if(massDipho.size()>0){
		    massLivia++;
		    h_selection->Fill(7.,perEveW);
		    numPassingCuts[7]++;
		
		    // Diphoton candidates choice: highest scalar sum pT
		    float maxSumPt = -999.;
		    int candIndex = 9999; // This int will store the index of the best diphoton candidate
		    for( size_t diphotonlooper = 0; diphotonlooper < massDipho.size(); diphotonlooper++ ) {  
		      int theDiphoton = massDipho[diphotonlooper];
		      Ptr<flashgg::DiPhotonCandidate> diphoPtr = diPhotons->ptrAt( theDiphoton );
		      float thisSumPt = diphoPtr->leadingPhoton()->et() + diphoPtr->subLeadingPhoton()->et();
		      if (thisSumPt>maxSumPt) {
			maxSumPt = thisSumPt;
			candIndex = theDiphoton;
		      }
		    }
	    
		    if (candIndex<999) {
	      
		      Ptr<flashgg::DiPhotonCandidate> candDiphoPtr = diPhotons->ptrAt( candIndex );
		 
		      // to be kept in the tree
		      float ptgg, mgg;
		      int eventClass;
		      float pt1,ptUncorr1, ptOverM1, eta1, phi1;
		      float sceta1;
		      float r91, sieie1, hoe1, scRawEne1;
		      float chiso1, phoiso1, neuiso1;
		      float pt2, ptUncorr2,ptOverM2, eta2, phi2;
		      float sceta2;
		      float r92, sieie2, hoe2, scRawEne2;
		      float chiso2, phoiso2, neuiso2;
		      int presel1, presel2, sel1, sel2, tightsel1, tightsel2, loosesel1, loosesel2;
		      int vtxIndex;
		      float vtxX, vtxY, vtxZ;
		      int genmatch1, genmatch2;
		      float genmgg;
		      float geniso1, geniso2;
		      float higgsVtxX, higgsVtxY, higgsVtxZ;
		      float genVtxX, genVtxY, genVtxZ; 
		      int eleveto1, eleveto2;
		      float pfmet,pfmetPhi, pfmetSumEt,t1pfmet,t1pfmetPhi, t1pfmetSumEt,calomet,calometPhi, calometSumEt, t1p2pfmet;
		      float t1pfmetJetEnUp ,t1pfmetJetEnDown ,t1pfmetJetResUp,t1pfmetJetResDown,t1pfmetMuonEnUp, t1pfmetMuonEnDown,t1pfmetElectronEnUp   ,t1pfmetElectronEnDown   ,t1pfmetTauEnUp,t1pfmetTauEnDown, t1pfmetPhotonEnUp, t1pfmetPhotonEnDown,t1pfmetUnclusteredEnUp,t1pfmetUnclusteredEnDown;
		      int passCHiso1, passCHiso2, passNHiso1, passNHiso2, passPHiso1, passPHiso2, passSieie1, passSieie2, passHoe1, passHoe2;
		      int passTightCHiso1, passTightCHiso2, passTightNHiso1, passTightNHiso2, passTightPHiso1, passTightPHiso2, passTightSieie1, passTightSieie2, passTightHoe1, passTightHoe2;
		      int passLooseCHiso1, passLooseCHiso2, passLooseNHiso1, passLooseNHiso2, passLoosePHiso1, passLoosePHiso2, passLooseSieie1, passLooseSieie2, passLooseHoe1, passLooseHoe2;
		      int nEle, nMuons, nJets, nLooseBjets, nMediumBjets;
		      int vhtruth;

		      float massCorrSmear, massCorrScale, massRaw;
		      float massCorrSmearUp, massCorrSmearDown;
		      float massCorrScaleUp, massCorrScaleDown;
		      int genZ;
		      float ptZ, etaZ, phiZ;
		      float mva1, mva2;

		      // fully selected event: tree re-initialization                                                                          
		      initTreeStructure();        
		
		      //met type1 corrected
		      t1pfmet = theMET->pt();
		      t1pfmetPhi = theMET->phi();
		      t1pfmetSumEt = theMET->sumEt();


		      //add MET systematic variables Livia
		      t1pfmetJetEnUp		= theMET->shiftedPt(pat::MET::JetEnUp);
		      t1pfmetJetEnDown	= theMET->shiftedPt(pat::MET::JetEnDown);
		      t1pfmetJetResUp		= theMET->shiftedPt(pat::MET::JetResUp);
		      t1pfmetJetResDown	= theMET->shiftedPt(pat::MET::JetResDown);
		      t1pfmetMuonEnUp		= theMET->shiftedPt(pat::MET::MuonEnUp);
		      t1pfmetMuonEnDown	= theMET->shiftedPt(pat::MET::MuonEnDown);
		      t1pfmetElectronEnUp	= theMET->shiftedPt(pat::MET::ElectronEnUp);
		      t1pfmetElectronEnDown	= theMET->shiftedPt(pat::MET::ElectronEnDown);
		      t1pfmetTauEnUp		= theMET->shiftedPt(pat::MET::TauEnUp);
		      t1pfmetTauEnDown	= theMET->shiftedPt(pat::MET::TauEnDown);
		      t1pfmetPhotonEnUp	= theMET->shiftedPt(pat::MET::PhotonEnUp);
		      t1pfmetPhotonEnDown	= theMET->shiftedPt(pat::MET::PhotonEnDown);
		      t1pfmetUnclusteredEnUp	= theMET->shiftedPt(pat::MET::UnclusteredEnUp);
		      t1pfmetUnclusteredEnDown= theMET->shiftedPt(pat::MET::UnclusteredEnDown);

		      //met correction type 1+2
		      t1p2pfmet = theMET->corPt(pat::MET::Type1XY);

		      //uncorrected met
		      pfmet = theMET->uncorPt();
		      pfmetPhi = theMET->uncorPhi();
		      pfmetSumEt = theMET->uncorSumEt();
		      calomet = theMET->caloMETPt();
		      calometPhi = theMET->caloMETPhi();
		      calometSumEt = theMET->caloMETSumEt();

		
		      //-------> diphoton system properties 
		      ptgg = candDiphoPtr->pt();
		      massRaw  = candDiphoPtr->mass();

		      //-------> individual photon properties
		      sceta1    = (candDiphoPtr->leadingPhoton()->superCluster())->eta();
		      r91	  = candDiphoPtr->leadingPhoton()->full5x5_r9();
		      ptUncorr1       = candDiphoPtr->leadingPhoton()->et();
		      pt1     = getPtCorrected(ptUncorr1, sceta1, r91, run, sampleID);
		      ptOverM1  = pt1/massRaw;
		      eta1      = candDiphoPtr->leadingPhoton()->eta();
		      phi1      = candDiphoPtr->leadingPhoton()->phi();
		      sieie1	  = candDiphoPtr->leadingPhoton()->full5x5_sigmaIetaIeta();
		      hoe1      = candDiphoPtr->leadingPhoton()->hadTowOverEm();
		      scRawEne1 = candDiphoPtr->leadingPhoton()->superCluster()->rawEnergy();
		      chiso1    = TMath::Max(candDiphoPtr->leadingPhoton()->egChargedHadronIso()- rho * getChargedHadronEAForPhotonIso((candDiphoPtr->leadingPhoton()->superCluster())->eta()),0.);
		      neuiso1   = TMath::Max(candDiphoPtr->leadingPhoton()->egNeutralHadronIso()- rho * getNeutralHadronEAForPhotonIso((candDiphoPtr->leadingPhoton()->superCluster())->eta()),0.);
		      phoiso1   = TMath::Max(candDiphoPtr->leadingPhoton()->egPhotonIso()- rho * getGammaEAForPhotonIso((candDiphoPtr->leadingPhoton()->superCluster())->eta()),0.);

		      eleveto1  = 0;
		      if (candDiphoPtr->leadingPhoton()->passElectronVeto()) eleveto1 = 1;
		      sceta2    = (candDiphoPtr->subLeadingPhoton()->superCluster())->eta();
		      r92	  = candDiphoPtr->subLeadingPhoton()->full5x5_r9();
		      ptUncorr2       = candDiphoPtr->subLeadingPhoton()->et();
		      pt2     = getPtCorrected(ptUncorr2, sceta2, r92, run, sampleID);
		      ptOverM2  = pt2/massRaw;
		      eta2      = candDiphoPtr->subLeadingPhoton()->eta();
		      phi2      = candDiphoPtr->subLeadingPhoton()->phi();
		      sieie2	  = candDiphoPtr->subLeadingPhoton()->full5x5_sigmaIetaIeta();
		      hoe2      = candDiphoPtr->subLeadingPhoton()->hadTowOverEm();
		      scRawEne2 = candDiphoPtr->subLeadingPhoton()->superCluster()->rawEnergy();
		      chiso2    = TMath::Max(candDiphoPtr->subLeadingPhoton()->egChargedHadronIso()- rho * getChargedHadronEAForPhotonIso((candDiphoPtr->subLeadingPhoton()->superCluster())->eta()),0.);
		      neuiso2   = TMath::Max(candDiphoPtr->subLeadingPhoton()->egNeutralHadronIso()- rho * getNeutralHadronEAForPhotonIso((candDiphoPtr->subLeadingPhoton()->superCluster())->eta()),0.);      	       
		      phoiso2   = TMath::Max(candDiphoPtr->subLeadingPhoton()->egPhotonIso()- rho * getGammaEAForPhotonIso((candDiphoPtr->subLeadingPhoton()->superCluster())->eta()),0.);
	
		      eleveto2  = 0;
		      if (candDiphoPtr->subLeadingPhoton()->passElectronVeto()) eleveto2 = 1;
	
		      //-------> photon selection (should be on, may be useful for extra studies
		      presel1 = isGammaPresel( sceta1, pt1, r91, chiso1, hoe1 ); 
		      presel2 = isGammaPresel( sceta2, pt2, r92, chiso2, hoe2 ); 
	
		
		      // correct mass for smearing and scaling
		      float leadSmearing	  = getSmearingValue( sceta1, r91, 0 );
		      float subleadSmearing	  = getSmearingValue( sceta2, r92, 0 );
		      // smear up and down for systematics
		      float leadSmearingUp	  = getSmearingValue( sceta1, r91, 1 );
		      float subleadSmearingUp	  = getSmearingValue( sceta2, r92, 1 );
		      float leadSmearingDown	  = getSmearingValue( sceta1, r91, -1 );
		      float subleadSmearingDown = getSmearingValue( sceta2, r92, -1 );


		      float gaussMean		= 1.0;
              	
		      TRandom Rand1(event);
		      float Smear1 		= Rand1.Gaus(gaussMean,leadSmearing);
		      float Smear1Up         	= Rand1.Gaus(gaussMean,leadSmearingUp);
		      float Smear1Down	= Rand1.Gaus(gaussMean,leadSmearingDown);

		      TRandom Rand2(event+83941);
		      float Smear2 		= Rand2.Gaus(gaussMean,subleadSmearing);
		      float Smear2Up	        = Rand2.Gaus(gaussMean,subleadSmearingUp);
		      float Smear2Down	= Rand2.Gaus(gaussMean,subleadSmearingDown);

		      massCorrSmear		= massRaw*sqrt(Smear1*Smear2);
		      massCorrSmearUp	        = massRaw*sqrt(Smear1Up*Smear2Up);
		      massCorrSmearDown	= massRaw*sqrt(Smear1Down*Smear2Down);

		      // scaling of Data
		      float leadScaling	= getScalingValue(sampleID, sceta1, r91, run, 0);
		      float subleadScaling	= getScalingValue(sampleID, sceta2, r92, run, 0);

		      // scale up and down for systematics
		      float leadScalingUp	= getScalingValue(sampleID, sceta1, r91 ,run, 1);
		      float subleadScalingUp	= getScalingValue(sampleID, sceta2, r92 ,run, 1);

		      float leadScalingDown	= getScalingValue(sampleID, sceta1, r91 ,run, -1);
		      float subleadScalingDown= getScalingValue(sampleID, sceta2, r92 ,run, -1);

		      float Scaling		= leadScaling*subleadScaling;
		      float ScalingUp		= leadScalingUp*subleadScalingUp;
		      float ScalingDown	= leadScalingDown*subleadScalingDown;

		      massCorrScale		= massRaw*sqrt(Scaling);
		      massCorrScaleUp		= massRaw*sqrt(ScalingUp);
		      massCorrScaleDown	= massRaw*sqrt(ScalingDown);

		      // final mgg (has Smearing or Scaling applied)
		      if (sampleID>0 && sampleID<10000){
			mgg = massCorrSmear;	  // smear mass for MC
		      }
		      else mgg = massCorrScale; // scale mass for Data
		
		      eff_end++;	  
		
	
		      //std::cout<<"run: "<<run<<" event: "<<event<<" mass: "<<massRaw<<std::endl;
		      //-------> pass each photon ID cut separately
		      // medium working point selection
		      passSieie1 = passSieieCuts( sceta1, sieie1);
		      passSieie2 = passSieieCuts( sceta2, sieie2);
		      passCHiso1 = passCHisoCuts( sceta1, chiso1, pt1);
		      passCHiso2 = passCHisoCuts( sceta2, chiso2, pt2);
		      passNHiso1 = passNHisoCuts( sceta1, neuiso1, pt1);
		      passNHiso2 = passNHisoCuts( sceta2, neuiso2, pt2);
		      passPHiso1 = passPHisoCuts( sceta1, phoiso1, pt1);
		      passPHiso2 = passPHisoCuts( sceta2, phoiso2, pt2);
		      passHoe1   = passHoeCuts( sceta1, hoe1);
		      passHoe2   = passHoeCuts( sceta2, hoe2);

		      // tight working point selection
		      passTightSieie1 = passTightSieieCuts( sceta1, sieie1);
		      passTightSieie2 = passTightSieieCuts( sceta2, sieie2);
		      passTightCHiso1 = passTightCHisoCuts( sceta1, chiso1, pt1);
		      passTightCHiso2 = passTightCHisoCuts( sceta2, chiso2, pt2);
		      passTightNHiso1 = passTightNHisoCuts( sceta1, neuiso1, pt1);
		      passTightNHiso2 = passTightNHisoCuts( sceta2, neuiso2, pt2);
		      passTightPHiso1 = passTightPHisoCuts( sceta1, phoiso1, pt1);
		      passTightPHiso2 = passTightPHisoCuts( sceta2, phoiso2, pt2);
		      passTightHoe1   = passTightHoeCuts( sceta1, hoe1);
		      passTightHoe2   = passTightHoeCuts( sceta2, hoe2);

		      // loose working point selection
		      passLooseSieie1 = passLooseSieieCuts( sceta1, sieie1);
		      passLooseSieie2 = passLooseSieieCuts( sceta2, sieie2);
		      passLooseCHiso1 = passLooseCHisoCuts( sceta1, chiso1, pt1);
		      passLooseCHiso2 = passLooseCHisoCuts( sceta2, chiso2, pt2);
		      passLooseNHiso1 = passLooseNHisoCuts( sceta1, neuiso1, pt1);
		      passLooseNHiso2 = passLooseNHisoCuts( sceta2, neuiso2, pt2);
		      passLoosePHiso1 = passLoosePHisoCuts( sceta1, phoiso1, pt1);
		      passLoosePHiso2 = passLoosePHisoCuts( sceta2, phoiso2, pt2);
		      passLooseHoe1   = passLooseHoeCuts( sceta1, hoe1);
		      passLooseHoe2   = passLooseHoeCuts( sceta2, hoe2);

		      //-------> pass all photon ID cuts above + electronVeto
		      sel1 = testPhotonIsolation( passSieie1, passCHiso1, passNHiso1, passPHiso1, passHoe1, eleveto1 );
		      sel2 = testPhotonIsolation( passSieie2, passCHiso2, passNHiso2, passPHiso2, passHoe2, eleveto2 );
		      tightsel1 = testPhotonIsolation( passTightSieie1, passTightCHiso1, passTightNHiso1, passTightPHiso1, passTightHoe1, eleveto1 );
		      tightsel2 = testPhotonIsolation( passTightSieie2, passTightCHiso2, passTightNHiso2, passTightPHiso2, passTightHoe2, eleveto2 );
		      loosesel1 = testPhotonIsolation( passLooseSieie1, passLooseCHiso1, passLooseNHiso1, passLoosePHiso1, passLooseHoe1, eleveto1 );
		      loosesel2 = testPhotonIsolation( passLooseSieie2, passLooseCHiso2, passLooseNHiso2, passLoosePHiso2, passLooseHoe2, eleveto2 );

		      //-------> store MVA info
		      mva1 = candDiphoPtr->leadingPhoton()->phoIdMvaDWrtVtx(candDiphoPtr->vtx());
		      mva2 = candDiphoPtr->subLeadingPhoton()->phoIdMvaDWrtVtx(candDiphoPtr->vtx());

		      //-------> event class
		      float maxEta = sceta1;
		      if (fabs(sceta2)>fabs(sceta1)) maxEta = sceta2;
		
		      float minR9 = r92;
		      if ( r91<r92 ) minR9 = r91;
		
		      eventClass = -1;
		      if (fabs(maxEta)<1.5 && minR9>0.94) eventClass = 0;
		      else if (fabs(maxEta)<1.5 && minR9<0.94) eventClass = 1;
		      else if (fabs(maxEta)>1.5 && minR9>0.94) eventClass = 2;
		      else if (fabs(maxEta)>1.5 && minR9<0.94) eventClass = 3;

		      //-------> vtx info
		      vtxIndex = candDiphoPtr->vertexIndex();
		      vtxX= candDiphoPtr->vtx()->x();
		      vtxY= candDiphoPtr->vtx()->y();
		      vtxZ= candDiphoPtr->vtx()->z();
		
		      //-------> generated vtx info
		      genVtxX = -999.;
		      genVtxY = -999.;
		      genVtxZ = -999.;
		      higgsVtxX  = -999.;
		      higgsVtxY  = -999.;
		      higgsVtxZ  = -999.;
		      if (sampleID>0 && sampleID<10000) {     // MC
			for( unsigned int genLoop = 0 ; genLoop < genParticles->size(); genLoop++ ) {
		    
			  if( genParticles->ptrAt( genLoop )->pdgId() != 2212 || genParticles->ptrAt( genLoop )->vertex().z() != 0. ) { // pdg1d=2212 is proton vtx
			    genVtxX = genParticles->ptrAt( genLoop )->vertex().x();
			    genVtxY = genParticles->ptrAt( genLoop )->vertex().y();
			    genVtxZ = genParticles->ptrAt( genLoop )->vertex().z();
			    break;
			  }
			}
		      }
		      if (sampleID>0 && sampleID<10000) {     // MC
			for( unsigned int genLoop = 0 ; genLoop < genParticles->size(); genLoop++ ) {
			  if( genParticles->ptrAt( genLoop )->pdgId() == 25 || genParticles->ptrAt( genLoop )->pdgId()==22 ){ // Higgs or Photon 
			    higgsVtxX = genParticles->ptrAt( genLoop )->vertex().x();// Margaret added Higgs vtx
			    higgsVtxY = genParticles->ptrAt( genLoop )->vertex().y();// Margaret added Higgs vtx
			    higgsVtxZ = genParticles->ptrAt( genLoop )->vertex().z();// Margaret added Higgs vtx
			    break;
			  }
			}
		      }
	      	
		      //-------> photons, MC truth match
		      genmatch1 = -999;
		      genmatch2 = -999;
		      geniso1   = -999.;
		      geniso2   = -999.;
		      if (sampleID>0 && sampleID<10000) {   

			const auto & genPhotons = *genPhotonsHandle;
		  
			if (candDiphoPtr->leadingPhoton()->hasMatchedGenPhoton()) {
			  genmatch1 = (candDiphoPtr->leadingPhoton()->genMatchType() == Photon::kPrompt); 
			  for (unsigned int j = 0 ; j < genPhotons.size() ; j++) {   
			    auto igen = genPhotons[j].ptr();
			    if ( igen->status() != 1 || igen->pdgId() != 22 ) continue; 
			    if ( fabs(igen->eta()-candDiphoPtr->leadingPhoton()->matchedGenPhoton()->eta())<0.001 && fabs(igen->phi()-candDiphoPtr->leadingPhoton()->matchedGenPhoton()->phi())<0.001 ) {
			      auto & extra = genPhotons[j];
			      geniso1 = extra.genIso();
			      break;
			    }
			  }
			}
		  
			if (candDiphoPtr->subLeadingPhoton()->hasMatchedGenPhoton()) {
			  genmatch2 = (candDiphoPtr->subLeadingPhoton()->genMatchType() == Photon::kPrompt); 
			  for (unsigned int j = 0 ; j < genPhotons.size() ; j++) {   
			    auto igen = genPhotons[j].ptr();
			    if ( igen->status() != 1 || igen->pdgId() != 22 ) continue; 
			    if ( fabs(igen->eta()-candDiphoPtr->subLeadingPhoton()->matchedGenPhoton()->eta())<0.001 && fabs(igen->phi()-candDiphoPtr->subLeadingPhoton()->matchedGenPhoton()->phi())<0.001 ) {
			      auto & extra = genPhotons[j];
			      geniso2 = extra.genIso();
			      break;
			    }
			  }
			}
		      }
		
	

		      // chiaraaaaa
		      // --> only for VH: check the mc truth for Higgs studies
		      vhtruth = -1;
		      if (sampleID==11) { //this is VH
			for( unsigned int genLoop = 0 ; genLoop < genParticles->size(); genLoop++ ) {
			  if (genParticles->ptrAt( genLoop )->mother(0)) {
			    int mothid = fabs(genParticles->ptrAt( genLoop )->mother(0)->pdgId());
			    if (mothid==23) {
			      if ( fabs(genParticles->ptrAt( genLoop )->pdgId())<=6 )  { vhtruth = 0; break; }
			      if ( fabs(genParticles->ptrAt( genLoop )->pdgId())==11 ) { vhtruth = 1; break; }
			      if ( fabs(genParticles->ptrAt( genLoop )->pdgId())==13 ) { vhtruth = 2; break; }
			    }
			    if (mothid==24) {
			      if ( fabs(genParticles->ptrAt( genLoop )->pdgId())<=6 )  { vhtruth = 3; break; }
			      if ( fabs(genParticles->ptrAt( genLoop )->pdgId())==11 ) { vhtruth = 4; break; }
			      if ( fabs(genParticles->ptrAt( genLoop )->pdgId())==13 ) { vhtruth = 5; break; }
			    } 
			    if (mothid==23 || mothid==24) {
			      if ( fabs(genParticles->ptrAt( genLoop )->pdgId())==15 ) { vhtruth = 6; break; }
			    }
			  }
			}
		      }

		      // Margaret added for Z'->ZH comparisons with SM ZH
		      genZ = -1;
		      ptZ  = -999.;
		      etaZ = -999.;
		      phiZ = -999.;
		      if (sampleID==11 || sampleID==20){ //VH or ZpZH
			for (unsigned int genLoop = 0; genLoop < genParticles->size(); genLoop++){
			  int thePdgId = fabs(genParticles->ptrAt( genLoop )->pdgId()); 
			  if (thePdgId!=23) continue;// if it is not a Z boson
			  genZ = 1;
			  ptZ = genParticles->ptrAt( genLoop )->pt();
			  etaZ = genParticles->ptrAt( genLoop )->eta();
			  phiZ = genParticles->ptrAt( genLoop )->phi();
			}
		      }
		      //--------> gen level mgg for signal samples
		      genmgg = -999.;
		      if (sampleID>99 && sampleID<10000) {  // signal only 

			for( unsigned int genLoop = 0 ; genLoop < genParticles->size(); genLoop++ ) {
		    
			  genmgg = -1999.;

			  if ( genParticles->ptrAt( genLoop )->pdgId()==5100039) {  // graviton

			    if (genParticles->ptrAt( genLoop )->numberOfDaughters()!=2) {
			      genmgg = -2999.;
			      break;
			    }

			    int statusd1 = genParticles->ptrAt( genLoop )->daughter(0)->status();
			    int statusd2 = genParticles->ptrAt( genLoop )->daughter(1)->status();
			    int pdgidd1  = genParticles->ptrAt( genLoop )->daughter(0)->pdgId();
			    int pdgidd2  = genParticles->ptrAt( genLoop )->daughter(1)->pdgId();
			    if (statusd1!=1 || statusd2!=1 || pdgidd1!=22 || pdgidd2!=22) { 
			      genmgg = -3999.;
			      break;
			    }

			    float ptd1  = genParticles->ptrAt( genLoop )->daughter(0)->pt();
			    float ptd2  = genParticles->ptrAt( genLoop )->daughter(1)->pt();
			    float etad1 = genParticles->ptrAt( genLoop )->daughter(0)->eta();
			    float etad2 = genParticles->ptrAt( genLoop )->daughter(1)->eta();
			    float phid1 = genParticles->ptrAt( genLoop )->daughter(0)->phi();
			    float phid2 = genParticles->ptrAt( genLoop )->daughter(1)->phi();
		      
			    TLorentzVector *myGenD1 = new TLorentzVector(0,0,0,0);
			    TLorentzVector *myGenD2 = new TLorentzVector(0,0,0,0);
			    myGenD1->SetPtEtaPhiM(ptd1, etad1, phid1, 0.);
			    myGenD2->SetPtEtaPhiM(ptd2, etad2, phid2, 0.);
			    genmgg = (*myGenD1+*myGenD2).M();

			    break;
			  }
			}
		      }
		
		      // leptons and jets
		      nEle   = 0;
		      nMuons = 0;
		      nJets  = 0;     
		      nLooseBjets  = 0;   
		      nMediumBjets = 0;  
			
		      // Muons =>
		      // 0.25 suggested by muon pog for loose isolation
		      // 0.3  (distance from the photons) => seems reasonable to me. 0.5 was used in globe
		      // pT>20
		      vector<Ptr<flashgg::Muon> > goodMuons = 
			selectMuons( theMuons->ptrs(), candDiphoPtr, primaryVertices->ptrs(), 2.4, 20., 0.25, 0.3, 0.3);  
		      nMuons = goodMuons.size();
		
		      // Electrons =>
		      // pT>20 (maybe can be put higher?)
		      // 0.3 (distance from the photons) => seems reasonable to me
		      std::vector<edm::Ptr<Electron> > goodElectrons = 
			selectMediumElectrons( theElectrons->ptrs(), primaryVertices->ptrs(), candDiphoPtr, rho, 20., 0.3, 0.3);
		      nEle = goodElectrons.size();
		
		      // Jets
		      // leading jet 
		      float ptJetLead=-999.;
		      float etaJetLead=-999.;
		      float phiJetLead=-999.;
		      float massJetLead=-999.;
		      unsigned int indexJetLead=-999;
		      float CHfracJet1 = -999.;
		      float NHfracJet1 = -999.;
		      float NEMfracJet1 = -999.;
		      float CEMfracJet1 = -999.;
		      float ELfracJet1 = -999.;
		      float PHfracJet1 = -999.;
		      float MUfracJet1 = -999.;
		      int CHmultJet1 = -999;
		      int NEmultJet1 = -999;

		      // subleading jet 
		      float ptJetSubLead=-999.;
		      float etaJetSubLead=-999.;
		      float phiJetSubLead=-999.;
		      float massJetSubLead=-999.;
		      unsigned int indexJetSubLead=-999;
		      float CHfracJet2 = -999.;
		      float NHfracJet2 = -999.;
		      float NEMfracJet2 = -999.;
		      float CEMfracJet2 = -999.;
		      float ELfracJet2 = -999.;
		      float PHfracJet2 = -999.;
		      float MUfracJet2 = -999.;
		      int CHmultJet2 = -999;
		      int NEmultJet2 = -999;
 
		      // 3rd jet 
		      float ptJet3=-999.;
		      float etaJet3=-999.;
		      float phiJet3=-999.;
		      float massJet3=-999.;
		      unsigned int indexJet3=-999;
		      float CHfracJet3 = -999.;
		      float NHfracJet3 = -999.;
		      float NEMfracJet3 = -999.;
		      float CEMfracJet3 = -999.;
		      float ELfracJet3 = -999.;
		      float PHfracJet3 = -999.;
		      float MUfracJet3 = -999.;
		      int CHmultJet3 = -999;
		      int NEmultJet3 = -999;

		      // 4th jet 
		      float ptJet4=-999.;
		      float etaJet4=-999.;
		      float phiJet4=-999.;
		      float massJet4=-999.;
		      unsigned int indexJet4=-999;
		      float CHfracJet4 = -999.;
		      float NHfracJet4 = -999.;
		      float NEMfracJet4 = -999.;
		      float CEMfracJet4 = -999.;
		      float ELfracJet4 = -999.;
		      float PHfracJet4 = -999.;
		      float MUfracJet4 = -999.;
		      int CHmultJet4 = -999;
		      int NEmultJet4 = -999;

		      unsigned int jetCollectionIndex = candDiphoPtr->jetCollectionIndex(); 
		      std::vector<edm::Ptr<flashgg::Jet> > tempJets(Jets[jetCollectionIndex]->size());

		      // make sure jets are sorted by pT
		      for( unsigned int jetIndex = 0; jetIndex < Jets[jetCollectionIndex]->size() ; jetIndex++) {
		        tempJets[jetIndex] = Jets[jetCollectionIndex]->ptrAt( jetIndex );
		      }
		      std::sort(tempJets.begin(),tempJets.end(),SortByJetPT);

		      // loop over sorted jets
		      unsigned int jetIndex = 0;
		      for (auto&& thejet : tempJets){
			jetIndex++;
			//std::cout << "jet pt =  " << thejet->pt() << std::endl;
			// jet selection: kinematics and id - hardcoded
			if( fabs( thejet->eta() ) > 4.7 ) continue;// Margaret changed to include all jets
			if( thejet->pt() < 30. ) continue;  
			if( !thejet->passesPuJetId( candDiphoPtr ) ) continue;   
		  
			// far from the photons => 0.3 seems reasonable to me   
			float dRPhoLeadJet    = deltaR( thejet->eta(), thejet->phi(), candDiphoPtr->leadingPhoton()->superCluster()->eta(), candDiphoPtr->leadingPhoton()->superCluster()->phi() ) ;
			float dRPhoSubLeadJet = deltaR( thejet->eta(), thejet->phi(), candDiphoPtr->subLeadingPhoton()->superCluster()->eta(), candDiphoPtr->subLeadingPhoton()->superCluster()->phi() );
			if( dRPhoLeadJet < 0.3 || dRPhoSubLeadJet < 0.3 ) continue;

			// close to muons?
			float matchMu = false;
			for( unsigned int muonIndex = 0; muonIndex < goodMuons.size(); muonIndex++ ) {  
			  Ptr<flashgg::Muon> muon = goodMuons[muonIndex];   
			  float dRJetMuon = deltaR( thejet->eta(), thejet->phi(), muon->eta(), muon->phi() ) ; 
			  if (dRJetMuon < 0.3 ) matchMu = true;   
			}
		  
			// close to electrons?
			float matchEle = false;
			for( unsigned int ElectronIndex = 0; ElectronIndex < goodElectrons.size(); ElectronIndex++ ) {   
			  Ptr<Electron> Electron = goodElectrons[ElectronIndex];  
			  float dRJetElectron = deltaR( thejet->eta(), thejet->phi(), Electron->eta(), Electron->phi() ) ;  
			  if( dRJetElectron < 0.3 ) matchEle = true;  
			}
		  
			// far from possible muons and electrons       
			if (matchMu || matchEle) continue;

			float bDiscriminatorValue = thejet->bDiscriminator( bTag_ );    
			if( bDiscriminatorValue > 0.244 ) nLooseBjets++;        // hardcoded
			if( bDiscriminatorValue > 0.679 ) nMediumBjets++;       // hardcoded 

			double chf = thejet->chargedHadronEnergyFraction();
			double nhf = thejet->neutralHadronEnergyFraction();
			double phf = thejet->photonEnergy()/(thejet->jecFactor(0)*thejet->energy());
			double elf = thejet->electronEnergy()/(thejet->jecFactor(0)*thejet->energy());
			double muf = thejet->muonEnergyFraction();

			//double hf_hf = thejet->HFHadronEnergyFraction();
			//double hf_emf= thejet->HFEMEnergyFraction();
			//double hof   = thejet->hoEnergyFraction();
		
			//int chm    = thejet->chargedHadronMultiplicity();
			int chMult = thejet->chargedMultiplicity();
			int neMult = thejet->neutralMultiplicity();
			int npr    = chMult + neMult;
		
			//int chHadMult = chm; //ijet->chargedHadronMultiplicity();
			//int neHadMult = thejet->neutralHadronMultiplicity();
			//int phoMult   = thejet->photonMultiplicity();

			double nemf  = thejet->neutralEmEnergyFraction();
			double cemf  = thejet->chargedEmEnergyFraction();
			int NumConst = npr;
		
			float eta  = thejet->eta(); 
			//float pt   = thejet->correctedJet(0).pt()*jecFactorsAK4.at(*i); // Is this OK? Correct corrected? -Juska

			// apply jet ID from:
			// https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetID
			// idL = Loose
			int idL = (nhf<0.99 && nemf<0.99 && NumConst>1 && muf < 0.8) && ((fabs(eta) <= 2.4 && chf>0 && chMult>0 && cemf<0.99) || fabs(eta)>2.4);
			// idT = Tight
			//int idT = (nhf<0.90 && nemf<0.90 && NumConst>1 && muf<0.8) && ((fabs(eta)<=2.4 && chf>0 && chMult>0 && cemf<0.90) || fabs(eta)>2.4);

			if(idL==0) continue;// jet does not pass loose ID
			nJets++;     

			//int pileupID = thejet->pileupjetIdWP();
			//if (pileupID == 1) std::cout << "works" << std::endl;

			// since jets are pt sorted take the first jet as leading
			if(thejet->pt()>ptJetLead){ // look at lead jet
			  ptJetLead = thejet->pt();
			  etaJetLead = thejet->eta();
			  phiJetLead = thejet->phi();
			  massJetLead = thejet->mass();
			  indexJetLead = jetIndex;
			  CHfracJet1 = chf;
			  NHfracJet1 = nhf;
			  NEMfracJet1 = nemf;
			  CEMfracJet1 = cemf;
			  ELfracJet1 = elf;
			  PHfracJet1 = phf;
			  MUfracJet1 = muf;
			  CHmultJet1 = chMult;
			  NEmultJet1 = neMult;
			}
			if(jetIndex==indexJetLead) continue;// now look at sublead jet
			if(thejet->pt() > ptJetSubLead){
			  ptJetSubLead = thejet->pt();
			  etaJetSubLead = thejet->eta();
			  phiJetSubLead = thejet->phi();
			  massJetSubLead = thejet->mass();
			  indexJetSubLead = jetIndex;
			  CHfracJet2 = chf;
			  NHfracJet2 = nhf;
			  NEMfracJet2 = nemf;
			  CEMfracJet2 = cemf;
			  ELfracJet2 = elf;
			  PHfracJet2 = phf;
			  MUfracJet2 = muf;
			  CHmultJet2 = chMult;
			  NEmultJet2 = neMult;
			}
			if(jetIndex==indexJetSubLead) continue;// now look at 3rd jet
			if(thejet->pt() > ptJet3){
			  ptJet3 = thejet->pt();
			  etaJet3 = thejet->eta();
			  phiJet3 = thejet->phi();
			  massJet3 = thejet->mass();
			  indexJet3 = jetIndex;
			  CHfracJet3 = chf;
			  NHfracJet3 = nhf;
			  NEMfracJet3 = nemf;
			  CEMfracJet3 = cemf;
			  ELfracJet3 = elf;
			  PHfracJet3 = phf;
			  MUfracJet3 = muf;
			  CHmultJet3 = chMult;
			  NEmultJet3 = neMult;
			}
			if(jetIndex==indexJet3) continue;// now look at 4th jet
			if(thejet->pt() > ptJet4){
			  ptJet4 = thejet->pt();
			  etaJet4 = thejet->eta();
			  phiJet4 = thejet->phi();
			  massJet4 = thejet->mass();
			  indexJet4 = jetIndex;
			  CHfracJet4 = chf;
			  NHfracJet4 = nhf;
			  NEMfracJet4 = nemf;
			  CEMfracJet4 = cemf;
			  ELfracJet4 = elf;
			  PHfracJet4 = phf;
			  MUfracJet4 = muf;
			  CHmultJet4 = chMult;
			  NEmultJet4 = neMult;
			}

		      } // loop over jets


		
		      // Variables for the tree
		      treeDipho_.hltPhoton26Photon16Mass60=hltPhoton26Photon16Mass60;
		      treeDipho_.hltPhoton36Photon22Mass15=hltPhoton36Photon22Mass15;
		      treeDipho_.hltPhoton42Photon25Mass15=hltPhoton42Photon25Mass15;
		      treeDipho_.hltDiphoton30Mass95=hltDiphoton30Mass95;
		      treeDipho_.hltDiphoton30Mass70=hltDiphoton30Mass70;
		      treeDipho_.hltDiphoton30Mass55=hltDiphoton30Mass55;
		      treeDipho_.hltDiphoton30Mass55PV=hltDiphoton30Mass55PV;
		      treeDipho_.hltDiphoton30Mass55EB=hltDiphoton30Mass55EB;
		      treeDipho_.run = run;
		      treeDipho_.event = event;
		      treeDipho_.lumi = lumi;
		      treeDipho_.nvtx = nvtx;
		      treeDipho_.rho = rho;
		      treeDipho_.sampleID = sampleID;  
		      treeDipho_.totXsec = totXsec;  
		      treeDipho_.pu_weight = pu_weight;
		      treeDipho_.pu_n = pu_n;
		      treeDipho_.sumDataset = sumDataset;
		      treeDipho_.perEveW = perEveW;
		      treeDipho_.pfmet = pfmet;
		      treeDipho_.pfmet = pfmetPhi;
		      treeDipho_.pfmet = pfmetSumEt;
		      treeDipho_.t1pfmet = t1pfmet;
		      treeDipho_.t1p2pfmet = t1p2pfmet;
		      treeDipho_.t1pfmetJetEnUp          = t1pfmetJetEnUp;           
		      treeDipho_.t1pfmetJetEnDown        = t1pfmetJetEnDown        ;
		      treeDipho_.t1pfmetJetResUp         = t1pfmetJetResUp         ;
		      treeDipho_.t1pfmetJetResDown       = t1pfmetJetResDown       ;
		      treeDipho_.t1pfmetMuonEnUp         = t1pfmetMuonEnUp         ;
		      treeDipho_.t1pfmetMuonEnDown       = t1pfmetMuonEnDown       ;
		      treeDipho_.t1pfmetElectronEnUp     = t1pfmetElectronEnUp     ;
		      treeDipho_.t1pfmetElectronEnDown   = t1pfmetElectronEnDown   ;
		      treeDipho_.t1pfmetTauEnUp          = t1pfmetTauEnUp          ;
		      treeDipho_.t1pfmetTauEnDown        = t1pfmetTauEnDown        ;
		      treeDipho_.t1pfmetPhotonEnUp       = t1pfmetPhotonEnUp       ;
		      treeDipho_.t1pfmetPhotonEnDown     = t1pfmetPhotonEnDown     ;
		      treeDipho_.t1pfmetUnclusteredEnUp  = t1pfmetUnclusteredEnUp  ;
		      treeDipho_.t1pfmetUnclusteredEnDown= t1pfmetUnclusteredEnDown;



		      treeDipho_.t1pfmetPhi = t1pfmetPhi;
		      treeDipho_.t1pfmetSumEt = t1pfmetSumEt;
		      treeDipho_.calomet = calomet;
		      treeDipho_.calometPhi = calometPhi;
		      treeDipho_.calometSumEt = calometSumEt;
		      treeDipho_.ptgg = ptgg;
		      treeDipho_.mgg = mgg;
		      treeDipho_.eventClass = eventClass;
		      treeDipho_.pt1 = pt1;
		      treeDipho_.ptUncorr1 = ptUncorr1;
		      treeDipho_.ptOverM1 = ptOverM1;
		      treeDipho_.eta1 = eta1;
		      treeDipho_.phi1 = phi1;
		      treeDipho_.sceta1 = sceta1;
		      treeDipho_.r91 = r91;
		      treeDipho_.sieie1 = sieie1;
		      treeDipho_.hoe1 = hoe1; 
		      treeDipho_.scRawEne1 = scRawEne1;
		      treeDipho_.chiso1 = chiso1; 
		      treeDipho_.phoiso1 = phoiso1; 
		      treeDipho_.neuiso1 = neuiso1;
		      treeDipho_.eleveto1 = eleveto1;
		      treeDipho_.pt2 = pt2;
		      treeDipho_.ptUncorr2 = ptUncorr2;
		      treeDipho_.ptOverM2 = ptOverM2;
		      treeDipho_.eta2 = eta2;
		      treeDipho_.phi2 = phi2;
		      treeDipho_.sceta2 = sceta2;
		      treeDipho_.r92 = r92;
		      treeDipho_.sieie2 = sieie2;
		      treeDipho_.hoe2 = hoe2; 
		      treeDipho_.scRawEne2 = scRawEne2;
		      treeDipho_.chiso2 = chiso2; 
		      treeDipho_.phoiso2 = phoiso2; 
		      treeDipho_.neuiso2 = neuiso2;
		      treeDipho_.eleveto2 = eleveto2;
		      treeDipho_.presel1 = presel1;
		      treeDipho_.presel2 = presel2;
		      treeDipho_.sel1 = sel1;
		      treeDipho_.sel2 = sel2;
		      treeDipho_.tightsel1 = tightsel1;
		      treeDipho_.tightsel2 = tightsel2;
		      treeDipho_.loosesel1 = loosesel1;
		      treeDipho_.loosesel2 = loosesel2;


		      //jet infos
		      treeDipho_.ptJetLead = ptJetLead;
		      treeDipho_.etaJetLead = etaJetLead;
		      treeDipho_.phiJetLead = phiJetLead;
		      treeDipho_.massJetLead = massJetLead;
		      treeDipho_.indexJetLead = indexJetLead;
		      treeDipho_.ptJetSubLead = ptJetSubLead;
		      treeDipho_.CHfracJet1 = CHfracJet1;
		      treeDipho_.NHfracJet1 = NHfracJet1;
		      treeDipho_.NEMfracJet1 = NEMfracJet1;
		      treeDipho_.CEMfracJet1 = CEMfracJet1;
		      treeDipho_.ELfracJet1 = ELfracJet1;
		      treeDipho_.PHfracJet1 = PHfracJet1;
		      treeDipho_.MUfracJet1 = MUfracJet1;
		      treeDipho_.CHmultJet1 = CHmultJet1;
		      treeDipho_.NEmultJet1 = NEmultJet1;

		      treeDipho_.etaJetSubLead = etaJetSubLead;
		      treeDipho_.phiJetSubLead = phiJetSubLead;
		      treeDipho_.massJetSubLead = massJetSubLead;
		      treeDipho_.indexJetSubLead = indexJetSubLead;
		      treeDipho_.CHfracJet2 = CHfracJet2;
		      treeDipho_.NHfracJet2 = NHfracJet2;
		      treeDipho_.NEMfracJet2 = NEMfracJet2;
		      treeDipho_.CEMfracJet2 = CEMfracJet2;
		      treeDipho_.ELfracJet2 = ELfracJet2;
		      treeDipho_.PHfracJet2 = PHfracJet2;
		      treeDipho_.MUfracJet2 = MUfracJet2;
		      treeDipho_.CHmultJet2 = CHmultJet2;
		      treeDipho_.NEmultJet2 = NEmultJet2;

		      treeDipho_.ptJet3 = ptJet3;
		      treeDipho_.etaJet3 = etaJet3;
		      treeDipho_.phiJet3 = phiJet3;
		      treeDipho_.massJet3 = massJet3;
		      treeDipho_.indexJet3 = indexJet3;
		      treeDipho_.CHfracJet3 = CHfracJet3;
		      treeDipho_.NHfracJet3 = NHfracJet3;
		      treeDipho_.NEMfracJet3 = NEMfracJet3;
		      treeDipho_.CEMfracJet3 = CEMfracJet3;
		      treeDipho_.ELfracJet3 = ELfracJet3;
		      treeDipho_.PHfracJet3 = PHfracJet3;
		      treeDipho_.MUfracJet3 = MUfracJet3;
		      treeDipho_.CHmultJet3 = CHmultJet3;
		      treeDipho_.NEmultJet3 = NEmultJet3;

		      treeDipho_.ptJet4 = ptJet4;
		      treeDipho_.etaJet4 = etaJet4;
		      treeDipho_.phiJet4 = phiJet4;
		      treeDipho_.massJet4 = massJet4;
		      treeDipho_.indexJet4 = indexJet4;
		      treeDipho_.CHfracJet4 = CHfracJet4;
		      treeDipho_.NHfracJet4 = NHfracJet4;
		      treeDipho_.NEMfracJet4 = NEMfracJet4;
		      treeDipho_.CEMfracJet4 = CEMfracJet4;
		      treeDipho_.ELfracJet4 = ELfracJet4;
		      treeDipho_.PHfracJet4 = PHfracJet4;
		      treeDipho_.MUfracJet4 = MUfracJet4;
		      treeDipho_.CHmultJet4 = CHmultJet4;
		      treeDipho_.NEmultJet4 = NEmultJet4;

		      treeDipho_.vtxIndex = vtxIndex;
		      treeDipho_.vtxX = vtxX;
		      treeDipho_.vtxY = vtxY;
		      treeDipho_.vtxZ = vtxZ;
		      treeDipho_.genmatch1 = genmatch1; 
		      treeDipho_.genmatch2 = genmatch2; 
		      treeDipho_.genmgg  = genmgg;        // -999: not enough gen level gamma; -1999: strange association with reco
		      treeDipho_.geniso1 = geniso1; 
		      treeDipho_.geniso2 = geniso2; 
		      treeDipho_.higgsVtxX = higgsVtxX;
		      treeDipho_.higgsVtxY = higgsVtxY;
		      treeDipho_.higgsVtxZ = higgsVtxZ;
		      treeDipho_.genVtxX = genVtxX;
		      treeDipho_.genVtxY = genVtxY;
		      treeDipho_.genVtxZ = genVtxZ;
		      treeDipho_.passCHiso1 = passCHiso1;
		      treeDipho_.passCHiso2 = passCHiso2;
		      treeDipho_.passNHiso1 = passNHiso1;
		      treeDipho_.passNHiso2 = passNHiso2;
		      treeDipho_.passPHiso1 = passPHiso1;
		      treeDipho_.passPHiso2 = passPHiso2;
		      treeDipho_.passSieie1 = passSieie1;
		      treeDipho_.passSieie2 = passSieie2;
		      treeDipho_.passHoe1 = passHoe1;
		      treeDipho_.passHoe2 = passHoe2;	
		      treeDipho_.passTightCHiso1 = passTightCHiso1;
		      treeDipho_.passTightCHiso2 = passTightCHiso2;
		      treeDipho_.passTightNHiso1 = passTightNHiso1;
		      treeDipho_.passTightNHiso2 = passTightNHiso2;
		      treeDipho_.passTightPHiso1 = passTightPHiso1;
		      treeDipho_.passTightPHiso2 = passTightPHiso2;
		      treeDipho_.passTightSieie1 = passTightSieie1;
		      treeDipho_.passTightSieie2 = passTightSieie2;
		      treeDipho_.passTightHoe1 = passTightHoe1;
		      treeDipho_.passTightHoe2 = passTightHoe2;	
		      treeDipho_.passLooseCHiso1 = passLooseCHiso1;
		      treeDipho_.passLooseCHiso2 = passLooseCHiso2;
		      treeDipho_.passLooseNHiso1 = passLooseNHiso1;
		      treeDipho_.passLooseNHiso2 = passLooseNHiso2;
		      treeDipho_.passLoosePHiso1 = passLoosePHiso1;
		      treeDipho_.passLoosePHiso2 = passLoosePHiso2;
		      treeDipho_.passLooseSieie1 = passLooseSieie1;
		      treeDipho_.passLooseSieie2 = passLooseSieie2;
		      treeDipho_.passLooseHoe1 = passLooseHoe1;
		      treeDipho_.passLooseHoe2 = passLooseHoe2;	
		      treeDipho_.nEle   = nEle;
		      treeDipho_.nMuons = nMuons;
		      treeDipho_.nJets  = nJets;
		      treeDipho_.nLooseBjets  = nLooseBjets;
		      treeDipho_.nMediumBjets = nMediumBjets;
		      treeDipho_.vhtruth = vhtruth;
		      treeDipho_.metF_GV = metF_GV;
		      treeDipho_.metF_HBHENoise = metF_HBHENoise;
		      treeDipho_.metF_HBHENoiseIso = metF_HBHENoiseIso;
		      treeDipho_.metF_CSC = metF_CSC;
		      treeDipho_.metF_eeBadSC = metF_eeBadSC;
		      treeDipho_.metF_HadronTrackRes = metF_HadronTrackRes;
		      treeDipho_.metF_MuonBadTrack = metF_MuonBadTrack;
		      treeDipho_.massCorrSmear = massCorrSmear;
		      treeDipho_.massCorrSmearUp = massCorrSmearUp;
		      treeDipho_.massCorrSmearDown = massCorrSmearDown;
		      treeDipho_.massCorrScale = massCorrScale;
		      treeDipho_.massCorrScaleUp = massCorrScaleUp;
		      treeDipho_.massCorrScaleDown = massCorrScaleDown;
		      treeDipho_.massRaw = massRaw;
		      treeDipho_.genZ	= genZ;
		      treeDipho_.ptZ = ptZ;
		      treeDipho_.etaZ = etaZ;
		      treeDipho_.phiZ = phiZ;
		      treeDipho_.mva1 = mva1;
		      treeDipho_.mva2 = mva2;
	
		      // Filling the trees
		      DiPhotonTree->Fill();
	
		    } // candIndex>-999
		  } // mass dipho
		} // vtx dipho
	      } // kin scaling 
	    } // kine
	  } // elveto
	} // selected
      } // preselected  
    }// at least one reco
  }//hlt trigger
  // delete
  //delete lazyToolnoZS;
}


bool NewDiPhoAnalyzer::geometrical_acceptance(float eta1, float eta2)
{
  float ae1(fabs(eta1));
  float ae2(fabs(eta2));
  return     (ae1 < 1.4442 || (ae1 > 1.566 && ae1 < 2.5))
    && (ae2 < 1.4442 || (ae2 > 1.566 && ae2 < 2.5));
}

void NewDiPhoAnalyzer::beginJob() {

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
  //cout << "now reading met filters lists" << endl;
  //listCSC     = readEventList("/afs/cern.ch/user/c/crovelli/public/monoH/metFilters/csc2015_Dec01.txt");
  //listEEbadSC = readEventList("/afs/cern.ch/user/c/crovelli/public/monoH/metFilters/ecalscn1043093_Dec01.txt");
  //listHadronTrackRes= readEventList("/afs/cern.ch/user/s/soffi/public/MonoHgg/MetFilters/badResolutionTrack_Jan13.txt");
  //listMuonBadTrack = readEventList("/afs/cern.ch/user/s/soffi/public/MonoHgg/MetFilters/muonBadTrack_Jan13.txt");
  //cout << "met filters lists read" << endl;

  // Trees
  DiPhotonTree = fs_->make<TTree>("DiPhotonTree","di-photon tree");

  // with all infos
  DiPhotonTree->Branch("hltPhoton26Photon16Mass60",&(treeDipho_.hltPhoton26Photon16Mass60),"hltPhoton26Photon16Mass60/I");
  DiPhotonTree->Branch("hltPhoton36Photon22Mass15",&(treeDipho_.hltPhoton36Photon22Mass15),"hltPhoton36Photon22Mass15/I");
  DiPhotonTree->Branch("hltPhoton42Photon25Mass15",&(treeDipho_.hltPhoton42Photon25Mass15),"hltPhoton42Photon25Mass15/I");
  DiPhotonTree->Branch("hltDiphoton30Mass95",&(treeDipho_.hltDiphoton30Mass95),"hltDiphoton30Mass95/I");
  DiPhotonTree->Branch("hltDiphoton30Mass70",&(treeDipho_.hltDiphoton30Mass70),"hltDiphoton30Mass70/I");
  DiPhotonTree->Branch("hltDiphoton30Mass55",&(treeDipho_.hltDiphoton30Mass55),"hltDiphoton30Mass55/I");
  DiPhotonTree->Branch("hltDiphoton30Mass55PV",&(treeDipho_.hltDiphoton30Mass55PV),"hltDiphoton30Mass55PV/I");
  DiPhotonTree->Branch("hltDiphoton30Mass55EB",&(treeDipho_.hltDiphoton30Mass55EB),"hltDiphoton30Mass55EB/I");

  DiPhotonTree->Branch("run",&(treeDipho_.run),"run/I");
  DiPhotonTree->Branch("event",&(treeDipho_.event),"event/I");
  DiPhotonTree->Branch("lumi",&(treeDipho_.lumi),"lumi/I");
  DiPhotonTree->Branch("nvtx",&(treeDipho_.nvtx),"nvtx/I");
  DiPhotonTree->Branch("rho",&(treeDipho_.rho),"rho/F");
  DiPhotonTree->Branch("sampleID",&(treeDipho_.sampleID),"sampleID/I");
  DiPhotonTree->Branch("totXsec",&(treeDipho_.totXsec),"totXsec/F");
  DiPhotonTree->Branch("pu_weight",&(treeDipho_.pu_weight),"pu_weight/F");
  DiPhotonTree->Branch("pu_n",&(treeDipho_.pu_n),"pu_n/F");
  DiPhotonTree->Branch("sumDataset",&(treeDipho_.sumDataset),"sumDataset/F");
  DiPhotonTree->Branch("perEveW",&(treeDipho_.perEveW),"perEveW/F");
  DiPhotonTree->Branch("pfmet",&(treeDipho_.pfmet),"pfmet/F");
  DiPhotonTree->Branch("pfmetPhi",&(treeDipho_.pfmetPhi),"pfmetPhi/F");
  DiPhotonTree->Branch("pfmetSumEt",&(treeDipho_.pfmetSumEt),"pfmetSumEt/F");
  DiPhotonTree->Branch("t1pfmet",&(treeDipho_.t1pfmet),"t1pfmet/F");
  DiPhotonTree->Branch("t1p2pfmet",&(treeDipho_.t1p2pfmet),"t1p2pfmet/F");


  DiPhotonTree->Branch("t1pfmetJetEnUp",&(treeDipho_.t1pfmetJetEnUp),"t1pfmetJetEnUp/F");         
  DiPhotonTree->Branch("t1pfmetJetEnDown",&(treeDipho_.t1pfmetJetEnDown),"t1pfmetJetEnDown/F");        
  DiPhotonTree->Branch("t1pfmetJetResUp",&(treeDipho_.t1pfmetJetResUp),"t1pfmetJetResUp/F");         
  DiPhotonTree->Branch("t1pfmetJetResDown",&(treeDipho_.t1pfmetJetResDown),"t1pfmetJetResDown/F");       
  DiPhotonTree->Branch("t1pfmetMuonEnUp",&(treeDipho_.t1pfmetMuonEnUp),"t1pfmetMuonEnUp/F");         
  DiPhotonTree->Branch("t1pfmetMuonEnDown",&(treeDipho_.t1pfmetMuonEnDown),"t1pfmetMuonEnDown/F");         
  DiPhotonTree->Branch("t1pfmetElectronEnUp",&(treeDipho_.t1pfmetElectronEnUp),"t1pfmetElectronEnUp/F");   
  DiPhotonTree->Branch("t1pfmetElectronEnDown",&(treeDipho_.t1pfmetElectronEnDown),"t1pfmetElectronEnDown/F");   
  DiPhotonTree->Branch("t1pfmetTauEnUp",&(treeDipho_.t1pfmetTauEnUp),"t1pfmetTauEnUp/F");        
  DiPhotonTree->Branch("t1pfmetTauEnDown",&(treeDipho_.t1pfmetTauEnDown),"t1pfmetTauEnDown/F");        
  DiPhotonTree->Branch("t1pfmetPhotonEnUp",&(treeDipho_.t1pfmetPhotonEnUp),"t1pfmetPhotonEnUp/F");     
  DiPhotonTree->Branch("t1pfmetPhotonEnDown",&(treeDipho_.t1pfmetPhotonEnDown),"t1pfmetPhotonEnDown/F");     
  DiPhotonTree->Branch("t1pfmetUnclusteredEnUp",&(treeDipho_.t1pfmetUnclusteredEnUp),"t1pfmetUnclusteredEnUp/F");
  DiPhotonTree->Branch("t1pfmetUnclusteredEnDown",&(treeDipho_.t1pfmetUnclusteredEnDown),"t1pfmetUnclusteredEnDown/F");

  DiPhotonTree->Branch("t1pfmetPhi",&(treeDipho_.t1pfmetPhi),"t1pfmetPhi/F");
  DiPhotonTree->Branch("t1pfmetSumEt",&(treeDipho_.t1pfmetSumEt),"t1pfmetSumEt/F");
  DiPhotonTree->Branch("calomet",&(treeDipho_.calomet),"calomet/F");
  DiPhotonTree->Branch("calometPhi",&(treeDipho_.calometPhi),"calometPhi/F");
  DiPhotonTree->Branch("calometSumEt",&(treeDipho_.calometSumEt),"calometSumEt/F");
  DiPhotonTree->Branch("ptgg",&(treeDipho_.ptgg),"ptgg/F");
  DiPhotonTree->Branch("mgg",&(treeDipho_.mgg),"mgg/F");
  DiPhotonTree->Branch("eventClass",&(treeDipho_.eventClass),"eventClass/I");
  DiPhotonTree->Branch("pt1",&(treeDipho_.pt1),"pt1/F");
  DiPhotonTree->Branch("ptUncorr1",&(treeDipho_.ptUncorr1),"ptUncorr1/F");
  DiPhotonTree->Branch("ptOverM1",&(treeDipho_.ptOverM1),"ptOverM1/F");
  DiPhotonTree->Branch("eta1",&(treeDipho_.eta1),"eta1/F");
  DiPhotonTree->Branch("phi1",&(treeDipho_.phi1),"phi1/F");
  DiPhotonTree->Branch("sceta1",&(treeDipho_.sceta1),"sceta1/F");
  DiPhotonTree->Branch("r91",&(treeDipho_.r91),"r91/F");
  DiPhotonTree->Branch("sieie1",&(treeDipho_.sieie1),"sieie1/F");
  DiPhotonTree->Branch("hoe1",&(treeDipho_.hoe1),"hoe1/F");
  DiPhotonTree->Branch("scRawEne1",&(treeDipho_.scRawEne1),"scRawEne1/F");
  DiPhotonTree->Branch("chiso1",&(treeDipho_.chiso1),"chiso1/F");
  DiPhotonTree->Branch("phoiso1",&(treeDipho_.phoiso1),"phoiso1/F");
  DiPhotonTree->Branch("neuiso1",&(treeDipho_.neuiso1),"neuiso1/F");
  DiPhotonTree->Branch("eleveto1",&(treeDipho_.eleveto1),"eleveto1/I");
  DiPhotonTree->Branch("pt2",&(treeDipho_.pt2),"pt2/F");
  DiPhotonTree->Branch("ptUncorr2",&(treeDipho_.ptUncorr2),"ptUncorr2/F");
  DiPhotonTree->Branch("ptOverM2",&(treeDipho_.ptOverM2),"ptOverM2/F");
  DiPhotonTree->Branch("eta2",&(treeDipho_.eta2),"eta2/F");
  DiPhotonTree->Branch("phi2",&(treeDipho_.phi2),"phi2/F");
  DiPhotonTree->Branch("sceta2",&(treeDipho_.sceta2),"sceta2/F");
  DiPhotonTree->Branch("r92",&(treeDipho_.r92),"r92/F");
  DiPhotonTree->Branch("sieie2",&(treeDipho_.sieie2),"sieie2/F");
  DiPhotonTree->Branch("hoe2",&(treeDipho_.hoe2),"hoe2/F");
  DiPhotonTree->Branch("scRawEne2",&(treeDipho_.scRawEne2),"scRawEne2/F");
  DiPhotonTree->Branch("chiso2",&(treeDipho_.chiso2),"chiso2/F");
  DiPhotonTree->Branch("phoiso2",&(treeDipho_.phoiso2),"phoiso2/F");
  DiPhotonTree->Branch("neuiso2",&(treeDipho_.neuiso2),"neuiso2/F");
  DiPhotonTree->Branch("eleveto2",&(treeDipho_.eleveto2),"eleveto2/I");
  DiPhotonTree->Branch("presel1",&(treeDipho_.presel1),"presel1/I");
  DiPhotonTree->Branch("presel2",&(treeDipho_.presel2),"presel2/I");
  DiPhotonTree->Branch("sel1",&(treeDipho_.sel1),"sel1/I");
  DiPhotonTree->Branch("sel2",&(treeDipho_.sel2),"sel2/I");
  DiPhotonTree->Branch("tightsel1",&(treeDipho_.tightsel1),"tightsel1/I");
  DiPhotonTree->Branch("tightsel2",&(treeDipho_.tightsel2),"tightsel2/I");
  DiPhotonTree->Branch("loosesel1",&(treeDipho_.loosesel1),"loosesel1/I");
  DiPhotonTree->Branch("loosesel2",&(treeDipho_.loosesel2),"loosesel2/I");
  DiPhotonTree->Branch("genmatch1",&(treeDipho_.genmatch1),"genmatch1/I");
  DiPhotonTree->Branch("genmatch2",&(treeDipho_.genmatch2),"genmatch12/I");
  DiPhotonTree->Branch("genmgg",&(treeDipho_.genmgg),"genmgg/F");
  DiPhotonTree->Branch("geniso1",&(treeDipho_.geniso1),"geniso1/F");
  DiPhotonTree->Branch("geniso2",&(treeDipho_.geniso2),"geniso2/F");

  DiPhotonTree->Branch("ptJetLead",&(treeDipho_.ptJetLead),"ptJetLead/F");
  DiPhotonTree->Branch("etaJetLead",&(treeDipho_.etaJetLead),"etaJetLead/F");
  DiPhotonTree->Branch("phiJetLead",&(treeDipho_.phiJetLead),"phiJetLead/F");
  DiPhotonTree->Branch("massJetLead",&(treeDipho_.massJetLead),"massJetLead/F");
  DiPhotonTree->Branch("indexJetLead",&(treeDipho_.indexJetLead),"indexJetLead/I");
  DiPhotonTree->Branch("NEMfracJet1",&(treeDipho_.NEMfracJet1),"NEMfracJet1/F");
  DiPhotonTree->Branch("CEMfracJet1",&(treeDipho_.CEMfracJet1),"CEMfracJet1/F");
  DiPhotonTree->Branch("ELfracJet1",&(treeDipho_.ELfracJet1),"ELfracJet1/F");
  DiPhotonTree->Branch("CHfracJet1",&(treeDipho_.CHfracJet1),"CHfracJet1/F");
  DiPhotonTree->Branch("NHfracJet1",&(treeDipho_.NHfracJet1),"NHfracJet1/F");
  DiPhotonTree->Branch("PHfracJet1",&(treeDipho_.PHfracJet1),"PHfracJet1/F");
  DiPhotonTree->Branch("MUfracJet1",&(treeDipho_.MUfracJet1),"MUfracJet1/F");
  DiPhotonTree->Branch("CHmultJet1",&(treeDipho_.CHmultJet1),"CHmultJet1/I");
  DiPhotonTree->Branch("NEmultJet1",&(treeDipho_.NEmultJet1),"NEmultJet1/I");

  DiPhotonTree->Branch("ptJetSubLead",&(treeDipho_.ptJetSubLead),"ptJetSubLead/F");
  DiPhotonTree->Branch("etaJetSubLead",&(treeDipho_.etaJetSubLead),"etaJetSubLead/F");
  DiPhotonTree->Branch("phiJetSubLead",&(treeDipho_.phiJetSubLead),"phiJetSubLead/F");
  DiPhotonTree->Branch("massJetSubLead",&(treeDipho_.massJetSubLead),"massJetSubLead/F");
  DiPhotonTree->Branch("indexJetSubLead",&(treeDipho_.indexJetSubLead),"indexJetSubLead/I");
  DiPhotonTree->Branch("NEMfracJet2",&(treeDipho_.NEMfracJet2),"NEMfracJet2/F");
  DiPhotonTree->Branch("CEMfracJet2",&(treeDipho_.CEMfracJet2),"CEMfracJet2/F");
  DiPhotonTree->Branch("ELfracJet2",&(treeDipho_.ELfracJet2),"ELfracJet2/F");
  DiPhotonTree->Branch("CHfracJet2",&(treeDipho_.CHfracJet2),"CHfracJet2/F");
  DiPhotonTree->Branch("NHfracJet2",&(treeDipho_.NHfracJet2),"NHfracJet2/F");
  DiPhotonTree->Branch("PHfracJet2",&(treeDipho_.PHfracJet2),"PHfracJet2/F");
  DiPhotonTree->Branch("MUfracJet2",&(treeDipho_.MUfracJet2),"MUfracJet2/F");
  DiPhotonTree->Branch("CHmultJet2",&(treeDipho_.CHmultJet2),"CHmultJet2/I");
  DiPhotonTree->Branch("NEmultJet2",&(treeDipho_.NEmultJet2),"NEmultJet2/I");

  DiPhotonTree->Branch("ptJet3",&(treeDipho_.ptJet3),"ptJet3/F");
  DiPhotonTree->Branch("etaJet3",&(treeDipho_.etaJet3),"etaJet3/F");
  DiPhotonTree->Branch("phiJet3",&(treeDipho_.phiJet3),"phiJet3/F");
  DiPhotonTree->Branch("massJet3",&(treeDipho_.massJet3),"massJet3/F");
  DiPhotonTree->Branch("indexJet3",&(treeDipho_.indexJet3),"indexJet3/I");
  DiPhotonTree->Branch("NEMfracJet3",&(treeDipho_.NEMfracJet3),"NEMfracJet3/F");
  DiPhotonTree->Branch("CEMfracJet3",&(treeDipho_.CEMfracJet3),"CEMfracJet3/F");
  DiPhotonTree->Branch("ELfracJet3",&(treeDipho_.ELfracJet3),"ELfracJet3/F");
  DiPhotonTree->Branch("CHfracJet3",&(treeDipho_.CHfracJet3),"CHfracJet3/F");
  DiPhotonTree->Branch("NHfracJet3",&(treeDipho_.NHfracJet3),"NHfracJet3/F");
  DiPhotonTree->Branch("PHfracJet3",&(treeDipho_.PHfracJet3),"PHfracJet3/F");
  DiPhotonTree->Branch("MUfracJet3",&(treeDipho_.MUfracJet3),"MUfracJet3/F");
  DiPhotonTree->Branch("CHmultJet3",&(treeDipho_.CHmultJet3),"CHmultJet3/I");
  DiPhotonTree->Branch("NEmultJet3",&(treeDipho_.NEmultJet3),"NEmultJet3/I");

  DiPhotonTree->Branch("ptJet4",&(treeDipho_.ptJet4),"ptJet4/F");
  DiPhotonTree->Branch("etaJet4",&(treeDipho_.etaJet4),"etaJet4/F");
  DiPhotonTree->Branch("phiJet4",&(treeDipho_.phiJet4),"phiJet4/F");
  DiPhotonTree->Branch("massJet4",&(treeDipho_.massJet4),"massJet4/F");
  DiPhotonTree->Branch("indexJet4",&(treeDipho_.indexJet4),"indexJet4/I");
  DiPhotonTree->Branch("NEMfracJet4",&(treeDipho_.NEMfracJet4),"NEMfracJet4/F");
  DiPhotonTree->Branch("CEMfracJet4",&(treeDipho_.CEMfracJet4),"CEMfracJet4/F");
  DiPhotonTree->Branch("ELfracJet4",&(treeDipho_.ELfracJet4),"ELfracJet4/F");
  DiPhotonTree->Branch("CHfracJet4",&(treeDipho_.CHfracJet4),"CHfracJet4/F");
  DiPhotonTree->Branch("NHfracJet4",&(treeDipho_.NHfracJet4),"NHfracJet4/F");
  DiPhotonTree->Branch("PHfracJet4",&(treeDipho_.PHfracJet4),"PHfracJet4/F");
  DiPhotonTree->Branch("MUfracJet4",&(treeDipho_.MUfracJet4),"MUfracJet4/F");
  DiPhotonTree->Branch("CHmultJet4",&(treeDipho_.CHmultJet4),"CHmultJet4/I");
  DiPhotonTree->Branch("NEmultJet4",&(treeDipho_.NEmultJet4),"NEmultJet4/I");

  DiPhotonTree->Branch("vtxIndex",&(treeDipho_.vtxIndex),"vtxIndex/I");
  DiPhotonTree->Branch("vtxX",&(treeDipho_.vtxX),"vtxX/F");
  DiPhotonTree->Branch("vtxY",&(treeDipho_.vtxY),"vtxY/F");
  DiPhotonTree->Branch("vtxZ",&(treeDipho_.vtxZ),"vtxZ/F");
  DiPhotonTree->Branch("higgsVtxX",&(treeDipho_.higgsVtxX),"higgsVtxX/F");
  DiPhotonTree->Branch("higgsVtxY",&(treeDipho_.higgsVtxY),"higgsVtxY/F");
  DiPhotonTree->Branch("higgsVtxZ",&(treeDipho_.higgsVtxZ),"higgsVtxZ/F");
  DiPhotonTree->Branch("genVtxX",&(treeDipho_.genVtxX),"genVtxX/F");
  DiPhotonTree->Branch("genVtxY",&(treeDipho_.genVtxY),"genVtxY/F");
  DiPhotonTree->Branch("genVtxZ",&(treeDipho_.genVtxZ),"genVtxZ/F");
  DiPhotonTree->Branch("passCHiso1",&(treeDipho_.passCHiso1),"passCHiso1/I");
  DiPhotonTree->Branch("passCHiso2",&(treeDipho_.passCHiso2),"passCHiso2/I");
  DiPhotonTree->Branch("passNHiso1",&(treeDipho_.passNHiso1),"passNHiso1/I");
  DiPhotonTree->Branch("passNHiso2",&(treeDipho_.passNHiso2),"passNHiso2/I");
  DiPhotonTree->Branch("passPHiso1",&(treeDipho_.passPHiso1),"passPHiso1/I");
  DiPhotonTree->Branch("passPHiso2",&(treeDipho_.passPHiso2),"passPHiso2/I");
  DiPhotonTree->Branch("passSieie1",&(treeDipho_.passSieie1),"passSieie1/I");
  DiPhotonTree->Branch("passSieie2",&(treeDipho_.passSieie2),"passSieie2/I");
  DiPhotonTree->Branch("passHoe1",&(treeDipho_.passHoe1),"passHoe1/I");
  DiPhotonTree->Branch("passHoe2",&(treeDipho_.passHoe2),"passHoe2/I");
  DiPhotonTree->Branch("passTightCHiso1",&(treeDipho_.passTightCHiso1),"passTightCHiso1/I");
  DiPhotonTree->Branch("passTightCHiso2",&(treeDipho_.passTightCHiso2),"passTightCHiso2/I");
  DiPhotonTree->Branch("passTightNHiso1",&(treeDipho_.passTightNHiso1),"passTightNHiso1/I");
  DiPhotonTree->Branch("passTightNHiso2",&(treeDipho_.passTightNHiso2),"passTightNHiso2/I");
  DiPhotonTree->Branch("passTightPHiso1",&(treeDipho_.passTightPHiso1),"passTightPHiso1/I");
  DiPhotonTree->Branch("passTightPHiso2",&(treeDipho_.passTightPHiso2),"passTightPHiso2/I");
  DiPhotonTree->Branch("passTightSieie1",&(treeDipho_.passTightSieie1),"passTightSieie1/I");
  DiPhotonTree->Branch("passTightSieie2",&(treeDipho_.passTightSieie2),"passTightSieie2/I");
  DiPhotonTree->Branch("passTightHoe1",&(treeDipho_.passTightHoe1),"passTightHoe1/I");
  DiPhotonTree->Branch("passTightHoe2",&(treeDipho_.passTightHoe2),"passTightHoe2/I");
  DiPhotonTree->Branch("passLooseCHiso1",&(treeDipho_.passLooseCHiso1),"passLooseCHiso1/I");
  DiPhotonTree->Branch("passLooseCHiso2",&(treeDipho_.passLooseCHiso2),"passLooseCHiso2/I");
  DiPhotonTree->Branch("passLooseNHiso1",&(treeDipho_.passLooseNHiso1),"passLooseNHiso1/I");
  DiPhotonTree->Branch("passLooseNHiso2",&(treeDipho_.passLooseNHiso2),"passLooseNHiso2/I");
  DiPhotonTree->Branch("passLoosePHiso1",&(treeDipho_.passLoosePHiso1),"passLoosePHiso1/I");
  DiPhotonTree->Branch("passLoosePHiso2",&(treeDipho_.passLoosePHiso2),"passLoosePHiso2/I");
  DiPhotonTree->Branch("passLooseSieie1",&(treeDipho_.passLooseSieie1),"passLooseSieie1/I");
  DiPhotonTree->Branch("passLooseSieie2",&(treeDipho_.passLooseSieie2),"passLooseSieie2/I");
  DiPhotonTree->Branch("passLooseHoe1",&(treeDipho_.passLooseHoe1),"passLooseHoe1/I");
  DiPhotonTree->Branch("passLooseHoe2",&(treeDipho_.passLooseHoe2),"passLooseHoe2/I");
  DiPhotonTree->Branch("nEle",&(treeDipho_.nEle),"nEle/I");
  DiPhotonTree->Branch("nMuons",&(treeDipho_.nMuons),"nMuons/I");
  DiPhotonTree->Branch("nJets",&(treeDipho_.nJets),"nJets/I");
  DiPhotonTree->Branch("nLooseBjets",&(treeDipho_.nLooseBjets),"nLooseBjets/I");
  DiPhotonTree->Branch("nMediumBjets",&(treeDipho_.nMediumBjets),"nMediumBjets/I");
  DiPhotonTree->Branch("vhtruth",&(treeDipho_.vhtruth),"vhtruth/I");
  DiPhotonTree->Branch("metF_GV",&(treeDipho_.metF_GV),"metF_GV/I");
  DiPhotonTree->Branch("metF_HBHENoise",&(treeDipho_.metF_HBHENoise),"metF_HBHENoise/I");
  DiPhotonTree->Branch("metF_HBHENoiseIso",&(treeDipho_.metF_HBHENoiseIso),"metF_HBHENoiseIso/I");
  DiPhotonTree->Branch("metF_CSC",&(treeDipho_.metF_CSC),"metF_CSC/I");
  DiPhotonTree->Branch("metF_eeBadSC",&(treeDipho_.metF_eeBadSC),"metF_eeBadSC/I");
  DiPhotonTree->Branch("metF_HadronTrackRes",&(treeDipho_.metF_HadronTrackRes),"metF_HadronTrackRes/I");
  DiPhotonTree->Branch("metF_MuonBadTrack",&(treeDipho_.metF_MuonBadTrack),"metF_MuonBadTrack/I");
  DiPhotonTree->Branch("massCorrSmear",&(treeDipho_.massCorrSmear),"massCorrSmear/F");
  DiPhotonTree->Branch("massCorrSmearUp",&(treeDipho_.massCorrSmearUp),"massCorrSmearUp/F");
  DiPhotonTree->Branch("massCorrSmearDown",&(treeDipho_.massCorrSmearDown),"massCorrSmearDown/F");
  DiPhotonTree->Branch("massCorrScale",&(treeDipho_.massCorrScale),"massCorrScale/F");
  DiPhotonTree->Branch("massCorrScaleUp",&(treeDipho_.massCorrScaleUp),"massCorrScaleUp/F");
  DiPhotonTree->Branch("massCorrScaleDown",&(treeDipho_.massCorrScaleDown),"massCorrScaleDown/F");
  DiPhotonTree->Branch("massRaw",&(treeDipho_.massRaw),"massRaw/F");
  DiPhotonTree->Branch("genZ",&(treeDipho_.genZ),"genZ/I");
  DiPhotonTree->Branch("ptZ",&(treeDipho_.ptZ),"ptZ/F");
  DiPhotonTree->Branch("etaZ",&(treeDipho_.etaZ),"etaZ/F");
  DiPhotonTree->Branch("phiZ",&(treeDipho_.phiZ),"phiZ/F");
  DiPhotonTree->Branch("mva1",&(treeDipho_.mva1),"mva1/F");
  DiPhotonTree->Branch("mva2",&(treeDipho_.mva2),"mva2/F");

  cout << " finished beginJob " << endl;

}

void NewDiPhoAnalyzer::endJob() { }

void NewDiPhoAnalyzer::initTreeStructure() {
  treeDipho_.hltPhoton26Photon16Mass60=-500;
  treeDipho_.hltPhoton36Photon22Mass15=-500;
  treeDipho_.hltPhoton42Photon25Mass15=-500;
  treeDipho_.hltDiphoton30Mass95=-500;   
  treeDipho_.hltDiphoton30Mass70=-500;   
  treeDipho_.hltDiphoton30Mass55=-500; 
  treeDipho_.hltDiphoton30Mass55PV=-500; 
  treeDipho_.hltDiphoton30Mass55EB=-500; 
 
  treeDipho_.run   = -500;
  treeDipho_.event = -500;
  treeDipho_.lumi  = -500;
  treeDipho_.nvtx  = -500;
  treeDipho_.rho   = -500.;
  treeDipho_.sampleID  = -500;
  treeDipho_.totXsec   = -500.;
  treeDipho_.pu_weight = -500.; 
  treeDipho_.pu_n = -500.;
  treeDipho_.sumDataset = -500.;
  treeDipho_.perEveW = -500.;
  treeDipho_.pfmet = -500.;
  treeDipho_.pfmetPhi = -500.;
  treeDipho_.pfmetSumEt = -500.;
  treeDipho_.t1pfmet = -500.;
  treeDipho_.t1pfmetPhi = -500.;
  treeDipho_.t1pfmetSumEt = -500.;
  treeDipho_.calomet = -500.;
  treeDipho_.calometPhi = -500.;
  treeDipho_.calometSumEt = -500.;
  treeDipho_.ptgg = -500.;
  treeDipho_.mgg  = -500.;
  treeDipho_.eventClass  = -500;
  treeDipho_.pt1  = -500.;
  treeDipho_.ptOverM1 = -500.;
  treeDipho_.eta1 = -500.;
  treeDipho_.phi1 = -500.;
  treeDipho_.sceta1 = -500.;
  treeDipho_.r91  = -500.;
  treeDipho_.sieie1 = -500.;
  treeDipho_.hoe1   = -500.;
  treeDipho_.scRawEne1 = -500.;
  treeDipho_.chiso1  = -500.;
  treeDipho_.phoiso1 = -500.;
  treeDipho_.neuiso1 = -500.;
  treeDipho_.eleveto1 = -500;
  treeDipho_.pt2  = -500.;
  treeDipho_.ptOverM2 = -500.;
  treeDipho_.eta2 = -500.;
  treeDipho_.phi2 = -500.;
  treeDipho_.sceta2 = -500.;
  treeDipho_.r92  = -500.;
  treeDipho_.sieie2 = -500.;
  treeDipho_.hoe2   = -500.;
  treeDipho_.scRawEne2 = -500.;
  treeDipho_.chiso2  = -500.;
  treeDipho_.phoiso2 = -500.;
  treeDipho_.neuiso2 = -500.;
  treeDipho_.eleveto2 = -500;
  treeDipho_.presel1 = -500;
  treeDipho_.presel2 = -500;
  treeDipho_.sel1 = -500;
  treeDipho_.sel2 = -500;
  treeDipho_.tightsel1 = -500;
  treeDipho_.tightsel2 = -500;
  treeDipho_.loosesel1 = -500;
  treeDipho_.loosesel2 = -500;

  treeDipho_.ptJetLead = -500;
  treeDipho_.etaJetLead = -500;
  treeDipho_.phiJetLead = -500;
  treeDipho_.massJetLead = -500;
  treeDipho_.indexJetLead = -500;
  treeDipho_.NEMfracJet1 = -500.;
  treeDipho_.CEMfracJet1 = -500.;
  treeDipho_.CHfracJet1 = -500.;
  treeDipho_.NHfracJet1 = -500.;
  treeDipho_.ELfracJet1 = -500.;
  treeDipho_.PHfracJet1 = -500.;
  treeDipho_.MUfracJet1 = -500.;
  treeDipho_.CHmultJet1 = -500;
  treeDipho_.NEmultJet1 = -500;

  treeDipho_.ptJetSubLead =-500 ;
  treeDipho_.etaJetSubLead =-500 ;
  treeDipho_.phiJetSubLead =-500 ;
  treeDipho_.massJetSubLead =-500 ;
  treeDipho_.indexJetSubLead = -500;
  treeDipho_.NEMfracJet2 = -500.;
  treeDipho_.CEMfracJet2 = -500.;
  treeDipho_.CHfracJet2 = -500.;
  treeDipho_.NHfracJet2 = -500.;
  treeDipho_.ELfracJet2 = -500.;
  treeDipho_.PHfracJet2 = -500.;
  treeDipho_.MUfracJet2 = -500.;
  treeDipho_.CHmultJet2 = -500;
  treeDipho_.NEmultJet2 = -500;

  treeDipho_.ptJet3 = -500;
  treeDipho_.etaJet3 = -500;
  treeDipho_.phiJet3 = -500;
  treeDipho_.massJet3 = -500;
  treeDipho_.indexJet3 = -500;
  treeDipho_.NEMfracJet3 = -500.;
  treeDipho_.CEMfracJet3 = -500.;
  treeDipho_.CHfracJet3 = -500.;
  treeDipho_.NHfracJet3 = -500.;
  treeDipho_.ELfracJet3 = -500.;
  treeDipho_.PHfracJet3 = -500.;
  treeDipho_.MUfracJet3 = -500.;
  treeDipho_.CHmultJet3 = -500;
  treeDipho_.NEmultJet3 = -500;

  treeDipho_.ptJet4 = -500;
  treeDipho_.etaJet4 = -500;
  treeDipho_.phiJet4 = -500;
  treeDipho_.massJet4 = -500;
  treeDipho_.indexJet4 = -500;
  treeDipho_.NEMfracJet4 = -500.;
  treeDipho_.CEMfracJet4 = -500.;
  treeDipho_.CHfracJet4 = -500.;
  treeDipho_.NHfracJet4 = -500.;
  treeDipho_.ELfracJet4 = -500.;
  treeDipho_.PHfracJet4 = -500.;
  treeDipho_.MUfracJet4 = -500.;
  treeDipho_.CHmultJet4 = -500;
  treeDipho_.NEmultJet4 = -500;

  treeDipho_.vtxIndex = -500;
  treeDipho_.vtxX = -500.;
  treeDipho_.vtxY = -500.;
  treeDipho_.vtxZ = -500.;
  treeDipho_.genmatch1 = -500;
  treeDipho_.genmatch2 = -500;
  treeDipho_.genmgg  = -500.;
  treeDipho_.geniso1 = -500.;
  treeDipho_.geniso2 = -500.;
  treeDipho_.higgsVtxX = -500.;
  treeDipho_.higgsVtxY = -500.;
  treeDipho_.higgsVtxZ = -500.;
  treeDipho_.genVtxX = -500.;
  treeDipho_.genVtxY = -500.;
  treeDipho_.genVtxZ = -500.;
  treeDipho_.passCHiso1 = -500;
  treeDipho_.passCHiso2 = -500;
  treeDipho_.passNHiso1 = -500;
  treeDipho_.passNHiso2 = -500;
  treeDipho_.passPHiso1 = -500;
  treeDipho_.passPHiso2 = -500;
  treeDipho_.passSieie1 = -500;
  treeDipho_.passSieie2 = -500;
  treeDipho_.passHoe1 = -500;
  treeDipho_.passHoe2 = -500;
  treeDipho_.passTightCHiso1 = -500;
  treeDipho_.passTightCHiso2 = -500;
  treeDipho_.passTightNHiso1 = -500;
  treeDipho_.passTightNHiso2 = -500;
  treeDipho_.passTightPHiso1 = -500;
  treeDipho_.passTightPHiso2 = -500;
  treeDipho_.passTightSieie1 = -500;
  treeDipho_.passTightSieie2 = -500;
  treeDipho_.passTightHoe1 = -500;
  treeDipho_.passTightHoe2 = -500;
  treeDipho_.passLooseCHiso1 = -500;
  treeDipho_.passLooseCHiso2 = -500;
  treeDipho_.passLooseNHiso1 = -500;
  treeDipho_.passLooseNHiso2 = -500;
  treeDipho_.passLoosePHiso1 = -500;
  treeDipho_.passLoosePHiso2 = -500;
  treeDipho_.passLooseSieie1 = -500;
  treeDipho_.passLooseSieie2 = -500;
  treeDipho_.passLooseHoe1 = -500;
  treeDipho_.passLooseHoe2 = -500;
  treeDipho_.nEle   = -500;
  treeDipho_.nMuons = -500;
  treeDipho_.nJets  = -500;
  treeDipho_.nLooseBjets  = -500;
  treeDipho_.nMediumBjets = -500;
  treeDipho_.vhtruth = -500;
  treeDipho_.metF_GV = -500;
  treeDipho_.metF_HBHENoise = -500;
  treeDipho_.metF_HBHENoiseIso = -500;
  treeDipho_.metF_CSC = -500;
  treeDipho_.metF_eeBadSC = -500;
  treeDipho_.metF_MuonBadTrack = -500;
  treeDipho_.metF_HadronTrackRes = -500;
  treeDipho_.massCorrSmear = -500;
  treeDipho_.massCorrSmearUp = -500;
  treeDipho_.massCorrSmearDown = -500;
  treeDipho_.massCorrScale = -500;
  treeDipho_.massCorrScaleUp = -500;
  treeDipho_.massCorrScaleDown = -500;
  treeDipho_.massRaw = -500;
  treeDipho_.genZ = -500;
  treeDipho_.ptZ = -500;
  treeDipho_.etaZ = -500;
  treeDipho_.phiZ = -500;
  treeDipho_.mva1 = -500;
  treeDipho_.mva2 = -500;
}

void NewDiPhoAnalyzer::SetPuWeights(std::string puWeightFile) {

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

float NewDiPhoAnalyzer::GetPUWeight(float pun) {
  
  float weight=1;
  if (sampleIndex_!=0 && pun<MAX_PU_REWEIGHT && puweights_.size()>0 && dopureweight_) 
    weight = puweights_[pun];
  return weight;
}

// miniAOD preselection + ECAL acceptance
bool NewDiPhoAnalyzer::isGammaPresel( float sceta, float pt, float r9, float chiso, float hoe) {

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

bool NewDiPhoAnalyzer::rediscoveryHLT(float sceta, float pt, float r9,float sieie, float pfIso,float trkSum03 ){
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


double NewDiPhoAnalyzer::getChargedHadronEAForPhotonIso(float eta){
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
double NewDiPhoAnalyzer::getNeutralHadronEAForPhotonIso(float eta) {
  if (fabs(eta) < 1.0) return 0.0599;
  else if (fabs(eta) >= 1.0 && fabs(eta) < 1.479)  return 0.0819;
  else if (fabs(eta) >= 1.479 && fabs(eta) < 2.0 ) return 0.0696;
  else if (fabs(eta) >= 2.0 && fabs(eta) < 2.2 )   return 0.0360;
  else if (fabs(eta) >= 2.2 && fabs(eta) < 2.3 )   return 0.0360;
  else if (fabs(eta) >= 2.3 && fabs(eta) < 2.4 )   return 0.0462;
  else if (fabs(eta) >= 2.4) return 0.0656;
  else return 0.;
}

double NewDiPhoAnalyzer::getGammaEAForPhotonIso(float eta) {
  if (fabs(eta) < 1.0) return 0.1271;
  else if (fabs(eta) >= 1.0 && fabs(eta) < 1.479)  return 0.1101;
  else if (fabs(eta) >= 1.479 && fabs(eta) < 2.0 ) return 0.0756;
  else if (fabs(eta) >= 2.0 && fabs(eta) < 2.2 )   return 0.1175;
  else if (fabs(eta) >= 2.2 && fabs(eta) < 2.3 )   return 0.1498;
  else if (fabs(eta) >= 2.3 && fabs(eta) < 2.4 )   return 0.1857;
  else if (fabs(eta) >= 2.4) return 0.2183;
  else return 0.;
}

bool NewDiPhoAnalyzer::LeadPhoTriggerSel(float eta, float hoe, float r9, float sieie, float phoiso, float pt){
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

bool NewDiPhoAnalyzer::SubLeadPhoTriggerSel(float eta, float hoe, float r9, float sieie, float phoiso, float chiso, float pt){
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
int NewDiPhoAnalyzer::passSieieCuts(float sceta, float sieie){
  int passes = 1;
  if (fabs(sceta)<1.4442 && sieie>0.0102) passes = 0;
  if ((fabs(sceta)>1.566 && fabs(sceta)<2.5) && sieie>0.0268) passes = 0;
  return passes;
}
int NewDiPhoAnalyzer::passCHisoCuts(float sceta, float chiso, float pt){
  int passes = 1;
  if (fabs(sceta)<1.4442 && chiso>1.37) passes = 0;
  if ((fabs(sceta)>1.566 && fabs(sceta)<2.5) && chiso>1.10) passes = 0;
  return passes;
}
int NewDiPhoAnalyzer::passNHisoCuts(float sceta, float nhiso, float pt){
  int passes = 1;
  if (fabs(sceta)<1.4442 && nhiso > (1.06 + 0.014*pt + 0.000019*pt*pt)) passes = 0;
  if ((fabs(sceta)>1.566 && fabs(sceta)<2.5) && nhiso > (2.69 + 0.0139*pt + 0.000025*pt*pt)) passes = 0;
  return passes;
}
int NewDiPhoAnalyzer::passPHisoCuts(float sceta, float phiso, float pt){
  int passes = 1;
  if (fabs(sceta)<1.4442 && phiso > (0.28 + 0.0053*pt)) passes = 0;
  if ((fabs(sceta)>1.566 && fabs(sceta)<2.5) && phiso > (0.39 + 0.0034*pt)) passes = 0;
  return passes;
}
int NewDiPhoAnalyzer::passHoeCuts(float sceta, float hoe){
  int passes = 1;
  if (fabs(sceta)<1.4442 && hoe>0.05) passes = 0;
  if ((fabs(sceta)>1.566 && fabs(sceta)<2.5) && hoe>0.05) passes = 0;
  return passes;
}

// tight working point
int NewDiPhoAnalyzer::passTightSieieCuts(float sceta, float sieie){
  int passes = 1;
  if (fabs(sceta)<1.4442 && sieie>0.0100) passes = 0;
  if ((fabs(sceta)>1.566 && fabs(sceta)<2.5) && sieie>0.0268) passes = 0;
  return passes;
}
int NewDiPhoAnalyzer::passTightCHisoCuts(float sceta, float chiso, float pt){
  int passes = 1;
  if (fabs(sceta)<1.4442 && chiso>0.76) passes = 0;
  if ((fabs(sceta)>1.566 && fabs(sceta)<2.5) && chiso>0.56) passes = 0;
  return passes;
}
int NewDiPhoAnalyzer::passTightNHisoCuts(float sceta, float nhiso, float pt){
  int passes = 1;
  if (fabs(sceta)<1.4442 && nhiso > (0.97 + 0.014*pt + 0.000019*pt*pt)) passes = 0;
  if ((fabs(sceta)>1.566 && fabs(sceta)<2.5) &&  nhiso > (2.09 + 0.0139*pt + 0.000025*pt*pt)) passes = 0;
  return passes;
}
int NewDiPhoAnalyzer::passTightPHisoCuts(float sceta, float phiso, float pt){
  int passes = 1;
  if (fabs(sceta)<1.4442 && phiso > (0.08 + 0.0053*pt)) passes = 0;
  if ((fabs(sceta)>1.566 && fabs(sceta)<2.5) &&  phiso > (0.16 + 0.0034*pt)) passes = 0;
  return passes;
}
int NewDiPhoAnalyzer::passTightHoeCuts(float sceta, float hoe){
  int passes = 1;
  if (fabs(sceta)<1.4442 && hoe>0.05) passes = 0;
  if ((fabs(sceta)>1.566 && fabs(sceta)<2.5) &&hoe>0.05) passes = 0;
  return passes;
}

// loose working point
int NewDiPhoAnalyzer::passLooseSieieCuts(float sceta, float sieie){
  int passes = 1;
  if (fabs(sceta)<1.4442 && sieie>0.0102) passes = 0;
  if ((fabs(sceta)>1.566 && fabs(sceta)<2.5) && sieie>0.0274) passes = 0;
  return passes;
}
int NewDiPhoAnalyzer::passLooseCHisoCuts(float sceta, float chiso, float pt){
  int passes = 1;
  if (fabs(sceta)<1.4442 && chiso>3.32) passes = 0;
  if ((fabs(sceta)>1.566 && fabs(sceta)<2.5) && chiso>1.97) passes = 0;
  return passes;
}
int NewDiPhoAnalyzer::passLooseNHisoCuts(float sceta, float nhiso, float pt){
  int passes = 1;
  if (fabs(sceta)<1.4442 && nhiso > (1.92 + 0.014*pt + 0.000019*pt*pt)) passes = 0;
  if ((fabs(sceta)>1.566 && fabs(sceta)<2.5) &&  nhiso > (11.86 + 0.0139*pt + 0.000025*pt*pt)) passes = 0;
  return passes;
}
int NewDiPhoAnalyzer::passLoosePHisoCuts(float sceta, float phiso, float pt){
  int passes = 1;
  if (fabs(sceta)<1.4442 && phiso > (0.81 + 0.0053*pt)) passes = 0;
  if ((fabs(sceta)>1.566 && fabs(sceta)<2.5) &&  phiso > (0.83 + 0.0034*pt)) passes = 0;
  return passes;
}
int NewDiPhoAnalyzer::passLooseHoeCuts(float sceta, float hoe){
  int passes = 1;
  if (fabs(sceta)<1.4442 && hoe>0.05) passes = 0;
  if ((fabs(sceta)>1.566 && fabs(sceta)<2.5) &&hoe>0.05) passes = 0;
  return passes;
}

// PhotonID: if all true then valid photonID
bool NewDiPhoAnalyzer::testPhotonIsolation(int passSieie, int passCHiso, int passNHiso, int passPHiso, int passHoe, int passEleVeto){
  if (passSieie == 1 && passHoe == 1 && passCHiso == 1 && passNHiso == 1 && passPHiso == 1 && passEleVeto == 1) return true; //passes all selection
  else return false;
}



bool NewDiPhoAnalyzer::isGammaSelected( float rho, float pt, float sceta, float r9, float chiso, float nhiso, float phoiso, float hoe, float sieie, bool passElectronVeto) {
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

int NewDiPhoAnalyzer::effectiveAreaRegion(float sceta) {

  int theEAregion = 999;
  if (fabs(sceta)<=0.9) theEAregion = 0;
  if (fabs(sceta)<=1.5 && fabs(sceta)>0.9)  theEAregion = 1;
  if (fabs(sceta)<=2.0 && fabs(sceta)>1.5)  theEAregion = 2;
  if (fabs(sceta)<=2.2 && fabs(sceta)>2.0)  theEAregion = 3;
  if (fabs(sceta)<=2.5 && fabs(sceta)>2.2)  theEAregion = 4;
  return theEAregion;
}

float NewDiPhoAnalyzer::getSmearingValue(float sceta, float r9, int syst){
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

float NewDiPhoAnalyzer::getScalingValue(int sampleID, float sceta, float r9, int runNumber, int syst){
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


float NewDiPhoAnalyzer::applyEnergySmearing(float ptUncorr, float sceta, float r9, int run){
  float Smearing	= getSmearingValue( sceta, r9, 0 );   
  TRandom Rand(run);
  float Smear 		= Rand.Gaus(1,Smearing);
  float pt = ptUncorr*Smear;
  return pt;
}

float NewDiPhoAnalyzer::applyEnergyScaling(int sampleID, float ptUncorr, float sceta, float r9, int run){
  float Scaling	= getScalingValue(sampleID, sceta, r9, run, 0 );
  float pt = ptUncorr*Scaling; 
  return pt;
}

float NewDiPhoAnalyzer::getPtCorrected(float ptUncorr, float sceta, float r9, int run, int sampleID){
  float pt;
  if(sampleID>=10000) pt = applyEnergyScaling(sampleID, ptUncorr,sceta,r9, run);// is data
  else pt = applyEnergySmearing(ptUncorr,sceta,r9, run);// is MC
  return pt;
}

DEFINE_FWK_MODULE(NewDiPhoAnalyzer);
