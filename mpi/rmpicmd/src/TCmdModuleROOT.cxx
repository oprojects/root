// @(#)root/mpi / Author: Omar.Zapata@cern.ch 2017 http://oproject.org
#include <Mpi/TCmdModuleROOT.h>
using namespace ROOT::Mpi;
TCmdModuleROOT::TCmdModuleROOT(const Int_t argc, const Char_t **argv)
   : TCmdModuleBase("ROOT", argc, argv), fFileName("")
{
   fExecPath = Form("%s/bin/root.exe", TROOT::GetRootSys().Data());
}

//______________________________________________________________________________
void TCmdModuleROOT::ParseOptions()
{
   for (auto i = 0; i < fArgc; i++) {
      TString arg = fArgv[i];
      arg.ReplaceAll(" ", "");
      if ((arg == "-b") || (arg == "-n") || (arg == "-l") || (arg == "-q") || (arg == "-x") || (arg == "-memstat"))
         fOptions[arg] = "";
      if (arg.Contains(".C") || arg.Contains(".cxx") || arg.Contains(".c++") || arg.Contains(".cpp"))
         fFileName = Form("\"%s\"", arg.Data());
   }
}
