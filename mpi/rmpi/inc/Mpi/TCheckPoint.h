// @(#)root/mpi / Author: Omar.Zapata@cern.ch 2017 http://oproject.org
#ifndef ROOT_Mpi_TCheckPoint
#define ROOT_Mpi_TCheckPoint

#include <Mpi/Globals.h>
#include <Mpi/TErrorHandler.h>
#include <Mpi/TIntraCommunicator.h>
#include <TFile.h>

namespace ROOT {

namespace Mpi {

/**
\class TCheckPoint
 \ingroup Mpi
 */

class TCkpFile;

class TCheckPoint : public TObject {
   TString fName;   // checkpoint file name
   TString fSuffix; // checkpoint file name suffix
   const Char_t *GetRankFile() const;
   TCkpFile *fCkpFile;

public:
   class TRestarter {
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
      TCkpFile *GetCkpFile();
   };

public:
   TCheckPoint(const TString name, const TString suffix = "root");
   virtual ~TCheckPoint();
   void Init();
   void Finalize();
   TCheckPoint::TRestarter GetRestarter();
   const Char_t *GetRouteFile() const;
   TCkpFile *GetCkpFile();
   void Start();
   void Complete(Bool_t valid = kTRUE);
   Int_t NeedCheckPoint() const;
   inline Int_t IsRequired() const { return NeedCheckPoint(); };

   ClassDef(TCheckPoint, 0)
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

class TCkpFile {
   friend class TCheckPoint;
   TFile *fFile;

protected:
   TCkpFile(const char *fname, Option_t *option, const char *ftitle = "", Int_t compress = 1);

public:
   virtual ~TCkpFile();
   inline Bool_t cd(const char *path = 0) { return fFile->cd(path); }
   inline void ReadAll(Option_t *option = "") { fFile->ReadAll(option); }
   template <class T>
   void WriteObject(const Char_t *name, const T *);
   template <class T>
   void ReadObject(const Char_t *name, T *obj);

   template <class T>
   void WriteVar(const Char_t *name, const T &);
   template <class T>
   void ReadVar(const Char_t *name, T &var);

   void Close();
};

template <class T>
void TCkpFile::WriteObject(const Char_t *name, const T *var)
{
   fFile->WriteObjectAny(var, gROOT->GetClass(typeid(T)), name);
   fFile->Flush();
}

template <class T>
void TCkpFile::ReadObject(const Char_t *name, T *obj)
{
   auto fobj = (T *)fFile->Get(name);
   if (!fobj) {
      // Error handling here
   }
   MemMove<T>(fobj, obj, 1);
}

template <class T>
void TCkpFile::WriteVar(const Char_t *name, const T &var)
{
   TCkpVar<T> *ckpvar = new TCkpVar<T>(&var, 1);
   WriteObject<TCkpVar<T>>(name, ckpvar);
   delete ckpvar;
}

template <class T>
void TCkpFile::ReadVar(const Char_t *name, T &var)
{
   TCkpVar<T> ckpvar;
   ReadObject<TCkpVar<T>>(name, &ckpvar);
   var = *ckpvar.GetVar();
}
}
}
#endif
