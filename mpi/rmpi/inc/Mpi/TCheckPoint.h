// @(#)root/mpi / Author: Omar.Zapata@cern.ch 2017 http://oproject.org
#ifndef ROOT_Mpi_TCheckPoint
#define ROOT_Mpi_TCheckPoint

#include <Mpi/Globals.h>
#include <Mpi/TErrorHandler.h>
#include <Mpi/TIntraCommunicator.h>
#include <TFile.h>
#include <ostream>

namespace ROOT {

namespace Mpi {

/**
\class TCheckPoint
Class to Write/Read state of the execution through a checkpoint.
This is a fault tolerance method based on the library SCR (Scalable Checkpoint/Restart for MPI)
implemented by Lawrence Livermore National Laboratory
https://computation.llnl.gov/projects/scalable-checkpoint-restart-for-mpi

With this class you can write the state of the execution periodically when it is required and in a incremental way,
according
to the configuration provided througth the methods in the classes ROOT::Mpi::TEnvironment and ROOT::Mpi::TCheckPoint or
using enviroment variables at the moment of the execution. In case of failure you can recover the state of the
execution.

According to the design, to create a checkpoint you need to set the environment using
the class ROOT::Mpi::TEnvironment and then, call the setter methods, such as, SetCkpJobId, SetCkpClusterName, etc, and
then
ROOT::Mpi::Enviroment:CkpInit to load the configuration into
the system.
After that, you need to create an instance of ROOT::Mpi:TCheckPoint that allows you to Write/Read your information into
temporal
files that are created by the library to preserve the required information in case something goes wrong.

To know when the checkpoint neeeds to write the state, you need to call the method ROOT::Mpi::TCheckPoint::IsRequired
and if this is true, you can get the current temporal file by calling the method
ROOT::Mpi::TCheckPoint::GetCkpFile() that returns an object of ROOT::Mpi::TCheckPoint::TCkpFile that basically is a
class
that
encapsulates a TFile objet with other information associated to the checkpoint.

If the application needs to retrieve the information from last checkpoint saved you can check this with the method
ROOT::Mpi::TCheckPoint::TRestarter::IsRequired.
In the case of you need to retrieve the variables to recover the state of the execution, you can
get a  ROOT::Mpi::TCheckPoint::TRestart::TCkpFile object to read it.

The most basic example, writing a checkpoint in a loop
\code{.cxx}
using namespace ROOT::Mpi;

void ckploop()
{
   TEnvironment env; // environment to start communication system
   env.CkpInit();
   TCheckPoint ckp("loop");
   auto rst = ckp.GetRestarter(); // restarter object to check if we need to read the last checkpoint

   auto chunk = 10000 / COMM_WORLD.GetSize();
   for (auto i = 0; i < chunk; i++) {
      // we need to check is we are in recovery mode
      if (rst.IsRequired()) {
         rst.Restart();                   // require to load the last checkpoint
         auto ckpfile = rst.GetCkpFile(); // file with the last checkpoint saved
         ckpfile->ReadVar<Int_t>("i", i); // getting the last i saved
         rst.Complete();                  // required to tell to the system that the checkpoint was successfully loaded.
      }
      // hard work here

      // conditional to try to save the checkpoint every number of iterations
      // and in the last iteratino
      if (i % 100 == 0 || i == (chunk - 1)) {
         cout << "Processing iteration " << i << "..."
              << endl; // printing tghe interation where we are doing the checkpoint
         // ask SCR whether we need to checkpoint
         if (ckp.IsRequired()) {
            ckp.Start(); // require to create a new temporal file where we will to save the checkpoint
            auto ckpfile = ckp.GetCkpFile(); // file to save the data
            ckpfile->WriteVar("i", i);       // saving the data
            ckp.Complete();                  // require to tell to the system that the recovery was successfully saved.
         }
      }
      gSystem->Sleep(10); // just to give some time kill the process and test the rcovery mode.
   }
     env.CkpFinalize();   // require to finalize the environment from SCR
}
\endcode

For more information please read the ROOTMpi users guide.


 \ingroup Mpi
 */

/**
 * \class TCheckPoint::TCkpFile
 * Class to Read/Write serialized information from checkpoint files.
 * Internally it have a TFile object and when the object is obtained
 * from TCheckPoint::TRestarter the internal TFile is open in READ mode to
 * retrieve the variables and recover the state, and when the object is
 * obtained from TCheckPoint::GetCkpFile the internal TFile is open in RECREATE
 * mode to save the required variables to save the state.
 *
 * \ingroup Mpi
*/

/**
 * \class TCheckPoint::TRestarter
 *  Class to recover the state of the execution reading the variables from last checkpoint.
 *
 * \ingroup Mpi
 */

class TCheckPoint : public TObject {
public:
   class TCkpFile : public TObject {
      friend class TCheckPoint;
      TFile *fFile;

   protected:
      TCkpFile(const char *fname, Option_t *option, const char *ftitle = "", Int_t compress = 1);

   public:
      virtual ~TCkpFile();
      inline Bool_t cd(const char *path = 0) { return fFile->cd(path); }
      template <class T>
      void WriteObject(const Char_t *name, const T *);
      template <class T>
      void ReadObject(const Char_t *name, T *obj);

