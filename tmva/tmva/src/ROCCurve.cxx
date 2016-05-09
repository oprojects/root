// @(#)root/tmva $Id$   
// Author: Omar Zapata, Lorenzo Moneta, Sergei Gleyzer

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : ROCCurve                                                              *
 *                                                                                *
 * Description:                                                                   *
 *      This is class to compute ROC Integral (AUC)                               *
 *                                                                                *
 * Authors :                                                                      *
 *      Omar Zapata     <Omar.Zapata@cern.ch>    - UdeA/ITM Colombia              *
 *      Lorenzo Moneta  <Lorenzo.Moneta@cern.ch> - CERN, Switzerland              *
 *      Sergei Gleyzer  <Sergei.Gleyzer@cern.ch> - U of Florida & CERN            *
 *                                                                                *
 * Copyright (c) 2015:                                                            *
 *      CERN, Switzerland                                                         *
 *      UdeA/ITM, Colombia                                                        *
 *      U. of Florida, USA                                                        *
 **********************************************************************************/

#ifndef ROOT_TMVA_Tools
#include "TMVA/Tools.h"
#endif
#ifndef ROOT_TMVA_ROCCurve
#include "TMVA/ROCCurve.h"
#endif
#ifndef ROOT_TMVA_Config
#include "TMVA/Config.h"
#endif
#ifndef ROOT_TMVA_Version
#include "TMVA/Version.h"
#endif
#ifndef ROOT_TMVA_MsgLogger
#include "TMVA/MsgLogger.h"
#endif
#ifndef ROOT_TGraph
#include "TGraph.h"
#endif

#include<vector>
#include <cassert>

using namespace std;


TMVA::ROCCurve::ROCCurve(const std::vector<Float_t> & mva, const std::vector<Bool_t> & mvat) :
   fLogger ( new TMVA::MsgLogger("ROCCurve") ),fGraph(NULL)
{
   assert(mva.size() == mvat.size() );
   for(UInt_t i=0;i<mva.size();i++)
   {
      if(mvat[i] ) fMvaS.push_back(mva[i]);
      else fMvaB.push_back(mva[i]);
   }
   EpsilonCount();//call base code to get ROC
}



////////////////////////////////////////////////////////////////////////////////
/// destructor

TMVA::ROCCurve::~ROCCurve() {
   delete fLogger;
   if(fGraph) delete fGraph;
}
      
////////////////////////////////////////////////////////////////////////////////
/// ROC Integral (AUC)

Double_t TMVA::ROCCurve::GetROCIntegral(){
  
  Float_t integral=0;
  for(UInt_t i=0;i<fEpsilonSig.size()-1;i++)
  {
      integral += 0.5*(fEpsilonSig[i+1]-fEpsilonSig[i])*(fEpsilonBgk[i]+fEpsilonBgk[i+1]);
  }
   return integral;
}

void TMVA::ROCCurve::EpsilonCount()
{
  int ndivisions = 40;
  fEpsilonSig.push_back(0);
  fEpsilonBgk.push_back(0);

  Float_t epsilon_s = 0.0;
  Float_t epsilon_b = 0.0;

  for(Float_t i=-1.0;i<1.0;i+=(1.0/ndivisions))
  {
      Float_t acounter = 0.0;
      Float_t bcounter = 0.0;
      Float_t ccounter = 0.0;
      Float_t dcounter = 0.0;
      
      for(UInt_t j=0;j<fMvaS.size();j++)
      {
        if(fMvaS[j] > i) acounter++;
        else            bcounter++;
	
        if(fMvaB[j] > i) ccounter++;
        else            dcounter++;
      }
      
      if(acounter != 0 || bcounter != 0)
      {
	epsilon_s = 1.0*bcounter/(acounter+bcounter);
      }
      fEpsilonSig.push_back(epsilon_s);
      
      if(ccounter != 0 || dcounter != 0)
      {
	epsilon_b = 1.0*dcounter/(ccounter+dcounter);
      }
      fEpsilonBgk.push_back(epsilon_b);
  }
  fEpsilonSig.push_back(1.0);
  fEpsilonBgk.push_back(1.0);
}

TGraph* TMVA::ROCCurve::GetROCCurve()
{
 if(!fGraph)    fGraph=new TGraph(fEpsilonSig.size(),&fEpsilonSig[0],&fEpsilonBgk[0]);
 return fGraph;
}




