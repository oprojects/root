// @(#)root/tmva $Id$
// Author: Omar Zapata, Andreas Hoecker, Peter Speckmayer, Joerg Stelzer, Helge Voss, Kai Voss, Eckhard von Toerne, Jan
// Therhaag

#include <TMVA/Classification.h>

#include <TMVA/ClassifierFactory.h>
#include <TMVA/Config.h>
#include <TMVA/Configurable.h>
#include <TMVA/Tools.h>
#include <TMVA/Ranking.h>
#include <TMVA/DataSet.h>
#include <TMVA/IMethod.h>
#include <TMVA/MethodBase.h>
#include <TMVA/DataInputHandler.h>
#include <TMVA/DataSetManager.h>
#include <TMVA/DataSetInfo.h>
#include <TMVA/DataLoader.h>
#include <TMVA/MethodBoost.h>
#include <TMVA/MethodCategory.h>
#include <TMVA/ROCCalc.h>
#include <TMVA/ROCCurve.h>
#include <TMVA/MsgLogger.h>

#include <TMVA/VariableInfo.h>
#include <TMVA/VariableTransform.h>

#include <TMVA/Results.h>
#include <TMVA/ResultsClassification.h>
#include <TMVA/ResultsRegression.h>
#include <TMVA/ResultsMulticlass.h>

#include <TMVA/Types.h>

#include <TSystem.h>
#include <TAxis.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TMath.h>
#include <TMatrixD.h>

#include <iostream>
#include <memory>

#define MinNoTrainingEvents 10

//_______________________________________________________________________
TMVA::Experimental::ClassificationResult::ClassificationResult()
{
}

//_______________________________________________________________________
void TMVA::Experimental::ClassificationResult::Print() const
{
   TMVA::MsgLogger::EnableOutput();
   TMVA::gConfig().SetSilent(kFALSE);
   TString hLine = "--------------------------------------------------- :";

   MsgLogger fLogger("Classification");

   fLogger << kINFO << hLine << Endl;
   fLogger << kINFO << "DataSet              MVA                            :" << Endl;
   fLogger << kINFO << "Name:                Method:          ROC-integ     :" << Endl;
   fLogger << kINFO << hLine << Endl;
   fLogger << kINFO << Form("%-20s %-15s  %#1.3f         :", fDataLoaderName.Data(),
                            fMethod.GetValue<TString>("MethodName").Data(), fROCIntegral)
           << Endl;
   fLogger << kINFO << hLine << Endl;

   TMVA::gConfig().SetSilent(kTRUE);
}

//_______________________________________________________________________
TCanvas *TMVA::Experimental::ClassificationResult::Draw(const TString name) const
{
   //     TMVA::TMVAGlob::Initialize();
   TCanvas *c = new TCanvas(name.Data());
   TGraph *roc = fROCCurve->GetROCCurve();
   roc->Draw("AL");
   roc->GetXaxis()->SetTitle(" Signal Efficiency ");
   roc->GetYaxis()->SetTitle(" Background Rejection ");
   c->BuildLegend(0.15, 0.15, 0.3, 0.3);
   c->SetTitle("ROC-Integral Curve");
   //     TMVA::TMVAGlob::plot_logo();
   c->Draw();
   return c;
}

Bool_t TMVA::Experimental::ClassificationResult::IsMethod(TString methodname, TString methodtitle)
{
   return fMethod.GetValue<TString>("MethodName") == methodname &&
                fMethod.GetValue<TString>("MethodTitle") == methodtitle
             ? kTRUE
             : kFALSE;
}

//_______________________________________________________________________
TMVA::Experimental::Classification::Classification(DataLoader *dataloader, TFile *file, TString options)
   : TMVA::Envelope("Classification", dataloader, file, options)
{
   DeclareOptionRef(fMulticlass, "Multiclass", "Set Multiclass=True to perform multi-class classification");
   ParseOptions();
   CheckForUnusedOptions();

   if (fModelPersistence)
      gSystem->MakeDirectory(fDataLoader->GetName()); // creating directory for DataLoader output
}

