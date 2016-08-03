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

namespace TMVA {

   class CrossValidationResultItem
   {
     friend class CrossValidation;
   private:
       std::map<UInt_t,Float_t>        fROCs;
       Float_t                         fROCAVG;
       std::shared_ptr<TMultiGraph>    fROCCurves;
   public:
       CrossValidationResultItem();
       CrossValidationResultItem(const CrossValidationResultItem &);
       ~CrossValidationResultItem(){fROCCurves=nullptr;}
       
       
       std::map<UInt_t,Float_t> GetROCValues(){return fROCs;}
       Float_t GetROCAverage(){return fROCAVG;}
       TMultiGraph *GetROCCurves(Bool_t fLegend=kTRUE);
       void Print() const ;
       
       TCanvas* Draw(const TString name="CrossValidation") const;
   };
   
   class CrossValidationResult
   {
       std::vector<CrossValidationResultItem> fMethods;
       public:
           CrossValidationResult(){}
           
           void AppendMethod(CrossValidationResultItem &method){fMethods.push_back(method);}
           const CrossValidationResultItem &GetMethod(const UInt_t i) const{return fMethods[i];}
           
           //number of methods
           UInt_t GetSize(){return fMethods.size();}
           
           void Print() const{for(auto &item:fMethods) item.Print();}
           void Draw(const TString name="CrossValidation") const{for(auto &item:fMethods) item.Draw(name);}
   }; 
   
    
   class CrossValidation : public Configurable {
       UInt_t fNFolds;
       std::vector<OptionMap>  fMethods;
       CrossValidationResult   fResults;
   public:

        explicit CrossValidation(DataLoader *loader);
       ~CrossValidation();
       
       void BookMethod( TString theMethodName, TString methodTitle, TString theOption = "", int NumFolds = 5);
       void BookMethod( Types::EMVA theMethod,  TString methodTitle, TString theOption = "", int NumFolds = 5 );
       
       void EvaluateAll();
       
       const CrossValidationResult& GetResults(){return fResults;}//I need to think about this which is the best way to get the results
       
       
       inline void SetDataLoader(DataLoader *loader){fDataLoader=std::shared_ptr<DataLoader>(loader);}
       inline DataLoader *GetDataLoader(){return fDataLoader.get();}
       
   private:
       std::shared_ptr<DataLoader>  fDataLoader;
       std::unique_ptr<Factory>     fClassifier;
   };
} 


#endif



