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
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include <iomanip>
#include <fstream>

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
#include "TMVA/Config.h"
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
   PyMethodBase(jobName, Types::kPyRandomForest, methodTitle, dsi, theOption, theTargetDir),
   n_estimators(10),
   criterion("gini"),
   max_features("auto"),
   max_depth(0),
   min_samples_leaf(1),
   min_weight_fraction_leaf(0),
   bootstrap(kTRUE),
   n_jobs(1)   
{
   // standard constructor for the PyRandomForest
 SetWeightFileDir( gConfig().GetIONames().fWeightFileDir );

}

//_______________________________________________________________________
MethodPyRandomForest::MethodPyRandomForest(DataSetInfo &theData, const TString &theWeightFile, TDirectory *theTargetDir)
   : PyMethodBase(Types::kPyRandomForest, theData, theWeightFile, theTargetDir),
   n_estimators(10),
   criterion("gini"),
   max_features("auto"),
   max_depth(0),
   min_samples_leaf(1),
   min_weight_fraction_leaf(0),
   bootstrap(kTRUE),
   n_jobs(1)   
{
     SetWeightFileDir( gConfig().GetIONames().fWeightFileDir );
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
void MethodPyRandomForest::DeclareOptions()
{
    MethodBase::DeclareCompatibilityOptions();

    DeclareOptionRef(n_estimators, "NEstimators", "Integer, optional (default=10). The number of trees in the forest.");
    DeclareOptionRef(criterion, "Criterion", "//string, optional (default='gini') \
    The function to measure the quality of a split. Supported criteria are \
    'gini' for the Gini impurity and 'entropy' for the information gain. \
    Note: this parameter is tree-specific.");
    DeclareOptionRef(max_depth, "MaxDepth", "nteger, optional (default=2) \
    The minimum number of samples required to split an internal node. \
    Note: this parameter is tree-specific.");
    DeclareOptionRef(min_samples_leaf, "MinSamplesLeaf", "integer, optional (default=1) \
    The minimum number of samples in newly created leaves.  A split is \
    discarded if after the split, one of the leaves would contain less then \
    ``min_samples_leaf`` samples.");
    DeclareOptionRef(min_weight_fraction_leaf, "MinWeightFractionLeaf", "//float, optional (default=0.) \
    The minimum weighted fraction of the input samples required to be at a \
    leaf node.");
    DeclareOptionRef(bootstrap, "Bootstrap", "boolean, optional (default=True) \
    Whether bootstrap samples are used when building trees.");
    DeclareOptionRef(n_jobs, "NJobs", " integer, optional (default=1) \
    The number of jobs to run in parallel for both `fit` and `predict`. \
    If -1, then the number of jobs is set to the number of cores.");
}

//_______________________________________________________________________
void MethodPyRandomForest::ProcessOptions()
{
    if (n_estimators <= 0) {
        Log() << kERROR << " NEstimators <=0... that does not work !! "
        << " I set it to 10 .. just so that the program does not crash"
        << Endl;
        n_estimators = 10;
    }
    //TODO: Error control for variables here  
}


//_______________________________________________________________________
void  MethodPyRandomForest::Init()
{
//         Log() << kERROR <<"INIT =" <<n_jobs<<Endl;
    ProcessOptions();
    _import_array();//require to use numpy arrays
    
    //Import sklearn
    // Convert the file name to a Python string.
    PyObject *pName= PyString_FromString("sklearn.ensemble");
    // Import the file as a Python module.
    fModule= PyImport_Import(pName);
    Py_DECREF(pName);
    
    if(!fModule)
    {
        Log() <<kFATAL<< "Can't import sklearn.ensemble" << Endl;
        Log() << Endl;
    }    
    
    
    //Training data
    UInt_t fNvars=Data()->GetNVariables();
    int fNrowsTraining=Data()->GetNTrainingEvents();//every row is an event, a class type and a weight
    int *dims=new int[2];
    dims[0]=fNrowsTraining;
    dims[1]=fNvars;
    fTrainData=(PyArrayObject*)PyArray_FromDims(2, dims, NPY_FLOAT);
    float* TrainData=(float*)(PyArray_DATA(fTrainData));
    
    
    fTrainDataClasses=(PyArrayObject*)PyArray_FromDims(1,&fNrowsTraining, NPY_FLOAT);
    float* TrainDataClasses=(float*)(PyArray_DATA(fTrainDataClasses));
    
    fTrainDataWeights=(PyArrayObject*)PyArray_FromDims(1,&fNrowsTraining, NPY_FLOAT);
    float* TrainDataWeights=(float*)(PyArray_DATA(fTrainDataWeights));
    
    for(int i=0;i<fNrowsTraining;i++)
    {
        const TMVA::Event *e=Data()->GetTrainingEvent(i);
        for(UInt_t j=0;j<fNvars;j++)
        {
            TrainData[j+i*fNvars]=e->GetValue(j);
        }
        if(e->GetClass()==TMVA::Types::kSignal) TrainDataClasses[i]=TMVA::Types::kSignal;
        else TrainDataClasses[i]=TMVA::Types::kBackground;
        
        TrainDataWeights[i]=e->GetWeight();
    }
}
// n_estimators=10, criterion='gini', max_depth=None, min_samples_split=2, min_samples_leaf=1, min_weight_fraction_leaf=0.0,
// max_features='auto', max_leaf_nodes=None, bootstrap=True, oob_score=False, n_jobs=1, random_state=None, verbose=0, warm_start=False, 
// class_weight=None
void MethodPyRandomForest::Train()
{
//     (isiiifsiiiiiiis)
    //NOTE: max_features must have 3 defferents variables int, float and string 
    //search a solution with PyObject
    PyObject* pomax_depth;
    if(max_depth<=0) pomax_depth=Py_None;
    else pomax_depth=PyInt_FromLong(max_depth);
   PyObject *args = Py_BuildValue("(isOiifsOiii)",n_estimators,criterion.Data(),pomax_depth,2, \
                                  min_samples_leaf,min_weight_fraction_leaf,max_features.Data(),Py_None,\
                                  bootstrap,kFALSE,n_jobs);//,NULL,0,kFALSE,kFALSE,NULL);
   Py_DECREF(pomax_depth);
   PyObject_Print(args,stdout,0);
   std::cout<<std::endl;
    
    PyObject *pDict = PyModule_GetDict(fModule);
    PyObject *fClassifierClass = PyDict_GetItemString(pDict, "RandomForestClassifier");
//    Log() << kFATAL <<"Train =" <<n_jobs<<Endl;
    
    // Create an instance of the class
    if (PyCallable_Check(fClassifierClass ))
    {
        //instance        
        fClassifier = PyObject_CallObject(fClassifierClass ,args);
        PyObject_Print(fClassifier, stdout, 0);
        
        Py_DECREF(args); 
    }else{
        PyErr_Print();
        Py_DECREF(pDict);
        Py_DECREF(fClassifierClass);
        Log() <<kFATAL<< "Can't call function RandomForestClassifier" << Endl;
        Log() << Endl;
        
    }   
    
    fClassifier=PyObject_CallMethod(fClassifier,(char*)"fit",(char*)"(OOO)", fTrainData,fTrainDataClasses,fTrainDataWeights);
    //     PyObject_Print(fClassifier, stdout, 0);
    //     pValue =PyObject_CallObject(fClassifier, PyString_FromString("classes_"));
    //     PyObject_Print(pValue, stdout, 0);
    
    TString path=GetWeightFileDir()+"/PyRFModel.PyData";
    Log() << Endl;
    Log() << gTools().Color("bold") << "--- Saving State File In:" << gTools().Color("reset")<<path<< Endl;
    Log() << Endl;
    
    PyObject *model_arg = Py_BuildValue("(O)",fClassifier);
    PyObject *model_data = PyObject_CallObject(fPickleDumps ,model_arg);
    std::ofstream PyData;
    PyData.open(path.Data());
    PyData<<PyString_AsString(model_data);
    PyData.close();
    Py_DECREF(model_arg); 
    Py_DECREF(model_data); 
}

//_______________________________________________________________________
void MethodPyRandomForest::TestClassification()
{
    MethodBase::TestClassification();
}


//_______________________________________________________________________
Double_t MethodPyRandomForest::GetMvaValue(Double_t *errLower, Double_t *errUpper)
{
    // cannot determine error
    NoErrorCalc(errLower, errUpper);

    if(!fClassifier) ReadStateFromFile();
        
    Double_t mvaValue;
    const TMVA::Event *e=Data()->GetEvent();
    UInt_t nvars=e->GetNVariables();
    PyObject *pEvent=PyTuple_New(nvars);
    for(UInt_t i=0;i<nvars;i++){
        
        PyObject *pValue=PyFloat_FromDouble(e->GetValue(i));
        if (!pValue)
        {
            Py_DECREF(pEvent);
            Py_DECREF(fTrainData);
            Log()<<kFATAL<<"Error Evaluating MVA "<<Endl;
        }
        PyTuple_SetItem(pEvent, i,pValue);
    }
    PyArrayObject *result=(PyArrayObject*)PyObject_CallMethod(fClassifier,"predict_proba","(O)",pEvent);
    double* proba=(double*)(PyArray_DATA(result));
    mvaValue=proba[1];//getting signal prob
    Py_DECREF(result);
    Py_DECREF(pEvent);
    return mvaValue;
}

//_______________________________________________________________________
void MethodPyRandomForest::ReadStateFromFile()
{
  if(!PyIsInitialized())
  {
    PyInitialize();
  }
  
  TString path=GetWeightFileDir()+"/PyRFModel.PyData";
  Log() << Endl;
  Log() << gTools().Color("bold") << "--- Loading State File From:" << gTools().Color("reset")<<path<< Endl;
  Log() << Endl;
  std::ifstream PyData;
  std::stringstream PyDataStream;
  std::string PyDataString;
    
  PyData.open(path.Data());
  PyDataStream<<PyData.rdbuf();
  PyDataString=PyDataStream.str();
  PyData.close();
  
//   std::cout<<"-----------------------------------\n";
//   std::cout<<PyDataString.c_str();
//   std::cout<<"-----------------------------------\n";
  PyObject *model_arg = Py_BuildValue("(s)",PyDataString.c_str());
  fClassifier = PyObject_CallObject(fPickleLoads ,model_arg);

  
  Py_DECREF(model_arg);   
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

