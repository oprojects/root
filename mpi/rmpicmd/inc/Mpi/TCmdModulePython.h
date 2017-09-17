// @(#)root/mpi / Author: Omar.Zapata@cern.ch 2017 http://oproject.org
#ifndef ROOT_Mpi_TCmdModulePython
#define ROOT_Mpi_TCmdModulePython

#include <Mpi/TCmdModuleBase.h>

namespace ROOT {
namespace Mpi {
/**
\class TCmdModulePython
Class to handle Python options from command line or environment variables.
\ingroup Mpi
*/

class TCmdModulePython : public TCmdModuleBase {
   TString fFileName;

public:
   TCmdModulePython(const Int_t argc, const Char_t **argv);
   TString GetFileName() { return fFileName; }
   void SetFileName(const TString file) { fFileName = file; }
   virtual void ParseOptions();
   ClassDef(TCmdModulePython, 0)
};
}
}

#endif
