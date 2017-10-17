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
#include <TMVA/ResultsClassification.h>
#include <TMVA/ResultsMulticlass.h>
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
class ClassificationResult : public TObject {
   friend class Classification;

private:
   //    TMVA::ResultsClassification *fClassifierResults; //!
   //    TMVA::ResultsMulticlass *fMulticlassResults;     //!
   std::shared_ptr<ROCCurve> fROCCurve; //!
   Float_t fROCIntegral;                //
   OptionMap fMethod;                   //
   TString fDataLoaderName;             //
   Bool_t fMulticlass;                  //
   std::vector<Float_t> fMvaRes;        //
   std::vector<Bool_t> fMvaResTypes;    //
   std::vector<Float_t> fMvaResWeights; //

   Bool_t IsMethod(TString methodname, TString methodtitle);

public:
   ClassificationResult();
   ClassificationResult(const ClassificationResult &cr);
   ~ClassificationResult() {}

   //    const TMVA::ResultsClassification *GetResultsClassification() const { return fClassifierResults; }
   //    const TMVA::ResultsMulticlass *GetResultsMulticlass() const { return fMulticlassResults; }
   const TString GetMethodName() const { return fMethod.GetValue<TString>("MethodName"); }
   const TString GetMethodTitle() const { return fMethod.GetValue<TString>("MethodTitle"); }
   //    const ROCCurve *GetROCCurve() const { return fROCCurve; }
   Float_t GetROCIntegral() { return fROCIntegral; }
   TString GetDataLoaderName() { return fDataLoaderName; }

   void Print(Option_t *option = "") const;

   void Draw(Option_t *option = "Classifier");
   TCanvas *GetCanvas(Option_t *option = "Classifier");
   ClassificationResult &operator=(const ClassificationResult &r);

   ClassDef(ClassificationResult, 3);
};

class Classification : public Envelope {
   std::vector<ClassificationResult> fResults;   //!
   Bool_t fMulticlass;                           //!
   std::vector<IMethod *> fIMethods;             //! vector of objects with booked methods
   Types::EAnalysisType fAnalysisType;           //!
   Bool_t fCorrelations;                         //!
   Bool_t fROC;                                  //!
public:
   explicit Classification(DataLoader *loader, TFile *file, TString options);
   explicit Classification(DataLoader *loader, TString options);
   ~Classification();

   virtual void Train();
   virtual void TrainMethod(TString methodname, TString methodtitle);
   virtual void TrainMethod(Types::EMVA method, TString methodtitle);

   virtual void Test();
   virtual void TestMethod(TString methodname, TString methodtitle);
   virtual void TestMethod(Types::EMVA method, TString methodtitle);

   virtual void Evaluate();

   const std::vector<ClassificationResult> &GetResults() const;

   MethodBase *GetMethod(TString methodname, TString methodtitle);

protected:
   void CreateEnvironment();
   TString GetMethodOptions(TString methodname, TString methodtitle);
   Bool_t HasMethodObject(TString methodname, TString methodtitle, Int_t &index);
   Bool_t IsCutsMethod(TMVA::MethodBase *method);
   TMVA::ROCCurve *
   GetROC(TMVA::MethodBase *method, UInt_t iClass = 0, TMVA::Types::ETreeType type = TMVA::Types::kTesting);
   TMVA::ROCCurve *GetROC(TString methodname, TString methodtitle, UInt_t iClass = 0,
                          TMVA::Types::ETreeType type = TMVA::Types::kTesting);

   Double_t GetROCIntegral(TString methodname, TString methodtitle, UInt_t iClass = 0);

   ClassificationResult &GetResults(TString methodname, TString methodtitle);
   ClassDef(Classification, 0);
};
} // namespace Experimental
} // namespace TMVA

#endif // ROOT_TMVA_Classification
