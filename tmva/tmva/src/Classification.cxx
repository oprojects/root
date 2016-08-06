// @(#)root/tmva $Id$
// Author: Omar Zapata (I need to put all Authors here ;)

#include <iostream>
#include <memory>

#include "TSystem.h"
#include "TAxis.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TMatrixD.h"
#include "TPrincipal.h"
#include "TMath.h"

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

#include "TMVA/tmvaglob.h"

const Int_t  MinNoTrainingEvents = 10;

#define READXML          kTRUE


TMVA::ClassificationResult::ClassificationResult()
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
    TGraph *roc=fROCCurve->GetROCCurve();
    roc->Draw("AL");
    roc->GetXaxis()->SetTitle(" Signal Efficiency ");
    roc->GetYaxis()->SetTitle(" Background Rejection ");
    c->BuildLegend(0.15,0.15,0.3,0.3);
    c->SetTitle("ROC-Integral Curve");
    TMVA::TMVAGlob::plot_logo();
    c->Draw();
    return c;
}

TMVA::Classification::Classification(DataLoader *dataloader,TString options,TFile *file):
Algorithm("Classification",dataloader,file,options),
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
//     if(IsSilentFile())TH1::AddDirectory(kFALSE);
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
    fResults.fMethod=fMethod;
    
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

