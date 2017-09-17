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
      if (arg.Contains("=")) {
         auto opt = ParseEqual(arg);
         auto key = opt.first;
         auto value = opt.second;

      if ((key == "--ckp-jobid")) {
         fOptions[key] = value;
         continue;
      }
      if ((key == "--ckp-jobname")) {
         fOptions[key] = value;
         continue;
      }
      if ((key == "--ckp-prefix")) {
         fOptions[key] = value;
         continue;
      }
      if ((key == "--ckp-debug")) {
         fOptions[key] = value;
         continue;
      }
      if ((key == "--ckp-cachebase")) {
         fOptions[key] = value;
         continue;
      }
      if ((key == "--ckp-interval")) {
         fOptions[key] = value;
         continue;
      }
      if ((key == "--ckp-seconds")) {
         fOptions[key] = value;
         continue;
      }
      if ((key == "--ckp-configfile")) {
         fOptions[key] = value;
         continue;
      }
      }
   }
}

//______________________________________________________________________________
TString TCmdModuleCkp::GetOptionsString()
{
   TString optstr;
   for (auto &opt : fOptions) {
      if (opt.first == "--ckp-clean")
         optstr += Form(" %s ", opt.first.Data());
      else
         optstr += Form(" %s=%s ", opt.first.Data(), opt.second.Data());
   }
   return optstr;
}
