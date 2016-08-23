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
         
         This is an example code to compute CrossValidation in parallel.
         \code{.cpp}
         \endcode

         
       */
      
      class ParallelExecutorMpi:public ParallelExecutorBase
      {
      private:
          Double_t fTimeStart;
          Double_t fTimeEnd;
          /**
           Private to eval a group of folds
           \param cv CrossValidation object
           \param jobs number of folds to run
           \return ParallelExecutorResults for MPI
         */
                  const TMVA::ParallelExecutorResults Execute(TMVA::CrossValidation*,UInt_t jobs,TMVA::OptionMap options=TMVA::OptionMap("ParallelExecutorMpi") );
          
      public:
          /**
           Contructor to start the MPI communication system. 
          */
          ParallelExecutorMpi();
          /**
           Destructor to finalize the MPI communication system. 
          */
          ~ParallelExecutorMpi();
        
          /**
           Method to share a dataloader object using MPI broadcast to the other processes using serialization.
           \param dl TMVA::DataLoader object.
          */
           void SharedDataLoader(DataLoader *dl);
    
          /**
           Method to see if I am in the main process.
           \return True if the number of the rank is the main process.
          */
           UInt_t IsMainProcess();
        
          /**
           Method to get the current rank from MPI
           \return the number of the rank.
          */
           UInt_t GetRank();
           
          /**
           Method to get the current rank from MPI
           \return the number of the rank.
          */
           UInt_t GetSize();
        
          using ParallelExecutorBase::Execute;//Defualt method for not supported algorithms
          /**
           Method execute cross validation in parallel.
           \return the number of the rank.
          */
          const TMVA::ParallelExecutorResults Execute(TMVA::CrossValidation*,TMVA::OptionMap options=TMVA::OptionMap("ParallelExecutorMpi") );
          
          
          ClassDef(ParallelExecutorMpi,0);    
      };
      
}


#endif
