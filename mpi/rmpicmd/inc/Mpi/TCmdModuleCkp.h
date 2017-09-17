// @(#)root/mpi / Author: Omar.Zapata@cern.ch 2017 http://oproject.org
#ifndef ROOT_Mpi_TCmdModuleCkp
#define ROOT_Mpi_TCmdModuleCkp

#include <Mpi/TCmdModuleBase.h>

namespace ROOT {
namespace Mpi {
/**
\class TCmdModuleCkp
Class to handle Ckp options from command line or environment variables.
\ingroup Mpi
*/

class TCmdModuleCkp : public TCmdModuleBase {
public:
   TCmdModuleCkp(const Int_t argc, const Char_t **argv);
   virtual void ParseOptions();
   virtual TString GetOptionsString();
   ClassDef(TCmdModuleCkp, 0)
};
}
}

#endif
