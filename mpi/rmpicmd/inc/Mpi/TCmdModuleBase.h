// @(#)root/mpi / Author: Omar.Zapata@cern.ch 2017 http://oproject.org
#ifndef ROOT_Mpi_TCmdModuleBase
#define ROOT_Mpi_TCmdModuleBase
#include <Rtypes.h>
#include <TSystem.h>
#include <TString.h>
#include <TROOT.h>

#include <map>
#include <iostream>

#include <Mpi/TMpiExport.h>

namespace ROOT {
namespace Mpi {
class TCmdModuleBase : public TObject {
protected:
   TString fName;
   std::map<TString, TString> fOptions;
   const Int_t fArgc;
   const Char_t **fArgv;
   TString fExecPath; // Path to required executable ex: python, root, valgrind etc..

   TCmdModuleBase(TString name, const Int_t argc, const Char_t **argv);
   std::pair<TString, TString> ParseEqual(TString opt);

public:
   //______________________________________________________________________________
   virtual Bool_t IsRequire() { return !fOptions.empty(); }

   //______________________________________________________________________________
   virtual Bool_t IsFound() { return !fExecPath.IsNull(); }

   //______________________________________________________________________________
   TString GetExecPath() { return fExecPath; }

   //______________________________________________________________________________
   // method to parse options fron environment and command line arguments
   virtual void ParseOptions() = 0;

   //______________________________________________________________________________
   virtual void SetOption(TString var, TString value) { fOptions[var] = value; }

   //______________________________________________________________________________
   virtual std::vector<TString> GetKeys()
   {
      std::vector<TString> keys;
      for (auto &opt : fOptions)
         keys.push_back(opt.first);
      return keys;
   }

   //______________________________________________________________________________
   virtual TString GetValue(TString key) { return fOptions[key]; }

   //______________________________________________________________________________
   virtual TString GetOptionsString()
   {
      TString optstr;
      if (!IsRequire())
         return optstr;
      for (auto &opt : fOptions)
         optstr += " " + opt.first + " " + opt.second + " ";
      optstr.ReplaceAll("  ", " "); // remove extra spaces
      return optstr;
   }
   //______________________________________________________________________________
   virtual void Print() const
   {
      std::cout << "ROOT Mpi CMD Module:" << fName << std::endl;
      std::cout << "Options:" << fName << std::endl;
      for (auto &opt : fOptions)
         std::cout << opt.first + ":" + opt.second << std::endl;
   }
};
}
}

#endif
