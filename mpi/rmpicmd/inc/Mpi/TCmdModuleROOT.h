// @(#)root/mpi / Author: Omar.Zapata@cern.ch 2017 http://oproject.org
#ifndef ROOT_Mpi_TCmdModuleROOT
#define ROOT_Mpi_TCmdModuleROOT

#include <Mpi/TCmdModuleBase.h>

namespace ROOT {
namespace Mpi {
/**
\class TCmdModuleROOT
Class to handle ROOT options from command line or environment variables.
\ingroup Mpi
*/

class TCmdModuleROOT : public TCmdModuleBase {
   TString fFileName;

public:
   TCmdModuleROOT(const Int_t argc, const Char_t **argv);
   TString GetFileName() { return fFileName; }
   void SetFileName(const TString file) { fFileName = file; }
   virtual void ParseOptions();
   ClassDef(TCmdModuleROOT, 0)
};
}
}

#endif