//_______________________________________________________________________
TMVA::Experimental::Classification::Classification(DataLoader *dataloader, TString options)
   : TMVA::Envelope("Classification", dataloader, nullptr, options), fMulticlass(kFALSE)
{

   // init configurable
   SetConfigDescription("Configuration options for Classification running");
   SetConfigName(GetName());

   DeclareOptionRef(fTransformations, "Transformations", "List of transformations to test; formatting example: "
                                                         "\"Transformations=I;D;P;U;G,D\", for identity, "
                                                         "decorrelation, PCA, Uniform and Gaussianisation followed by "
                                                         "decorrelation transformations");
   DeclareOptionRef(fMulticlass, "Multiclass", "Set Multiclass=True to perform multi-class classification");
   ParseOptions();
   CheckForUnusedOptions();
   if (fModelPersistence)
      gSystem->MakeDirectory(fDataLoader->GetName()); // creating directory for DataLoader output
   if (fMulticlass)
      fAnalysisType = TMVA::Types::kMulticlass;
   else
      fAnalysisType = TMVA::Types::kClassification;
}

//_______________________________________________________________________
TMVA::Experimental::Classification::~Classification()
{
}

//_______________________________________________________________________
TString TMVA::Experimental::Classification::GetMethodOptions(TString methodname, TString methodtitle)
{
   for (auto &meth : fMethods) {
      if (meth.GetValue<TString>("MethodName") == methodname && meth.GetValue<TString>("MethodTitle") == methodtitle)
         return meth.GetValue<TString>("MethodOptions");
   }
   return "";
}

//_______________________________________________________________________
void TMVA::Experimental::Classification::Evaluate()
{

   Train();
   TMVA::gConfig().SetSilent(kFALSE);
   Log() << kINFO << "Evaluation done." << Endl;
   TMVA::gConfig().SetSilent(kTRUE);
}

//_______________________________________________________________________
void TMVA::Experimental::Classification::Train()
{
   for (auto &meth : fMethods) {
      TrainMethod(meth.GetValue<TString>("MethodName"), meth.GetValue<TString>("MethodTitle"));
   }
}

//_______________________________________________________________________
void TMVA::Experimental::Classification::TrainMethod(TString methodname, TString methodtitle)
{

   auto method = GetMethod(methodname, methodtitle);
   if (!method) {
      Log() << kFATAL
            << Form("Trying to train method %s %s that maybe is not booked.", methodname.Data(), methodtitle.Data())
            << Endl;
   }
   Log() << kHEADER << gTools().Color("bold") << Form("Training method %s %s", methodname.Data(), methodtitle.Data())
         << gTools().Color("reset") << Endl;

   Event::SetIsTraining(kTRUE);
   if ((fAnalysisType == Types::kMulticlass || fAnalysisType == Types::kClassification) &&
       method->DataInfo().GetNClasses() < 2)
      Log() << kFATAL << "You want to do classification training, but specified less than two classes." << Endl;

   // first print some information about the default dataset
   //      if(!IsSilentFile()) WriteDataInformation(method->fDataSetInfo);

   if (method->Data()->GetNTrainingEvents() < MinNoTrainingEvents) {
      Log() << kWARNING << "Method " << method->GetMethodName() << " not trained (training tree has less entries ["
            << method->Data()->GetNTrainingEvents() << "] than required [" << MinNoTrainingEvents << "]" << Endl;
      return;
   }

   Log() << kHEADER << "Train method: " << method->GetMethodName() << " for "
         << (fAnalysisType == Types::kMulticlass ? "Multiclass classification" : "Classification") << Endl << Endl;
   method->TrainMethod();
   Log() << kHEADER << "Training finished" << Endl << Endl;
}

//_______________________________________________________________________
void TMVA::Experimental::Classification::TrainMethod(Types::EMVA method, TString methodtitle)
{
   TrainMethod(Types::Instance().GetMethodName(method), methodtitle);
}

