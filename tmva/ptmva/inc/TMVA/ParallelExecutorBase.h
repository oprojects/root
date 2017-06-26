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
#include <TNamed.h>
#endif
#include <iostream>
#include <sstream>

#ifndef ROOT_TMVA_Factory
#include "TMVA/Factory.h"
#endif

#ifndef ROOT_TROOT
#include <TROOT.h>
#endif

#ifndef ROOT_TStopwatch
#include <TStopwatch.h>
#endif

#ifndef ROOT_TMVA_OptionMap
#include <TMVA/OptionMap.h>
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

class ParallelExecutorResults : public TNamed {
protected:
  UInt_t fNJobs;           // Number of Jobs
  Double_t fExecutionTime; // CpuTime in seconds
  TMVA::MsgLogger fLogger; //! message logger
  OptionMap fOptions;      // options for the results
public:
  /**
  Constructor to build the results.
  \param name name of the results
  \param njobs number of jobs used in the execution.
  \param extime execution time.
  \param options options used for the results
  */
  ParallelExecutorResults(const TString name, UInt_t njobs, Double_t exetime,
                          OptionMap options)
      : TNamed(name.Data(), "ParallelExecutorResults"), fNJobs(njobs),
        fExecutionTime(exetime), fLogger(name.Data()), fOptions(options) {}

  /**
  Method to get the execution time.
  \return time in seconds
  */
  Double_t GetExecutionTime() { return fExecutionTime; }
  const TMVA::OptionMap GetResultsMap() { return fOptions; }
  using TNamed::Print;
  /**
  Method to print in the stdout the results of the execution.
  */
  virtual void Print() {
    TMVA::MsgLogger::EnableOutput();
    TMVA::gConfig().SetSilent(kFALSE);
    fLogger << kINFO << GetName() << Endl;
    fLogger << kINFO << "Number of Jobs : " << fNJobs << Endl;
    fLogger << kINFO
            << "ExecutionTime (Seconds) : " << Form("%f", fExecutionTime)
            << Endl;
    fOptions.Print();
    TMVA::gConfig().SetSilent(kTRUE);
  }
};

/**
\class ParallelExecutorBase
   Base class for all parallel executions in differents paradigms.
   \ingroup TMVA
 */

class ParallelExecutorBase : public Configurable {
protected:
  UInt_t fNJobs;     // number of jobs
  TStopwatch fTimer; // timer to measute the time.
  MsgLogger fLogger; //! message logger
public:
  /**
   Constructor to start thread safety.
   \param name the name of the new executor.
  */

  ParallelExecutorBase(const TString &name = "")
      : Configurable(name), fLogger(name.Data()) {
    ROOT::EnableThreadSafety();
  }
  using TObject::Execute;

  /**
   Method to be overloaded to execute the implemented algorithm in parallel.
   \return the number of the rank.
  */
  const ParallelExecutorResults
  Execute(Configurable *algorithm, UInt_t /*jobs*/,
          OptionMap map = OptionMap("ParallelExecutorBase")) {
    fLogger << kINFO << algorithm->GetName()
            << " parallization is not implemented yet." << Endl;
    return ParallelExecutorResults("", 0, 0, map);
  }
  ClassDef(ParallelExecutorBase, 0);
};
}

#endif
