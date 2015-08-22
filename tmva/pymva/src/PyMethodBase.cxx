// @(#)root/tmva $Id$
// Author: Omar Zapata 2015

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : PyMethodBase                                                          *
 *                                                                                *
 * Description:                                                                   *
 *      Virtual base class for all MVA method based on python                     *
 *                                                                                *
 **********************************************************************************/

#include<TMVA/PyMethodBase.h>
#include<TApplication.h>
using namespace TMVA;

ClassImp(PyMethodBase)
static Bool_t PyInitializeStatus=kFALSE;

//_______________________________________________________________________
PyMethodBase::PyMethodBase(const TString &jobName,
                         Types::EMVA methodType,
                         const TString &methodTitle,
                         DataSetInfo &dsi,
                         const TString &theOption ,
                         TDirectory *theBaseDir): MethodBase(jobName, methodType, methodTitle, dsi, theOption, theBaseDir)
{
  if(!PyInitializeStatus)
  {
    Py_Initialize();
    PyInitializeStatus=kTRUE;
  }
}

//_______________________________________________________________________
PyMethodBase::PyMethodBase(Types::EMVA methodType,
                         DataSetInfo &dsi,
                         const TString &weightFile,
                         TDirectory *theBaseDir): MethodBase(methodType, dsi, weightFile, theBaseDir)
{
  if(!PyInitializeStatus)
  {
    Py_Initialize();
    PyInitializeStatus=kTRUE;
  }
}
