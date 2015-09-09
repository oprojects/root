// @(#)root/tmva/pymva $Id$
// Authors: Omar Zapata, Lorenzo Moneta, Sergei Gleyzer 2015

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : MethodPyAdaBoost                                                      *
 * Web    : http://oproject.org                                                   *
 *                                                                                *
 * Description:                                                                   *
 *      scikit-learn Package AdaBoostClassifier      method based on python       *
 *                                                                                *
 **********************************************************************************/

#ifndef ROOT_TMVA_MethodPyAdaBoost
#define ROOT_TMVA_MethodPyAdaBoost

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// MethodPyAdaBoost                                                     //
//                                                                      //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TMVA_PyMethodBase
#include "TMVA/PyMethodBase.h"
#endif

namespace TMVA {

   class Factory;  // DSMTEST
   class Reader;   // DSMTEST
   class DataSetManager;  // DSMTEST
   class Types;
   class MethodPyAdaBoost : public PyMethodBase {

   public :

      // constructors
      MethodPyAdaBoost( const TString& jobName,
                   const TString& methodTitle,
                   DataSetInfo& theData,
                   const TString& theOption = "",
                   TDirectory* theTargetDir = NULL );

      MethodPyAdaBoost( DataSetInfo& dsi,
                   const TString& theWeightFile,
                   TDirectory* theTargetDir = NULL );


      ~MethodPyAdaBoost( void );
      void     Train();
      // options treatment
      void     Init();
      void     DeclareOptions();
      void     ProcessOptions();
      // create ranking
      const Ranking* CreateRanking(){return NULL;}// = 0;
      
      
      Bool_t HasAnalysisType( Types::EAnalysisType type, UInt_t numberClasses, UInt_t numberTargets );
     
      // performs classifier testing
      virtual void     TestClassification();
      
      
      Double_t GetMvaValue( Double_t* errLower = 0, Double_t* errUpper = 0);
      
      using MethodBase::ReadWeightsFromStream;
      // the actual "weights"
      virtual void AddWeightsXMLTo      ( void* parent ) const {}// = 0;
      virtual void ReadWeightsFromXML   ( void* wghtnode ){}// = 0;
      virtual void ReadWeightsFromStream( std::istream& ) {}//= 0;       // backward compatibility
      void ReadStateFromFile();
   private :
      DataSetManager*    fDataSetManager;     // DSMTEST
      friend class Factory;                   // DSMTEST
      friend class Reader;                    // DSMTEST      
   protected:
       //RandromForest options
       Int_t n_estimators;//integer, optional (default=10)
                             //The number of trees in the forest.
       
      // get help message text
      void GetHelpMessage() const;

      
      ClassDef(MethodPyAdaBoost,0)
   };
} // namespace TMVA
#endif
