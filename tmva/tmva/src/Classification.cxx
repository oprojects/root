// @(#)root/tmva $Id$
// Author: Omar Zapata (I need to put all Authors here ;)

#include <iostream>
#include <memory>

#include "TSystem.h"
#include "TAxis.h"
#include "TCanvas.h"
#include "TGraph.h"

#include "TMVA/ClassifierFactory.h"
#include "TMVA/Config.h"
#include "TMVA/Configurable.h"
#include "TMVA/Tools.h"
#include "TMVA/Ranking.h"
#include "TMVA/DataSet.h"
#include "TMVA/IMethod.h"
#include "TMVA/MethodBase.h"
#include "TMVA/DataInputHandler.h"
#include "TMVA/DataSetManager.h"
#include "TMVA/DataSetInfo.h"
#include "TMVA/DataLoader.h"
#include "TMVA/MethodBoost.h"
#include "TMVA/MethodCategory.h"
#include "TMVA/ROCCalc.h"
#include "TMVA/ROCCurve.h"
#include "TMVA/MsgLogger.h"

#include "TMVA/Classification.h"
#include "TMVA/ResultsClassification.h"

#include "TMVA/tmvaglob.h"

const Int_t  MinNoTrainingEvents = 10;

#define READXML          kTRUE


TMVA::ClassificationResult::ClassificationResult()
{
}

TMVA::ClassificationResult::ClassificationResult(const ClassificationResult &/*obj*/)
{
}


void TMVA::ClassificationResult::Print() const
{
    MsgLogger fLogger("Classification");
}


TCanvas* TMVA::ClassificationResult::Draw(const TString name) const
{
    TMVA::TMVAGlob::Initialize();
    TCanvas *c=new TCanvas(name.Data());
//     fROCCurves->Draw("AL");
//     fROCCurves->GetXaxis()->SetTitle(" Signal Efficiency ");
//     fROCCurves->GetYaxis()->SetTitle(" Background Rejection ");
//     Float_t adjust=1+fROCs.size()*0.01;
//     c->BuildLegend(0.15,0.15,0.4*adjust,0.5*adjust);
//     c->SetTitle("Cross Validation ROC Curves");
    TMVA::TMVAGlob::plot_logo();
//     c->Draw();
    return c;
}

TMVA::Classification::Classification(TString name,TString options,DataLoader *dataloader,TFile *file):
Algorithm(Form("Classification(%s)",name.Data()),dataloader,file,options),
fTransformations      ( "I" )
{
    fLogger->SetSource(GetName());
    
    // render silent
    if (gTools().CheckForSilentOption( GetOptions() )) Log().InhibitOutput(); // make sure is silent if wanted to
    
    
    // init configurable
    SetConfigDescription( "Configuration options for Factory running" );
    SetConfigName( GetName() );
    
    // histograms are not automatically associated with the current
    // directory and hence don't go out of scope when closing the file
    // TH1::AddDirectory(kFALSE);
    Bool_t silent          = kFALSE;
    #ifdef WIN32
    // under Windows, switch progress bar and color off by default, as the typical windows shell doesn't handle these (would need different sequences..)
    Bool_t color           = kFALSE;
    Bool_t drawProgressBar = kFALSE;
    #else
    Bool_t color           = !gROOT->IsBatch();
    Bool_t drawProgressBar = kTRUE;
    #endif
    DeclareOptionRef( fVerbose, "V", "Verbose flag" );
    DeclareOptionRef( color,    "Color", "Flag for coloured screen output (default: True, if in batch mode: False)" );
    DeclareOptionRef( fTransformations, "Transformations", "List of transformations to test; formatting example: \"Transformations=I;D;P;U;G,D\", for identity, decorrelation, PCA, Uniform and Gaussianisation followed by decorrelation transformations" );
    DeclareOptionRef( silent,   "Silent", "Batch mode: boolean silent flag inhibiting any output from TMVA after the creation of the factory class object (default: False)" );
    DeclareOptionRef( drawProgressBar,
                      "DrawProgressBar", "Draw progress bar to display training, testing and evaluation schedule (default: True)" );
    DeclareOptionRef( fModelPersistence,
                      "ModelPersistence",
                      "Option to save the trained model in xml file or using serialization");
    
    
    ParseOptions();
    CheckForUnusedOptions();
    
    if (IsVerbose()) Log().SetMinType( kVERBOSE );
    
    // global settings
    gConfig().SetUseColor( color );
    gConfig().SetSilent( silent );
    gConfig().SetDrawProgressBar( drawProgressBar );
    
//     Greetings();//this need to be updated
    
}


