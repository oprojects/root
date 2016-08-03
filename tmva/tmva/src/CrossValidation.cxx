// @(#)root/tmva $Id$
// Author: Omar Zapata, Thomas James Stevenson.

#include <iostream>

#include "TMVA/CrossValidation.h"
#include "TMVA/MethodBase.h"
#include "TMVA/ResultsClassification.h"
#include "TSystem.h"
#include <memory>
//CrossValidationResult stuff
// ClassImp(TMVA::CrossValidationResult)

// TMVA::CrossValidationResult::CrossValidationResult(std::map<Float_t> rocs,Float_t rocavg,TMultiGraph   *rocurves):TObject(),
// fROCs(rocs),
// fROCAVG(rocavg),
// fROCCurves(rocurves)
// {   
// }

TMVA::CrossValidationResult::CrossValidationResult()
{
}

TMVA::CrossValidationResult::~CrossValidationResult()
{

}

TMultiGraph *TMVA::CrossValidationResult::GetROCCurves(Bool_t /*fLegend*/)
{
//TODO: summer student
    return 0;
}

//CrossValidation class stuff
// ClassImp(TMVA::CrossValidation)//serialization is not support yet in so many class TMVA



TMVA::CrossValidation::CrossValidation(TMVA::DataLoader *loader):Configurable(),
fNFolds(5),fDataLoader(loader)
{
    fClassifier=std::unique_ptr<Factory>(new TMVA::Factory("CrossValidation","!V:!ROC:Silent:Color:DrawProgressBar:AnalysisType=Classification"));
}

TMVA::CrossValidation::~CrossValidation()
{
}

const TMVA::CrossValidationResult* TMVA::CrossValidation::Evaluate( Types::EMVA theMethod, TString methodTitle, TString theOption, int numFolds)
{
    return Evaluate(Types::Instance().GetMethodName( theMethod ),methodTitle,theOption,numFolds);
}
const TMVA::CrossValidationResult* TMVA::CrossValidation::Evaluate( TString theMethodName, TString methodTitle, TString theOption, int NumFolds)
{
  
  CrossValidationResult * result = new CrossValidationResult();

  fDataLoader->MakeKFoldDataSet(NumFolds);

  const int nbits = fDataLoader->GetDefaultDataSetInfo().GetNVariables();
  std::vector<TString> varName = fDataLoader->GetDefaultDataSetInfo().GetListOfVariables();
  
  for(Int_t i = 0; i < NumFolds; ++i){
    
    TString foldTitle = methodTitle;
    foldTitle += "_fold";
    foldTitle += i+1;
    
    fDataLoader->PrepareTrainingAndTestTree(i, TMVA::Types::kTesting);

    TMVA::DataLoader * foldloader = new TMVA::DataLoader(foldTitle);

    for(int index = 0; index < nbits; ++ index){

        foldloader->AddVariable(varName.at(index), 'F');

    }

    DataLoaderCopy(foldloader,fDataLoader.get());
    
    fClassifier->BookMethod(foldloader, theMethodName, methodTitle, theOption);

    fClassifier->TrainAllMethods();
    fClassifier->TestAllMethods();
    fClassifier->EvaluateAllMethods();

    result->fROCs[i]=fClassifier->GetROCIntegral(foldloader->GetName(),methodTitle);

    TMVA::MethodBase * smethod = dynamic_cast<TMVA::MethodBase*>(fClassifier->fMethodsMap[foldloader->GetName()]->at(0));
    TMVA::ResultsClassification * sresults = (TMVA::ResultsClassification*)smethod->Data()->GetResults(smethod->GetMethodName(), Types::kTesting, Types::kClassification);
    sresults->Delete();

    delete foldloader;
    
    fClassifier->DeleteAllMethods();

    fClassifier->fMethodsMap.clear();
  }

  for(int r = 0; r < NumFolds; ++r){
    result->fROCAVG += result->fROCs.at(r);
  }
  result->fROCAVG /= NumFolds;

  
  return result;
}




