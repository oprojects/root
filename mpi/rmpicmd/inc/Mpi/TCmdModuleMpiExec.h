// @(#)root/mpi / Author: Omar.Zapata@cern.ch 2017 http://oproject.org
#ifndef ROOT_Mpi_TCmdModuleMpiExec
#define ROOT_Mpi_TCmdModuleMpiExec

#include <Mpi/TCmdModuleBase.h>

namespace ROOT {
namespace Mpi {
/**
\class TCmdModuleMpiExec
Class to handle mpiexec options from command line or environment variables.
\ingroup Mpi
*/

class TCmdModuleMpiExec : public TCmdModuleBase {
public:
   TCmdModuleMpiExec(const Int_t argc, const Char_t **argv);
   virtual void ParseOptions();
   ClassDef(TCmdModuleMpiExec, 0)
};
}
}

#endif