//_______________________________________________________________________
TMVA::MethodBase *TMVA::Experimental::Classification::GetMethod(TString methodname, TString methodtitle)
{

   if (!HasMethod(methodname, methodtitle)) {
      std::cout << methodname << " " << methodtitle << std::endl;
      Log() << kERROR << "Trying to get method not booked." << Endl;
      return 0;
   }
   Int_t index = -1;
   if (HasMethodObject(methodname, methodtitle, index)) {
      return dynamic_cast<MethodBase *>(fIMethods[index]);
   }
   // if is not created then lets to create it.
   if (GetDataLoaderDataInput().GetEntries() <=
       1) { // 0 entries --> 0 events, 1 entry --> dynamical dataset (or one entry)
      Log() << kFATAL << "No input data for the training provided!" << Endl;
   }
   Log() << kHEADER << "Loading booked method: " << gTools().Color("bold") << methodname << " " << methodtitle
         << gTools().Color("reset") << Endl << Endl;

   TString moptions = GetMethodOptions(methodname, methodtitle);

   // interpret option string with respect to a request for boosting (i.e., BostNum > 0)
   Int_t boostNum = 0;
   auto conf = new TMVA::Configurable(moptions);
   conf->DeclareOptionRef(boostNum = 0, "Boost_num", "Number of times the classifier will be boosted");
   conf->ParseOptions();
   delete conf;

   TString fFileDir;
   if (fModelPersistence) {
      fFileDir = fDataLoader->GetName();
      fFileDir += "/" + gConfig().GetIONames().fWeightFileDir;
   }

   // initialize methods
   IMethod *im;
   TString fJobName = GetName();
   if (!boostNum) {
      im = ClassifierFactory::Instance().Create(std::string(methodname.Data()), fJobName, methodtitle,
                                                GetDataLoaderDataSetInfo(), moptions);
   } else {
      // boosted classifier, requires a specific definition, making it transparent for the user
      Log() << kDEBUG << "Boost Number is " << boostNum << " > 0: train boosted classifier" << Endl;
      im = ClassifierFactory::Instance().Create(std::string("Boost"), fJobName, methodtitle, GetDataLoaderDataSetInfo(),
                                                moptions);
      MethodBoost *methBoost = dynamic_cast<MethodBoost *>(im);
      if (!methBoost)
         Log() << kFATAL << "Method with type kBoost cannot be casted to MethodCategory. /Classification" << Endl;

      if (fModelPersistence)
         methBoost->SetWeightFileDir(fFileDir);
      methBoost->SetModelPersistence(fModelPersistence);
      methBoost->SetBoostedMethodName(methodname);
      methBoost->fDataSetManager = GetDataLoaderDataSetManager();
      methBoost->SetFile(fFile.get());
      methBoost->SetSilentFile(IsSilentFile());
   }

   MethodBase *method = dynamic_cast<MethodBase *>(im);
   if (method == 0)
      return 0; // could not create method

   // set fDataSetManager if MethodCategory (to enable Category to create datasetinfo objects)
   if (method->GetMethodType() == Types::kCategory) {
      MethodCategory *methCat = (dynamic_cast<MethodCategory *>(im));
      if (!methCat)
         Log() << kFATAL << "Method with type kCategory cannot be casted to MethodCategory. /Classification" << Endl;

      if (fModelPersistence)
         methCat->SetWeightFileDir(fFileDir);
      methCat->SetModelPersistence(fModelPersistence);
      methCat->fDataSetManager = GetDataLoaderDataSetManager();
      methCat->SetFile(fFile.get());
      methCat->SetSilentFile(IsSilentFile());
   }

   if (!method->HasAnalysisType(fAnalysisType, GetDataLoaderDataSetInfo().GetNClasses(),
                                GetDataLoaderDataSetInfo().GetNTargets())) {
      Log() << kWARNING << "Method " << method->GetMethodTypeName() << " is not capable of handling ";
      if (fAnalysisType == Types::kMulticlass) {
         Log() << "multiclass classification with " << GetDataLoaderDataSetInfo().GetNClasses() << " classes." << Endl;
      } else {
         Log() << "classification with " << GetDataLoaderDataSetInfo().GetNClasses() << " classes." << Endl;
      }
      return 0;
   }

   if (fModelPersistence)
      method->SetWeightFileDir(fFileDir);
   method->SetModelPersistence(fModelPersistence);
   method->SetAnalysisType(fAnalysisType);
   method->SetupMethod();
   method->ParseOptions();
   method->ProcessSetup();
   method->SetFile(fFile.get());
   method->SetSilentFile(IsSilentFile());

   // check-for-unused-options is performed; may be overridden by derived classes
   method->CheckSetup();
   fIMethods.push_back(method);
   return method;
}

//_______________________________________________________________________
Bool_t TMVA::Experimental::Classification::HasMethodObject(TString methodname, TString methodtitle, Int_t &index)
{
   if (fIMethods.empty())
      return kFALSE;
   for (UInt_t i = 0; i < fIMethods.size(); i++) {
      // they put method title like method name in MethodBase and type is type name
      auto methbase = dynamic_cast<MethodBase *>(fIMethods[i]);
      if (methbase->GetMethodTypeName() == methodname && methbase->GetMethodName() == methodtitle) {
         index = i;
         return kTRUE;
      }
   }
   return kFALSE;
}

//_______________________________________________________________________
void TMVA::Experimental::Classification::Test()
{
}

