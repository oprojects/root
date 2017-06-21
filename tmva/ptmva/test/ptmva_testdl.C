#include "TMVA/DataLoader.h"
#include "TMVA/Tools.h"
#include "TFile.h"
#include "TTree.h"

//DataLoader seralization
void ptmva_testdl()
{
   TMVA::Tools::Instance();
   
   TFile *input = TFile::Open( "http://root.cern.ch/files/tmva_class_example.root" );
   TTree *signal     = (TTree*)input->Get("TreeS");
   TTree *background = (TTree*)input->Get("TreeB");

   TMVA::DataLoader *dataloader=new TMVA::DataLoader("dataset");

   dataloader->AddVariable( "myvar1 := var1+var2", 'F' );
   dataloader->AddVariable( "myvar2 := var1-var2", "Expression 2", "", 'F' );
   dataloader->AddVariable( "var3",                "Variable 3", "units", 'F' );
   dataloader->AddVariable( "var4",                "Variable 4", "units", 'F' );

   Double_t signalWeight     = 1.0;
   Double_t backgroundWeight = 1.0;
   
   // You can add an arbitrary number of signal or background trees
   dataloader->AddSignalTree    ( signal,     signalWeight     );
   dataloader->AddBackgroundTree( background, backgroundWeight );
   dataloader->SetBackgroundWeightExpression( "weight" );


   dataloader->PrepareTrainingAndTestTree( "", "",
                                        "nTrain_Signal=1000:nTrain_Background=1000:SplitMode=Random:NormMode=NumEvents:!V" );
   
   TFile f("dataset.root","RECREATE");
   dataloader->Write("dataset");
   f.Close();
}


Int_t main()
{
ptmva_testdl();
}
