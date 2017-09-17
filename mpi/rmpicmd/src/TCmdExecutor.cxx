#include <Mpi/TCmdExecutor.h>

using namespace ROOT::Mpi;
//______________________________________________________________________________
TCmdExecutor::TCmdExecutor(const Int_t argc, const Char_t **argv)
   : fArgc(argc), fArgv(argv), fModuleROOT(argc, argv), fModulePython(argc, argv), fModuleMpiExec(argc, argv),
     fModuleValgrind(argc, argv)
{
// export TMPDIR for OpenMPI at mac is required
// https://www.open-mpi.org/faq/?category=osx#startup-errors-with-open-mpi-2.0.x
#if defined(R__MACOSX) && defined(OPEN_MPI)
   gSystem->Setenv("TMPDIR", "/tmp");
#endif

   fModuleROOT.ParseOptions();
   fModulePython.ParseOptions();
   fModuleMpiExec.ParseOptions();
   fModuleValgrind.ParseOptions();
}

//______________________________________________________________________________
Int_t TCmdExecutor::Execute()
{
   TString cmd = Form("%s %s ", fModuleMpiExec.GetExecPath().Data(), fModuleMpiExec.GetOptionsString().Data());

   if (fModuleValgrind.IsRequire()) {
      if (fModuleValgrind.IsFound()) {
         cmd += Form("%s %s ", fModuleValgrind.GetExecPath().Data(), fModuleValgrind.GetOptionsString().Data());
      } else {
         // error handling here
      }
   }

   if (!fModuleROOT.GetFileName().IsNull()) {
      cmd += Form("%s -l -q -b %s %s", fModuleROOT.GetExecPath().Data(), fModuleROOT.GetOptionsString().Data(),
                  fModuleROOT.GetFileName().Data());
   }
   if (!fModulePython.GetFileName().IsNull()) {
      cmd += Form("%s %s %s", fModulePython.GetExecPath().Data(), fModulePython.GetOptionsString().Data(),
                  fModulePython.GetFileName().Data());
   }
   if (gSystem->Getenv("ROOT_MPI_VERBOSE") != NULL)
      printf("\nEXECUTING %s\n", cmd.Data());
   return gSystem->Exec(cmd.Data());
}

//______________________________________________________________________________
void ROOT::Mpi::TCmdExecutor::SetMpiExecOption(const TString opt, const TString value)
{
   fModuleMpiExec.SetOption(opt, value);
}

//______________________________________________________________________________
void TCmdExecutor::SetValgrindOption(const TString opt, const TString value)
{
   fModuleValgrind.SetOption(opt, value);
}

//______________________________________________________________________________
void TCmdExecutor::SetPythonOption(const TString opt, const TString value)
{
   fModulePython.SetOption(opt, value);
}

//______________________________________________________________________________
void TCmdExecutor::SetExecFile(const TString file, Bool_t /*binary*/)
{
   if (file.Contains(".py"))
      fModulePython.SetFileName(file);
   else
      fModuleROOT.SetFileName(file);
}
