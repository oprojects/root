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

#ifndef ROOT_TMVA_OptionMap
#include<TMVA/OptionMap.h>
#endif

#ifndef ROOT_TMVA_Algorithm
#include<TMVA/Algorithm.h>
#endif

namespace TMVA {

   class CrossValidationResult
   {
     friend class CrossValidation;
   private:
       std::map<UInt_t,Float_t>        fROCs;
       std::shared_ptr<TMultiGraph>    fROCCurves;
   public:
       CrossValidationResult();
       CrossValidationResult(const CrossValidationResult &);
       ~CrossValidationResult(){fROCCurves=nullptr;}
       
       
       std::map<UInt_t,Float_t> GetROCValues(){return fROCs;}
       Float_t GetROCAverage() const;
       TMultiGraph *GetROCCurves(Bool_t fLegend=kTRUE);
       void Print() const ;
       
       TCanvas* Draw(const TString name="CrossValidation") const;
   };
   
    
   class CrossValidation : public Algorithm {
       UInt_t                 fNumFolds;     //!
       CrossValidationResult  fResults;      //!
   public:
        explicit CrossValidation(DataLoader *loader);
       ~CrossValidation();
       
       void SetNumFolds(UInt_t i){fNumFolds=i;}
       UInt_t GetNumFolds(){return fNumFolds;}
       
       virtual void Evaluate();
       void EvaluateFold(UInt_t fold);//used in ParallelExecution
       
       const CrossValidationResult& GetResults() const {return fResults;}//I need to think about this which is the best way to get the results
                     
   private:
       std::unique_ptr<Factory>     fClassifier; //!
       ClassDef(CrossValidation,0);
   };
} 


#endif



