#include<TMVA/ParallelExecutorMpi.h>
#include<TMVA/ResultsClassification.h>
#include<TMVA/MethodBase.h>
#include<ThreadPool.h>
#include<TMVA/MsgLogger.h>
#include<mpi/mpi.h>
#include<TMessage.h>

TMVA::ParallelExecutorMpi::ParallelExecutorMpi():ParallelExecutorBase(){
        ROOT::EnableThreadSafety();
        fArgc=0;
        fArgs=NULL;
        MPI::Init(fArgc, fArgs);
}
TMVA::ParallelExecutorMpi::~ParallelExecutorMpi(){
        MPI::Finalize();
    }

UInt_t TMVA::ParallelExecutorMpi::IsMainProcess(){return MPI::COMM_WORLD.Get_rank()==0;}    
UInt_t TMVA::ParallelExecutorMpi::GetRank(){return MPI::COMM_WORLD.Get_rank();}
UInt_t TMVA::ParallelExecutorMpi::GetSize(){return MPI::COMM_WORLD.Get_size();}

class MpiMessage:public TMessage
{
public:
    MpiMessage(char *buffer, int size):TMessage(buffer, size){}
};

void TMVA::ParallelExecutorMpi::SharedDataLoader(DataLoader *dl)
{
    Int_t size;
    Char_t * buffer=nullptr;
    TMessage msg(kMESS_OBJECT);
    
    if(IsMainProcess())
    {
        msg.WriteObject(dl);
        size = msg.BufferSize();
        buffer = msg.Buffer();    
    }
    MPI::COMM_WORLD.Bcast(&size, 1,MPI::INT, 0);
    
    if(!IsMainProcess())buffer=new Char_t[size];
    MPI::COMM_WORLD.Bcast(buffer, size,MPI::CHAR, 0);
    
    if(!IsMainProcess())
    {
        MpiMessage *_msg=new MpiMessage(buffer,size);
        memcpy(dl, _msg->ReadObject(dl->Class()), sizeof(TMVA::DataLoader)); 
        delete _msg;
    }
}

const TMVA::ParallelExecutorResults TMVA::ParallelExecutorMpi::Execute(TMVA::CrossValidation *cv,UInt_t jobs,TMVA::OptionMap options)
{
    UInt_t endProc;
    UInt_t initProc; 
    if(GetSize()<=cv->GetNumFolds())
    {    
         endProc = (GetSize()-GetRank())*jobs;
         initProc = endProc - jobs;
    }else
    {
        endProc  = GetRank()+1;
        initProc = GetRank();
    }
    
    
    Log().SetName("ParallelExecutorMpi(CV)");
    for(UInt_t fold=initProc;fold<endProc;fold++)
    {
       TMVA::MsgLogger::InhibitOutput();
       TMVA::gConfig().SetSilent(kTRUE);  
       TMVA::gConfig().SetUseColor( kFALSE);
       TMVA::gConfig().SetDrawProgressBar( kFALSE);
       cv->EvaluateFold(fold);
       auto result=cv->GetResults();
       auto roc=result.GetROCValues()[fold];
       TMVA::MsgLogger::EnableOutput();
       TMVA::gConfig().SetSilent(kFALSE);  
       Log()<<kINFO<<Form("Fold  %i ROC-Int : %f",fold,roc)<<Endl;
       TMVA::gConfig().SetSilent(kTRUE);  
    }
    
    TMVA::MsgLogger::EnableOutput();
    TMVA::gConfig().SetSilent(kFALSE);
    
    return TMVA::ParallelExecutorResults("ParallelExecutorMpi(CV)",GetSize(),fTimer.RealTime(),options);
}

const TMVA::ParallelExecutorResults TMVA::ParallelExecutorMpi::Execute(TMVA::CrossValidation *cv,TMVA::OptionMap options)
{
    MPI::COMM_WORLD.Barrier();
    fTimeStart = MPI::Wtime();
    UInt_t rproc=cv->GetNumFolds()%GetSize();//remaining process
    UInt_t proc=cv->GetNumFolds()/GetSize();
    TMVA::ParallelExecutorResults Results("ParallelExecutorMpi(CV)",GetSize(),0,options);
//     std::cout<<"proc "<<proc<<" rank "<<GetRank()<<" size "<<GetSize()<<" folds "<<cv->GetNumFolds()<<std::endl;
    if(GetSize()<=cv->GetNumFolds())
    {
        if(rproc==0) Results=Execute(cv,proc,options);
        else{
            if(GetRank()==GetSize()-1)  Results=Execute(cv,proc+rproc,options);
            else Results=Execute(cv,proc,options);
        }
    }else{//more process that folds
        if(GetRank()<cv->GetNumFolds()) Results=Execute(cv,1,options);
    }
    MPI::COMM_WORLD.Barrier();
    fTimeEnd = MPI::Wtime();
    
    return TMVA::ParallelExecutorResults("ParallelExecutorMpi(CV)",GetSize(),fTimeEnd-fTimeStart,options);
}
