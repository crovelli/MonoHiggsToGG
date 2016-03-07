#! /bin/sh
#run extractJSONS.py for all samples in file

# FLASHgg version 1_1_0 w/MET syst for 25ns: 
python extractJSONS.py -i datasets/datasets_rereco74x_fromLivia.json		-o DoubleEG 		-d lists_25ns_v1_1_0_wMETsys/Data

python extractJSONS.py -i datasets/datasets_rereco74x_fromLivia_ewkbkg.json	-o TTGJets  		-d lists_25ns_v1_1_0_wMETsys/MC
python extractJSONS.py -i datasets/datasets_rereco74x_fromLivia_ewkbkg.json	-o TGJets 		-d lists_25ns_v1_1_0_wMETsys/MC
python extractJSONS.py -i datasets/datasets_rereco74x_fromLivia_ewkbkg.json	-o WGToLNuG 		-d lists_25ns_v1_1_0_wMETsys/MC
python extractJSONS.py -i datasets/datasets_rereco74x_fromLivia_ewkbkg.json	-o ZGTo2LG   		-d lists_25ns_v1_1_0_wMETsys/MC

python extractJSONS.py -i datasets/datasets_rereco74x_fromLivia_dy.json	-o DYJetsToLL		-d lists_25ns_v1_1_0_wMETsys/MC

python extractJSONS.py -i datasets/datasets_reminiaod74x_fromLivia.json	-o ttHJetToGG_M125	-d lists_25ns_v1_1_0_wMETsys/MC
python extractJSONS.py -i datasets/datasets_reminiaod74x_fromLivia.json	-o VBFHToGG_M-125	-d lists_25ns_v1_1_0_wMETsys/MC
python extractJSONS.py -i datasets/datasets_reminiaod74x_fromLivia.json	-o DiPhoton		-d lists_25ns_v1_1_0_wMETsys/MC
python extractJSONS.py -i datasets/datasets_reminiaod74x_fromLivia.json	-o QCD_Pt-30to40	-d lists_25ns_v1_1_0_wMETsys/MC
python extractJSONS.py -i datasets/datasets_reminiaod74x_fromLivia.json	-o QCD_Pt-40toInf	-d lists_25ns_v1_1_0_wMETsys/MC
python extractJSONS.py -i datasets/datasets_reminiaod74x_fromLivia.json	-o QCD_Pt-30toInf	-d lists_25ns_v1_1_0_wMETsys/MC
python extractJSONS.py -i datasets/datasets_reminiaod74x_fromLivia.json	-o GJet_Pt-20to40	-d lists_25ns_v1_1_0_wMETsys/MC
python extractJSONS.py -i datasets/datasets_reminiaod74x_fromLivia.json	-o GJet_Pt-40toInf	-d lists_25ns_v1_1_0_wMETsys/MC
python extractJSONS.py -i datasets/datasets_reminiaod74x_fromLivia.json	-o GluGluHToGG_M-125	-d lists_25ns_v1_1_0_wMETsys/MC
python extractJSONS.py -i datasets/datasets_reminiaod74x_fromLivia.json	-o VHToGG_M125		-d lists_25ns_v1_1_0_wMETsys/MC

python extractJSONS.py -i datasets/datasets_reminiaod74x_fromLivia.json	-o 2HDM_MZP600		-d lists_25ns_v1_1_0_wMETsys/MC
python extractJSONS.py -i datasets/datasets_reminiaod74x_fromLivia.json	-o 2HDM_MZP800		-d lists_25ns_v1_1_0_wMETsys/MC
python extractJSONS.py -i datasets/datasets_reminiaod74x_fromLivia.json	-o 2HDM_MZP1000		-d lists_25ns_v1_1_0_wMETsys/MC
python extractJSONS.py -i datasets/datasets_reminiaod74x_fromLivia.json	-o 2HDM_MZP1200		-d lists_25ns_v1_1_0_wMETsys/MC
python extractJSONS.py -i datasets/datasets_reminiaod74x_fromLivia.json	-o 2HDM_MZP1400		-d lists_25ns_v1_1_0_wMETsys/MC
python extractJSONS.py -i datasets/datasets_reminiaod74x_fromLivia.json	-o 2HDM_MZP1700		-d lists_25ns_v1_1_0_wMETsys/MC
python extractJSONS.py -i datasets/datasets_reminiaod74x_fromLivia.json	-o 2HDM_MZP2500		-d lists_25ns_v1_1_0_wMETsys/MC






