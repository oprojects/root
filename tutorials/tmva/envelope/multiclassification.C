#include "TMVA/DataLoader.h"
#include "TMVA/Tools.h"
#include "TMVA/Classification.h"

void multiclassification()
{
   TMVA::Tools::Instance();

   TFile *input(0);
   TString outfileName = "TMVAMulticlass.root";
   TFile *outputFile = TFile::Open(outfileName, "RECREATE");

   TString fname = "./tmva_example_multiple_background.root";
   if (!gSystem->AccessPathName(fname)) {
      // first we try to find the file in the local directory
      std::cout << "--- TMVAMulticlass   : Accessing " << fname << std::endl;
      input = TFile::Open(fname);
   } else {
      std::cout << "Creating testdata...." << std::endl;
      TString createDataMacro = gROOT->GetTutorialDir() + "/tmva/createData.C";
      gROOT->ProcessLine(TString::Format(".L %s", createDataMacro.Data()));
      gROOT->ProcessLine("create_MultipleBackground(2000)");
      std::cout << " created tmva_example_multiple_background.root for tests of the multiclass features" << std::endl;
      input = TFile::Open(fname);
   }
   if (!input) {
      std::cout << "ERROR: could not open data file" << std::endl;
      exit(1);
   }
   TMVA::DataLoader *dataloader = new TMVA::DataLoader("dataset");

   dataloader->AddVariable("var1", 'F');
   dataloader->AddVariable("var2", "Variable 2", "", 'F');
   dataloader->AddVariable("var3", "Variable 3", "units", 'F');
   dataloader->AddVariable("var4", "Variable 4", "units", 'F');

   TTree *signalTree = (TTree *)input->Get("TreeS");
   TTree *background0 = (TTree *)input->Get("TreeB0");
   TTree *background1 = (TTree *)input->Get("TreeB1");
   TTree *background2 = (TTree *)input->Get("TreeB2");

   gROOT->cd(outfileName + TString(":/"));
   dataloader->AddTree(signalTree, "Signal");
   dataloader->AddTree(background0, "bg0");
   dataloader->AddTree(background1, "bg1");
   dataloader->AddTree(background2, "bg2");

   dataloader->PrepareTrainingAndTestTree("", "SplitMode=Random:NormMode=NumEvents:!V");

   TMVA::Experimental::Classification *cl = new TMVA::Experimental::Classification(dataloader, ""); //,&outputFile);

   // gradient boosted decision trees
   cl->BookMethod(
      TMVA::Types::kBDT, "BDTG",
      "!H:!V:NTrees=1000:BoostType=Grad:Shrinkage=0.10:UseBaggedBoost:BaggedSampleFraction=0.50:nCuts=20:MaxDepth=2");
   // neural network
   cl->BookMethod(TMVA::Types::kMLP, "MLP",
                  "!H:!V:NeuronType=tanh:NCycles=1000:HiddenLayers=N+5,5:TestRate=5:EstimatorType=MSE");
   // functional discriminant with GA minimizer
   cl->BookMethod(TMVA::Types::kFDA, "FDA_GA", "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);"
                                               "(-10,10);(-10,10);(-10,10):FitMethod=GA:PopSize=300:Cycles=3:Steps=20:"
                                               "Trim=True:SaveBestGen=1");
   // PDE-Foam approach
   cl->BookMethod(
      TMVA::Types::kPDEFoam, "PDEFoam",
      "!H:!V:TailCut=0.001:VolFrac=0.0666:nActiveCells=500:nSampl=2000:nBin=5:Nmin=100:Kernel=None:Compress=T");

   //     cl->Train();
   //     cl->Test();
   //     cl->TrainMethod(TMVA::Types::kBDT,"BDT");
   //     cl->TrainMethod(TMVA::Types::kBDT,"BDTG");
   //     cl->TestMethod(TMVA::Types::kBDT,"BDT");
   //     cl->TestMethod(TMVA::Types::kBDT,"BDTG");

   cl->Evaluate();
   //    auto &r=cl->GetResults();
   //    r.Print();
   //    r.Draw();
   //    outputFile.Close();
   delete cl;
}
