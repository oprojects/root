// @(#)root/tmva:$Id$
// Author: Omar Zapata   2016

/*************************************************************************
 * Copyright (C) 2016, Omar Andres Zapata Mesa                           *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/
#ifndef ROOT_TMVA_Algorithm
#define ROOT_TMVA_Algorithm

#ifndef ROOT_TNamed
#include<TNamed.h>
#endif
#include <sstream>
#include<iostream>
#include <memory>

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
        *      \class Algorithm
        *         Base class for all machine learning algorithms
        *         \ingroup TMVA
        */
      
      class Algorithm:public Configurable
      {
      protected:
          OptionMap                    fMethod;//Booked method information
          std::shared_ptr<DataLoader>  fDataLoader;
          
          Algorithm(const TString &name="Algorithm");
          Algorithm(DataLoader *dalaloader,const TString &name="Algorithm");
      public:
          ~Algorithm();
          
          virtual void BookMethod( TString methodName, TString methodTitle, TString theOption = "");
          virtual void BookMethod( Types::EMVA theMethod,  TString methodTitle, TString theOption = "");
          
          OptionMap &GetMethod();
          
          DataLoader *GetDataLoader();
          void SetDataLoader(DataLoader *dalaloader);
          
          virtual void Evaluate() = 0;
      };
}

#endif
