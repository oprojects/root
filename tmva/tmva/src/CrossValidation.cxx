// @(#)root/tmva $Id$
// Author: Omar Zapata, Thomas James Stevenson.

#include "TMVA/CrossValidation.h"

//CrossValidationResult stuff
// ClassImp(TMVA::CrossValidationResult)

TMVA::CrossValidationResult::CrossValidationResult(std::vector<Float_t> rocs,Float_t rocavg,TMultiGraph   *rocurves):TObject(),
fROCs(rocs),
fROCAVG(fROCAVG),
fROCCurves(rocurves)
{   
}

TMVA::CrossValidationResult::~CrossValidationResult()
{
    if(fROCCurves) delete fROCCurves;
}

TMultiGraph *TMVA::CrossValidationResult::GetROCCurves(Bool_t fLegend)
{
//TODO: summer student
}

//CrossValidation class stuff
// ClassImp(TMVA::CrossValidation)//serialization is not support yet in so many class TMVA

TMVA::CrossValidation::CrossValidation():Configurable( ),
fDataLoader(0)
{
    fClassifier=new TMVA::Factory("CrossValidation","!V:Silent:Color:DrawProgressBar:AnalysisType=Classification");
}


TMVA::CrossValidation::CrossValidation(TMVA::DataLoader *loader):Configurable(),
fDataLoader(loader)
{
    fClassifier=new TMVA::Factory("CrossValidation","!V:Silent:Color:DrawProgressBar:AnalysisType=Classification");    
}

TMVA::CrossValidation::~CrossValidation()
{
    if(fClassifier) delete fClassifier;
}

TMVA::CrossValidationResult* TMVA::CrossValidation::CrossValidate( TString theMethodName, TString methodTitle, TString theOption )
{
    //TODO by Thomas Stevenson 
    //
    
}




