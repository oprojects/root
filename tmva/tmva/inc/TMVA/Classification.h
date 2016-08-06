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

namespace TMVA {

   class ClassificationResult
   {
       friend class Classifier;
   private:
//        ResultsClassification  fClassifierResults;
   public:
       ClassificationResult();
       ClassificationResult(const ClassificationResult&);
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
       Classification(TString name,TString options,DataLoader *dataloader=nullptr,TFile *file=nullptr);
//        Classification(TString name,TString options);
       ~Classification();
       
       void Train();
       void Test();
       
       MethodBase *GetMethod(){return fMethodBase.get();}
       
       virtual void Evaluate();//call Train/Test
              
       const ClassificationResult& GetResults() const {return fResults;}
       
   };
} 


#endif



