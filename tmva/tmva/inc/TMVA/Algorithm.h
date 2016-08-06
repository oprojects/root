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

#include <sstream>
#include<iostream>
#include <memory>

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

#ifndef ROOT_TMVA_Tools
#include "TMVA/Tools.h"
#endif

#ifndef ROOT_TMVA_DataLoader
#include "TMVA/DataLoader.h"
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
          std::shared_ptr<TFile>       fFile;
          Bool_t                       fModelPersistence;
          Bool_t                       fVerbose;
          Algorithm(const TString &name,DataLoader *dalaloader=nullptr,TFile *file=nullptr,const TString options="");
          
      public:
          ~Algorithm();
          
          virtual void BookMethod( TString methodName, TString methodTitle, TString theOption = "");
          virtual void BookMethod( Types::EMVA theMethod,  TString methodTitle, TString theOption = "");

          //file related methods
          Bool_t  IsSilentFile();
          TFile* GetFile();
          void   SetFile(TFile *file);
          
          //dataloader related methods
          DataLoader *GetDataLoader();
          void SetDataLoader(DataLoader *dalaloader);

          //model persistence
          Bool_t IsModelPersistence();//is you Algorithm saved in files?
          void SetModelPersistence(Bool_t status=kTRUE);
          
          //verbose mode related methods
          Bool_t IsVerbose(){return fVerbose;}
          void SetVerbose(Bool_t status){fVerbose=status;}
          
          virtual void Evaluate() = 0;
          
      protected:
          OptionMap &GetMethod();
          
          //dataloader related utility methods
          DataInputHandler&        GetDataLoaderDataInput() { return *fDataLoader->fDataInputHandler; }
          DataSetInfo&             GetDataLoaderDataSetInfo(){return fDataLoader->DefaultDataSetInfo();}
          DataSetManager*          GetDataLoaderDataSetManager(){return fDataLoader->fDataSetManager;}
          
      };
}

#endif
