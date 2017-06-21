#include "TMVA/ParallelExecutor.h"
#include "TMVA/Factory.h"
#include "TMVA/DataLoader.h"
#include "TMVA/Tools.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TColor.h"
#include "TStyle.h"
#include "TMVA/CrossValidation.h"
void ptmva_testfactory(Int_t jobs)
{
   TMVA::Tools::Instance();
 
   TFile f("dataset.root");

   TMVA::DataLoader *dataloader=(TMVA::DataLoader *)f.Get("dataset");
  

   TCut mycuts = ""; 
   TCut mycutb = ""; 


   
   auto factory = new TMVA::Factory( "TMVAClassification","!V:!DrawProgressBar:!ROC:Silent:!ModelPersistence:!Color:AnalysisType=Classification");

    factory->BookMethod(dataloader,TMVA::Types::kBDT,"BDT",
                   "!H:!V:NTrees=200:MinNodeSize=2.5%:MaxDepth=3:BoostType=AdaBoost:nCuts=20");

    factory->BookMethod(dataloader,TMVA::Types::kSVM,"SVM",
                   "Gamma=0.25:Tol=0.01:VarTransform=Norm" );

    factory->BookMethod(dataloader,TMVA::Types::kMLP,"MLP",
                   "!H:!V:NeuronType=tanh:VarTransform=N:NCycles=300:HiddenLayers=N+5:TestRate=5:!UseRegulator" );

    factory->BookMethod(dataloader,TMVA::Types::kLD, "LD",
                   "H:!V:VarTransform=None:CreateMVAPdfs:PDFInterpolMVAPdf=Spline2:NbinsMVAPdf=50:NsmoothMVAPdf=10");


     
   TMVA::ParallelExecutor ex;

   
   auto re=ex.Execute(factory,jobs);
   re.Print();
   auto rmap=re.GetResultsMap();
   rmap.Print();
   
   f.Close();
}

Int_t main()
{
    std::vector<int> proc={1,2,3,4};
    for(auto &i:proc)
    {
        ptmva_testfactory(i);
    }
    return 0;
}

