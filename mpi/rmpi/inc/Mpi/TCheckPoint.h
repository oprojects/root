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
 \ingroup Mpi
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

protected:
   TString fName;   // checkpoint file name
   TString fSuffix; // checkpoint file name suffix
   const Char_t *GetRankFile() const;
   TCkpFile *fCkpFile;

public:
   TCheckPoint(const TString name, const TString suffix = "root");
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

template <class T>
void TCheckPoint::TCkpFile::WriteObject(const Char_t *name, const T *var)
{
   fFile->WriteObjectAny(var, gROOT->GetClass(typeid(T)), name);
   fFile->Flush();
}

template <class T>
void TCheckPoint::TCkpFile::ReadObject(const Char_t *name, T *obj)
{
   auto fobj = (T *)fFile->Get(name);
   if (!fobj) {
      // Error handling here
   }
   MemMove<T>(fobj, obj, 1);
}

template <class T>
void TCheckPoint::TCkpFile::WriteVar(const Char_t *name, const T &var)
{
   TCkpVar<T> *ckpvar = new TCkpVar<T>(&var, 1);
   WriteObject<TCkpVar<T>>(name, ckpvar);
   delete ckpvar;
}

template <class T>
void TCheckPoint::TCkpFile::ReadVar(const Char_t *name, T &var)
{
   TCkpVar<T> ckpvar;
   ReadObject<TCkpVar<T>>(name, &ckpvar);
   var = *ckpvar.GetVar();
}
}
}
#endif
