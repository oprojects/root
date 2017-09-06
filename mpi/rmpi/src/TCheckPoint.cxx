#include <Mpi/TCheckPoint.h>
#include <Mpi/TEnvironment.h>
#include <Mpi/TIntraCommunicator.h>

#include <iostream>
using namespace ROOT::Mpi;

//______________________________________________________________________________
TCheckPoint::TCheckPoint(const TString name, const TString suffix) : fName(name), fSuffix(suffix), fCkpFile(nullptr)
{
}

//______________________________________________________________________________
TCheckPoint::~TCheckPoint()
{
   if (fCkpFile) {
      delete fCkpFile;
      fCkpFile = nullptr;
   }
}

//______________________________________________________________________________
TCheckPoint::TRestarter::TRestarter(Bool_t haverestart, Char_t *dataset, TCheckPoint *ckp)
   : fHaveRestart(haverestart), fDataSet(dataset), fCkp(ckp), fCkpFile(nullptr)
{
}

//______________________________________________________________________________
TCheckPoint::TRestarter::TRestarter(const TRestarter &obj)
{
   fHaveRestart = obj.fHaveRestart;
   fDataSet = obj.fDataSet;
   fCkp = obj.fCkp;
}

//______________________________________________________________________________
Bool_t TCheckPoint::TRestarter::HaveRestart() const
{
   return fHaveRestart;
}

//______________________________________________________________________________
TCheckPoint::TRestarter::~TRestarter()
{
   if (fCkpFile)
      delete fCkpFile;
}

//______________________________________________________________________________
void TCheckPoint::TRestarter::Restart()
{
   SCR_Start_restart(fDataSet);
   if (!fCkpFile)
      fCkpFile = new TCkpFile(fCkp->GetRouteFile(), "READ", "CkpFile");
}

//______________________________________________________________________________
void TCheckPoint::TRestarter::Complete(Bool_t valid)
{
   if (fCkpFile) {
      fCkpFile->Close();
      delete fCkpFile;
      fCkpFile = nullptr;
   }
   SCR_Complete_restart(valid);
   if (valid)
      fHaveRestart = kFALSE;
}

//______________________________________________________________________________
const Char_t *TCheckPoint::TRestarter::GetRouteFile() const
{
   return fCkp->GetRouteFile();
}

//______________________________________________________________________________
TCkpFile *TCheckPoint::TRestarter::GetCkpFile()
{
   //     if(fCkpFile) return *fCkpFile;
   //     else{
   //         //error handling here
   return fCkpFile;
   //     }
}

//______________________________________________________________________________
TCkpFile *TCheckPoint::GetCkpFile()
{
   //     if(fCkpFile) return *fCkpFile;
   //     else{
   //         //error handling here
   return fCkpFile;
   //     }
}

//______________________________________________________________________________
void TCheckPoint::Init()
{
   SCR_Init();
}

//______________________________________________________________________________
void TCheckPoint::Finalize()
{
   SCR_Finalize();
}

//______________________________________________________________________________
TCheckPoint::TRestarter TCheckPoint::GetRestarter()
{
   Int_t have_restart;
   Char_t *dset = new Char_t[SCR_MAX_FILENAME];
   Int_t scr_retval = SCR_Have_restart(&have_restart, dset);
   if (scr_retval != SCR_SUCCESS) {
      printf("%d: failed calling SCR_Have_restart: %d: @%s:%d\n", COMM_WORLD.GetRank(), scr_retval, __FILE__, __LINE__);
   }
   return TRestarter(have_restart, dset, this);
}

//______________________________________________________________________________
const Char_t *TCheckPoint::GetRouteFile() const
{
   Char_t *file = new Char_t[SCR_MAX_FILENAME];
   SCR_Route_file(GetRankFile(), file);
   return const_cast<Char_t *>(file);
}

//______________________________________________________________________________
const Char_t *TCheckPoint::GetRankFile() const
{
   return Form("%s_%d.%s", fName.Data(), COMM_WORLD.GetRank(), fSuffix.Data());
}

//______________________________________________________________________________
void TCheckPoint::Start()
{
   SCR_Start_checkpoint();
   if (!fCkpFile)
      fCkpFile = new TCkpFile(GetRouteFile(), "RECREATE", "CkpFile");
}

//______________________________________________________________________________
void TCheckPoint::Complete(Bool_t valid)
{
   if (fCkpFile) {
      fCkpFile->Close();
      delete fCkpFile;
      fCkpFile = nullptr;
   }
   SCR_Complete_checkpoint(valid);
}

//______________________________________________________________________________
Int_t TCheckPoint::NeedCheckPoint() const
{
   Int_t flag = 0;
   SCR_Need_checkpoint(&flag);
   return flag;
}

//______________________________________________________________________________
TCkpFile::TCkpFile(const char *fname, Option_t *option, const char *ftitle, Int_t compress)
{
   fFile = new TFile(fname, option, ftitle, compress);
}

//______________________________________________________________________________
TCkpFile::~TCkpFile()
{
   if (fFile) {
      fFile->Close();
      delete fFile;
      fFile = nullptr;
   }
}

//______________________________________________________________________________
void TCkpFile::Close()
{
   if (fFile)
      fFile->Close();
}
