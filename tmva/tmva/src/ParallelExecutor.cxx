#include<TMVA/ParallelExecutor.h>
#include<TMVA/ResultsClassification.h>
#include<TMVA/MethodBase.h>
#include<ThreadPool.h>

const TMVA::ParallelExecutorResults TMVA::ParallelExecutor::Execute(TMVA::Factory *factory,UInt_t jobs,TMVA::OptionMap options)
{
    fWorkers.SetNWorkers(jobs);
    
    std::vector<std::pair<TString,UInt_t> > methods;//the pair that have the dataset name and the method positon in the vector 
    for(auto& _methods : factory->GetMethodsMap())
    {
        for(UInt_t i=0;i<_methods.second->size();i++)
        methods.push_back(std::pair<TString,UInt_t>(_methods.first,i));
    }

    if(factory->GetAnalysisType()==TMVA::Types::EAnalysisType::kClassification)
    {
        auto executor = [factory,methods](UInt_t workerID)->int{
            factory->TrainMethod(methods[workerID].first,methods[workerID].second);
            factory->TestMethod(methods[workerID].first,methods[workerID].second);
            factory->EvaluateMethod(methods[workerID].first,methods[workerID].second);
            return 0;
        };
        fTimer.Reset();
        fTimer.Start();
        auto fResults=fWorkers.Map(executor, ROOT::TSeqI(methods.size()));
        fTimer.Stop();
        return TMVA::ParallelExecutorResults("ParallelExecutor(Classification)",jobs,fTimer.RealTime(),options);
    }
    
    
    
    
    return TMVA::ParallelExecutorResults("Unknow",jobs,fTimer.RealTime(),options);
}

const TMVA::ParallelExecutorResults TMVA::ParallelExecutor::Execute(TMVA::CrossValidation *cv,UInt_t jobs,TMVA::OptionMap options)
{
    fWorkers.SetNWorkers(jobs);
    auto dataloader = cv->GetDataLoader();
    dataloader->MakeKFoldDataSet(cv->GetNumFolds());
    
    
    auto executor = [cv](UInt_t workerID)->Double_t{
            cv->EvaluateFold(workerID);
            auto result=cv->GetResults();
            auto roc=result.GetROCValues()[workerID];
            return roc;
    };
    
    fTimer.Reset();
    fTimer.Start();
    auto fResults=fWorkers.Map(executor, ROOT::TSeqI(cv->GetNumFolds()));
    fTimer.Stop();
    
    Double_t fROCAvg=0;
    for(Double_t &item:fResults)
    {
        std::cout<<"ROC  : "<<item<<std::endl;
        fROCAvg+=item;
    }
    std::cout<<"ROCAvg :"<<fROCAvg/fResults.size()<<std::endl;
    return TMVA::ParallelExecutorResults("ParallelExecutor(CrossValidation)",jobs,fTimer.RealTime(),options);
}

