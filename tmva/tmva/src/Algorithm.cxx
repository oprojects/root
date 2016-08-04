// @(#)root/tmva $Id$
// Author: Omar Zapata

#include <iostream>

#include "TMVA/Algorithm.h"
#include "TMVA/MethodBase.h"
#include "TMVA/ResultsClassification.h"
#include "TSystem.h"
#include "TAxis.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TMVA/tmvaglob.h"
#include "TMVA/DataLoader.h"

using namespace TMVA;

Algorithm::Algorithm(const TString &name):Configurable(name),fDataLoader(nullptr)
{}

Algorithm::Algorithm(DataLoader *dalaloader,const TString &name):Configurable(name),fDataLoader(dalaloader)
{}

Algorithm::~Algorithm()
{}

OptionMap &Algorithm::GetMethod()
{
    return fMethod;
}

DataLoader *Algorithm::GetDataLoader()
{
    return fDataLoader.get();
}

void Algorithm::SetDataLoader(DataLoader *dalaloader)
{
        fDataLoader=std::shared_ptr<DataLoader>(dalaloader) ;
}

void TMVA::Algorithm::BookMethod(Types::EMVA method, TString methodTitle, TString options)
{
    return BookMethod(Types::Instance().GetMethodName( method ),methodTitle,options);
}

void TMVA::Algorithm::BookMethod(TString methodName, TString methodTitle, TString options)
{
    fMethod["MethodName"]    = methodName;
    fMethod["MethodTitle"]   = methodTitle;
    fMethod["MethodOptions"] = options;
}
