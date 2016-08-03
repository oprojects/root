// @(#)root/tmva $Id$
// Author: Omar Zapata, Thomas James Stevenson and Pourya Vakilipourtakalou. 2016


#ifndef ROOT_TMVA_CrossValidation
#define ROOT_TMVA_CrossValidation


#ifndef ROOT_TString
#include "TString.h"
#endif

#ifndef ROOT_TMultiGraph
#include "TMultiGraph.h"
#endif

#ifndef ROOT_TMVA_IMethod
#include "TMVA/IMethod.h"
#endif
#ifndef ROOT_TMVA_Configurable
#include "TMVA/Configurable.h"
#endif
#ifndef ROOT_TMVA_Types
#include "TMVA/Types.h"
#endif
#ifndef ROOT_TMVA_DataSet
#include "TMVA/DataSet.h"
#endif
#ifndef ROOT_TMVA_Event
#include "TMVA/Event.h"
#endif
#ifndef ROOT_TMVA_Results
#include<TMVA/Results.h>
#endif

#ifndef ROOT_TMVA_Factory
#include<TMVA/Factory.h>
#endif

#ifndef ROOT_TMVA_DataLoader
#include<TMVA/DataLoader.h>
#endif


namespace TMVA {

   class CrossValidationResult
   {
     friend class CrossValidation;
   private:
       std::map<UInt_t,Float_t>        fROCs;
       Float_t                         fROCAVG;
       std::unique_ptr<TMultiGraph>    fROCCurves;
   public:
        CrossValidationResult();
       ~CrossValidationResult();
       
       
       std::map<UInt_t,Float_t> GetROCValues(){return fROCs;}
       Float_t GetROCAverage(){return fROCAVG;}
       TMultiGraph *GetROCCurves(Bool_t fLegend=kTRUE);
       void Print() const ;
       
       TCanvas* Draw(const TString name="CrossValidation") const;
       
   };
    
   class CrossValidation : public Configurable {
       UInt_t fNFolds;
   public:

        explicit CrossValidation(DataLoader *loader);
       ~CrossValidation();
       
       const CrossValidationResult* Evaluate( TString theMethodName, TString methodTitle, TString theOption = "", int NumFolds = 5);
       const CrossValidationResult* Evaluate( Types::EMVA theMethod,  TString methodTitle, TString theOption = "", int NumFolds = 5 );
       
       inline void SetDataLoader(DataLoader *loader){fDataLoader=std::shared_ptr<DataLoader>(loader);}
       inline DataLoader *GetDataLoader(){return fDataLoader.get();}
       
   private:
       std::shared_ptr<DataLoader>  fDataLoader;
       std::unique_ptr<Factory>     fClassifier;
   };
} 


#endif



