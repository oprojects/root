// @(#)root/tmva $Id$
// Author: Omar Zapata, Thomas James Stevenson.


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


namespace TMVA {

   class CrossValidationResult:public TObject
   {
   private:
       std::vector<Float_t> fROCs;
       Float_t              fROCAVG;
       TMultiGraph         *fROCCurves;
   public:
       CrossValidationResult(std::vector<Float_t> rocs,Float_t rocavg,TMultiGraph   *rocurves);
       ~CrossValidationResult();
       
       
       std::vector<Float_t> GetROCValues(){return fROCs;}
       Float_t GetROCAverage(){return fROCAVG;}
       TMultiGraph *GetROCCurves(Bool_t fLegend=kTRUE);
//        ClassDef(CrossValidationResult,0);  
   };
    
   class CrossValidation : public Configurable {

   public:

       CrossValidation();
       CrossValidation(DataLoader *loader);
       ~CrossValidation();
       
       CrossValidationResult* CrossValidate( TString theMethodName, TString methodTitle, TString theOption = "" );
       CrossValidationResult* CrossValidate( Types::EMVA theMethod,  TString methodTitle, TString theOption = "" );
       
       inline void SetDataLoader(DataLoader *loader){fDataLoader=loader;}
       inline DataLoader *GetDataLoader(){return fDataLoader;}
       
   private:
       DataLoader *fDataLoader;
       Factory    *fClassifier;

   public:
//        ClassDef(CrossValidation,1);  
   };
} 


#endif