# FLASHgg version 1_0_0 for 25ns: 
#python extractJSONS.py -i datasets/datasets_rereco74x_all-1_1_0-Or-1_2_0-25ns.json	-o DoubleEG 	-d lists_25ns_v1_1_0/Data
#
#python extractJSONS.py -i datasets/datasets_7415_v1-1-0_all.json	-o ttHJetToGG_M125	-d lists_25ns_v1_1_0/MC
#python extractJSONS.py -i datasets/datasets_7415_v1-1-0_all.json	-o VBFHToGG_M-125	-d lists_25ns_v1_1_0/MC
#python extractJSONS.py -i datasets/datasets_7415_v1-1-0_all.json	-o DiPhoton		-d lists_25ns_v1_1_0/MC
#python extractJSONS.py -i datasets/datasets_7415_v1-1-0_all.json	-o DYJetsToLL		-d lists_25ns_v1_1_0/MC
#python extractJSONS.py -i datasets/datasets_7415_v1-1-0_all.json	-o QCD_Pt-30to40	-d lists_25ns_v1_1_0/MC
#python extractJSONS.py -i datasets/datasets_7415_v1-1-0_all.json	-o QCD_Pt-40toInf	-d lists_25ns_v1_1_0/MC
#python extractJSONS.py -i datasets/datasets_7415_v1-1-0_all.json	-o QCD_Pt-30toInf	-d lists_25ns_v1_1_0/MC
#python extractJSONS.py -i datasets/datasets_7415_v1-1-0_all.json	-o GJet_Pt-20to40	-d lists_25ns_v1_1_0/MC
#python extractJSONS.py -i datasets/datasets_7415_v1-1-0_all.json	-o GJet_Pt-40toInf	-d lists_25ns_v1_1_0/MC
#python extractJSONS.py -i datasets/datasets_7415_v1-1-0_all.json	-o GluGluHToGG_M-125	-d lists_25ns_v1_1_0/MC
#python extractJSONS.py -i datasets/datasets_7415_v1-1-0_all.json	-o VHToGG_M125		-d lists_25ns_v1_1_0/MC
#python extractJSONS.py -i datasets/datasets_VH.json			-o VH			-d lists_25ns_v1_1_0/MC # privVH sample for MC validation
#python extractJSONS.py -i datasets/datasets_7415_v1-1-0_ZpZH.json	-o ZpZH			-d lists_25ns_v1_1_0/MC # privZpZH sample for MC validation
#
#python extractJSONS.py -i datasets/datasets_7415_v1-1-0_sig.json	-o 2HDM_MZp-600		-d lists_25ns_v1_1_0/MC
#python extractJSONS.py -i datasets/datasets_7415_v1-1-0_sig.json	-o 2HDM_MZp-1000	-d lists_25ns_v1_1_0/MC
#python extractJSONS.py -i datasets/datasets_7415_v1-1-0_sig.json	-o 2HDM_MZp-1200	-d lists_25ns_v1_1_0/MC
#python extractJSONS.py -i datasets/datasets_7415_v1-1-0_sig.json	-o 2HDM_MZp-1400	-d lists_25ns_v1_1_0/MC
#python extractJSONS.py -i datasets/datasets_7415_v1-1-0_sig.json	-o 2HDM_MZp-1700	-d lists_25ns_v1_1_0/MC
#python extractJSONS.py -i datasets/datasets_7415_v1-1-0_sig.json	-o 2HDM_MZp-2000	-d lists_25ns_v1_1_0/MC
#python extractJSONS.py -i datasets/datasets_7415_v1-1-0_sig.json	-o 2HDM_MZp-2500	-d lists_25ns_v1_1_0/MC
##python extractJSONS.py -i datasets/datasets_7415_v1-1-0_sig.json	-o 2HDM_MZp-800		-d lists_25ns_v1_1_0/MC
#
#python extractJSONS.py -i datasets/datasets_7415_v1-1-0_sigPrivMC.json	-o 2HDM_MZP600		-d lists_25ns_v1_1_0/MC
#python extractJSONS.py -i datasets/datasets_7415_v1-1-0_sigPrivMC.json	-o 2HDM_MZP1000		-d lists_25ns_v1_1_0/MC
#python extractJSONS.py -i datasets/datasets_7415_v1-1-0_sigPrivMC.json	-o 2HDM_MZP1200		-d lists_25ns_v1_1_0/MC
#python extractJSONS.py -i datasets/datasets_7415_v1-1-0_sigPrivMC.json	-o 2HDM_MZP1400		-d lists_25ns_v1_1_0/MC
#python extractJSONS.py -i datasets/datasets_7415_v1-1-0_sigPrivMC.json	-o 2HDM_MZP1700		-d lists_25ns_v1_1_0/MC
#python extractJSONS.py -i datasets/datasets_7415_v1-1-0_sigPrivMC.json	-o 2HDM_MZP2500		-d lists_25ns_v1_1_0/MC
#python extractJSONS.py -i datasets/datasets_7415_v1-1-0_sigPrivMC.json	-o 2HDM_MZP800		-d lists_25ns_v1_1_0/MC
##python extractJSONS.py -i datasets/datasets_7415_v1-1-0_sigPrivMC.json	-o 2HDM_MZP2000		-d lists_25ns_v1_1_0/MC
#
## dataset C
##python extractJSONS.py -i datasets/datasets_7415_v1-1-0_all.json	-o DoubleEG/RunIISpring15-ReMiniAOD-1_1_0-25ns-1_1_0-v0-Run2015C_25ns-05Oct2015-v1	-d lists_25ns_v1_1_0/Data
#python extractJSONS.py -i datasets/datasets_7415_v1-1-0_all.json	-o DoubleEG/RunIISpring15-ReMiniAOD-1_1_0-25ns-1_1_0-v0-Run2015D-05Oct2015-v1	-d lists_25ns_v1_1_0/Data
#python extractJSONS.py -i datasets/datasets_7415_v1-1-0_all.json	-o DoubleEG/RunIISpring15-Prompt-1_1_0-25ns-1_1_0-v0-Run2015D-PromptReco-v4	-d lists_25ns_v1_1_0/Data
#python extractJSONS.py -i datasets/datasets_7415_v1-1-0_doubleEGrereco.json	-o DoubleEG	-d lists_25ns_v1_1_0/Data
#python extractJSONS.py -i datasets/datasets_7415_v1-1-0_doubleEGprompt.json	-o DoubleEG/RunIISpring15-Prompt-1_1_0-25ns-1_1_0	-d lists_25ns_v1_1_0/Data


