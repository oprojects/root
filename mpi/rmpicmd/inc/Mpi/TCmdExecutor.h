// @(#)root/mpi / Author: Omar.Zapata@cern.ch 2017 http://oproject.org
#ifndef ROOT_Mpi_TCmdExecutor
#define ROOT_Mpi_TCmdExecutor

#include <Mpi/TCmdModuleROOT.h>
#include <Mpi/TCmdModulePython.h>
#include <Mpi/TCmdModuleMpiExec.h>
#include <Mpi/TCmdModuleValgrind.h>
#include <Mpi/TCmdModuleCkp.h>

namespace ROOT {
namespace Mpi {
/**
\class TCmdExecutor
Class to execute ROOT Mpi code from command line, parsing options from a set of plugins.
\ingroup Mpi
*/

class TCmdExecutor : TObject {
   const Int_t fArgc;
   const Char_t **fArgv;
   TCmdModuleROOT fModuleROOT;         // module to parse ROOT options
   TCmdModulePython fModulePython;     // module to parse Python options
   TCmdModuleMpiExec fModuleMpiExec;   // module to parse MpiExec options
   TCmdModuleValgrind fModuleValgrind; // module to parse Valgrind options
   TCmdModuleCkp fModuleCkp;           // module to parse CheckPoint options
public:
   TCmdExecutor(const Int_t argc, const Char_t **argv);
   void SetMpiExecOption(const TString opt, const TString value);
   void SetValgrindOption(const TString opt, const TString value);
   void SetPythonOption(const TString opt, const TString value);
   void SetExecFile(const TString file, Bool_t binary = kFALSE); // file to execute
   Int_t Execute();
   ClassDef(TCmdExecutor, 1)
};
}
}

#endif
