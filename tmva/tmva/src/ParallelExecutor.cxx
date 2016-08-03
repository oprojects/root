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
        auto TrainExecutor = [factory,methods](UInt_t workerID)->int{
            factory->TrainMethod(methods[workerID].first,methods[workerID].second);
            factory->TestMethod(methods[workerID].first,methods[workerID].second);
            factory->EvaluateMethod(methods[workerID].first,methods[workerID].second);
            return 0;
        };
        fTimer.Reset();
        fTimer.Start();
        auto fResults=fWorkers.Map(TrainExecutor, ROOT::TSeqI(methods.size()));
        fTimer.Stop();
        return TMVA::ParallelExecutorResults("ParallelExecutor(Classification)",jobs,fTimer.RealTime(),options);
    }
    
    
    
    
    return TMVA::ParallelExecutorResults("Unknow",jobs,fTimer.RealTime(),options);
}

