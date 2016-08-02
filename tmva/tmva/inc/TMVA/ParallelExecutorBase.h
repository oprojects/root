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

#ifndef ROOT_R_TNamed
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
       public:
           ParallelExecutorResults(const TString name, UInt_t njobs,Double_t exetime):TNamed(name.Data(),"ParallelExecutorResults"),fNJobs(njobs),fExecutionTime(exetime),fLogger(name.Data()){}
           
           using TNamed::Print;
           virtual void Print()
           {
               fLogger<<kINFO<<"Process:"<<GetName()<<Endl;
               fLogger<<kINFO<<"Number of Jobs:"<<fNJobs<<Endl;
               fLogger<<kINFO<<"ExecutionTime (Seconds):"<<Form("%f",fExecutionTime)<<Endl;
           }
           
       };
       
      /**
      \class ParallelExecutorBase
         Base class for all parallel executions
         \ingroup TMVA
       */
      
      class ParallelExecutorBase
      {
      protected:
          UInt_t fNJobs;
          TStopwatch fTimer;
//           ParallelExecutorResults fResuls;
          //           std::vector<TParallelExecutorOptionBase> fOptions;//will be implemented later with std::ostream
      public:
          ParallelExecutorBase()
          {
              ROOT::EnableThreadSafety();
          }
          virtual ParallelExecutorResults* Execute(TMVA::Factory */*factory*/,UInt_t /*jobs*/)//spacial case, the other algorithm must be from clas algorithm
          {
            std::cout<<"Factory parallization is not implemented yet."<<std::endl;
            return nullptr;
          }
      };
}

#endif
