// @(#)root/tmva:$Id$
// Author: Omar Zapata   2016

/*************************************************************************
 * Copyright (C) 2016, Omar Andres Zapata Mesa                           *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/
#ifndef ROOT_TMVA_ParallelExecutorBase
#define ROOT_TMVA_ParallelExecutorBase

#ifndef ROOT_TNamed
#include<TNamed.h>
#endif
#include <sstream>
#include<iostream>

#ifndef ROOT_TMVA_Factory
#include "TMVA/Factory.h"
#endif

#ifndef ROOT_TROOT
#include<TROOT.h>
#endif

#ifndef ROOT_TStopwatch
#include<TStopwatch.h>
#endif

#ifndef ROOT_TMVA_OptionMap
#include<TMVA/OptionMap.h>
#endif

#ifndef ROOT_TMVA_Config
#include "TMVA/Config.h"
#endif

namespace TMVA {    
    
       /**
        *      \class ParallelExecutorResults
        *         Base class for all parallel executions results
        *         \ingroup TMVA
        */
       
       class ParallelExecutorResults:public TNamed
       {
       protected:
           UInt_t fNJobs;           //Number of Jobs
           Double_t fExecutionTime;   //CpuTime in seconds
           TMVA::MsgLogger fLogger; //
           OptionMap fOptions;
       public:
           ParallelExecutorResults(const TString name, UInt_t njobs,Double_t exetime,OptionMap options):
           TNamed(name.Data(),"ParallelExecutorResults"),fNJobs(njobs),fExecutionTime(exetime),fLogger(name.Data()),fOptions(options){}
           
           Double_t GetExecutionTime(){return fExecutionTime;}
           
           using TNamed::Print;
           virtual void Print()
           {
               TMVA::MsgLogger::EnableOutput();
               TMVA::gConfig().SetSilent(kFALSE);
               fLogger<<kINFO<<GetName()<<Endl;
               fLogger<<kINFO<<"Number of Jobs : "<<fNJobs<<Endl;
               fLogger<<kINFO<<"ExecutionTime (Seconds) : "<<Form("%f",fExecutionTime)<<Endl;
               fOptions.Print();
               TMVA::gConfig().SetSilent(kTRUE);
           }
           
       };
       
      /**
      \class ParallelExecutorBase
         Base class for all parallel executions
         \ingroup TMVA
       */
      
      class ParallelExecutorBase:public Configurable
      {
      protected:
          UInt_t fNJobs;
          TStopwatch fTimer;
          MsgLogger fLogger;
      public:
          ParallelExecutorBase(const TString &name=""):Configurable(name), fLogger(name.Data())
          {
              ROOT::EnableThreadSafety();
          }
          using TObject::Execute;
          const ParallelExecutorResults Execute(Configurable *algorithm,UInt_t /*jobs*/,OptionMap map=OptionMap("ParallelExecutorBase"))
          {
              fLogger<<kINFO<<algorithm->GetName()<<" parallization is not implemented yet."<<Endl;
              return ParallelExecutorResults("",0,0,map);
          }
          ClassDef(ParallelExecutorBase,0);
      };
}

#endif
