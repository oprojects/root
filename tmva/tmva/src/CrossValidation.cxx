// @(#)root/tmva $Id$
// Author: Omar Zapata, Thomas James Stevenson.

#include <iostream>

#include "TMVA/CrossValidation.h"
#include "TMVA/MethodBase.h"
#include "TMVA/ResultsClassification.h"
#include "TSystem.h"
#include "TAxis.h"
#include "TCanvas.h"
#include "TGraph.h"
#include <memory>
#include "TMVA/tmvaglob.h"

TMVA::CrossValidationResult::CrossValidationResult():fROCCurves(new TMultiGraph())
{
}

TMVA::CrossValidationResult::CrossValidationResult(const CrossValidationResult &obj)
{
    fROCs=obj.fROCs;
    fROCCurves = obj.fROCCurves;
}


TMultiGraph *TMVA::CrossValidationResult::GetROCCurves(Bool_t /*fLegend*/)
{
    return fROCCurves.get();
}

Float_t TMVA::CrossValidationResult::GetROCAverage() const
{
    Float_t avg=0;
    for(auto &roc:fROCs) avg+=roc.second;
    return avg/fROCs.size();
}


void TMVA::CrossValidationResult::Print() const
{
    TMVA::MsgLogger::EnableOutput();
    TMVA::gConfig().SetSilent(kFALSE);   
    
    MsgLogger fLogger("CrossValidation");
    for(auto &item:fROCs)
        fLogger<<kINFO<<Form("Fold  %i ROC-Int : %f",item.first,item.second)<<std::endl;
    
    fLogger<<kINFO<<Form("Average ROC-Int : %f",GetROCAverage())<<Endl;

    TMVA::gConfig().SetSilent(kTRUE);   
    
}


TCanvas* TMVA::CrossValidationResult::Draw(const TString name) const
{
    TMVA::TMVAGlob::Initialize();
    TCanvas *c=new TCanvas(name.Data());
    fROCCurves->Draw("AL");
    fROCCurves->GetXaxis()->SetTitle(" Signal Efficiency ");
    fROCCurves->GetYaxis()->SetTitle(" Background Rejection ");
    Float_t adjust=1+fROCs.size()*0.01;
    c->BuildLegend(0.15,0.15,0.4*adjust,0.5*adjust);
    c->SetTitle("Cross Validation ROC Curves");
    TMVA::TMVAGlob::plot_logo();
    c->Draw();
    return c;
}

TMVA::CrossValidation::CrossValidation(TMVA::DataLoader *dataloader):TMVA::Algorithm("CrossValidation",dataloader),
fNumFolds(5),fClassifier(new TMVA::Factory("CrossValidation","!V:!ROC:Silent:!ModelPersistence:!Color:!DrawProgressBar:AnalysisType=Classification"))
{
    fFoldStatus=kFALSE;
}

TMVA::CrossValidation::~CrossValidation()
{
    fClassifier=nullptr;
}

void TMVA::CrossValidation::SetNumFolds(UInt_t i)
{
    fNumFolds=i;
    fDataLoader->MakeKFoldDataSet(fNumFolds);
    fFoldStatus=kTRUE;
}


void TMVA::CrossValidation::Evaluate()
{
    TString methodName    = fMethod.GetValue<TString>("MethodName");
    TString methodTitle   = fMethod.GetValue<TString>("MethodTitle");
    TString methodOptions = fMethod.GetValue<TString>("MethodOptions");
    if(!fFoldStatus)
    {
        fDataLoader->MakeKFoldDataSet(fNumFolds);
        fFoldStatus=kTRUE;
    }
      const UInt_t nbits = fDataLoader->GetDefaultDataSetInfo().GetNVariables();
      std::vector<TString> varName = fDataLoader->GetDefaultDataSetInfo().GetListOfVariables();
  
      for(UInt_t i = 0; i < fNumFolds; ++i){    
        TString foldTitle = methodTitle;
        foldTitle += "_fold";
        foldTitle += i+1;
    
        fDataLoader->PrepareTrainingAndTestTree(i, TMVA::Types::kTesting);

        TMVA::DataLoader * foldloader = new TMVA::DataLoader(foldTitle);

        for(UInt_t index = 0; index < nbits; ++ index) foldloader->AddVariable(varName.at(index), 'F');

        DataLoaderCopy(foldloader,fDataLoader.get());
    
        fClassifier->BookMethod(foldloader, methodName, methodTitle, methodOptions);

        fClassifier->TrainAllMethods();
        fClassifier->TestAllMethods();
        fClassifier->EvaluateAllMethods();

        fResults.fROCs[i]=fClassifier->GetROCIntegral(foldloader->GetName(),methodTitle);

        auto  gr=fClassifier->GetROCCurve(foldloader->GetName(), methodTitle, true);
    
        gr->SetLineColor(i+1);
        gr->SetLineWidth(2);
        gr->SetTitle(foldloader->GetName());
    
        fResults.fROCCurves->Add(gr);
    
        delete foldloader;
    
        fClassifier->DeleteAllMethods();
        }
        TMVA::MsgLogger::EnableOutput();
        TMVA::gConfig().SetSilent(kFALSE);   
        Log()<<kINFO<<"Evaluation done."<<Endl;
        TMVA::gConfig().SetSilent(kTRUE);   
        

}

void TMVA::CrossValidation::EvaluateFold(UInt_t fold)//you need to create the folds in dataloader first
{
    TString methodName    = fMethod.GetValue<TString>("MethodName");
    TString methodTitle   = fMethod.GetValue<TString>("MethodTitle");
    TString methodOptions = fMethod.GetValue<TString>("MethodOptions");
            
    const UInt_t nbits = fDataLoader->GetDefaultDataSetInfo().GetNVariables();
    std::vector<TString> varName = fDataLoader->GetDefaultDataSetInfo().GetListOfVariables();

    TString foldTitle = methodTitle;
    foldTitle += "_fold";
    foldTitle += fold+1;
    
    if(!fFoldStatus){
    fDataLoader->MakeKFoldDataSet(fNumFolds);
    fFoldStatus=kTRUE;
    }
    
    fDataLoader->PrepareTrainingAndTestTree(fold, TMVA::Types::kTesting);
        
    TMVA::DataLoader * foldloader = new TMVA::DataLoader(foldTitle);
        
    for(UInt_t index = 0; index < nbits; ++ index) foldloader->AddVariable(varName.at(index), 'F');
        
    DataLoaderCopy(foldloader,fDataLoader.get());
        
    TMVA::MethodBase *smethod=fClassifier->BookMethod(foldloader, methodName, methodTitle, methodOptions);
        
    fClassifier->TrainAllMethods();
    fClassifier->TestAllMethods();
    fClassifier->EvaluateAllMethods();
        
    fResults.fROCs[fold]=fClassifier->GetROCIntegral(foldloader->GetName(),methodTitle);
        
    auto  gr=fClassifier->GetROCCurve(foldloader->GetName(), methodTitle, true);
        
    gr->SetLineColor(fold+1);
    gr->SetLineWidth(2);
    gr->SetTitle(foldloader->GetName());
        
    fResults.fROCCurves->Add(gr);
        
    TMVA::ResultsClassification * sresults = (TMVA::ResultsClassification*)smethod->Data()->GetResults(smethod->GetMethodName(), Types::kTesting, Types::kClassification);
    if(sresults)
    {
        sresults->Delete();
        delete sresults;
    }
    delete foldloader;
        
    fClassifier->DeleteAllMethods();
    fClassifier->fMethodsMap.clear();
    
}

