// @(#)root/mpi / Author: Omar.Zapata@cern.ch 2017 http://oproject.org
#include <Mpi/TCmdModuleBase.h>
#include <TObjString.h>
#include <TObjArray.h>
using namespace ROOT::Mpi;

//______________________________________________________________________________
TCmdModuleBase::TCmdModuleBase(TString name, const Int_t argc, const Char_t **argv)
   : fName(name), fArgc(argc), fArgv(argv)
{
}

//______________________________________________________________________________
std::pair<TString, TString> TCmdModuleBase::ParseEqual(TString opt)
{
   if (!opt.Contains("=")) {
      Error(Form("%s(...) %s[%d]", __FUNCTION__, __FILE__, __LINE__),
            "\nParsing option %s,dont have equal(=) simbol to parse.", opt.Data());
   }
   auto data = opt.Tokenize("=");
   TString key = static_cast<TObjString *>(data->At(0))->CopyString();
   TString value = static_cast<TObjString *>(data->At(1))->CopyString();
   return std::make_pair(key, value);
}
