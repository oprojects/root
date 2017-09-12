// @(#)root/mpi / Author: Omar.Zapata@cern.ch 2016 http://oproject.org
#ifndef ROOT_Mpi_TEnvironment
#define ROOT_Mpi_TEnvironment

#include <Mpi/Globals.h>

namespace ROOT {

namespace Mpi {
/**
\class TEnvironment
   Class manipulate mpi environment, with this class you can to start/stop the communication system and
   to hanlde some information about the communication environment.
   \ingroup Mpi
 */

class TErrorHandler;
class TMpiSignalHandler;
class TCommunicator;
#if defined(ROOT_MPI_SCR)
class TCheckPoint;
#endif
class TEnvironment : public TObject {
#if defined(ROOT_MPI_SCR)
   friend class TCheckPoint;
#endif
   friend class TCommunicator;
   friend class TMpiSignalHandler;
   friend class TErrorHandler;

private:
   static TString fStdOut; // stding object for stdout
   static TString fStdErr;
   static Bool_t fSyncOutput;
   static Int_t fStdOutPipe[2];
   static Int_t fStdErrPipe[2];
   static Int_t fSavedStdErr;
   static Int_t fSavedStdOut;
   static Int_t fProfiling;

   static TErrorHandler fErrorHandler;
   static Int_t fCompressionAlgorithm;
   static Int_t fCompressionLevel;

   static FILE *fOutput;

   TMpiSignalHandler *fInterruptSignal;
   TMpiSignalHandler *fTerminationSignal;
   TMpiSignalHandler *fSigSegmentationViolationSignal;

protected:
   void InitSignalHandlers();
   template <class T>
   void SetEnv(const Char_t *var, T value, Bool_t overwrite);

public:
   TEnvironment(Int_t level = ROOT::Mpi::THREAD_SINGLE);
   TEnvironment(Int_t argc, Char_t **argv, Int_t level = ROOT::Mpi::THREAD_SINGLE);
   ~TEnvironment();

   void Finalize();

   // static public functions TODO
   static void SyncOutput(Bool_t status = kTRUE, FILE *output = NULL);

   static Bool_t IsFinalized();

   static Bool_t IsInitialized();

   static TString GetProcessorName();

   static Int_t GetThreadLevel();

   static Bool_t IsMainThread();

   static void SetCompression(Int_t level, Int_t algorithm = 0);

   static void InitCapture();

   static void EndCapture();

   static void Flush();

   static void Flush(TCommunicator *comm);

   static void ClearBuffers();

   static TString GetStdOut();

   static TString GetStdErr();

   static Bool_t IsSyncOutput();

   static Int_t GetCompressionAlgorithm();

   static Int_t GetCompressionLevel();

   static void SetProfiling(Int_t value);

   static Int_t IsProfiling();

   static void SetVerbose(Bool_t status = kTRUE);

#if defined(ROOT_MPI_SCR)
   // CheckPoint related methods
   static void CkpInit();

   static void CkpFinalize();

   static Bool_t IsCpkFinalized();

   static Bool_t IsCpkInitialized();

   void CkpCleanCache();

   void SetCkpJobId(UInt_t value, Bool_t overwrite = kTRUE);

   void SetCkpJobName(const Char_t *value, Bool_t overwrite = kTRUE);

   void SetCkpPrefix(const Char_t *value, Bool_t overwrite = kTRUE);

   void SetCkpDebug(Bool_t value, Bool_t overwrite = kTRUE);

   void SetCkpCacheBase(const Char_t *value, Bool_t overwrite = kTRUE);
   
   void SetCkpInterval(UInt_t value, Bool_t overwrite = kTRUE);
   
   void SetCkpSeconds(UInt_t value, Bool_t overwrite = kTRUE);
   
   void SetCkpConfigFile(const Char_t *value, Bool_t overwrite = kTRUE);

   void SetCkpFlush(UInt_t value, Bool_t overwrite = kTRUE);

#endif

   ClassDef(TEnvironment, 1)
};

template <class T>
void TEnvironment::SetEnv(const Char_t *var, T value, Bool_t overwrite)
{
   std::stringstream ost;
   ost << value;
   if (overwrite) {
      gSystem->Setenv(var, ost.str().c_str());
   } else {
      if (!gSystem->Getenv(var))
         gSystem->Setenv(var, ost.str().c_str());
   }
}
}
}

#endif