//_______________________________________________________________________
void TMVA::Experimental::Classification::TestMethod(TString methodname, TString methodtitle)
{
   auto resutls = GetResults(methodname, methodtitle);
   auto method = GetMethod(methodname, methodtitle);
   if (!method) {
      Log() << kFATAL
            << Form("Trying to train method %s %s that maybe is not booked.", methodname.Data(), methodtitle.Data())
            << Endl;
   }

   Log() << kHEADER << gTools().Color("bold") << "Test all methods" << gTools().Color("reset") << Endl;
   Event::SetIsTraining(kFALSE);

   Types::EAnalysisType analysisType = method->GetAnalysisType();
   Log() << kHEADER << "Test method: " << method->GetMethodName() << " for "
         << (analysisType == Types::kMulticlass ? "Multiclass classification" : "Classification") << " performance"
         << Endl << Endl;
   method->AddOutput(Types::kTesting, analysisType);
}

//_______________________________________________________________________
const std::vector<TMVA::Experimental::ClassificationResult *> &TMVA::Experimental::Classification::GetResults() const
{
   if (fResults.size() == 0)
      Log() << kFATAL << "No Classification results available" << Endl;
   return fResults;
}

//_______________________________________________________________________
Bool_t TMVA::Experimental::Classification::IsCutsMethod(TMVA::MethodBase *method)
{
   return method->GetMethodType() == Types::kCuts ? kTRUE : kFALSE;
}

//_______________________________________________________________________
TMVA::Experimental::ClassificationResult *
TMVA::Experimental::Classification::GetResults(TString methodname, TString methodtitle)
{
   for (auto &result : fResults) {
      if (result->IsMethod(methodname, methodtitle))
         return result;
   }
   auto result = new ClassificationResult();
   result->fMethod["MethodName"] = methodname;
   result->fMethod["MethodTitle"] = methodtitle;
   result->fDataLoaderName = fDataLoader->GetName();
   fResults.push_back(result);
   return result;
}

//_______________________________________________________________________
TMVA::ROCCurve *
TMVA::Experimental::Classification::GetROC(TMVA::MethodBase *method, UInt_t iClass, Types::ETreeType type)
{
   TMVA::DataSet *dataset = method->Data();
   dataset->SetCurrentType(type);
   TMVA::Results *results = dataset->GetResults(method->GetName(), type, this->fAnalysisType);

   UInt_t nClasses = method->DataInfo().GetNClasses();
   if (this->fAnalysisType == Types::kMulticlass && iClass >= nClasses) {
      Log() << kERROR << Form("Given class number (iClass = %i) does not exist. There are %i classes in dataset.",
                              iClass, nClasses)
            << Endl;
      return nullptr;
   }

   TMVA::ROCCurve *rocCurve = nullptr;
   if (this->fAnalysisType == Types::kClassification) {

      std::vector<Float_t> *mvaRes = dynamic_cast<ResultsClassification *>(results)->GetValueVector();
      std::vector<Bool_t> *mvaResTypes = dynamic_cast<ResultsClassification *>(results)->GetValueVectorTypes();
      std::vector<Float_t> mvaResWeights;

      auto eventCollection = dataset->GetEventCollection(type);
      mvaResWeights.reserve(eventCollection.size());
      for (auto ev : eventCollection) {
         mvaResWeights.push_back(ev->GetWeight());
      }

      rocCurve = new TMVA::ROCCurve(*mvaRes, *mvaResTypes, mvaResWeights);

   } else if (this->fAnalysisType == Types::kMulticlass) {
      std::vector<Float_t> mvaRes;
      std::vector<Bool_t> mvaResTypes;
      std::vector<Float_t> mvaResWeights;

      std::vector<std::vector<Float_t>> *rawMvaRes = dynamic_cast<ResultsMulticlass *>(results)->GetValueVector();

      // Vector transpose due to values being stored as
      //    [ [0, 1, 2], [0, 1, 2], ... ]
      // in ResultsMulticlass::GetValueVector.
      mvaRes.reserve(rawMvaRes->size());
      for (auto item : *rawMvaRes) {
         mvaRes.push_back(item[iClass]);
      }

      auto eventCollection = dataset->GetEventCollection(type);
      mvaResTypes.reserve(eventCollection.size());
      mvaResWeights.reserve(eventCollection.size());
      for (auto ev : eventCollection) {
         mvaResTypes.push_back(ev->GetClass() == iClass);
         mvaResWeights.push_back(ev->GetWeight());
      }

      rocCurve = new TMVA::ROCCurve(mvaRes, mvaResTypes, mvaResWeights);
   }

   return rocCurve;
}
