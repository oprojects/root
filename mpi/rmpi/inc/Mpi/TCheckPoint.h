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
   TString fName;   // checkpoint file name
   TString fSuffix; // checkpoint file name suffix
public:
   class TRestarter {
      friend class TCheckPoint;
      Bool_t fHaveRestart;
      Char_t *fDataSet;
      TCheckPoint *fCkp;
      TRestarter(Bool_t haverestart, Char_t *dataset, TCheckPoint *ckp);
      TRestarter(const TRestarter &obj);

   public:
      Bool_t HaveRestart() const;
      void Restart() const;
      void Complete(Bool_t valid = 1);
      const Char_t *GetRouteFile() const;
   };

public:
   TCheckPoint(const TString name, const TString suffix);
   void Init();
   void Finalize();
   TCheckPoint::TRestarter GetRestarter();
   const Char_t *GetRouteFile() const;
   const Char_t *GetRankFile() const;
   void Start();
   void Complete(Bool_t valid = kTRUE);
   Int_t NeedCheckPoint();

   ClassDef(TCheckPoint, 0)
};
}
}
#endif
