// @(#)root/tmva/rmva $Id$
// Author: Omar Zapata,Lorenzo Moneta, Sergei Gleyzer 2015

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : RMethodRSNNS                                                          *
 *                                                                                *
 * Description:                                                                   *
 *      R´s Package RSNNS  method based on ROOTR                                  *
 *                                                                                *
 **********************************************************************************/

#ifndef ROOT_TMVA_RMethodRSNNS
#define ROOT_TMVA_RMethodRSNNS

#ifndef ROOT_TMVA_RMethodBase
#include "TMVA/RMethodBase.h"
#endif
/**
 @namespace TMVA
 namespace associated TMVA package for ROOT.
 
 */

namespace TMVA {

   class Factory;  // DSMTEST
   class Reader;   // DSMTEST
   class DataSetManager;  // DSMTEST
   class Types;
   
       /**
        \class MethodRSNNS
         \brief RMVA class for all methods  based on RSNNS
          The Stuttgart Neural Network Simulator (SNNS) is a library containing many standard implementations of neural networks. 
          This package wraps the SNNS functionality to make it available from within R. 
          Using the RSNNS low-level interface, all of the algorithmic functionality and flexibility of SNNS can be accessed. 
          Furthermore, the package contains a convenient high-level interface, so that the most common neural network topologies and learning algorithms integrate seamlessly into R.          
          
          \section BookingRMLP The Booking options for RMLP are
          
         \authors Omar Zapata(ITM/UdeA), Lorenzo Moneta(CERN), Sergei Gleyzer(U. of Florida)
         
         \link http://oproject.org/RMVA
         \ingroup TMVA
       */
   
   class MethodRSNNS : public RMethodBase {

   public :

         /**
         Default constructor that inherits from TMVA::RMethodBase and it have a ROOT::R::TRInterface instance for internal use.
         \param jobName Name taken from method type
         \param methodType Associate TMVA::Types::EMVA (available MVA methods)
         \param methodTitle Sub method associate to method type.
         \param dsi TMVA::DataSetInfo object
         \param theOption Booking options for method
         \param theBaseDir object to TDirectory with the path to calculate histograms and results for current method.
         */
      MethodRSNNS(const TString &jobName,
                  const TString &methodTitle,
                  DataSetInfo &theData,
                  const TString &theOption = "",
                  TDirectory *theTargetDir = NULL);
         /**
         Constructor used for Testing + Application of the MVA, only (no training), using given weight file. 
         inherits from TMVA::MethodBase and it have a ROOT::R::TRInterface instance for internal use.
         \param methodType Associate TMVA::Types::EMVA (available MVA methods)
         \param dsi TMVA::DataSetInfo object
         \param theBaseDir object to TDirectory with the path to calculate histograms and results for current method.
         */
      MethodRSNNS(DataSetInfo &dsi,
                  const TString &theWeightFile,
                  TDirectory *theTargetDir = NULL);


      ~MethodRSNNS(void);
         /**
         Pure abstract method to build the train system 
         and to save the fModel object in a .RData file for model persistence
         */

      void     Train();
         /**
         Pure abstract method for options treatment(some default options initialization)
         and package options initialization
         */
      void     Init();
         /**
         Pure abstract method to declare booking options associate to multivariate algorithm.
        \see BookingRMLP
         */      
      void     DeclareOptions();
         /**
         Pure abstract method to parse booking options associate to multivariate algorithm.
         */      
      void     ProcessOptions();
      // create ranking
      const Ranking *CreateRanking()
      {
         return NULL;  // = 0;
      }


      Bool_t HasAnalysisType(Types::EAnalysisType type, UInt_t numberClasses, UInt_t numberTargets);

      // performs classifier testing
      virtual void     TestClassification();


      Double_t GetMvaValue(Double_t *errLower = 0, Double_t *errUpper = 0);

      using MethodBase::ReadWeightsFromStream;
      // the actual "weights"
      virtual void AddWeightsXMLTo(void *parent) const {}        // = 0;
      virtual void ReadWeightsFromXML(void *wghtnode) {}    // = 0;
      virtual void ReadWeightsFromStream(std::istream &) {} //= 0;       // backward compatibility
      void ReadStateFromFile();
   private :
      DataSetManager    *fDataSetManager;     // DSMTEST
      friend class Factory;                   // DSMTEST
      friend class Reader;                    // DSMTEST
   protected:
      UInt_t fMvaCounter;
      std::vector<Float_t> fProbResultForTrainSig;
      std::vector<Float_t> fProbResultForTestSig;

      TString fNetType;//default RMPL
      //RSNNS Options for all NN methods
      TString  fSize;//number of units in the hidden layer(s)
      UInt_t fMaxit;//maximum of iterations to learn

      TString fInitFunc;//the initialization function to use
      TString fInitFuncParams;//the parameters for the initialization function (type 6 see getSnnsRFunctionTable() in RSNNS package)

      TString fLearnFunc;//the learning function to use
      TString fLearnFuncParams;//the parameters for the learning function

      TString fUpdateFunc;//the update function to use
      TString fUpdateFuncParams;//the parameters for the update function

      TString fHiddenActFunc;//the activation function of all hidden units
      Bool_t fShufflePatterns;//should the patterns be shuffled?
      Bool_t fLinOut;//sets the activation function of the output units to linear or logistic

      TString fPruneFunc;//the pruning function to use
      TString fPruneFuncParams;//the parameters for the pruning function. Unlike the
      //other functions, these have to be given in a named list. See
      //the pruning demos for further explanation.
      std::vector<UInt_t>  fFactorNumeric;   //factors creations
      //RSNNS mlp require a numeric factor then background=0 signal=1 from fFactorTrain
      static Bool_t IsModuleLoaded;
      ROOT::R::TRFunctionImport predict;
      ROOT::R::TRFunctionImport mlp;
      ROOT::R::TRFunctionImport asfactor;
      ROOT::R::TRObject         *fModel;
      // get help message text
      void GetHelpMessage() const;

      ClassDef(MethodRSNNS, 0)
   };
} // namespace TMVA
#endif