void TMVA::Classification::Test()
{
    UInt_t fROC=1;
    
    if (fMethodBase==nullptr) {
        Log() << kINFO << "...nothing found to test" << Endl;
        return;
    }
    Event::SetIsTraining(kFALSE);
    fMethodBase->SetFile(fFile.get());
    fMethodBase->SetSilentFile(IsSilentFile());
    
    
    Log() << kINFO << "Test method: " << fMethodBase->GetMethodName() << " for Classification performance" << Endl;
    fMethodBase->AddOutput( Types::kTesting, Types::kClassification );
    Int_t isel;                  // will be 0 for a Method; 1 for a Variable
    Int_t nmeth_used[2] = {0,0}; // 0 Method; 1 Variable
    
    std::vector<std::vector<TString> >  mname(2);
    std::vector<std::vector<Double_t> > sig(2), sep(2), roc(2);
    std::vector<std::vector<Double_t> > eff01(2), eff10(2), eff30(2), effArea(2);
    std::vector<std::vector<Double_t> > eff01err(2), eff10err(2), eff30err(2);
    std::vector<std::vector<Double_t> > trainEff01(2), trainEff10(2), trainEff30(2);
    
    std::vector<std::vector<Float_t> > multiclass_testEff;
    std::vector<std::vector<Float_t> > multiclass_trainEff;
    std::vector<std::vector<Float_t> > multiclass_testPur;
    std::vector<std::vector<Float_t> > multiclass_trainPur;
    
    // same as above but for 'truncated' quantities (computed for events within 2sigma of RMS)
    std::vector<std::vector<Double_t> > biastrainT(1);
    std::vector<std::vector<Double_t> > biastestT(1);
    std::vector<std::vector<Double_t> > devtrainT(1);
    std::vector<std::vector<Double_t> > devtestT(1);
    std::vector<std::vector<Double_t> > rmstrainT(1);
    std::vector<std::vector<Double_t> > rmstestT(1);
    std::vector<std::vector<Double_t> > minftrainT(1);
    std::vector<std::vector<Double_t> > minftestT(1);

    Log() << kINFO << "Evaluate classifier: " << fMethodBase->GetMethodName() << Endl;
    isel = (fMethodBase->GetMethodTypeName().Contains("Variable")) ? 1 : 0;
    
    // perform the evaluation
    fMethodBase->TestClassification();
    
    
    // evaluate the classifier
    mname[isel].push_back( fMethodBase->GetMethodName() );
    sig[isel].push_back  ( fMethodBase->GetSignificance() );
    sep[isel].push_back  ( fMethodBase->GetSeparation() );
    roc[isel].push_back  ( fMethodBase->GetROCIntegral() );
    
    Double_t err;
    eff01[isel].push_back( fMethodBase->GetEfficiency("Efficiency:0.01", Types::kTesting, err) );
    eff01err[isel].push_back( err );
    eff10[isel].push_back( fMethodBase->GetEfficiency("Efficiency:0.10", Types::kTesting, err) );
    eff10err[isel].push_back( err );
    eff30[isel].push_back( fMethodBase->GetEfficiency("Efficiency:0.30", Types::kTesting, err) );
    eff30err[isel].push_back( err );
    effArea[isel].push_back( fMethodBase->GetEfficiency("",              Types::kTesting, err)  ); // computes the area (average)
    
    trainEff01[isel].push_back( fMethodBase->GetTrainingEfficiency("Efficiency:0.01") ); // the first pass takes longer
    trainEff10[isel].push_back( fMethodBase->GetTrainingEfficiency("Efficiency:0.10") );
    trainEff30[isel].push_back( fMethodBase->GetTrainingEfficiency("Efficiency:0.30") );
    
    nmeth_used[isel]++;
    
    if(!IsSilentFile()) 
    {
        Log() << kINFO << "Write evaluation histograms to file" << Endl;
        fMethodBase->WriteEvaluationHistosToFile(Types::kTesting);
        fMethodBase->WriteEvaluationHistosToFile(Types::kTraining);
    }
    // now sort the variables according to the best 'eff at Beff=0.10'
    for (Int_t k=0; k<2; k++) {
        std::vector< std::vector<Double_t> > vtemp;
        vtemp.push_back( effArea[k] );  // this is the vector that is ranked
        vtemp.push_back( eff10[k] );
        vtemp.push_back( eff01[k] );
        vtemp.push_back( eff30[k] );
        vtemp.push_back( eff10err[k] ); 
        vtemp.push_back( eff01err[k] );
        vtemp.push_back( eff30err[k] );
        vtemp.push_back( trainEff10[k] );
        vtemp.push_back( trainEff01[k] );
        vtemp.push_back( trainEff30[k] );
        vtemp.push_back( sig[k] );
        vtemp.push_back( sep[k] );
        vtemp.push_back( roc[k] );
        std::vector<TString> vtemps = mname[k];
        gTools().UsefulSortDescending( vtemp, &vtemps );
        effArea[k]    = vtemp[0];
        eff10[k]      = vtemp[1];
        eff01[k]      = vtemp[2];
        eff30[k]      = vtemp[3];
        eff10err[k]   = vtemp[4];
        eff01err[k]   = vtemp[5];
        eff30err[k]   = vtemp[6];
        trainEff10[k] = vtemp[7];
        trainEff01[k] = vtemp[8];
        trainEff30[k] = vtemp[9];
        sig[k]        = vtemp[10];
        sep[k]        = vtemp[11];
        roc[k]        = vtemp[12];
        mname[k]      = vtemps;
    }
        
    if(fROC)
    {
        Log().EnableOutput();
        gConfig().SetSilent(kFALSE);
        Log() << Endl;
        TString hLine = "-------------------------------------------------------------------------------------------------------------------";
        Log() << kINFO << "Evaluation results ranked by best signal efficiency and purity (area)" << Endl;
        Log() << kINFO << hLine << Endl;
        Log() << kINFO << "DataSet              MVA              Signal efficiency at bkg eff.(error):                | Sepa-    Signifi- "   << Endl;
        Log() << kINFO << "Name:                Method:          @B=0.01    @B=0.10    @B=0.30    ROC-integ    ROCCurve| ration:  cance:   "   << Endl;
        Log() << kINFO << hLine << Endl;
        Int_t k=0;
        if (k == 1 && nmeth_used[k] > 0) {
            Log() << kINFO << hLine << Endl;
            Log() << kINFO << "Input Variables: " << Endl << hLine << Endl;
        }
        
        if (k == 1) mname[k][0].ReplaceAll( "Variable_", "" );
        
        TMVA::ResultsClassification *results=dynamic_cast<ResultsClassification *>(fMethodBase->Data()->GetResults(mname[k][0],Types::kTesting,Types::kClassification));
        
        
        std::vector<Float_t> *mvaRes = results->GetValueVector();
        std::vector<Bool_t>  *mvaResType = results->GetValueVectorTypes();
        
        if (mvaResType->size() != 0) { 
            fResults.fROCCurve =std::unique_ptr<ROCCurve>( new TMVA::ROCCurve(*mvaRes, *mvaResType));
            fResults.fROCIntegral = fResults.fROCCurve->GetROCIntegral();
            fResults.fROCCurve->GetROCCurve()->SetTitle(Form("%s : %.3f",fMethod.GetValue<TString>("MethodTitle").Data(),fResults.fROCIntegral));
            fResults.fROCCurve->GetROCCurve()->SetName(fMethod.GetValue<TString>("MethodName"));
            
        }
        
        fResults.fClassifierResults=std::shared_ptr<TMVA::ResultsClassification>(results);
        if (sep[k][0] < 0 || sig[k][0] < 0) {
            // cannot compute separation/significance -> no MVA (usually for Cuts)
            
            Log() << kINFO << Form("%-20s %-15s: %#1.3f(%02i)  %#1.3f(%02i)  %#1.3f(%02i)    %#1.3f       %#1.3f | --       --",
                                   fDataLoader->GetName(), 
                                   (const char*)mname[k][0], 
                                   eff01[k][0], Int_t(1000*eff01err[k][0]), 
                                   eff10[k][0], Int_t(1000*eff10err[k][0]), 
                                   eff30[k][0], Int_t(1000*eff30err[k][0]), 
                                   effArea[k][0],fResults.fROCIntegral) << Endl;
        }
        else {
            Log() << kINFO << Form("%-20s %-15s: %#1.3f(%02i)  %#1.3f(%02i)  %#1.3f(%02i)    %#1.3f       %#1.3f | %#1.3f    %#1.3f",
                                   fDataLoader->GetName(), 
                                   (const char*)mname[k][0], 
                                   eff01[k][0], Int_t(1000*eff01err[k][0]), 
                                   eff10[k][0], Int_t(1000*eff10err[k][0]), 
                                   eff30[k][0], Int_t(1000*eff30err[k][0]), 
                                   effArea[k][0],fResults.fROCIntegral, 
                                   sep[k][0], sig[k][0]) << Endl;
        }
        
        Log() << kINFO << hLine << Endl;
        Log() << kINFO << Endl;
        Log() << kINFO << "Testing efficiency compared to training efficiency (overtraining check)" << Endl;
        Log() << kINFO << hLine << Endl;
        Log() << kINFO << "DataSet              MVA              Signal efficiency: from test sample (from training sample) "   << Endl;
        Log() << kINFO << "Name:                Method:          @B=0.01             @B=0.10            @B=0.30   "   << Endl;
        Log() << kINFO << hLine << Endl;
        if (k == 1 && nmeth_used[k] > 0) {
            Log() << kINFO << hLine << Endl;
            Log() << kINFO << "Input Variables: " << Endl << hLine << Endl;
        }
        if (k == 1) mname[k][0].ReplaceAll( "Variable_", "" );
        
        Log() << kINFO << Form("%-20s %-15s: %#1.3f (%#1.3f)       %#1.3f (%#1.3f)      %#1.3f (%#1.3f)",
                               fDataLoader->GetName(), 
                               (const char*)mname[k][0], 
                               eff01[k][0],trainEff01[k][0], 
                               eff10[k][0],trainEff10[k][0],
                               eff30[k][0],trainEff30[k][0]) << Endl;
                               Log() << kINFO << hLine << Endl;
                               Log() << kINFO << Endl; 
        if (gTools().CheckForSilentOption( GetOptions() )) Log().InhibitOutput();
    }//end fROC
    

    if(!IsSilentFile())
    {
        for (Int_t k=0; k<2; k++) {
            for (Int_t i=0; i<nmeth_used[k]; i++) {
                
                // write test/training trees
                fFile->cd(fMethodBase->fDataSetInfo.GetName());
                fMethodBase->fDataSetInfo.GetDataSet()->GetTree(Types::kTesting)->Write( "", TObject::kOverwrite );
                fMethodBase->fDataSetInfo.GetDataSet()->GetTree(Types::kTraining)->Write( "", TObject::kOverwrite );
            }
        }
    }
        
}


void TMVA::Classification::Evaluate()
{
Train();
Test();
}