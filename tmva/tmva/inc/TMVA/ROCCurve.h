#ifndef ROOT_TMVA_ROCCurve
#define ROOT_TMVA_ROCCurve
#include <vector>
#include <sstream>
#include <iostream>
#include <iomanip>

class TList;
class TTree;
class TString;
class TH1;
class TH2;
class TH2F;
class TSpline;
class TSpline1;

namespace TMVA {

  class MsgLogger;


  class ROCCurve {
    
  public:
    ROCCurve( std::vector<Float_t> mvaS, std::vector<Float_t> mvaB);
    ROCCurve( std::vector<Float_t> mvaS,std::vector<Bool_t> mvat);
    
    ~ROCCurve();
    

    Double_t GetROCIntegral();
    
  private:
    std::vector<Float_t> mvaS;
    std::vector<Float_t> mvaB;

    mutable MsgLogger* fLogger;   //! message logger
    MsgLogger& Log() const { return *fLogger; }                       

  };
}
#endif