# All FLASHgg version Spring15BetaV7:
#python extractJSONS.py -i datasets/datasets_7415_betaV7_sig.json	-o 2HDM_MZP600		-d lists_25ns_v7/MC
#python extractJSONS.py -i datasets/datasets_7415_betaV7_sig.json	-o 2HDM_MZP800		-d lists_25ns_v7/MC
#python extractJSONS.py -i datasets/datasets_7415_betaV7_sig.json	-o 2HDM_MZP1000		-d lists_25ns_v7/MC
#python extractJSONS.py -i datasets/datasets_7415_betaV7_sig.json	-o 2HDM_MZP1200		-d lists_25ns_v7/MC
#python extractJSONS.py -i datasets/datasets_7415_betaV7_sig.json	-o 2HDM_MZP1400		-d lists_25ns_v7/MC
#python extractJSONS.py -i datasets/datasets_7415_betaV7_sig.json	-o 2HDM_MZP1700		-d lists_25ns_v7/MC
#python extractJSONS.py -i datasets/datasets_7415_betaV7_sig.json	-o 2HDM_MZP2000		-d lists_25ns_v7/MC
#python extractJSONS.py -i datasets/datasets_7415_betaV7_sig.json	-o 2HDM_MZP2500		-d lists_25ns_v7/MC
#python extractJSONS.py -i datasets/datasets_7415_betaV7.json	-o DiPhoton		-d lists_25ns_v7/MC
#python extractJSONS.py -i datasets/datasets_7415_betaV7.json	-o DYJetsToLL		-d lists_25ns_v7/MC
#python extractJSONS.py -i datasets/datasets_7415_betaV7.json	-o QCD_Pt-30to40	-d lists_25ns_v7/MC
#python extractJSONS.py -i datasets/datasets_7415_betaV7.json	-o QCD_Pt-40toInf	-d lists_25ns_v7/MC
#python extractJSONS.py -i datasets/datasets_7415_betaV7.json	-o QCD_Pt-30toInf	-d lists_25ns_v7/MC
#python extractJSONS.py -i datasets/datasets_7415_betaV7.json	-o GJet_Pt-20to40	-d lists_25ns_v7/MC
#python extractJSONS.py -i datasets/datasets_7415_betaV7.json	-o GJet_Pt-40toInf	-d lists_25ns_v7/MC
#python extractJSONS.py -i datasets/datasets_7415_betaV7.json	-o GluGluHToGG_M-125	-d lists_25ns_v7/MC
#python extractJSONS.py -i datasets/datasets_7415_betaV7.json	-o VHToGG_M125		-d lists_25ns_v7/MC
#
#python extractJSONS.py -i datasets/datasets_7415_betaV7metaV1.json	-o DoubleEG 	-d lists_25ns_v7/Data
# FLASHgg v7 data separate by runD & runC:
#python extractJSONS.py -i datasets/datasets_7415_betaV7metaV1.json	-o DoubleEG/RunIISpring15-ReMiniAOD-BetaV7-25ns-Spring15BetaV7-v0-Run2015D 	-d lists_25ns_v7/Data
#python extractJSONS.py -i datasets/datasets_7415_betaV7.json	-o DoubleEG/RunIISpring15-ReMiniAOD-BetaV7-25ns-Spring15BetaV7-v0-Run2015C	-d lists_25ns_v7/Data




