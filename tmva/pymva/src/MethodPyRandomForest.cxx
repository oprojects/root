// @(#)root/tmva/pymva $Id$
// Author: Omar Zapata 2015

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : MethodPyRandomForest                                                  *
 * Web    : http://tmva.sourceforge.net                                           *
 *                                                                                *
 * Description:                                                                   *
 *      Random Forest Classifiear from Scikit learn                               *
 *                                                                                *
 *                                                                                *
 * Redistribution and use in source and binary forms, with or without             *
 * modification, are permitted according to the terms listed in LICENSE           *
 * (http://tmva.sourceforge.net/LICENSE)                                          *
 *                                                                                *
 **********************************************************************************/

#include <iomanip>

#include "TMath.h"
#include "Riostream.h"
#include "TMatrix.h"
#include "TMatrixD.h"
#include "TVectorD.h"

#include "TMVA/VariableTransformBase.h"
#include "TMVA/MethodPyRandomForest.h"
#include "TMVA/Tools.h"
#include "TMVA/Ranking.h"
#include "TMVA/Types.h"
#include "TMVA/PDF.h"
#include "TMVA/ClassifierFactory.h"

#include "TMVA/Results.h"

using namespace TMVA;

REGISTER_METHOD(PyRandomForest)

ClassImp(MethodPyRandomForest)


//_______________________________________________________________________
MethodPyRandomForest::MethodPyRandomForest(const TString &jobName,
                     const TString &methodTitle,
                     DataSetInfo &dsi,
                     const TString &theOption,
                     TDirectory *theTargetDir) :
   PyMethodBase(jobName, Types::kPyRandomForest, methodTitle, dsi, theOption, theTargetDir)
{
   // standard constructor for the PyRandomForest


}

//_______________________________________________________________________
MethodPyRandomForest::MethodPyRandomForest(DataSetInfo &theData, const TString &theWeightFile, TDirectory *theTargetDir)
   : PyMethodBase(Types::kPyRandomForest, theData, theWeightFile, theTargetDir)
{
}


//_______________________________________________________________________
MethodPyRandomForest::~MethodPyRandomForest(void)
{
}

//_______________________________________________________________________
Bool_t MethodPyRandomForest::HasAnalysisType(Types::EAnalysisType type, UInt_t numberClasses, UInt_t numberTargets)
{
   if (type == Types::kClassification && numberClasses == 2) return kTRUE;
   return kFALSE;
}


//_______________________________________________________________________
void  MethodPyRandomForest::Init()
{

}

void MethodPyRandomForest::Train()
{
}

//_______________________________________________________________________
void MethodPyRandomForest::DeclareOptions()
{
}

//_______________________________________________________________________
void MethodPyRandomForest::ProcessOptions()
{
}

//_______________________________________________________________________
void MethodPyRandomForest::TestClassification()
{
   MethodBase::TestClassification();
}


//_______________________________________________________________________
Double_t MethodPyRandomForest::GetMvaValue(Double_t *errLower, Double_t *errUpper)
{
   Double_t mvaValue;
   return mvaValue;
}

//_______________________________________________________________________
void MethodPyRandomForest::GetHelpMessage() const
{
// get help message text
//
// typical length of text line:
//         "|--------------------------------------------------------------|"
   Log() << Endl;
   Log() << gTools().Color("bold") << "--- Short description:" << gTools().Color("reset") << Endl;
   Log() << Endl;
   Log() << "Decision Trees and Rule-Based Models " << Endl;
   Log() << Endl;
   Log() << gTools().Color("bold") << "--- Performance optimisation:" << gTools().Color("reset") << Endl;
   Log() << Endl;
   Log() << Endl;
   Log() << gTools().Color("bold") << "--- Performance tuning via configuration options:" << gTools().Color("reset") << Endl;
   Log() << Endl;
   Log() << "<None>" << Endl;
}

