#! /bin/sh

#############################################
#
# fitterFormatting_METcat input arguements:
#   1st: input directory
#   2nd: output directory
#   3rd: type (sig, bkg, data)
#   4th: prompt (for duplicate removal)
#   5th: input filename 
#   6th: sample name
#   7th: outfile name
#
# final files used for the fit are:
#   outdir/Output_MC.root
#   outdir/Output_Data.root
#
#############################################


indir="../macro/data/25ns_v1-1-0_ReReco/"
outdir="ntuples4fit_pho_met50_100"

mkdir -p $outdir

# Sidenote: Safely ignore warning message "tab completion not implemented for this context" 
# This comes from the tabs included for making the inputs to fitterFormatting easier to read below 

root -l -b << EOF
.L fitterFormatting_METcat.cc++

fitterFormatting("$indir","$outdir","sig",0,	"2HDM_mZP600.root",	"sig_2HDM_mZP600_mA0300",	"2HDM_mZP600_new.root")
fitterFormatting("$indir","$outdir","sig",0,	"2HDM_mZP800.root",	"sig_2HDM_mZP800_mA0300",	"2HDM_mZP800_new.root")
fitterFormatting("$indir","$outdir","sig",0,	"2HDM_mZP1000.root",	"sig_2HDM_mZP1000_mA0300",	"2HDM_mZP1000_new.root")
fitterFormatting("$indir","$outdir","sig",0,	"2HDM_mZP1200.root",	"sig_2HDM_mZP1200_mA0300",	"2HDM_mZP1200_new.root")
fitterFormatting("$indir","$outdir","sig",0,	"2HDM_mZP1400.root",	"sig_2HDM_mZP1400_mA0300",	"2HDM_mZP1400_new.root")
fitterFormatting("$indir","$outdir","sig",0,	"2HDM_mZP1700.root",	"sig_2HDM_mZP1700_mA0300",	"2HDM_mZP1700_new.root")
fitterFormatting("$indir","$outdir","sig",0,	"2HDM_mZP2500.root",	"sig_2HDM_mZP2500_mA0300",	"2HDM_mZP2500_new.root")

fitterFormatting("$indir","$outdir","bkg",0,	"GluGluHToGG.root",	"GluGluHToGG",		"GluGluHToGG_new.root")
fitterFormatting("$indir","$outdir","bkg",0,	"VH.root",		"VH",			"VH_new.root")
fitterFormatting("$indir","$outdir","bkg",2,	"QCD.root",		"QCD",			"QCD_new.root")
fitterFormatting("$indir","$outdir","bkg",1,	"GJets.root",		"GJets",		"GJets_new.root")
fitterFormatting("$indir","$outdir","bkg",0,	"DiPhoton.root",	"DiPhoton",		"DiPhoton_new.root")
fitterFormatting("$indir","$outdir","bkg",0,	"DYJetsToLL.root",	"DYJetsToLL",		"DYJetsToLL_new.root")

fitterFormatting("$indir","$outdir","data",0,	"DoubleEG.root",	"DoubleEG",		"Output_Data.root")

.q

EOF
echo "Done"

echo "Adding MC Files Together"

hadd $outdir/Output_MC.root $outdir/2HDM_mZP* $outdir/GluGluHToGG_new.root $outdir/VH_new.root $outdir/QCD_new.root $outdir/GJets_new.root $outdir/DiPhoton_new.root $outdir/DYJetsToLL_new.root 