## with mix of Flashgg versions (spring15 betaV2 and betaV4):

#python extractJSONS.py -i datasets_746_dipho.json -o DiPhoton				-d lists_50ns/MC
#python extractJSONS.py -i datasets_746_WZH.json -o WplusH				-d lists_50ns/MC
#python extractJSONS.py -i datasets_746_WZH.json -o WminusH				-d lists_50ns/MC
#python extractJSONS.py -i datasets_746_WZH.json -o ZH 					-d lists_50ns/MC
#python extractJSONS.py -i datasets_746_sig.json -o Higgs_scalar_nohdecay_gg_1000GeV	-d lists_50ns/MC
#python extractJSONS.py -i datasets_746_sig.json -o Higgs_scalar_nohdecay_gg_100GeV	-d lists_50ns/MC
#python extractJSONS.py -i datasets_746_sig.json -o Higgs_scalar_nohdecay_gg_10GeV	-d lists_50ns/MC
#python extractJSONS.py -i datasets_746_sig.json -o Higgs_scalar_nohdecay_gg_1GeV	-d lists_50ns/MC
#python extractJSONS.py -i datasets.json 	-o GJet_Pt-20to40			-d lists_50ns/MC
#python extractJSONS.py -i datasets.json 	-o GJet_Pt-40toInf 			-d lists_50ns/MC
#python extractJSONS.py -i datasets.json 	-o QCD_Pt-30to40  			-d lists_50ns/MC
#python extractJSONS.py -i datasets.json 	-o QCD_Pt-30toInf 			-d lists_50ns/MC
#python extractJSONS.py -i datasets.json 	-o QCD_Pt-40toInf 			-d lists_50ns/MC
#python extractJSONS.py -i datasets_746.json	-o GluGluHToGG				-d lists_50ns/MC
#python extractJSONS.py -i datasets.json 	-o ttHJetToGG_M125 			-d lists_50ns/MC
##Data
#python extractJSONS.py -i datasets.json 	-o SinglePhoton 			-d lists_50ns/Data
#python extractJSONS.py -i datasets_746dEG.json	-o DoubleEG	 			-d lists_50ns/Data
#python extractJSONS.py -i datasets.json 	-o EGamma	 			-d lists_50ns/Data
