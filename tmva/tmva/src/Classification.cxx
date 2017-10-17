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


#include <TMVA/Types.h>

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TLeaf.h>
#include <TEventList.h>
#include <TH2.h>
#include <TText.h>
#include <TLegend.h>
#include <TGraph.h>
#include <TStyle.h>
#include <TMatrixF.h>
#include <TMatrixDSym.h>
#include <TMultiGraph.h>
#include <TPaletteAxis.h>
#include <TPrincipal.h>
#include <TMath.h>
#include <TObjString.h>
#include <TSystem.h>
#include <TCanvas.h>

#include <iostream>
#include <memory>

#define MinNoTrainingEvents 10

//_______________________________________________________________________
TMVA::Experimental::ClassificationResult::ClassificationResult() : fROCCurve(nullptr)
{
}

//_______________________________________________________________________
TMVA::Experimental::ClassificationResult::ClassificationResult(const ClassificationResult &cr) : TObject(cr)
{
   fROCIntegral = cr.fROCIntegral;
   fMethod = cr.fMethod;
   fDataLoaderName = cr.fDataLoaderName;
   fMvaRes = cr.fMvaRes;
   fMvaResTypes = cr.fMvaResTypes;
   fMvaResWeights = cr.fMvaResWeights;
   fMulticlass = cr.fMulticlass;
}

//_______________________________________________________________________
TMVA::Experimental::ClassificationResult &TMVA::Experimental::ClassificationResult::
operator=(const TMVA::Experimental::ClassificationResult &cr)
{
   fROCIntegral = cr.fROCIntegral;
   fMethod = cr.fMethod;
   fDataLoaderName = cr.fDataLoaderName;
   fMvaRes = cr.fMvaRes;
   fMvaResTypes = cr.fMvaResTypes;
   fMvaResWeights = cr.fMvaResWeights;
   fMulticlass = cr.fMulticlass;
   return *this;
}

//_______________________________________________________________________
void TMVA::Experimental::ClassificationResult::Print(Option_t * /*option*/) const
{
   TMVA::MsgLogger::EnableOutput();
   TMVA::gConfig().SetSilent(kFALSE);
   TString hLine = "--------------------------------------------------- :";

   MsgLogger fLogger("Classification");

   fLogger << kINFO << hLine << Endl;
   fLogger << kINFO << "DataSet              MVA                            :" << Endl;
   fLogger << kINFO << "Name:                Method/Title:    ROC-integ     :" << Endl;
   fLogger << kINFO << hLine << Endl;
   fLogger << kINFO << Form("%-20s %-15s  %#1.3f         :", fDataLoaderName.Data(),
                            Form("%s/%s", fMethod.GetValue<TString>("MethodName").Data(),
                                 fMethod.GetValue<TString>("MethodTitle").Data()),
                            fROCIntegral)
           << Endl;
   fLogger << kINFO << hLine << Endl;

   TMVA::gConfig().SetSilent(kTRUE);
}

//_______________________________________________________________________
void TMVA::Experimental::ClassificationResult::Draw(Option_t *name)
{
   auto c = GetCanvas(name);
   c->Draw();
}

