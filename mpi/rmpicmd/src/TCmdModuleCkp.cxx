// @(#)root/mpi / Author: Omar.Zapata@cern.ch 2017 http://oproject.org
#include <Mpi/TCmdModuleCkp.h>
using namespace ROOT::Mpi;

TCmdModuleCkp::TCmdModuleCkp(const Int_t argc, const Char_t **argv) : TCmdModuleBase("Ckp", argc, argv)
{
}

//______________________________________________________________________________
void TCmdModuleCkp::ParseOptions()
{
   for (auto i = 0; i < fArgc; i++) {
      TString arg = fArgv[i];
      arg.ReplaceAll(" ", "");
      if ((arg == "--ckp-clean")) {
         fOptions[arg] = "";
         continue;
      }
      if ((arg == "--ckp-jobid")) {
         fOptions[arg] = fArgv[i + 1];
         i++;
         continue;
      }
      if ((arg == "--ckp-jobname")) {
         fOptions[arg] = fArgv[i + 1];
         i++;
         continue;
      }
      if ((arg == "--ckp-prefix")) {
         fOptions[arg] = fArgv[i + 1];
         i++;
         continue;
      }
      if ((arg == "--ckp-debug")) {
         fOptions[arg] = fArgv[i + 1];
         i++;
         continue;
      }
      if ((arg == "--ckp-cachebase")) {
         fOptions[arg] = fArgv[i + 1];
         i++;
         continue;
      }
      if ((arg == "--ckp-interval")) {
         fOptions[arg] = fArgv[i + 1];
         i++;
         continue;
      }
      if ((arg == "--ckp-seconds")) {
         fOptions[arg] = fArgv[i + 1];
         i++;
         continue;
      }
      if ((arg == "--ckp-configfile")) {
         fOptions[arg] = fArgv[i + 1];
         i++;
         continue;
      }
   }
}