TMVA::Classification::~Classification()
{
}

void TMVA::Classification::Train()
{
    if (fMethod.IsEmpty()) {
        Log() << kINFO << "...nothing found to train" << Endl;
        return;
    }
    
    TString methodName    = fMethod.GetValue<TString>("MethodName");
    TString methodTitle   = fMethod.GetValue<TString>("MethodTitle");
    TString methodOptions = fMethod.GetValue<TString>("MethodOptions");
    
    if(IsModelPersistence()) gSystem->MakeDirectory(fDataLoader->GetName());//creating directory for DataLoader output
    
    // interpret option string with respect to a request for boosting (i.e., BostNum > 0)
    Int_t    boostNum = 0;
    TMVA::Configurable* conf = new TMVA::Configurable( methodOptions );
    conf->DeclareOptionRef( boostNum = 0, "Boost_num",
                            "Number of times the classifier will be boosted" );
    conf->ParseOptions();
    delete conf;
    TString fFileDir;
    if(fModelPersistence)
    {
        fFileDir=fDataLoader->GetName();
        fFileDir+="/"+gConfig().GetIONames().fWeightFileDir;
    }
    // initialize methods
    IMethod* im;
    if (!boostNum) {
        im = ClassifierFactory::Instance().Create( std::string(methodName),
                                                   GetName(),
                                                   methodTitle,
                                                   GetDataLoaderDataSetInfo(),
                                                   methodOptions );
    }
    else {
        // boosted classifier, requires a specific definition, making it transparent for the user
        Log() << "Boost Number is " << boostNum << " > 0: train boosted classifier" << Endl;
        im = ClassifierFactory::Instance().Create( std::string("Boost"),
                                                   GetName(),
                                                   methodTitle,
                                                   GetDataLoaderDataSetInfo(),
                                                   methodOptions );
        MethodBoost* methBoost = dynamic_cast<MethodBoost*>(im); // DSMTEST divided into two lines
        if (!methBoost) // DSMTEST
            Log() << kFATAL << "Method with type kBoost cannot be casted to MethodCategory. /Factory" << Endl; // DSMTEST
            
        if(fModelPersistence) methBoost->SetWeightFileDir(fFileDir);
        methBoost->SetModelPersistence(fModelPersistence);
        methBoost->SetBoostedMethodName( methodName); // DSMTEST divided into two lines
        methBoost->fDataSetManager = GetDataLoaderDataSetManager(); // DSMTEST
        methBoost->SetFile(fFile.get());
        methBoost->SetSilentFile(IsSilentFile());
    }
    
    MethodBase *method = dynamic_cast<MethodBase*>(im);
    if (method==0) return ; // could not create method
    
    // set fDataSetManager if MethodCategory (to enable Category to create datasetinfo objects) // DSMTEST
    if (method->GetMethodType() == Types::kCategory) { // DSMTEST
        MethodCategory *methCat = (dynamic_cast<MethodCategory*>(im)); // DSMTEST
        if (!methCat) // DSMTEST
            Log() << kFATAL << "Method with type kCategory cannot be casted to MethodCategory. /Factory" << Endl; // DSMTEST
            
        if(fModelPersistence) methCat->SetWeightFileDir(fFileDir);
        methCat->SetModelPersistence(fModelPersistence);
        methCat->fDataSetManager = GetDataLoaderDataSetManager(); // DSMTEST
        methCat->SetFile(fFile.get());
        methCat->SetSilentFile(IsSilentFile());
    } // DSMTEST
    
    if(fModelPersistence) method->SetWeightFileDir(fFileDir);
    method->SetModelPersistence(fModelPersistence);
    method->SetAnalysisType( Types::kClassification );
    method->SetupMethod();
    method->ParseOptions();
    method->ProcessSetup();
    method->SetFile(fFile.get());
    method->SetSilentFile(IsSilentFile());
    
    // check-for-unused-options is performed; may be overridden by derived classes
    method->CheckSetup();
    
    fMethodBase=std::unique_ptr<MethodBase>(method);
    
    Event::SetIsTraining(kTRUE);
    
    
    if(GetDataLoaderDataInput().GetEntries() <=1) { // 0 entries --> 0 events, 1 entry --> dynamical dataset (or one entry)
        Log() << kFATAL << "No input data for the training provided!" << Endl;
    }
    
    if( fMethodBase->DataInfo().GetNClasses() < 2 ) 
        Log() << kFATAL << "You want to do classification training, but specified less than two classes." << Endl;
    
    
    if (fMethodBase->Data()->GetNTrainingEvents() < MinNoTrainingEvents) {
        Log() << kWARNING << "Method " << fMethodBase->GetMethodName()
        << " not trained (training tree has less entries ["
        << fMethodBase->Data()->GetNTrainingEvents()
        << "] than required [" << MinNoTrainingEvents << "]" << Endl;
    }
    
    Log() << kINFO << "Train method: " << fMethodBase->GetMethodName() << " for Classification" << Endl;
    fMethodBase->TrainMethod();
    Log() << kINFO << "Training finished" << Endl;
    
    if (fModelPersistence) {
        
        Log() << kINFO << "=== Destroy and recreate all methods via weight files for testing ===" << Endl << Endl;
        if(!IsSilentFile()) fFile->cd();
        
        
        TMVA::Types::EMVA methodType = fMethodBase->GetMethodType();
        TString           weightfile = fMethodBase->GetWeightFileName();
        
        // decide if .txt or .xml file should be read:
        if (READXML) weightfile.ReplaceAll(".txt",".xml");
        
        DataSetInfo& dataSetInfo = fMethodBase->DataInfo();
        TString      testvarName = fMethodBase->GetTestvarName();
        fMethodBase=nullptr; //itrMethod[i];
        
        // recreate
        MethodBase*    m = dynamic_cast<MethodBase*>( ClassifierFactory::Instance()
        .Create( std::string(Types::Instance().GetMethodName(methodType)), 
                 dataSetInfo, weightfile ) );
        if( m->GetMethodType() == Types::kCategory ){ 
            MethodCategory *methCat = (dynamic_cast<MethodCategory*>(m));
            if( !methCat ) Log() << kFATAL << "Method with type kCategory cannot be casted to MethodCategory. /Factory" << Endl; 
            else methCat->fDataSetManager = m->DataInfo().GetDataSetManager();
        }
        
        m->SetWeightFileDir(fFileDir);
        m->SetModelPersistence(fModelPersistence);
        m->SetSilentFile(IsSilentFile());
        m->SetAnalysisType(Types::kClassification);
        m->SetupMethod();
        m->ReadStateFromFile();
        m->SetTestvarName(testvarName);
        
        // replace trained method by newly created one (from weight file) in methods vector
        fMethodBase = std::unique_ptr<MethodBase>(m);
    }
}



void TMVA::Classification::Evaluate()
{
    TString methodName    = fMethod.GetValue<TString>("MethodName");
    TString methodTitle   = fMethod.GetValue<TString>("MethodTitle");
    TString methodOptions = fMethod.GetValue<TString>("MethodOptions");
      
}