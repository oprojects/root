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

Algorithm::Algorithm(const TString &name,DataLoader *dalaloader,TFile *file,const TString options):Configurable(options,name),fDataLoader(dalaloader),fFile(file),fVerbose(kFALSE)
{}

Algorithm::~Algorithm()
{}

Bool_t  Algorithm::IsSilentFile(){return fFile==nullptr;}

TFile* Algorithm::GetFile(){return fFile.get();}

void   Algorithm::SetFile(TFile *file){fFile=std::shared_ptr<TFile>(file);}

OptionMap &Algorithm::GetMethod(){     return fMethod;}

DataLoader *Algorithm::GetDataLoader(){    return fDataLoader.get();}

void Algorithm::SetDataLoader(DataLoader *dalaloader){
        fDataLoader=std::shared_ptr<DataLoader>(dalaloader) ;
}

Bool_t TMVA::Algorithm::IsModelPersistence(){return fModelPersistence; }

void TMVA::Algorithm::SetModelPersistence(Bool_t status){fModelPersistence=status;}


void TMVA::Algorithm::BookMethod(Types::EMVA method, TString methodTitle, TString options){
    return BookMethod(Types::Instance().GetMethodName( method ),methodTitle,options);
}

void TMVA::Algorithm::BookMethod(TString methodName, TString methodTitle, TString options){
    fMethod["MethodName"]    = methodName;
    fMethod["MethodTitle"]   = methodTitle;
    fMethod["MethodOptions"] = options;
}
