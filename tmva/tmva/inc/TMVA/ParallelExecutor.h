// @(#)root/tmva:$Id$
// Author: Omar Zapata   2016

/*************************************************************************
 * Copyright (C) 2016, Omar Andres Zapata Mesa                           *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/
#ifndef ROOT_TMVA_ParallelExecutor
#define ROOT_TMVA_ParallelExecutor

#ifndef ROOT_TNamed
#include<TNamed.h>
#endif

#ifndef ROOT_TMVA_ParallelExecutorBase
#include<TMVA/ParallelExecutorBase.h>
#endif

#ifndef ROOT_TMVA_Factory
#include<TMVA/Factory.h>
#endif

#ifndef ROOT_TMVA_CrossValidation
#include<TMVA/CrossValidation.h>
#endif

#ifndef ROOT_TProcPool
#include<TProcPool.h>
#endif

#ifndef ROOT_TROOT
#include<TROOT.h>
#endif
namespace TMVA {
              
      /**
      \class ParallelExecutor
         class for parallel executions using threads.
         if you want to add support for your class here, you just 
         need to overload the template method Execute
         \ingroup TMVA
       */
      
      class ParallelExecutor:public ParallelExecutorBase
      {
      private:
          TProcPool fWorkers;
          
          
      public:
          ParallelExecutor():ParallelExecutorBase(){ROOT::EnableThreadSafety();}
          using ParallelExecutorBase::Execute;//Defualt method for not supported algorithms
          const TMVA::ParallelExecutorResults Execute(TMVA::Factory*,UInt_t jobs,TMVA::OptionMap options=TMVA::OptionMap("ParallelExecutor") );
          const TMVA::ParallelExecutorResults Execute(TMVA::CrossValidation*,UInt_t jobs,TMVA::OptionMap options=TMVA::OptionMap("ParallelExecutor") );
          
      };
      
}


#endif
