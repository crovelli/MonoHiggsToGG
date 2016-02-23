-----------------------------------------------------------
# MonoHiggsToGG analysis
-----------------------------------------------------------
This repo is https://github.com/mez34/MonoHiggsToGG

But originally comes from: https://github.com/mez34/MonoHgg

Check there for History before 17 Nov 2015

-----------------------------------------------------------


-----------------------------------------------------------
# Run the Analysis
-----------------------------------------------------------

# MicroAOD to diPhotonTrees 
This package depends on [flashgg](https://github.com/cms-analysis/flashgg).

(Follow what is done here from P. Musella: https://github.com/cms-analysis/flashgg/tree/master/MetaData 
 and here from C. Rovelli: https://github.com/musella/diphotons/tree/master/fullAnalysisRoma )

## Step 1) Produce catalogue of MicroAODs
Once MicroAOD files are produced run these scripts to create the json file (catalogue) and  compute the weights:

- `fggManageSamples.py -C testMonoH -V  import`
- `fggManageSamples.py -C testMonoH review`
- `fggManageSamples.py -C testMonoH check`

## Step 2) Copy json to the scripts/list* directory
- Copy directly from FLASHgg catalogue or list as produced in Step 1.
- If json file is not separated by name can extract smaller json files with:
`./runExtractJSONS.sh` which calls: `python extractJSONS.py -i input.json -o samplename -d outputdir` 

## Step 3) Extract files and weights
This creates .list and .weight files in list* directory

Write the proper name of the catalogue in the extract*.py scripts and write the name of the samples in:
- `python extractWeights.py`
- `python extractFiles.py`

OR run `./runExtractFilesAndWeights.sh` does the same thing (takes input list of .json files and outputs weight and files in same list dir.)

## Step 4) Run in local the diphoton analyzer (from python directory):
- Write by hand one microAOD file that can be taken from the json file
- Fix by hand xsec and sumDataSet that can be found from the json file corresponding
- Optional: put in PU reweighting file
- called by `cmsRun diPhoAna.py`
   
## Step 5) Run in batch the diphoton analyzer (from script directory):
To make it works one needs:
- a list of files in script/list directory with the name of the samples 
- a list of weights in script/list directoy with the name of the samples
- the name of the list directory needs to be written in submitBatchDiPho.py
- the value of the xsec
- add optional PU reweighting & input PU reweighting file
- the output directory either in eos or in lxpus (this has to be fixed in the submitBatchDiPho.py script by hand)

Example on how to run: 
``` 
./submitBatchDiPho.py --cfg diPhoAnaBATCH.py GJets_HT-100to200 0 7 pippo 1534. 1 
```
NB. The name GJets_HT-100to200 has to match the one of the .list and the .weight files.

Can also run: `./submitAll_DiPhioton.py`
      
## Step 6) Manage the output trees before making plots 
From the macro directory:

- Merge the output files with `./mergeTrees.sh`
- Add the weights to the trees with addWeightsToTree.cc run by `./weighTrees.sh` which addWeights for lumi (in pb^-1)
- Merge the species with `./mergeSpecies.sh`

NB. The structure of how to use these scripts can be seen in `doAll.sh`

## Step 7) Produce plots 
The analysis is done in CMSSW_7_4_15
- `make` (to compile) 
- `./main` (to run)

In main.cpp set the following bools:
- (makePURWfiles) : calls ReweightPU.cpp --- makes PURW files for samples)
- (doReweightPU)  : opens PURW files     --- does PURW instead of weighting=1
- (doMETCorr)	  : apply MET phi corr   --- MET phi correction is applied to t1pfmet 
- (doBlind)	  : blinds data in Plots --- blinds the data mass & met distributions
- (doQCDrescale)  : rescale GJet for QCD --- rescale GJet by QCD integral to estimate the QCD contribution

- (doPlots) 	  : calls Plotter.cpp 	 --- makes the histos for each sample individually
- (doComb)  	  : calls Combiner.cpp 	 --- overlays and stacks samples in plots
- (doABCD)	  : calls ABCDMethod.cpp --- does the ABCD/C&C analysis

The style for the plots is set with Style.cpp.

-----------------------------------------------------------
# Copy the Framework from Github
-----------------------------------------------------------
```
cmsrel CMSSW_7_4_15
cmsenv 

cd ${CMSSW_BASE}/src
git cms-init

# clone flashgg 
cd ${CMSSW_BASE}/src
git clone https://github.com/cms-analysis/flashgg.git
cd flashgg

# get latest version of FLASHgg
git checkout 1_1_0

cd ${CMSSW_BASE}/src
bash flashgg/setup.sh | tee setup.log

# clone this repository
cd ${CMSSW_BASE}/src
git clone git@github.com:mez34/MonoHiggsToGG.git

# add Math package
git cms-addpkg DataFormats/Math

# now compile everything
cd ${CMSSW_BASE}/src
scram b -j 16
```


###MicroAOD file to test the dumper:
root://eoscms//eos/cms/store/group/phys_higgs/soffi/MonoX/MonoH/MicroAOD/test/MicroAOD_GluGluToHToGG_M-125_13TeV.root
