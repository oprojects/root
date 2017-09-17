// @(#)root/mpi / Author: Omar.Zapata@cern.ch 2017 http://oproject.org
#ifndef ROOT_Mpi_TCmdModuleValgrind
#define ROOT_Mpi_TCmdModuleValgrind

#include <Mpi/TCmdModuleBase.h>

namespace ROOT {
namespace Mpi {
/**
\class TCmdModuleValgrind
Class to handle valgrind options from command line or environment variables.
\ingroup Mpi
*/

class TCmdModuleValgrind : public TCmdModuleBase {
   TString fROOTSupFile; // ROOT Suppression file for valgrind
public:
   TCmdModuleValgrind(const Int_t argc, const Char_t **argv);
   virtual void ParseOptions();
   virtual TString GetOptionsString();
   ClassDef(TCmdModuleValgrind, 0)
};
}
}

#endif
