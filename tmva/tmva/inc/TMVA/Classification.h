// @(#)root/tmva $Id$
// Author: Omar Zapata (I need to put all Authors here ;). 2016


#ifndef ROOT_TMVA_Classification
#define ROOT_TMVA_Classification


#ifndef ROOT_TString
#include "TString.h"
#endif


#ifndef ROOT_TMVA_Configurable
#include "TMVA/Configurable.h"
#endif
#ifndef ROOT_TMVA_Types
#include "TMVA/Types.h"
#endif

#ifndef ROOT_TMVA_Factory
#include<TMVA/ResultsClassification.h>
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

#ifndef ROOT_TMVA_ResultsClassification
#include "TMVA/ResultsClassification.h"
#endif

namespace TMVA {

   class ClassificationResult
   {
       friend class Classification;
   private:
       std::shared_ptr<ResultsClassification>   fClassifierResults;
       std::unique_ptr<ROCCurve>                fROCCurve;
       Float_t                                  fROCIntegral;
       OptionMap                                fMethod;
       
   public:
       ClassificationResult();
       ~ClassificationResult(){}
              
       
       void Print() const ;
       
       TCanvas* Draw(const TString name="Classifier") const;
   };
   
    
   class Classification : public Algorithm {
   private:
       ClassificationResult fResults;
       TString              fTransformations;
       std::unique_ptr<MethodBase> fMethodBase;
   public:
       Classification(DataLoader *dataloader,TString options="",TFile *file=nullptr);
       ~Classification();
       
       void Train();
       void Test();
              
       virtual void Evaluate();//call Train/Test
              
       const ClassificationResult& GetResults() const {return fResults;}
       
   };
} 


#endif



