// @(#)root/tmva $Id$ 2017
// Authors:  Omar Zapata, Andreas Hoecker, Peter Speckmayer, Joerg Stelzer, Helge Voss, Kai Voss, Eckhard von Toerne,
// Jan Therhaag

#ifndef ROOT_TMVA_Classification
#define ROOT_TMVA_Classification

#include <TString.h>
#include <TMultiGraph.h>

#include <TMVA/IMethod.h>
#include <TMVA/MethodBase.h>
#include <TMVA/Configurable.h>
#include <TMVA/Types.h>
#include <TMVA/DataSet.h>
#include <TMVA/Event.h>
#include <TMVA/Results.h>
#include <TMVA/Factory.h>
#include <TMVA/DataLoader.h>
#include <TMVA/OptionMap.h>
#include <TMVA/Envelope.h>

/*! \class TMVA::ClassificationResult
 * Class to save the results of the classifier.
\ingroup TMVA
*/

/*! \class TMVA::Classification
 * Class to perform two class and multi-class classification.
\ingroup TMVA
*/

namespace TMVA {
class ResultsClassification;
namespace Experimental {
class ClassificationResult {
   friend class Classification;

private:
   std::shared_ptr<ResultsClassification> fClassifierResults;
   std::shared_ptr<ROCCurve> fROCCurve;
   Float_t fROCIntegral;
   OptionMap fMethod;
   TString fDataLoaderName;

   Bool_t IsMethod(TString methodname, TString methodtitle);

public:
   ClassificationResult();
   ~ClassificationResult() {}
   ClassificationResult(const ClassificationResult &) = default;

   void Print() const;

   TCanvas *Draw(const TString name = "Classifier") const;
};

class Classification : public Envelope {
   std::vector<ClassificationResult *> fResults; //!
   Bool_t fMulticlass;                           //!
   TString fTransformations;                     //! List of transformations to test
   std::vector<IMethod *> fIMethods;             //! vector of objects with booked methods
   Types::EAnalysisType fAnalysisType;           //!
public:
   explicit Classification(DataLoader *loader, TFile *file, TString options);
   explicit Classification(DataLoader *loader, TString options);
   ~Classification();

   virtual void Train();
   virtual void TrainMethod(TString methodname, TString methodtitle);
   virtual void TrainMethod(Types::EMVA method, TString methodtitle);

   virtual void Test();
   virtual void TestMethod(TString methodname, TString methodtitle);

   virtual void Evaluate();

   const std::vector<ClassificationResult *> &GetResults() const;

   MethodBase *GetMethod(TString methodname, TString methodtitle);

private:
   void CreateEnvironment();
   TString GetMethodOptions(TString methodname, TString methodtitle);
   Bool_t HasMethodObject(TString methodname, TString methodtitle, Int_t &index);
   Bool_t IsCutsMethod(TMVA::MethodBase *method);
   TMVA::ROCCurve *GetROC(TMVA::MethodBase *method, UInt_t iClass = 0, Types::ETreeType type = Types::kTesting);
   ClassificationResult *GetResults(TString methodname, TString methodtitle);
   ClassDef(Classification, 0);
};
} // namespace Experimental
} // namespace TMVA

#endif // ROOT_TMVA_Classification
