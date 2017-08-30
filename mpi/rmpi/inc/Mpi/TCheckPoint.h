// @(#)root/mpi / Author: Omar.Zapata@cern.ch 2017 http://oproject.org
#ifndef ROOT_Mpi_TCheckPoint
#define ROOT_Mpi_TCheckPoint

#include <Mpi/Globals.h>
#include <Mpi/TErrorHandler.h>
#include <Mpi/TIntraCommunicator.h>

namespace ROOT {

namespace Mpi {
/**
\class TCheckPoint
 \ingroup Mpi
 */
class TCheckPoint : public TObject {
   std::map<TString, TString> fEnvVars; // SCR enviromental variable
   TFile *fCkpFile;                     // Check Point file
   TString fCkpFileName;                // Tmp Check point file name
   TString fName;                       // CheckPoint name
   TString fPath;                       // CheckPoint path
public:
   TCheckPoint(const TString name, const TString path);
   TString CkpPathName();
   static void Start();
   static void Complete(Int_t valid = 1);
   static Int_t NeedCheckPoint();

   ClassDef(TCheckPoint, 0)
};
}
}
#endif
