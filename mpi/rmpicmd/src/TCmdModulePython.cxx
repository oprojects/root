// @(#)root/mpi / Author: Omar.Zapata@cern.ch 2017 http://oproject.org
#include <Mpi/TCmdModulePython.h>
using namespace ROOT::Mpi;

TCmdModulePython::TCmdModulePython(const Int_t argc, const Char_t **argv)
   : TCmdModuleBase("Python", argc, argv), fFileName("")
{
#if defined(PYTHONINTERP_FOUND)
   fExecPath = PYTHON_EXECUTABLE;
#endif
}

//______________________________________________________________________________
void TCmdModulePython::ParseOptions()
{
   for (auto i = 0; i < fArgc; i++) {
      TString arg = fArgv[i];
      arg.ReplaceAll(" ", "");
      if ((arg == "-B") || (arg == "-d") || (arg == "-E") || (arg == "-i") || (arg == "-O") || (arg == "-O0") ||
          (arg == "-R") || (arg == "-s") || (arg == "-S") || (arg == "-t") || (arg == "-u") || (arg == "-v"))
         fOptions[arg] = "";
      if (arg.EndsWith(".py"))
         fFileName = arg;
   }
}
