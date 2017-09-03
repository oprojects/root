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
   TString fName; // checkpoint name
protected:
   class TRestarter {
      friend class TCheckPoint;
      Char_t *fDataSet;
      Bool_t fHaveRestart;
      TRestarter(Bool_t haverestart, Char_t *dataset);
      TRestarter(const TRestarter &obj);

   public:
      Bool_t HaveRestart() const;
      void Restart() const;
      void Complete(Bool_t valid = 1);
   };

public:
   TCheckPoint(const TString name);
   void Init();
   void Finalize();
   TCheckPoint::TRestarter GetRestarter();
   const Char_t *GetRouteFile() const;
   void Start();
   void Complete(Bool_t valid = kTRUE);
   Int_t NeedCheckPoint();

   ClassDef(TCheckPoint, 0)
};
}
}
#endif