      template <class T>
      void WriteVar(const Char_t *name, const T &);
      template <class T>
      void ReadVar(const Char_t *name, T &var);

      template <class T>
      void WriteArray(const Char_t *name, const T *, const Int_t size);
      template <class T>
      void ReadArray(const Char_t *name, T *, const Int_t size);

      void Close();
      ClassDef(TCkpFile, 0)
   };

   class TRestarter : public TObject {
      friend class TCheckPoint;
      Bool_t fHaveRestart;
      Char_t *fDataSet;
      TCheckPoint *fCkp;
      TCkpFile *fCkpFile;
      TRestarter(Bool_t haverestart, Char_t *dataset, TCheckPoint *ckp);

   public:
      TRestarter(const TRestarter &obj);
      virtual ~TRestarter();
      Bool_t HaveRestart() const;
      inline Bool_t IsRequired() const { return HaveRestart(); }
      void Restart();
      void Complete(Bool_t valid = 1);
      const Char_t *GetRouteFile() const;
      TCheckPoint::TCkpFile *GetCkpFile();
      ClassDef(TRestarter, 0)
   };
   template <class T>
   class TCkpVar : public TObject {
      UInt_t fSize;  ///<
      const T *fVar; ///<[fSize] value to encapsulate
   public:
      TCkpVar(const T *var = nullptr, UInt_t size = 0) : fSize(size), fVar(var) {}
      UInt_t GetSize() { return fSize; }
      const T *GetVar() { return fVar; }
      ClassDef(TCkpVar, 1)
   };

protected:
   TString fName;   // checkpoint file name
   const Char_t *GetRankFile() const;
   TCkpFile *fCkpFile;

public:
   TCheckPoint(const TString name);
   virtual ~TCheckPoint();
   TCheckPoint::TRestarter GetRestarter();
   TCheckPoint::TCkpFile *GetCkpFile();
   const Char_t *GetRouteFile() const;
   void Start();
   void Complete(Bool_t valid = kTRUE);
   Int_t NeedCheckPoint() const;
   inline Int_t IsRequired() const { return NeedCheckPoint(); };

   ClassDef(TCheckPoint, 0)
};

//______________________________________________________________________________
/**
 * Method to write data to checkpoint file.
 * \param name name of the variable.
 * \param obj  any serializable object.
 */
template <class T>
void TCheckPoint::TCkpFile::WriteObject(const Char_t *name, const T *obj)
{
   fFile->WriteObjectAny(obj, gROOT->GetClass(typeid(T)), name);
   fFile->Flush();
}

//______________________________________________________________________________
/**
 * Method to read data from checkpoint file.
 * \param name name of the variable.
 * \param obj  unserialized obj assigned (output).
 */
template <class T>
void TCheckPoint::TCkpFile::ReadObject(const Char_t *name, T *obj)
{
   auto fobj = (T *)fFile->Get(name);
   if (!fobj) {
      // Error handling here
   }
   MemMove<T>(fobj, obj, 1);
}

//______________________________________________________________________________
/**
 * Method to write data to checkpoint file.
 * \param name name of the variable.
 * \param var any variable (Double_t, Int_t, Char_t..).
 */
template <class T>
void TCheckPoint::TCkpFile::WriteVar(const Char_t *name, const T &var)
{
   TCkpVar<T> *ckpvar = new TCkpVar<T>(&var, 1);
   WriteObject<TCkpVar<T>>(name, ckpvar);
   delete ckpvar;
}

//______________________________________________________________________________
/**
 * Method to read data to checkpoint file.
 * \param name name of the variable.
 * \param var any variable (Double_t, Int_t, Char_t..) (output).
 */
template <class T>
void TCheckPoint::TCkpFile::ReadVar(const Char_t *name, T &var)
{
   TCkpVar<T> ckpvar;
   ReadObject<TCkpVar<T>>(name, &ckpvar);
   var = *ckpvar.GetVar();
}

//______________________________________________________________________________
/**
 * Method to write data to checkpoint file.
 * \param name name of the variable.
 * \param array any variable array (Double_t*, Int_t*, Char_t*..).
 * \param size size of the array
 */
template <class T>
void TCheckPoint::TCkpFile::WriteArray(const Char_t *name, const T *array, const Int_t size)
{
   TCkpVar<T> *ckpvar = new TCkpVar<T>(array, size);
   WriteObject<TCkpVar<T>>(name, ckpvar);
   delete ckpvar;
}

//______________________________________________________________________________
/**
 * Method to Read data to checkpoint file.
 * \param name name of the variable.
 * \param array any variable array (Double_t*, Int_t*, Char_t*..) (output).
 * \param size size of the array
 */
template <class T>
void TCheckPoint::TCkpFile::ReadArray(const Char_t *name, T *array, const Int_t size)
{
   TCkpVar<T> ckpvar;
   ReadObject<TCkpVar<T>>(name, &ckpvar);
   MemMove(ckpvar.GetVar(), array, size);
}
}
}
#endif
