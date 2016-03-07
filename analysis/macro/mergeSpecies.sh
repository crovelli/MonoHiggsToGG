#! /bin/sh 
# this scripts creates a merged root file in the self-created mergedFinal

#mkdir -p data/mergedFinal

hadd data/25ns_v76X_v0/GJets.root	data/25ns_v76X_v0/GJet_Pt-20to40.root	data/25ns_v76X_v0/GJet_Pt-40toInf.root 
hadd data/25ns_v76X_v0/QCD.root		data/25ns_v76X_v0/QCD_Pt-30to40.root	data/25ns_v76X_v0/QCD_Pt-40toInf.root	#data/25ns_v76X_v0/QCD_Pt-30toInf.root

#hadd data/25ns_v76X_v0/DoubleEG.root	data/25ns_v76X_v0/DoubleEG_05Oct.root	data/25ns_v76X_v0/DoubleEG_PromptV4.root 

