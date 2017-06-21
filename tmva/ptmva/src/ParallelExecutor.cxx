#include<TMVA/ParallelExecutor.h>
#include<TMVA/ResultsClassification.h>
#include<TMVA/MethodBase.h>
#include<TMVA/MsgLogger.h>

//______________________________________________________________________________
const TMVA::ParallelExecutorResults TMVA::ParallelExecutor::Execute(TMVA::Factory *factory, UInt_t jobs, TMVA::OptionMap options)
{
   fWorkers.SetNWorkers(jobs);

   std::vector<std::pair<TString, UInt_t> > methods; //the pair that have the dataset name and the method positon in the vector
   for (auto &_methods : factory->GetMethodsMap()) {
      for (UInt_t i = 0; i < _methods.second->size(); i++)
         methods.push_back(std::pair<TString, UInt_t>(_methods.first, i));
   }

   if (factory->GetAnalysisType() == TMVA::Types::EAnalysisType::kClassification) {

      auto executor = [factory, methods](UInt_t workerID)->OptionMap{
         TMVA::MsgLogger::InhibitOutput();
         TMVA::gConfig().SetSilent(kTRUE);
         TMVA::gConfig().SetUseColor(kFALSE);
         TMVA::gConfig().SetDrawProgressBar(kFALSE);
         OptionMap r;
         r["dataset"] = methods[workerID].first;
         r["rocint"]  = 0;
         r["method"]  = factory->GetMethodName(methods[workerID].first, methods[workerID].second);
         factory->TrainMethod(methods[workerID].first, methods[workerID].second);
         factory->TestMethod(methods[workerID].first, methods[workerID].second);
         factory->EvaluateMethod(methods[workerID].first, methods[workerID].second);
         r["rocint"] = factory->GetROCIntegral(methods[workerID].first, r.GetValue<TString>("method"));
         return r;
      };

      fTimer.Reset();
      fTimer.Start();
      auto fResults = fWorkers.Map(executor, ROOT::TSeqI(methods.size()));
      fTimer.Stop();
      TMVA::MsgLogger::EnableOutput();
      TMVA::gConfig().SetSilent(kFALSE);

      OptionMap fOptResults;

      Log().SetSource("ParallelExecutor(Factory)");
      Log() << kINFO << "-----------------------------------------------------" << Endl;
      Log() << kINFO << Form("%-20s %-15s %-15s", "Dataset", "Method", "ROC-Integral") << Endl;
      Log() << kINFO << "-----------------------------------------------------" << Endl;
      auto counter = 0;
      for (auto &item : fResults) {
         Log() << kINFO << Form("%-20s %-15s %#1.3f",
                                item.GetValue<TString>("dataset").Data(), item.GetValue<TString>("method").Data(), item.GetValue<Double_t>("rocint")) << Endl;
//         fOptResults[Form("%d",counter)]=item;
         counter++;
      }
      Log() << kINFO << "-----------------------------------------------------" << Endl;
      TMVA::gConfig().SetSilent(kTRUE);
      return TMVA::ParallelExecutorResults("ParallelExecutor(Factory)", jobs, fTimer.RealTime(), fOptResults);
   }




   return TMVA::ParallelExecutorResults("Unknow", jobs, fTimer.RealTime(), options);
}

//______________________________________________________________________________
const TMVA::ParallelExecutorResults TMVA::ParallelExecutor::Execute(TMVA::CrossValidation *cv, UInt_t jobs, TMVA::OptionMap options)
{
   fWorkers.SetNWorkers(jobs);

   auto executor = [cv](UInt_t workerID)->Double_t{
      TMVA::MsgLogger::InhibitOutput();
      TMVA::gConfig().SetSilent(kTRUE);
      TMVA::gConfig().SetUseColor(kFALSE);
      TMVA::gConfig().SetDrawProgressBar(kFALSE);
      cv->EvaluateFold(workerID);
      auto result = cv->GetResults();
      auto roc = result.GetROCValues()[workerID];
      return roc;
   };

   fTimer.Reset();
   fTimer.Start();
   auto fResults = fWorkers.Map(executor, ROOT::TSeqI(cv->GetNumFolds()));
   fTimer.Stop();

   Log().SetSource("ParallelExecutor(CV)");
   TMVA::MsgLogger::EnableOutput();
   TMVA::gConfig().SetSilent(kFALSE);

   Float_t fROCAvg = 0;
   for (UInt_t i = 0; i < fResults.size(); i++) {
      Log() << kINFO << Form("Fold  %i ROC-Int : %f", i, fResults[i]) << Endl;
      fROCAvg += fResults[i];
   }
   Log() << kINFO << "Average ROC-Int : " << fROCAvg / fResults.size() << Endl;
   TMVA::MsgLogger::InhibitOutput();
   TMVA::gConfig().SetSilent(kTRUE);
   return TMVA::ParallelExecutorResults("ParallelExecutor(CV)", jobs, fTimer.RealTime(), options);
}