//_______________________________________________________________________
TCanvas *TMVA::Experimental::ClassificationResult::GetCanvas(Option_t *name)
{
   //    TMVA::TMVAGlob::Initialize();
   TCanvas *c = new TCanvas(name);
   TGraph *roc = fROCCurve->GetROCCurve();
   roc->Draw("AL");
   roc->GetXaxis()->SetTitle(" Signal Efficiency ");
   roc->GetYaxis()->SetTitle(" Background Rejection ");
   c->BuildLegend(0.15, 0.15, 0.3, 0.3);
   c->SetTitle("ROC-Integral Curve");
   //    TMVA::TMVAGlob::plot_logo();
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
   : TMVA::Envelope("Classification", dataloader, file, options), fMulticlass(kFALSE),
     fAnalysisType(Types::kClassification), fCorrelations(kFALSE), fROC(kTRUE)
{
   DeclareOptionRef(fMulticlass, "Multiclass", "Set Multiclass=True to perform multi-class classification");
   DeclareOptionRef(fCorrelations, "Correlations", "boolean to show correlation in output");
   DeclareOptionRef(fROC, "ROC", "boolean to show ROC in output");
   ParseOptions();
   CheckForUnusedOptions();

   if (fModelPersistence)
      gSystem->MakeDirectory(fDataLoader->GetName()); // creating directory for DataLoader output
}

//_______________________________________________________________________
TMVA::Experimental::Classification::Classification(DataLoader *dataloader, TString options)
   : TMVA::Envelope("Classification", dataloader, NULL, options), fMulticlass(kFALSE),
     fAnalysisType(Types::kClassification), fCorrelations(kFALSE), fROC(kTRUE)
{

   // init configurable
   SetConfigDescription("Configuration options for Classification running");
   SetConfigName(GetName());

   DeclareOptionRef(fMulticlass, "Multiclass", "Set Multiclass=True to perform multi-class classification");
   DeclareOptionRef(fCorrelations, "Correlations", "boolean to show correlation in output");
   DeclareOptionRef(fROC, "ROC", "boolean to show ROC in output");
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
   fTimer.Reset();
   fTimer.Start();

   Bool_t roc = fROC;
   if (!fMulticlass)
      fROC = kFALSE;
   if (fJobs <= 1) {
      Train();
      Test();
   } else {
      for (auto &meth : fMethods) {
         GetMethod(meth.GetValue<TString>("MethodName"), meth.GetValue<TString>("MethodTitle"));
      }
      fWorkers.SetNWorkers(fJobs);
      auto executor = [=](UInt_t workerID) -> ClassificationResult {
         TMVA::MsgLogger::InhibitOutput();
         TMVA::gConfig().SetSilent(kTRUE);
         TMVA::gConfig().SetUseColor(kFALSE);
         TMVA::gConfig().SetDrawProgressBar(kFALSE);
         auto methodname = fMethods[workerID].GetValue<TString>("MethodName");
         auto methodtitle = fMethods[workerID].GetValue<TString>("MethodTitle");
         TrainMethod(methodname, methodtitle);
         TestMethod(methodname, methodtitle);
         auto result = GetResults(methodname, methodtitle);
         return result;
      };

      fResults = fWorkers.Map(executor, ROOT::TSeqI(fMethods.size()));
   }
   if (!fMulticlass) {
      fROC = roc;
      TMVA::gConfig().SetSilent(kFALSE);

      TString hLine = "--------------------------------------------------- :";
      Log() << kINFO << hLine << Endl;
      Log() << kINFO << "DataSet              MVA                            :" << Endl;
      Log() << kINFO << "Name:                Method/Title:    ROC-integ     :" << Endl;
      Log() << kINFO << hLine << Endl;
      for (auto &r : fResults) {

         Log() << kINFO << Form("%-20s %-15s  %#1.3f         :", r.GetDataLoaderName().Data(),
                                Form("%s/%s", r.GetMethodName().Data(), r.GetMethodTitle().Data()), r.GetROCIntegral())
               << Endl;
      }
      Log() << kINFO << hLine << Endl;
   }
   Log() << kINFO << "-----------------------------------------------------" << Endl;
   Log() << kHEADER << "Evaluation done." << Endl << Endl;
   Log() << kINFO << Form("Jobs = %d Real Time = %lf ", fJobs, fTimer.RealTime()) << Endl;
   Log() << kINFO << "-----------------------------------------------------" << Endl;
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
   for (auto &meth : fMethods) {
      TestMethod(meth.GetValue<TString>("MethodName"), meth.GetValue<TString>("MethodTitle"));
   }
}

//_______________________________________________________________________
void TMVA::Experimental::Classification::TestMethod(TString methodname, TString methodtitle)
{
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

   // -----------------------------------------------------------------------
   // First part of evaluation process
   // --> compute efficiencies, and other separation estimators
   // -----------------------------------------------------------------------

   // although equal, we now want to separate the output for the variables
   // and the real methods
   Int_t isel;                   // will be 0 for a Method; 1 for a Variable
   Int_t nmeth_used[2] = {0, 0}; // 0 Method; 1 Variable

   std::vector<std::vector<TString>> mname(2);
   std::vector<std::vector<Double_t>> sig(2), sep(2), roc(2);
   std::vector<std::vector<Double_t>> eff01(2), eff10(2), eff30(2), effArea(2);
   std::vector<std::vector<Double_t>> eff01err(2), eff10err(2), eff30err(2);
   std::vector<std::vector<Double_t>> trainEff01(2), trainEff10(2), trainEff30(2);

   std::vector<std::vector<Float_t>> multiclass_testEff;
   std::vector<std::vector<Float_t>> multiclass_trainEff;
   std::vector<std::vector<Float_t>> multiclass_testPur;
   std::vector<std::vector<Float_t>> multiclass_trainPur;

   // Multiclass confusion matrices.
   std::vector<TMatrixD> multiclass_trainConfusionEffB01;
   std::vector<TMatrixD> multiclass_trainConfusionEffB10;
   std::vector<TMatrixD> multiclass_trainConfusionEffB30;
   std::vector<TMatrixD> multiclass_testConfusionEffB01;
   std::vector<TMatrixD> multiclass_testConfusionEffB10;
   std::vector<TMatrixD> multiclass_testConfusionEffB30;

   method->SetFile(fFile.get());
   method->SetSilentFile(IsSilentFile());

   MethodBase *methodNoCuts = NULL;
   if (!IsCutsMethod(method))
      methodNoCuts = method;

   if (method->DoMulticlass()) {
      // ====================================================================
      // === Multiclass evaluation
      // ====================================================================
      //         doMulticlass = kTRUE;
      Log() << kINFO << "Evaluate multiclass classification method: " << method->GetMethodName() << Endl;

      // Confusion matrix at three background efficiency levels
      multiclass_trainConfusionEffB01.push_back(method->GetMulticlassConfusionMatrix(0.01, Types::kTraining));
      multiclass_trainConfusionEffB10.push_back(method->GetMulticlassConfusionMatrix(0.10, Types::kTraining));
      multiclass_trainConfusionEffB30.push_back(method->GetMulticlassConfusionMatrix(0.30, Types::kTraining));

      multiclass_testConfusionEffB01.push_back(method->GetMulticlassConfusionMatrix(0.01, Types::kTesting));
      multiclass_testConfusionEffB10.push_back(method->GetMulticlassConfusionMatrix(0.10, Types::kTesting));
      multiclass_testConfusionEffB30.push_back(method->GetMulticlassConfusionMatrix(0.30, Types::kTesting));

      if (not IsSilentFile()) {
         Log() << kDEBUG << "\tWrite evaluation histograms to file" << Endl;
         method->WriteEvaluationHistosToFile(Types::kTesting);
         method->WriteEvaluationHistosToFile(Types::kTraining);
      }

      nmeth_used[0]++;
      mname[0].push_back(method->GetMethodName());
   } else {

      Log() << kHEADER << "Evaluate classifier: " << method->GetMethodName() << Endl << Endl;
      isel = (method->GetMethodTypeName().Contains("Variable")) ? 1 : 0;

      // perform the evaluation
      method->TestClassification();

      // evaluate the classifier
      mname[isel].push_back(method->GetMethodName());
      sig[isel].push_back(method->GetSignificance());
      sep[isel].push_back(method->GetSeparation());
      roc[isel].push_back(method->GetROCIntegral());

      Double_t err;
      eff01[isel].push_back(method->GetEfficiency("Efficiency:0.01", Types::kTesting, err));
      eff01err[isel].push_back(err);
      eff10[isel].push_back(method->GetEfficiency("Efficiency:0.10", Types::kTesting, err));
      eff10err[isel].push_back(err);
      eff30[isel].push_back(method->GetEfficiency("Efficiency:0.30", Types::kTesting, err));
      eff30err[isel].push_back(err);
      effArea[isel].push_back(method->GetEfficiency("", Types::kTesting, err)); // computes the area (average)

      trainEff01[isel].push_back(method->GetTrainingEfficiency("Efficiency:0.01")); // the first pass takes longer
      trainEff10[isel].push_back(method->GetTrainingEfficiency("Efficiency:0.10"));
      trainEff30[isel].push_back(method->GetTrainingEfficiency("Efficiency:0.30"));

      nmeth_used[isel]++;

      if (!IsSilentFile()) {
         Log() << kDEBUG << "\tWrite evaluation histograms to file" << Endl;
         method->WriteEvaluationHistosToFile(Types::kTesting);
         method->WriteEvaluationHistosToFile(Types::kTraining);
      }
   }

   if (!fMulticlass) {
      // now sort the variables according to the best 'eff at Beff=0.10'
      for (Int_t k = 0; k < 2; k++) {
         std::vector<std::vector<Double_t>> vtemp;
         vtemp.push_back(effArea[k]); // this is the vector that is ranked
         vtemp.push_back(eff10[k]);
         vtemp.push_back(eff01[k]);
         vtemp.push_back(eff30[k]);
         vtemp.push_back(eff10err[k]);
         vtemp.push_back(eff01err[k]);
         vtemp.push_back(eff30err[k]);
         vtemp.push_back(trainEff10[k]);
         vtemp.push_back(trainEff01[k]);
         vtemp.push_back(trainEff30[k]);
         vtemp.push_back(sig[k]);
         vtemp.push_back(sep[k]);
         vtemp.push_back(roc[k]);
         std::vector<TString> vtemps = mname[k];
         gTools().UsefulSortDescending(vtemp, &vtemps);
         effArea[k] = vtemp[0];
         eff10[k] = vtemp[1];
         eff01[k] = vtemp[2];
         eff30[k] = vtemp[3];
         eff10err[k] = vtemp[4];
         eff01err[k] = vtemp[5];
         eff30err[k] = vtemp[6];
         trainEff10[k] = vtemp[7];
         trainEff01[k] = vtemp[8];
         trainEff30[k] = vtemp[9];
         sig[k] = vtemp[10];
         sep[k] = vtemp[11];
         roc[k] = vtemp[12];
         mname[k] = vtemps;
      }
   }

   // -----------------------------------------------------------------------
   // Second part of evaluation process
   // --> compute correlations among MVAs
   // --> compute correlations between input variables and MVA (determines importance)
   // --> count overlaps
   // -----------------------------------------------------------------------
   if (fCorrelations) {
      const Int_t nmeth = methodNoCuts == NULL ? 0 : 1;
      const Int_t nvar = method->fDataSetInfo.GetNVariables();
      if (!fMulticlass) {

         if (nmeth > 0) {

            //              needed for correlations
            Double_t *dvec = new Double_t[nmeth + nvar];
            std::vector<Double_t> rvec;

            //              for correlations
            TPrincipal *tpSig = new TPrincipal(nmeth + nvar, "");
            TPrincipal *tpBkg = new TPrincipal(nmeth + nvar, "");

            //              set required tree branch references
            std::vector<TString> *theVars = new std::vector<TString>;
            std::vector<ResultsClassification *> mvaRes;
            theVars->push_back(methodNoCuts->GetTestvarName());
            rvec.push_back(methodNoCuts->GetSignalReferenceCut());
            theVars->back().ReplaceAll("MVA_", "");
            mvaRes.push_back(dynamic_cast<ResultsClassification *>(methodNoCuts->Data()->GetResults(
               methodNoCuts->GetMethodName(), Types::kTesting, Types::kMaxAnalysisType)));

            //              for overlap study
            TMatrixD *overlapS = new TMatrixD(nmeth, nmeth);
            TMatrixD *overlapB = new TMatrixD(nmeth, nmeth);
            (*overlapS) *= 0; // init...
            (*overlapB) *= 0; // init...

            //              loop over test tree
            DataSet *defDs = method->fDataSetInfo.GetDataSet();
            defDs->SetCurrentType(Types::kTesting);
            for (Int_t ievt = 0; ievt < defDs->GetNEvents(); ievt++) {
               const Event *ev = defDs->GetEvent(ievt);

               //                 for correlations
               TMatrixD *theMat = 0;
               for (Int_t im = 0; im < nmeth; im++) {
                  //                    check for NaN value
                  Double_t retval = (Double_t)(*mvaRes[im])[ievt][0];
                  if (TMath::IsNaN(retval)) {
                     Log() << kWARNING << "Found NaN return value in event: " << ievt << " for method \""
                           << methodNoCuts->GetName() << "\"" << Endl;
                     dvec[im] = 0;
                  } else
                     dvec[im] = retval;
               }
               for (Int_t iv = 0; iv < nvar; iv++)
                  dvec[iv + nmeth] = (Double_t)ev->GetValue(iv);
               if (method->fDataSetInfo.IsSignal(ev)) {
                  tpSig->AddRow(dvec);
                  theMat = overlapS;
               } else {
                  tpBkg->AddRow(dvec);
                  theMat = overlapB;
               }

               //                 count overlaps
               for (Int_t im = 0; im < nmeth; im++) {
                  for (Int_t jm = im; jm < nmeth; jm++) {
                     if ((dvec[im] - rvec[im]) * (dvec[jm] - rvec[jm]) > 0) {
                        (*theMat)(im, jm)++;
                        if (im != jm)
                           (*theMat)(jm, im)++;
                     }
                  }
               }
            }

            //              renormalise overlap matrix
            (*overlapS) *= (1.0 / defDs->GetNEvtSigTest());  // init...
            (*overlapB) *= (1.0 / defDs->GetNEvtBkgdTest()); // init...

            tpSig->MakePrincipals();
            tpBkg->MakePrincipals();

            const TMatrixD *covMatS = tpSig->GetCovarianceMatrix();
            const TMatrixD *covMatB = tpBkg->GetCovarianceMatrix();

            const TMatrixD *corrMatS = gTools().GetCorrelationMatrix(covMatS);
            const TMatrixD *corrMatB = gTools().GetCorrelationMatrix(covMatB);

            //              print correlation matrices
            if (corrMatS != 0 && corrMatB != 0) {

               //                 extract MVA matrix
               TMatrixD mvaMatS(nmeth, nmeth);
               TMatrixD mvaMatB(nmeth, nmeth);
               for (Int_t im = 0; im < nmeth; im++) {
                  for (Int_t jm = 0; jm < nmeth; jm++) {
                     mvaMatS(im, jm) = (*corrMatS)(im, jm);
                     mvaMatB(im, jm) = (*corrMatB)(im, jm);
                  }
               }

               //                 extract variables - to MVA matrix
               std::vector<TString> theInputVars;
               TMatrixD varmvaMatS(nvar, nmeth);
               TMatrixD varmvaMatB(nvar, nmeth);
               for (Int_t iv = 0; iv < nvar; iv++) {
                  theInputVars.push_back(method->fDataSetInfo.GetVariableInfo(iv).GetLabel());
                  for (Int_t jm = 0; jm < nmeth; jm++) {
                     varmvaMatS(iv, jm) = (*corrMatS)(nmeth + iv, jm);
                     varmvaMatB(iv, jm) = (*corrMatB)(nmeth + iv, jm);
                  }
               }

               if (nmeth > 1) {
                  Log() << kINFO << Endl;
                  Log() << kINFO << Form("Dataset[%s] : ", method->fDataSetInfo.GetName())
                        << "Inter-MVA correlation matrix (signal):" << Endl;
                  gTools().FormattedOutput(mvaMatS, *theVars, Log());
                  Log() << kINFO << Endl;

                  Log() << kINFO << Form("Dataset[%s] : ", method->fDataSetInfo.GetName())
                        << "Inter-MVA correlation matrix (background):" << Endl;
                  gTools().FormattedOutput(mvaMatB, *theVars, Log());
                  Log() << kINFO << Endl;
               }

               Log() << kINFO << Form("Dataset[%s] : ", method->fDataSetInfo.GetName())
                     << "Correlations between input variables and MVA response (signal):" << Endl;
               gTools().FormattedOutput(varmvaMatS, theInputVars, *theVars, Log());
               Log() << kINFO << Endl;

               Log() << kINFO << Form("Dataset[%s] : ", method->fDataSetInfo.GetName())
                     << "Correlations between input variables and MVA response (background):" << Endl;
               gTools().FormattedOutput(varmvaMatB, theInputVars, *theVars, Log());
               Log() << kINFO << Endl;
            } else
               Log() << kWARNING << Form("Dataset[%s] : ", method->fDataSetInfo.GetName())
                     << "<TestAllMethods> cannot compute correlation matrices" << Endl;

            //              print overlap matrices
            Log() << kINFO << Form("Dataset[%s] : ", method->fDataSetInfo.GetName())
                  << "The following \"overlap\" matrices contain the fraction of events for which " << Endl;
            Log() << kINFO << Form("Dataset[%s] : ", method->fDataSetInfo.GetName())
                  << "the MVAs 'i' and 'j' have returned conform answers about \"signal-likeness\"" << Endl;
            Log() << kINFO << Form("Dataset[%s] : ", method->fDataSetInfo.GetName())
                  << "An event is signal-like, if its MVA output exceeds the following value:" << Endl;
            gTools().FormattedOutput(rvec, *theVars, "Method", "Cut value", Log());
            Log() << kINFO << Form("Dataset[%s] : ", method->fDataSetInfo.GetName())
                  << "which correspond to the working point: eff(signal) = 1 - eff(background)" << Endl;

            //              give notice that cut method has been excluded from this test
            if (nmeth != 1)
               Log() << kINFO << Form("Dataset[%s] : ", method->fDataSetInfo.GetName())
                     << "Note: no correlations and overlap with cut method are provided at present" << Endl;

            if (nmeth > 1) {
               Log() << kINFO << Endl;
               Log() << kINFO << Form("Dataset[%s] : ", method->fDataSetInfo.GetName())
                     << "Inter-MVA overlap matrix (signal):" << Endl;
               gTools().FormattedOutput(*overlapS, *theVars, Log());
               Log() << kINFO << Endl;

               Log() << kINFO << Form("Dataset[%s] : ", method->fDataSetInfo.GetName())
                     << "Inter-MVA overlap matrix (background):" << Endl;
               gTools().FormattedOutput(*overlapB, *theVars, Log());
            }

            //              cleanup
            delete tpSig;
            delete tpBkg;
            delete corrMatS;
            delete corrMatB;
            delete theVars;
            delete overlapS;
            delete overlapB;
            delete[] dvec;
         }
      }
   }

   // -----------------------------------------------------------------------
   // Third part of evaluation process
   // --> output
   // -----------------------------------------------------------------------

   if (fMulticlass) {
      // ====================================================================
      // === Multiclass Output
      // ====================================================================

      TString hLine =
         "-------------------------------------------------------------------------------------------------------";

      // --- 1 vs Rest ROC AUC, signal efficiency @ given background efficiency
      // --------------------------------------------------------------------
      TString header1 = Form("%-15s%-15s%-15s%-15s%-15s%-15s", "Dataset", "MVA Method", "ROC AUC", "Sig eff@B=0.01",
                             "Sig eff@B=0.10", "Sig eff@B=0.30");
      TString header2 = Form("%-15s%-15s%-15s%-15s%-15s%-15s", "Name:", "/ Class:", "test  (train)", "test  (train)",
                             "test  (train)", "test  (train)");
      Log() << kINFO << Endl;
      Log() << kINFO << "1-vs-rest performance metrics per class" << Endl;
      Log() << kINFO << hLine << Endl;
      Log() << kINFO << Endl;
      Log() << kINFO << "Considers the listed class as signal and the other classes" << Endl;
      Log() << kINFO << "as background, reporting the resulting binary performance." << Endl;
      Log() << kINFO << "A score of 0.820 (0.850) means 0.820 was acheived on the" << Endl;
      Log() << kINFO << "test set and 0.850 on the training set." << Endl;

      Log() << kINFO << Endl;
      Log() << kINFO << header1 << Endl;
      Log() << kINFO << header2 << Endl;
      for (Int_t k = 0; k < 2; k++) {
         for (Int_t i = 0; i < nmeth_used[k]; i++) {
            if (k == 1) {
               mname[k][i].ReplaceAll("Variable_", "");
            }

            const TString datasetName = fDataLoader->GetName();

            Log() << kINFO << Endl;
            TString row = Form("%-15s%-15s", fDataLoader->GetName(), methodname.Data());
            Log() << kINFO << row << Endl;
            Log() << kINFO << "------------------------------" << Endl;

            UInt_t numClasses = method->fDataSetInfo.GetNClasses();
            for (UInt_t iClass = 0; iClass < numClasses; ++iClass) {

               ROCCurve *rocCurveTrain = GetROC(methodname, methodtitle, iClass, Types::kTraining);
               ROCCurve *rocCurveTest = GetROC(methodname, methodtitle, iClass, Types::kTesting);

               const TString className = method->DataInfo().GetClassInfo(iClass)->GetName();
               const Double_t rocaucTrain = rocCurveTrain->GetROCIntegral();
               const Double_t effB01Train = rocCurveTrain->GetEffSForEffB(0.01);
               const Double_t effB10Train = rocCurveTrain->GetEffSForEffB(0.10);
               const Double_t effB30Train = rocCurveTrain->GetEffSForEffB(0.30);
               const Double_t rocaucTest = rocCurveTest->GetROCIntegral();
               const Double_t effB01Test = rocCurveTest->GetEffSForEffB(0.01);
               const Double_t effB10Test = rocCurveTest->GetEffSForEffB(0.10);
               const Double_t effB30Test = rocCurveTest->GetEffSForEffB(0.30);
               const TString rocaucCmp = Form("%5.3f (%5.3f)", rocaucTest, rocaucTrain);
               const TString effB01Cmp = Form("%5.3f (%5.3f)", effB01Test, effB01Train);
               const TString effB10Cmp = Form("%5.3f (%5.3f)", effB10Test, effB10Train);
               const TString effB30Cmp = Form("%5.3f (%5.3f)", effB30Test, effB30Train);
               row = Form("%-15s%-15s%-15s%-15s%-15s%-15s", "", className.Data(), rocaucCmp.Data(), effB01Cmp.Data(),
                          effB10Cmp.Data(), effB30Cmp.Data());
               Log() << kINFO << row << Endl;

               delete rocCurveTrain;
               delete rocCurveTest;
            }
         }
      }
      Log() << kINFO << Endl;
      Log() << kINFO << hLine << Endl;
      Log() << kINFO << Endl;

      // --- Confusion matrices
      // --------------------------------------------------------------------
      auto printMatrix = [](TMatrixD const &matTraining, TMatrixD const &matTesting, std::vector<TString> classnames,
                            UInt_t numClasses, MsgLogger &stream) {
         // assert (classLabledWidth >= valueLabelWidth + 2)
         // if (...) {Log() << kWARN << "..." << Endl; }

         // TODO: Ensure matrices are same size.

         TString header = Form(" %-14s", " ");
         TString headerInfo = Form(" %-14s", " ");
         ;
         for (UInt_t iCol = 0; iCol < numClasses; ++iCol) {
            header += Form(" %-14s", classnames[iCol].Data());
            headerInfo += Form(" %-14s", " test (train)");
         }
         stream << kINFO << header << Endl;
         stream << kINFO << headerInfo << Endl;

         for (UInt_t iRow = 0; iRow < numClasses; ++iRow) {
            stream << kINFO << Form(" %-14s", classnames[iRow].Data());

            for (UInt_t iCol = 0; iCol < numClasses; ++iCol) {
               if (iCol == iRow) {
                  stream << kINFO << Form(" %-14s", "-");
               } else {
                  Double_t trainValue = matTraining[iRow][iCol];
                  Double_t testValue = matTesting[iRow][iCol];
                  TString entry = Form("%-5.3f (%-5.3f)", testValue, trainValue);
                  stream << kINFO << Form(" %-14s", entry.Data());
               }
            }
            stream << kINFO << Endl;
         }
      };

      Log() << kINFO << Endl;
      Log() << kINFO << "Confusion matrices for all methods" << Endl;
      Log() << kINFO << hLine << Endl;
      Log() << kINFO << Endl;
      Log() << kINFO << "Does a binary comparison between the two classes given by a " << Endl;
      Log() << kINFO << "particular row-column combination. In each case, the class " << Endl;
      Log() << kINFO << "given by the row is considered signal while the class given " << Endl;
      Log() << kINFO << "by the column index is considered background." << Endl;
      Log() << kINFO << Endl;

      UInt_t numClasses = method->fDataSetInfo.GetNClasses();

      std::vector<TString> classnames;
      for (UInt_t iCls = 0; iCls < numClasses; ++iCls) {
         classnames.push_back(method->fDataSetInfo.GetClassInfo(iCls)->GetName());
      }
      Log() << kINFO << "=== Showing confusion matrix for method : " << Form("%-15s", (const char *)mname[0][0])
            << Endl;
      Log() << kINFO << "(Signal Efficiency for Background Efficiency 0.01%)" << Endl;
      Log() << kINFO << "---------------------------------------------------" << Endl;
      printMatrix(multiclass_testConfusionEffB01[0], multiclass_trainConfusionEffB01[0], classnames, numClasses, Log());
      Log() << kINFO << Endl;

      Log() << kINFO << "(Signal Efficiency for Background Efficiency 0.10%)" << Endl;
      Log() << kINFO << "---------------------------------------------------" << Endl;
      printMatrix(multiclass_testConfusionEffB10[0], multiclass_trainConfusionEffB10[0], classnames, numClasses, Log());
      Log() << kINFO << Endl;

      Log() << kINFO << "(Signal Efficiency for Background Efficiency 0.30%)" << Endl;
      Log() << kINFO << "---------------------------------------------------" << Endl;
      printMatrix(multiclass_testConfusionEffB30[0], multiclass_trainConfusionEffB30[0], classnames, numClasses, Log());
      Log() << kINFO << Endl;
      //    }
      Log() << kINFO << hLine << Endl;
      Log() << kINFO << Endl;
   } else {
      // Binary classification
      if (fROC) {
         Log().EnableOutput();
         gConfig().SetSilent(kFALSE);
         Log() << Endl;
         TString hLine = "------------------------------------------------------------------------------------------"
                         "-------------------------";
         Log() << kINFO << "Evaluation results ranked by best signal efficiency and purity (area)" << Endl;
         Log() << kINFO << hLine << Endl;
         Log() << kINFO << "DataSet       MVA                       " << Endl;
         Log() << kINFO << "Name:         Method:          ROC-integ" << Endl;

         Log() << kDEBUG << hLine << Endl;
         for (Int_t k = 0; k < 2; k++) {
            if (k == 1 && nmeth_used[k] > 0) {
               Log() << kINFO << hLine << Endl;
               Log() << kINFO << "Input Variables: " << Endl << hLine << Endl;
            }
            for (Int_t i = 0; i < nmeth_used[k]; i++) {
               TString datasetName = fDataLoader->GetName();
               TString methodName = mname[k][i];

               if (k == 1) {
                  methodName.ReplaceAll("Variable_", "");
               }

               TMVA::DataSet *dataset = method->Data();
               TMVA::Results *results = dataset->GetResults(methodName, Types::kTesting, this->fAnalysisType);
               std::vector<Bool_t> *mvaResType = dynamic_cast<ResultsClassification *>(results)->GetValueVectorTypes();

               Double_t rocIntegral = 0.0;
               if (mvaResType->size() != 0) {
                  rocIntegral = GetROCIntegral(methodname, methodtitle);
               }

               if (sep[k][i] < 0 || sig[k][i] < 0) {
                  // cannot compute separation/significance -> no MVA (usually for Cuts)
                  Log() << kINFO
                        << Form("%-13s %-15s: %#1.3f", fDataLoader->GetName(), methodName.Data(), effArea[k][i])
                        << Endl;
               } else {
                  Log() << kINFO << Form("%-13s %-15s: %#1.3f", datasetName.Data(), methodName.Data(), rocIntegral)
                        << Endl;
               }
            }
         }
         Log() << kINFO << hLine << Endl;
         Log() << kINFO << Endl;
         Log() << kINFO << "Testing efficiency compared to training efficiency (overtraining check)" << Endl;
         Log() << kINFO << hLine << Endl;
         Log() << kINFO
               << "DataSet              MVA              Signal efficiency: from test sample (from training sample) "
               << Endl;
         Log() << kINFO << "Name:                Method:          @B=0.01             @B=0.10            @B=0.30   "
               << Endl;
         Log() << kINFO << hLine << Endl;
         for (Int_t k = 0; k < 2; k++) {
            if (k == 1 && nmeth_used[k] > 0) {
               Log() << kINFO << hLine << Endl;
               Log() << kINFO << "Input Variables: " << Endl << hLine << Endl;
            }
            for (Int_t i = 0; i < nmeth_used[k]; i++) {
               if (k == 1)
                  mname[k][i].ReplaceAll("Variable_", "");

               Log() << kINFO << Form("%-20s %-15s: %#1.3f (%#1.3f)       %#1.3f (%#1.3f)      %#1.3f (%#1.3f)",
                                      method->fDataSetInfo.GetName(), (const char *)mname[k][i], eff01[k][i],
                                      trainEff01[k][i], eff10[k][i], trainEff10[k][i], eff30[k][i], trainEff30[k][i])
                     << Endl;
            }
         }
         Log() << kINFO << hLine << Endl;
         Log() << kINFO << Endl;

         if (gTools().CheckForSilentOption(GetOptions()))
            Log().InhibitOutput();
      } // end fROC
   }
   auto &results = GetResults(methodname, methodtitle);
   TMVA::DataSet *dataset = method->Data();
   dataset->SetCurrentType(Types::kTesting);
   TMVA::Results *result = dataset->GetResults(method->GetName(), Types::kTesting, this->fAnalysisType);

   if (this->fAnalysisType == Types::kClassification) {
      results.fMvaRes = *dynamic_cast<ResultsClassification *>(result)->GetValueVector();
      results.fMvaResTypes = *dynamic_cast<ResultsClassification *>(result)->GetValueVectorTypes();

      auto eventCollection = dataset->GetEventCollection(Types::kTesting);
      results.fMvaResWeights.reserve(eventCollection.size());
      for (auto ev : eventCollection) {
         results.fMvaResWeights.push_back(ev->GetWeight());
      }
   }

   results.fROCIntegral = GetROCIntegral(methodname, methodtitle);
   results.fROCCurve = std::shared_ptr<ROCCurve>(GetROC(methodname, methodtitle));
   results.fROCCurve->GetROCCurve()->SetTitle(Form("%s : ROC-Int %.3f", methodtitle.Data(), results.fROCIntegral));
   results.fROCCurve->GetROCCurve()->SetName(methodname);
   if (!IsSilentFile()) {
      // write test/training trees
      RootBaseDir()->cd(method->fDataSetInfo.GetName());
      method->fDataSetInfo.GetDataSet()->GetTree(Types::kTesting)->Write("", TObject::kOverwrite);
      method->fDataSetInfo.GetDataSet()->GetTree(Types::kTraining)->Write("", TObject::kOverwrite);
   }
}

//_______________________________________________________________________
void TMVA::Experimental::Classification::TestMethod(Types::EMVA method, TString methodtitle)
{
   TestMethod(Types::Instance().GetMethodName(method), methodtitle);
}

//_______________________________________________________________________
const std::vector<TMVA::Experimental::ClassificationResult> &TMVA::Experimental::Classification::GetResults() const
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
TMVA::Experimental::ClassificationResult &
TMVA::Experimental::Classification::GetResults(TString methodname, TString methodtitle)
{
   for (auto &result : fResults) {
      if (result.IsMethod(methodname, methodtitle))
         return result;
   }
   ClassificationResult result;
   result.fMethod["MethodName"] = methodname;
   result.fMethod["MethodTitle"] = methodtitle;
   result.fDataLoaderName = fDataLoader->GetName();
   fResults.push_back(result);
   return fResults.back();
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

//_______________________________________________________________________
TMVA::ROCCurve *TMVA::Experimental::Classification::GetROC(TString methodname, TString methodtitle, UInt_t iClass,
                                                           TMVA::Types::ETreeType type)
{
   return GetROC(GetMethod(methodname, methodtitle), iClass, type);
}

//_______________________________________________________________________
Double_t TMVA::Experimental::Classification::GetROCIntegral(TString methodname, TString methodtitle, UInt_t iClass)
{
   TMVA::ROCCurve *rocCurve = GetROC(methodname, methodtitle, iClass);
   if (!rocCurve) {
      Log() << kFATAL
            << Form("ROCCurve object was not created in MethodName = %s MethodTitle = %s not found with Dataset = %s ",
                    methodname.Data(), methodtitle.Data(), fDataLoader->GetName())
            << Endl;
      return 0;
   }

   Int_t npoints = TMVA::gConfig().fVariablePlotting.fNbinsXOfROCCurve + 1;
   Double_t rocIntegral = rocCurve->GetROCIntegral(npoints);
   delete rocCurve;

   return rocIntegral;
}
