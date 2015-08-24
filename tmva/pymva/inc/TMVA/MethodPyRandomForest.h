// @(#)root/rmva $Id$
// Author: Omar Zapata 2015

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : MethodPyRandomForest                                                  *
 *                                                                                *
 * Description:                                                                   *
 *      scikit-learn Package RandomForestClassifier  method based on python       *
 *                                                                                *
 **********************************************************************************/

#ifndef ROOT_TMVA_MethodPyRandomForest
#define ROOT_TMVA_MethodPyRandomForest

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// MethodPyRandomForest                                                 //
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
   class MethodPyRandomForest : public PyMethodBase {

   public :

      // constructors
      MethodPyRandomForest( const TString& jobName,
                   const TString& methodTitle,
                   DataSetInfo& theData,
                   const TString& theOption = "",
                   TDirectory* theTargetDir = NULL );

      MethodPyRandomForest( DataSetInfo& dsi,
                   const TString& theWeightFile,
                   TDirectory* theTargetDir = NULL );


      ~MethodPyRandomForest( void );
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

   private :
      DataSetManager*    fDataSetManager;     // DSMTEST
      friend class Factory;                   // DSMTEST
      friend class Reader;                    // DSMTEST      
   protected:
       //RandromForest options
       Int_t n_estimators;//integer, optional (default=10)
                             //The number of trees in the forest.
       TString criterion;//string, optional (default="gini")
                                //The function to measure the quality of a split. Supported criteria are
                                //"gini" for the Gini impurity and "entropy" for the information gain.
                                //Note: this parameter is tree-specific.
       TString max_features;
       Int_t max_depth;//integer or None, optional (default=None)
                         //The maximum depth of the tree. If None, then nodes are expanded until
                         //all leaves are pure or until all leaves contain less than
                         
       Int_t min_samples_leaf;//integer, optional (default=1)
                              //The minimum number of samples in newly created leaves.  A split is
                              //discarded if after the split, one of the leaves would contain less then
                              //``min_samples_leaf`` samples.
                              //Note: this parameter is tree-specific.
       Float_t min_weight_fraction_leaf;//float, optional (default=0.)
                                        //The minimum weighted fraction of the input samples required to be at a
                                        //leaf node.
                                        //Note: this parameter is tree-specific.
       Bool_t bootstrap;//boolean, optional (default=True)
                              //Whether bootstrap samples are used when building trees.
       
       Int_t n_jobs;// : integer, optional (default=1)
                      //The number of jobs to run in parallel for both `fit` and `predict`.
                      //If -1, then the number of jobs is set to the number of cores.
       //NOTE: book options not implemente yet
       //->max_features
       //->max_leaf_nodes
       //->oob_score
       //->random_state
       //->verbose
       //->warm_start
       //->class_weight
       
       
       
      // get help message text
      void GetHelpMessage() const;

      
      ClassDef(MethodPyRandomForest,0)
   };
} // namespace TMVA
#endif
