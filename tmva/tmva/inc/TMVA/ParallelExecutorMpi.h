// @(#)root/tmva:$Id$
// Author: Omar Zapata   2016

/*************************************************************************
 * Copyright (C) 2016, Omar Andres Zapata Mesa                           *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/
#ifndef ROOT_TMVA_ParallelExecutorMpi
#define ROOT_TMVA_ParallelExecutorMpi

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
      \class ParallelExecutorMpi
         class for parallel executions using MPI.
         if you want to add support for your class here, you just 
         need to overload the template method Execute
         \ingroup TMVA
       */
      
      class ParallelExecutorMpi:public ParallelExecutorBase
      {
      private:
          Int_t   fArgc;
          Char_t  **fArgs;
          Double_t fTimeStart;
          Double_t fTimeEnd;
          const TMVA::ParallelExecutorResults Execute(TMVA::CrossValidation*,UInt_t jobs,TMVA::OptionMap options=TMVA::OptionMap("ParallelExecutorMpi") );
          
      public:
          ParallelExecutorMpi();
        ~ParallelExecutorMpi();
        
        void SharedDataLoader(DataLoader *dl);
        UInt_t IsMainProcess();
        
        UInt_t GetRank();
        UInt_t GetSize();
        
          using ParallelExecutorBase::Execute;//Defualt method for not supported algorithms
          const TMVA::ParallelExecutorResults Execute(TMVA::CrossValidation*,TMVA::OptionMap options=TMVA::OptionMap("ParallelExecutorMpi") );
          //           const TMVA::ParallelExecutorResults Execute(TMVA::Factory*,UInt_t jobs,TMVA::OptionMap options=TMVA::OptionMap("ParallelExecutor") );
//           ClassDef(ParallelExecutorMpi,0);    
      };
      
}


#endif
