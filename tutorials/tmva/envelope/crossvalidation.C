#include "TMVA/Factory.h"
#include "TMVA/DataLoader.h"
#include "TMVA/Tools.h"

#include "TMVA/CrossValidation.h"

void crossvalidation()
{
   TMVA::Tools::Instance();

   TFile *input(0);
   TString fname = "./tmva_class_example.root";
   if (!gSystem->AccessPathName(fname)) {
      input = TFile::Open(fname); // check if file in local directory exists
   } else {
      TFile::SetCacheFileDir(".");
      input = TFile::Open("http://root.cern.ch/files/tmva_class_example.root", "CACHEREAD");
   }
   if (!input) {
      std::cout << "ERROR: could not open data file" << std::endl;
      exit(1);
   }
   TTree *signal = (TTree *)input->Get("TreeS");
   TTree *background = (TTree *)input->Get("TreeB");

   TMVA::DataLoader *dataloader = new TMVA::DataLoader("dataset");

   dataloader->AddVariable("myvar1 := var1+var2", 'F');
   dataloader->AddVariable("myvar2 := var1-var2", "Expression 2", "", 'F');
   dataloader->AddVariable("var3", "Variable 3", "units", 'F');
   dataloader->AddVariable("var4", "Variable 4", "units", 'F');

   Double_t signalWeight = 1.0;
   Double_t backgroundWeight = 1.0;

   // You can add an arbitrary number of signal or background trees
   dataloader->AddSignalTree(signal, signalWeight);
   dataloader->AddBackgroundTree(background, backgroundWeight);
   dataloader->SetBackgroundWeightExpression("weight");

   dataloader->PrepareTrainingAndTestTree(
      "", "", "nTrain_Signal=1000:nTrain_Background=1000:SplitMode=Random:NormMode=NumEvents:!V");

   TMVA::CrossValidation *cv = new TMVA::CrossValidation(dataloader);

   cv->BookMethod(TMVA::Types::kBDT, "BDT", "!H:!V:NTrees=850:MinNodeSize=2.5%:MaxDepth=3:BoostType=AdaBoost:"
                                            "AdaBoostBeta=0.5:UseBaggedBoost:BaggedSampleFraction=0.5:SeparationType="
                                            "GiniIndex:nCuts=20");
   cv->BookMethod(TMVA::Types::kBDT, "BDTG", "!H:!V:NTrees=1000:MinNodeSize=2.5%:BoostType=Grad:Shrinkage=0.10:"
                                             "UseBaggedBoost:BaggedSampleFraction=0.5:nCuts=20:MaxDepth=2");

   cv->SetNumFolds(4);

   cv->Evaluate();

   TMVA::MsgLogger::EnableOutput();
   TMVA::gConfig().SetSilent(kFALSE);
   cv->GetResults()[0].Print();
   cv->GetResults()[0].Draw();
   cv->GetResults()[1].Print();
   cv->GetResults()[1].Draw();
}
