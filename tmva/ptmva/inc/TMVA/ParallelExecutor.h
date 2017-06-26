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
#include <TNamed.h>
#endif

#ifndef ROOT_TMVA_ParallelExecutorBase
#include <TMVA/ParallelExecutorBase.h>
#endif

#ifndef ROOT_TMVA_Factory
#include <TMVA/Factory.h>
#endif

#ifndef ROOT_TMVA_CrossValidation
#include <TMVA/CrossValidation.h>
#endif

#ifndef ROOT_TProcPool
#include <TProcPool.h>
#endif

#ifndef ROOT_TROOT
#include <TROOT.h>
#endif
namespace TMVA {

/**
\class ParallelExecutor
   class for parallel executions using MultiProc.
\ingroup TMVA
 */

class ParallelExecutor : public ParallelExecutorBase {
private:
  TProcPool fWorkers; //! procpool object

public:
  /*
    Default constructor.
   */
  ParallelExecutor() : ParallelExecutorBase() {}

  using ParallelExecutorBase::Execute; // Defualt method for not supported
                                       // algorithms

  /**
    Method Train/Test in parallel for every booked method in TMVA::Factory.
    \param factory TMVA::Factory object with the booked methods to execute
    \param jobs number of jobs
    \param options extra options, not needed in this case
    \return returning an object with TMVA::ParallelExecutorResults
  */
  const TMVA::ParallelExecutorResults
  Execute(TMVA::Factory *, UInt_t jobs,
          TMVA::OptionMap map = OptionMap("ParallelExecutor"));

  /**
    Method to execute Cross Validation in parallel, the algorithm is splitted by
    folds.
    \param cv TMVA::CrossValidation object.
    \param jobs number of jobs
    \param options extra options, not needed in this case
    \return returning an object with TMVA::ParallelExecutorResults
  */
  const TMVA::ParallelExecutorResults
  Execute(TMVA::CrossValidation *, UInt_t jobs,
          TMVA::OptionMap options = TMVA::OptionMap("ParallelExecutor"));

  ClassDef(ParallelExecutor, 0);
};
}

#endif
