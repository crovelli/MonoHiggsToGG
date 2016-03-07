#! /bin/sh

#############################################
#
# fitterFormatting input arguements:
#   1st: input directory
#   2nd: output directory
#   3rd: type (sig, bkg, data)
#   4th: prompt (for duplicate removal)
#   5th: input filename 
#   6th: sample name
#   7th: outfile name
#   8th: category type (NOT YET USED)
#
# final files used for the fit are:
#   outdir/Output_MC.root
#   outdir/Output_Data.root
#
#############################################
# Sidenote: Safely ignore warning message "tab completion not implemented for this context" 
# This comes from the tabs included for making the inputs to fitterFormatting easier to read below 

indir="../macro/data/25ns_v1-1-0_ReReco_wAllJetInfo/"
outdir="ntuples4fit_boostchiara_met80"

## Categories not implemented yet
## just an idea for making these tools easier
categType="0";
#############################################
# 
#   Category Types
#   0 = Photon Categories, No additional Sel.
#   1 = Photon Categories, MET > 50
#   2 = Photon Categories, pT1 > 80, pT2 > 30
#   3 = Photon Categories, pT1 > 80, pT2 > 30, MET > 50
#   4 = MET Categories
#   5 = MET Categories, pT1 > 80, pT2 > 30  
#
# Photon Categories = all,EE,EB,EEHighR9,EELowR9,EBHighR9,EBLowR9
# MET Categories = met0 (inclusive), met0-50, met50 (50 & above)
#
#############################################

mkdir -p $outdir

root -l -b << EOF
.L fitterFormatting.cc++

fitterFormatting("$indir","$outdir","sig",0,	"2HDM_mZP600.root",	"sig_2HDM_mZP600_mA0300",	"2HDM_mZP600_new.root",$categType)
fitterFormatting("$indir","$outdir","sig",0,	"2HDM_mZP800.root",	"sig_2HDM_mZP800_mA0300",	"2HDM_mZP800_new.root",$categType)
fitterFormatting("$indir","$outdir","sig",0,	"2HDM_mZP1000.root",	"sig_2HDM_mZP1000_mA0300",	"2HDM_mZP1000_new.root",$categType)
fitterFormatting("$indir","$outdir","sig",0,	"2HDM_mZP1200.root",	"sig_2HDM_mZP1200_mA0300",	"2HDM_mZP1200_new.root",$categType)
fitterFormatting("$indir","$outdir","sig",0,	"2HDM_mZP1400.root",	"sig_2HDM_mZP1400_mA0300",	"2HDM_mZP1400_new.root",$categType)
fitterFormatting("$indir","$outdir","sig",0,	"2HDM_mZP1700.root",	"sig_2HDM_mZP1700_mA0300",	"2HDM_mZP1700_new.root",$categType)
fitterFormatting("$indir","$outdir","sig",0,	"2HDM_mZP2500.root",	"sig_2HDM_mZP2500_mA0300",	"2HDM_mZP2500_new.root",$categType)

fitterFormatting("$indir","$outdir","bkg",0,	"GluGluHToGG.root",	"GluGluHToGG",		"GluGluHToGG_new.root",$categType)
fitterFormatting("$indir","$outdir","bkg",0,	"VH.root",		"VH",			"VH_new.root",$categType)
fitterFormatting("$indir","$outdir","bkg",0,	"TTGJets.root",		"TTGJets",		"TTGJets_new.root",$categType)
fitterFormatting("$indir","$outdir","bkg",0,	"TGJets.root",		"TGJets",		"TGJets_new.root",$categType)
fitterFormatting("$indir","$outdir","bkg",0,	"WGToLNuG.root",	"WGToLNuG",		"WGToLNuG_new.root",$categType)
fitterFormatting("$indir","$outdir","bkg",0,	"ZGTo2LG.root",		"ZGTo2LG",		"ZGTo2LG_new.root",$categType)
fitterFormatting("$indir","$outdir","bkg",0,	"ttHJetToGG.root",	"ttHJetToGG",		"ttHJetToGG_new.root",$categType)
fitterFormatting("$indir","$outdir","bkg",0,	"VBFHToGG.root",	"VBFHToGG",		"VBFHToGG_new.root",$categType)
fitterFormatting("$indir","$outdir","bkg",2,	"QCD.root",		"QCD",			"QCD_new.root",$categType)
fitterFormatting("$indir","$outdir","bkg",1,	"GJets.root",		"GJets",		"GJets_new.root",$categType)
fitterFormatting("$indir","$outdir","bkg",0,	"DiPhoton.root",	"DiPhoton",		"DiPhoton_new.root",$categType)
fitterFormatting("$indir","$outdir","bkg",0,	"DYJetsToLL.root",	"DYJetsToLL",		"DYJetsToLL_new.root",$categType)

fitterFormatting("$indir","$outdir","data",0,	"DoubleEG.root",	"DoubleEG",		"Output_Data.root",$categType)

.q

EOF
echo "Done"

echo "Adding MC Files Together"

hadd $outdir/Output_MC.root $outdir/2HDM_mZP* $outdir/GluGluHToGG_new.root $outdir/VH_new.root $outdir/QCD_new.root $outdir/GJets_new.root $outdir/DiPhoton_new.root $outdir/DYJetsToLL_new.root $outdir/VBFHToGG_new.root $outdir/ttHJetToGG_new.root $outdir/TGJets_new.root $outdir/TTGJets_new.root $outdir/WGToLNuG_new.root $outdir/ZGTo2LG_new.root 

