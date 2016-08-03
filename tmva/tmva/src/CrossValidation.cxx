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

TMVA::CrossValidationResultItem::CrossValidationResultItem():fROCCurves(new TMultiGraph())
{
}

TMVA::CrossValidationResultItem::CrossValidationResultItem(const CrossValidationResultItem &obj)
{
    fROCs=obj.fROCs;
    fROCAVG=obj.fROCAVG;
    fROCCurves = obj.fROCCurves;
}


TMultiGraph *TMVA::CrossValidationResultItem::GetROCCurves(Bool_t /*fLegend*/)
{
    return fROCCurves.get();
}


void TMVA::CrossValidationResultItem::Print() const
{
    MsgLogger fLogger("CrossValidation");
    for(auto &item:fROCs)
        fLogger<<kINFO<<Form("Fold  %i ROC-Int : %f",item.first,item.second)<<std::endl;
    
    fLogger<<kINFO<<Form("Average ROC-Int : %f",fROCAVG)<<Endl;
}


TCanvas* TMVA::CrossValidationResultItem::Draw(const TString name) const
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

TMVA::CrossValidation::CrossValidation(TMVA::DataLoader *loader):Configurable(),
fNFolds(5),fDataLoader(loader)
{
    fClassifier=std::unique_ptr<Factory>(new TMVA::Factory("CrossValidation","!V:!ROC:Silent:Color:DrawProgressBar:AnalysisType=Classification"));
}

TMVA::CrossValidation::~CrossValidation()
{
}



void TMVA::CrossValidation::BookMethod( Types::EMVA method, TString methodTitle, TString options, int numFolds)
{
    return BookMethod(Types::Instance().GetMethodName( method ),methodTitle,options,numFolds);
}

void TMVA::CrossValidation::BookMethod( TString methodName, TString methodTitle, TString options, int numFolds)
{
    OptionMap opt(methodName);
    opt["MethodName"]    = methodName;
    opt["MethodTitle"]   = methodTitle;
    opt["MethodOptions"] = options;
    opt["NumFolds"]      = numFolds;
    
    fMethods.push_back(opt); 
}
void TMVA::CrossValidation::EvaluateAll()
{
  for(auto &item:fMethods)
  {
      item.Print();
      TString methodName    = item.GetValue<TString>("MethodName");
      TString methodTitle   = item.GetValue<TString>("MethodTitle");
      TString methodOptions = item.GetValue<TString>("MethodOptions");
      UInt_t  numFolds      = item.GetValue<UInt_t>("NumFolds");
      
      CrossValidationResultItem resultItem;

      fDataLoader->MakeKFoldDataSet(numFolds);

      const UInt_t nbits = fDataLoader->GetDefaultDataSetInfo().GetNVariables();
      std::vector<TString> varName = fDataLoader->GetDefaultDataSetInfo().GetListOfVariables();
  
      for(UInt_t i = 0; i < numFolds; ++i){    
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

        resultItem.fROCs[i]=fClassifier->GetROCIntegral(foldloader->GetName(),methodTitle);

        auto  gr=fClassifier->GetROCCurve(foldloader->GetName(), methodTitle, true);
    
        gr->SetLineColor(i+1);
        gr->SetLineWidth(2);
        gr->SetTitle(foldloader->GetName());
    
        resultItem.fROCCurves->Add(gr);
    
        TMVA::MethodBase * smethod = dynamic_cast<TMVA::MethodBase*>(fClassifier->fMethodsMap[foldloader->GetName()]->at(0));
        TMVA::ResultsClassification * sresults = (TMVA::ResultsClassification*)smethod->Data()->GetResults(smethod->GetMethodName(), Types::kTesting, Types::kClassification);
        sresults->Delete();

        delete foldloader;
    
        fClassifier->DeleteAllMethods();
        fClassifier->fMethodsMap.clear();
        }

    for(UInt_t r = 0; r < numFolds; ++r){
        resultItem.fROCAVG += resultItem.fROCs.at(r);
    }
    resultItem.fROCAVG /= numFolds;
    fResults.AppendMethod(resultItem);
  }
}




