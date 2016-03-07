#! /bin/sh 
# this scripts creates a merged root file in the self-created merged

# FLASHgg version 1_1_0
mkdir -p data/25ns_v76X_v0/
mkdir -p /afs/cern.ch/work/m/mzientek/private/25ns_v76X_v0/

hadd /afs/cern.ch/work/m/mzientek/private/25ns_v76X_v0/DiPhoton.root			../../output/job_201634_11/DiPhoton/DiPhoton*.root
hadd /afs/cern.ch/work/m/mzientek/private/25ns_v76X_v0/GJet_Pt-20to40.root		../../output/job_201634_11/GJet_Pt-20to40/GJet_Pt-20to40*.root
hadd /afs/cern.ch/work/m/mzientek/private/25ns_v76X_v0/GJet_Pt-40toInf.root		../../output/job_201634_11/GJet_Pt-40toInf/GJet_Pt-40toInf*.root
hadd /afs/cern.ch/work/m/mzientek/private/25ns_v76X_v0/QCD_Pt-30to40.root		../../output/job_201634_11/QCD_Pt-30to40/QCD_Pt-30to40*.root
#hadd /afs/cern.ch/work/m/mzientek/private/25ns_v76X_v0/QCD_Pt-30toInf.root		../../output/job_201634_11/QCD_Pt-30toInf/QCD_Pt-30toInf*.root
hadd /afs/cern.ch/work/m/mzientek/private/25ns_v76X_v0/QCD_Pt-40toInf.root		../../output/job_201634_11/QCD_Pt-40toInf/QCD_Pt-40toInf*.root
hadd /afs/cern.ch/work/m/mzientek/private/25ns_v76X_v0/GluGluHToGG.root			../../output/job_201634_11/GluGluHToGG/GluGluHToGG*.root
hadd /afs/cern.ch/work/m/mzientek/private/25ns_v76X_v0/VH.root				../../output/job_201634_11/VH/VH*.root
hadd /afs/cern.ch/work/m/mzientek/private/25ns_v76X_v0/DYJetsToLL.root			../../output/job_201634_11/DYJetsToLL/DYJetsToLL*.root
hadd /afs/cern.ch/work/m/mzientek/private/25ns_v76X_v0/ttHJetToGG.root			../../output/job_201634_11/ttHJetToGG/ttHJetToGG*.root
hadd /afs/cern.ch/work/m/mzientek/private/25ns_v76X_v0/VBFHToGG.root			../../output/job_201634_11/VBFHToGG/VBFHToGG*.root
hadd /afs/cern.ch/work/m/mzientek/private/25ns_v76X_v0/WGToLNuG.root			../../output/job_201634_11/WGToLNuG/WGToLNuG*.root
hadd /afs/cern.ch/work/m/mzientek/private/25ns_v76X_v0/ZGTo2LG.root			../../output/job_201634_11/ZGTo2LG/ZGTo2LG*.root
hadd /afs/cern.ch/work/m/mzientek/private/25ns_v76X_v0/TGJets.root			../../output/job_201634_11/TGJets/TGJets*.root
hadd /afs/cern.ch/work/m/mzientek/private/25ns_v76X_v0/TTGJets.root			../../output/job_201634_11/TTGJets/TTGJets*.root
hadd /afs/cern.ch/work/m/mzientek/private/25ns_v76X_v0/TTGG_0Jets.root			../../output/job_201634_11/TTGG_0Jets/TTGG_0Jets*.root

#hadd /afs/cern.ch/work/m/mzientek/private/25ns_v76X_v0/2HDM_mZP600.root		../../output/job_201634_11/2HDM_MZP600/2HDM_MZP600*.root
#hadd /afs/cern.ch/work/m/mzientek/private/25ns_v76X_v0/2HDM_mZP800.root		../../output/job_201634_11/2HDM_MZP800/2HDM_MZP800*.root
#hadd /afs/cern.ch/work/m/mzientek/private/25ns_v76X_v0/2HDM_mZP1000.root		../../output/job_201634_11/2HDM_MZP1000/2HDM_MZP1000*.root
#hadd /afs/cern.ch/work/m/mzientek/private/25ns_v76X_v0/2HDM_mZP1200.root		../../output/job_201634_11/2HDM_MZP1200/2HDM_MZP1200*.root
#hadd /afs/cern.ch/work/m/mzientek/private/25ns_v76X_v0/2HDM_mZP1400.root		../../output/job_201634_11/2HDM_MZP1400/2HDM_MZP1400*.root
#hadd /afs/cern.ch/work/m/mzientek/private/25ns_v76X_v0/2HDM_mZP1700.root		../../output/job_201634_11/2HDM_MZP1700/2HDM_MZP1700*.root
#hadd /afs/cern.ch/work/m/mzientek/private/25ns_v76X_v0/2HDM_mZP2500.root		../../output/job_201634_11/2HDM_MZP2500/2HDM_MZP2500*.root

hadd /afs/cern.ch/work/m/mzientek/private/25ns_v76X_v0/DoubleEG.root  			../../output/job_201634_11/DoubleEG/DoubleEG*.root

