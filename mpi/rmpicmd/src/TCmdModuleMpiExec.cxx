// @(#)root/mpi / Author: Omar.Zapata@cern.ch 2017 http://oproject.org
#include <Mpi/TCmdModuleMpiExec.h>
using namespace ROOT::Mpi;
TCmdModuleMpiExec::TCmdModuleMpiExec(const Int_t argc, const Char_t **argv) : TCmdModuleBase("MpiExec", argc, argv)
{
#if !defined(ROOT_MPI_EXEC)
#error ROOT_MPI_EXEC not defined, check your ROOT compilation
#endif
   fExecPath = gSystem->Getenv("ROOT_MPIEXEC");
   if (fExecPath.IsNull()) {
      if (!gSystem->AccessPathName(ROOT_MPI_EXEC)) {
         fExecPath = ROOT_MPI_EXEC;
      } else {
         Error(Form("%s(...) %s[%d]", __FUNCTION__, __FILE__, __LINE__),
               "\nFinding mpiexec.. You can provide path to mpiexec with environment variable ROOT_MPIEXEC\n");
         gSystem->Exit(1);
      }
   }
}

//______________________________________________________________________________
void TCmdModuleMpiExec::ParseOptions()
{
   for (auto i = 0; i < fArgc; i++) {
      TString arg = fArgv[i];
      arg.ReplaceAll(" ", "");
      if (arg == "-np") {
         fOptions[arg] = fArgv[i + 1];
         i++;
      }
      if ((arg == "-machinefile") || (arg == "-hostfile") || (arg == "--machinefile") || (arg == "--hostfile")) {
         fOptions[arg] = fArgv[i + 1];
         i++;
      }
      if ((arg == "-rf") || (arg == "---rankfile")) {
         fOptions[arg] = fArgv[i + 1];
         i++;
      }
   }
}
