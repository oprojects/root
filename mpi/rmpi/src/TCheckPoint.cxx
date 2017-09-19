#include <Mpi/TCheckPoint.h>
#include <Mpi/TEnvironment.h>
#include <Mpi/TIntraCommunicator.h>

#include <iostream>
using namespace ROOT::Mpi;

// Bool_t TCkpEnvironment::fStatus=kFALSE;
//
// //______________________________________________________________________________
// TCkpEnvironment::TCkpEnvironment() : TObject()
// {
//     if(TEnvironment::IsInitialized())
//     {
//     }else{
//         #warning Error handling here
//     }
// //    // setting up default options
// //    fOptions["SCR_CACHE_SIZE"] = "1";
// //    fOptions["SCR_CHECKPOINT_OVERHEAD"] = "";
// //    fOptions["SCR_CLUSTER_NAME"] = "";
// //    fOptions["SCR_CNTL_BASE"] = "";
// //    fOptions["SCR_COPY_TYPE"] = "SINGLE"; // Set to one of: SINGLE, PARTNER, XOR, or FILE.
// //    fOptions["SCR_CRC_ON_COPY"] = "0";
// //    fOptions["SCR_CRC_ON_DELETE"] = "0";
// //    fOptions["SCR_CRC_ON_FLUSH"] = "1";
// //    fOptions["SCR_DISTRIBUTE"] = "1";
// //    fOptions["SCR_FETCH "] = "1";
// //    fOptions["SCR_FETCH_WIDTH"] = "256";
// //    fOptions["SCR_FILE_BUF_SIZE"] = "1048576";
// //    fOptions["SCR_FLUSH"] = "10";
// //    fOptions["SCR_FLUSH_WIDTH"] = "256";
// //    fOptions["SCR_FLUSH_ON_RESTART"] = "0";
// //    fOptions["SCR_GLOBAL_RESTART"] = "";
// //    fOptions["SCR_GROUP"] = "NODE"; //
// //    fOptions["SCR_HALT SECONDS"] = "0";
// //    fOptions["SCR_HALT_ENABLED"] = "";
// //    fOptions["SCR_LOG_ENABLE"] = "0";
// //    fOptions["SCR_MPI_BUF_SIZE"] = "131072";
// //    fOptions["SCR_RUNS"] = "0";
// //    fOptions["SCR_SET_SIZE"] = "8";
// //    fOptions["SCR_USER_NAME"] = gSystem->GetUserInfo(gSystem->GetUid())->fUser;
// //    fOptions["SCR_USE_CONTAINERS"] = "";
// //    fOptions["SCR_CONTAINER_SIZE"] = "";
// }

// //______________________________________________________________________________
// void TCkpEnvironment::Finalize()
// {
//    SCR_Finalize();
// }

//______________________________________________________________________________
TCheckPoint::TCheckPoint(const TString name) : fName(name), fCkpFile(nullptr)
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
/**
 * Contructor for class to restart the checkpoint.
 * \param haverestart have a restart file? was a checkpoint saved?
 * \param dataset     name of temporal file with the data
 * \param ckp         TCheckPoint object for current restart
 */
TCheckPoint::TRestarter::TRestarter(Bool_t haverestart, Char_t *dataset, TCheckPoint *ckp)
   : TObject(), fHaveRestart(haverestart), fDataSet(dataset), fCkp(ckp), fCkpFile(nullptr)
{
}

//______________________________________________________________________________
TCheckPoint::TRestarter::TRestarter(const TRestarter &obj) : TObject(obj)
{
   fHaveRestart = obj.fHaveRestart;
   fDataSet = obj.fDataSet;
   fCkp = obj.fCkp;
}

//______________________________________________________________________________
/**
 * Method to check is checkpoint needs restart
 * \return true if have a restart checkpoint.
 */
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
/**
 * Inform library that restart is starting, get name of restart that is available.
 */
void TCheckPoint::TRestarter::Restart()
{
   SCR_Start_restart(fDataSet);
   if (!fCkpFile)
      fCkpFile = new TCkpFile(fCkp->GetRouteFile(), "READ", "CkpFile");
}

//______________________________________________________________________________
/**
 *   Inform library that the current restart is complete and marks it as valid or not.
 *   \param valid if valid then TCheckPoint::TRestarter::IsRequired is false, because checkpoint was correctly loaded.
 */
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
/**
 * Method to get the name of the file with the last checkpoint.
 *
 * \return file name to last checkpoint.
 */
const Char_t *TCheckPoint::TRestarter::GetRouteFile() const
{
   return fCkp->GetRouteFile();
}

//______________________________________________________________________________
/**
 * Method to get a ROOT::Mpi::TCheckPoint::TCkpFile object to read data from
 * last cehckpoint.
 *
 * \return ROOT::Mpi::TCheckPoint::TCkpFile object.
 */
TCheckPoint::TCkpFile *TCheckPoint::TRestarter::GetCkpFile()
{
   return fCkpFile;
}

//______________________________________________________________________________
/**
 * Method to get a ROOT::Mpi::TCheckPoint::TCkpFile object to write data in the checkpoint
 * file.
 *
 * \return ROOT::Mpi::TCheckPoint::TCkpFile object.
 */
TCheckPoint::TCkpFile *TCheckPoint::GetCkpFile()
{
   // error handling here(check if Start was called)
   return fCkpFile;
}

//______________________________________________________________________________
/**
 * Method to get an ROOT::Mpi::TCheckPoint::TRestarter object
 * to restore previos saved checkpoint.
 * \return ROOT::Mpi::TCheckPoint::TRestarter object
 */
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
/**
 * Method to get the name of the file to write data to the checkpoint.
 *
 * \return file name to checkpoint.
 */
const Char_t *TCheckPoint::GetRouteFile() const
{
   Char_t *file = new Char_t[SCR_MAX_FILENAME];
   SCR_Route_file(GetRankFile(), file);
   return const_cast<Char_t *>(file);
}

//______________________________________________________________________________
/**
 * File name with current rank and the checkpoint name.
 *
 * \return file name with the name of the checkpoint and rank.
 */
const Char_t *TCheckPoint::GetRankFile() const
{
   return Form("%s_%d.root", fName.Data(), COMM_WORLD.GetRank());
}

//______________________________________________________________________________
/**
 * Informs SCR that a fresh checkpoint set is about to start.
 */
void TCheckPoint::Start()
{
   SCR_Start_checkpoint();
   if (!fCkpFile)
      fCkpFile = new TCkpFile(GetRouteFile(), "RECREATE", "CkpFile");
}

//______________________________________________________________________________
/**
 * completes the checkpoint set and marks it as valid or not.
 */
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
/**
 * Returns kTRUE if a checkpoint should be taken.
 * \return kTRUE if checkpoint is required
 */
Int_t TCheckPoint::NeedCheckPoint() const
{
   Int_t flag = 0;
   SCR_Need_checkpoint(&flag);
   return flag;
}

//______________________________________________________________________________
TCheckPoint::TCkpFile::TCkpFile(const char *fname, Option_t *option, const char *ftitle, Int_t compress)
{
   fFile = new TFile(fname, option, ftitle, compress);
}

//______________________________________________________________________________
TCheckPoint::TCkpFile::~TCkpFile()
{
   if (fFile) {
      fFile->Close();
      delete fFile;
      fFile = nullptr;
   }
}

//______________________________________________________________________________
void TCheckPoint::TCkpFile::Close()
{
   if (fFile)
      fFile->Close();
}
