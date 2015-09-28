#include <algorithm>
#include <cstdlib>
#include <errno.h>

#include "TObjString.h"
#include "TMath.h"
#include "TString.h"
#include "TTree.h"
#include "TLeaf.h"
#include "TH1.h"
#include "TH2.h"
#include "TList.h"
#include "TSpline.h"
#include "TVector.h"
#include "TMatrixD.h"
#include "TMatrixDSymEigen.h"
#include "TVectorD.h"
#include "TTreeFormula.h"
#include "TXMLEngine.h"
#include "TROOT.h"
#include "TMatrixDSymEigen.h"
#include "TColor.h"
#include "TMVA/Config.h"


#ifndef ROOT_TMVA_Tools
#include "TMVA/Tools.h"
#endif
#ifndef ROOT_TMVA_ROCCurve
#include "TMVA/ROCCurve.h"
#endif
#ifndef ROOT_TMVA_Config
#include "TMVA/Config.h"
#endif
#ifndef ROOT_TMVA_Event
#include "TMVA/Event.h"
#endif
#ifndef ROOT_TMVA_Version
#include "TMVA/Version.h"
#endif
#ifndef ROOT_TMVA_PDF
#include "TMVA/PDF.h"
#endif
#ifndef ROOT_TMVA_MsgLogger
#include "TMVA/MsgLogger.h"
#endif

#include "TMVA/PDF.h"
#include "TMVA/TSpline1.h"
#include "TMVA/TSpline2.h"

using namespace std;


TMVA::ROCCurve::ROCCurve(std::vector<Float_t> mvas,std::vector<Float_t> mvab) :
   mvaS(mvas),
   mvaB(mvab),
   fLogger ( new TMVA::MsgLogger("ROCCurve") )
{
}

TMVA::ROCCurve::ROCCurve(std::vector<Float_t> mva,std::vector<Bool_t> mvat) :
   fLogger ( new TMVA::MsgLogger("ROCCurve") )
{
//   mvaS.resize();
//   mvaB.resize(mvaS.size());
  for(int i=0;i<mva.size();i++)
  {
    if(mvat[i]) mvaS.push_back(mva[i]);
    else mvaB.push_back(mva[i]);
  }
}



////////////////////////////////////////////////////////////////////////////////
/// destructor

TMVA::ROCCurve::~ROCCurve() {
   delete fLogger;
}

// # Fragment of auto-generated .py file for area under the ROC (auc) computation
// # Originally made for single test decision trees with etotal #of nodes 
// # esignal(ebackgnd)[i] contain the signal/background counts of events in the final signal/background decision leaves.
// 
//   etotal = 2 # Array index (# of Nodes -1)
//   SNR = numpy.zeros((20000, 1))
//   y_signal = []
//   y_background = []
//   for i in range(etotal+1):
//       if (esignal[i] or ebackgnd[i]):
//           SNR[i] = 1.0*esignal[i]/(esignal[i]+ebackgnd[i])
//           for j in range(esignal[i]):
//               y_signal.append(SNR[i])
//           for k in range(ebackgnd[i]):
//               y_background.append(SNR[i])
//   #print esignal[i], ebackgnd[i], etotal
//   ndivisions = 12.0
//   vec_epsilon_s = [0.0]
//   vec_epsilon_b = [0.0]
//   epsilon_s = 0.0
//   epsilon_b = 0.0
//   for i in numpy.arange(1.0/ndivisions, 1, 1.0/ndivisions):
//       acounter = 0.0
//       bcounter = 0.0
//       ccounter = 0.0
//       dcounter = 0.0
// 
//       for j in range(len(y_signal)):
//           if(y_signal[j] > i):
//               acounter += 1
//           else:
//               bcounter += 1
//       if(acounter != 0 or bcounter != 0):
//               epsilon_s = 1.0*bcounter/(acounter+bcounter)
//               #print acounter, bcounter
//       vec_epsilon_s.append(epsilon_s)
//       for n in range(len(y_background)):
//           if(y_background[n] > i):
//               ccounter += 1
//           else:
//               dcounter += 1
//       if(ccounter != 0 or dcounter != 0):
//           epsilon_b = 1.0*dcounter/(ccounter+dcounter)
//       vec_epsilon_b.append(epsilon_b)
// 
//   vec_epsilon_s.append(1.0)
//   vec_epsilon_b.append(1.0)
//   area = 0.0
//   for i in range(len(vec_epsilon_s)-1):
//       area += 0.5*(vec_epsilon_s[i+1]-vec_epsilon_s[i])*(vec_epsilon_b[i]+vec_epsilon_b[i+1])
      
Double_t TMVA::ROCCurve::GetROCIntegral(){
  
  Float_t integral=0;
  int ndivisions = 20;
  std::vector<Float_t> vec_epsilon_s(1);
  vec_epsilon_s.push_back(0);
  
  std::vector<Float_t>  vec_epsilon_b(1);
  vec_epsilon_b.push_back(0);
  
  Float_t epsilon_s = 0.0;
  Float_t epsilon_b = 0.0;

  for(Float_t i=-1.0;i<1.0;i+=(1.0/ndivisions))
  {
      Float_t acounter = 0.0;
      Float_t bcounter = 0.0;
      Float_t ccounter = 0.0;
      Float_t dcounter = 0.0;
      
      for(int j=0;j<mvaS.size();j++)
      {
        if(mvaS[j] > i) acounter++;
        else            bcounter++;
	
        if(mvaB[j] > i) ccounter++;
        else            dcounter++;
      }
      
      if(acounter != 0 || bcounter != 0)
      {
	epsilon_s = 1.0*bcounter/(acounter+bcounter);
      }
      vec_epsilon_s.push_back(epsilon_s);
      
      if(ccounter != 0 || dcounter != 0)
      {
	epsilon_b = 1.0*dcounter/(ccounter+dcounter);
      }
      vec_epsilon_b.push_back(epsilon_b);      
  }
  vec_epsilon_s.push_back(1.0);
  vec_epsilon_b.push_back(1.0);
  for(int i=0;i<vec_epsilon_s.size()-1;i++)
  {
      integral += 0.5*(vec_epsilon_s[i+1]-vec_epsilon_s[i])*(vec_epsilon_b[i]+vec_epsilon_b[i+1]);
  }
   return integral;
}







