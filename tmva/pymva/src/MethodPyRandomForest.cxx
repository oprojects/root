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

// Convert the file name to a Python string.
PyObject *pName= PyString_FromString("sklearn.ensemble");
// Import the file as a Python module.
fModuleSklearn = PyImport_Import(pName);
Py_DECREF(pName);

if(!fModuleSklearn)
{
   Log() <<kFATAL<< "Can't import sklearn.ensemble" << Endl;
   Log() << Endl;
}

PyObject *pDict = PyModule_GetDict(fModuleSklearn);
PyObject *fClassifierClass = PyDict_GetItemString(pDict, "RandomForestClassifier");

// Create an instance of the class
if (PyCallable_Check(fClassifierClass ))
    {
        //instance
        fClassifier = PyObject_CallObject(fClassifierClass , NULL);//not arguments at the moment 
    }else{
        PyErr_Print();
        Py_DECREF(pDict);
        Py_DECREF(fClassifierClass);
        Log() <<kFATAL<< "Can't call function RandomForestClassifier" << Endl;
        Log() << Endl;
        
    }

//Training data
UInt_t fNvars=Data()->GetNVariables();
UInt_t fNrowsTraining=Data()->GetNTrainingEvents();//every row is an event, a class type and a weight

fTrainDataClasses=PyTuple_New(fNrowsTraining);
fTrainData=PyTuple_New(fNrowsTraining);
fTrainDataWeights=PyTuple_New(fNrowsTraining);

for(int i=0;i<fNrowsTraining;i++)
{
    PyObject *TrainDataRow=PyTuple_New(fNvars);
    const TMVA::Event *e=Data()->GetTrainingEvent(i);
    for(int j=0;j<fNvars;j++)
    {
        PyObject *pValue=PyFloat_FromDouble(e->GetValue(j));
        if (!pValue)
        {
           Py_DECREF(fTrainDataClasses);
           Py_DECREF(fTrainData);
           Log()<<kFATAL<<"Error passing Training Data to Tuples(Variables)"<<Endl;
        }
        
        PyTuple_SetItem(TrainDataRow, j,pValue);
    }
    PyObject *pClassValue;
    if(e->GetClass()==TMVA::Types::kSignal) pClassValue=PyInt_FromLong(TMVA::Types::kSignal);
    else pClassValue=PyInt_FromLong(TMVA::Types::kBackground);
    if (!pClassValue)
    {
       Py_DECREF(fTrainDataClasses);
       Py_DECREF(fTrainData);
       Log()<<kFATAL<<"Error passing Training Data to Tuples(Class Type)"<<Endl;
    }
    PyObject *pWeightValue=PyFloat_FromDouble(e->GetWeight());
    if (!pWeightValue)
    {
       Py_DECREF(fTrainDataClasses);
       Py_DECREF(fTrainData);
       Log()<<kFATAL<<"Error passing Training Data to Tuples(Weight)"<<Endl;
    }
        
    PyTuple_SetItem(fTrainData, i,TrainDataRow);
    PyTuple_SetItem(fTrainDataClasses, i,pClassValue);
    PyTuple_SetItem(fTrainDataWeights, i,pWeightValue);
}

// PyObject_Print(TraindDataArray, stdout, 0);

//  PyObject_Print(item, stdout, 0);
}

void MethodPyRandomForest::Train()
{
    //https://docs.python.org/2/c-api/object.html
//     PyObject_CallMethodObjArgs(PyObject *o, PyObject *name, ..., NULL)

//     PyObject_Print(fClassifier, stdout, 0);
//     PyObject_Print(fTrainData, stdout, 0);
//     PyObject_Print(fTrainDataClasses, stdout, 0);
//     PyObject_Print(fTrainDataClasses, stdout, 0);
    
    PyObject *pValue=PyObject_CallMethod(fClassifier,"fit","(OOO)", fTrainData,fTrainDataClasses,fTrainDataWeights);
    PyObject_Print(pValue, stdout, 0);
//     pValue =PyObject_CallObject(fClassifier, PyString_FromString("classes_"));
//     PyObject_Print(pValue, stdout, 0);
     
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
   const TMVA::Event *e=Data()->GetEvent();
   UInt_t nvars=e->GetNVariables();
   PyObject *pEvent=PyTuple_New(nvars);
   for(int i=0;i<nvars;i++){
       
       PyObject *pValue=PyFloat_FromDouble(e->GetValue(i));
       if (!pValue)
        {
            Py_DECREF(pEvent);
            Py_DECREF(fTrainData);
            Log()<<kFATAL<<"Error Evaluating MVA "<<Endl;
        }
       PyTuple_SetItem(pEvent, i,pValue);
   }
   
   //optimisation require here, predict_proba must be class's attribute
   PyObject *result=PyObject_CallMethodObjArgs(fClassifier,PyString_FromString("predict_proba"),pEvent,NULL);

   PyObject_Print(result, stdout, 0);
   std::cout<<std::endl;
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

